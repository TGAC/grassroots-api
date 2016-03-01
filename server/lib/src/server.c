/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <string.h>

#include "server.h"
#include "linked_list.h"
#include "jansson.h"
#include "json_tools.h"
#include "json_util.h"
#include "service.h"
#include "time_util.h"
#include "operation.h"
#include "query.h"
#include "connect.h"
#include "user.h"
#include "key_value_pair.h"
#include "service.h"
#include "service_matcher.h"

#include "handler.h"
#include "handler_utils.h"

#include "math_utils.h"
#include "string_utils.h"
#include "jobs_manager.h"
#include "parameter_set.h"

#include "servers_pool.h"
#include "uuid/uuid.h"

//#include "irods_handle.h"


#ifdef _DEBUG
	#define SERVER_DEBUG	(STM_LEVEL_FINE)
#else
	#define SERVER_DEBUG	(STM_LEVEL_NONE)
#endif


/*****************************/
/***** STATIC PROTOTYPES *****/
/*****************************/



#if IRODS_ENABLED == 1
static json_t *GetAllModifiedData (const json_t * const req_p, const json_t *credentials_p);
#endif


static json_t *GetInterestedServices (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetAllServices (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServicesAsJSON (const char * const services_path_s, const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p);

static int8 RunServiceFromJSON (const json_t *req_p, json_t *credentials_p, json_t *res_p, uuid_t user_uuid);

static json_t *RunKeywordServices (const json_t * const req_p, json_t *config_p, const char *keyword_s);

static Operation GetOperation (json_t *ops_p);

static json_t *GetNamedServices (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServiceStatus (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServiceResultsAsJSON (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServiceData (const json_t * const req_p, const json_t *credentials_p, bool (*callback_fn) (json_t *services_p, uuid_t service_id, const char *uuid_s));

static bool AddServiceDataToJSON (json_t *results_p, uuid_t job_id, const char *uuid_s, const char * const identifier_s, json_t *(*get_job_json_fn) (ServiceJob *job_p));

static bool AddServiceStatusToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static bool AddServiceResultsToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static bool AddServiceCleanUpToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static json_t *CleanUpJobs (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetRequestedResource (const json_t * const req_p, const json_t *credentials_p);

static int32 AddPairedServices (Service *internal_service_p, const char *username_s, const char *password_s);

static int32 AddAllPairedServices (LinkedList *internal_services_p, const char *username_s, const char *password_s);



/***************************/
/***** API DEFINITIONS *****/
/***************************/


json_t *ProcessServerRawMessage (const char * const request_s, const int socket_fd)
{
	json_error_t error;
	json_t *req_p = json_loads (request_s, JSON_PRESERVE_ORDER, &error);
	json_t *res_p = NULL;
	
	if (req_p)
		{
			const char *error_s = NULL;

			res_p = ProcessServerJSONMessage (req_p, socket_fd, &error_s);

			if (error_s)
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Error \"%s\" from ProcessServerJSONMessage for :\n%s\n", error_s, request_s);
				}

		}
	else
		{
			/* error decoding the request */
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Could not get load json from:\n%s\n", request_s);
		}	

	return res_p;
}


json_t *ProcessServerJSONMessage (json_t *req_p, const int socket_fd, const char **error_ss)
{
	json_t *res_p = NULL;

	if (req_p)
		{
			if (json_is_object (req_p))
				{
					json_t *op_p = NULL;
					json_t *credentials_p = json_object_get (req_p, CREDENTIALS_S);
					json_t *uri_p = NULL;

					if (!credentials_p)
						{
							credentials_p = json_object ();

							if (credentials_p)
								{
									if (json_object_set_new (req_p, CREDENTIALS_S, credentials_p) != 0)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add credentials json");
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create credentials json");
								}
						}


					/* add a unique id if not already there */
					if (credentials_p)
						{
							json_t *uuid_p = json_object_get (credentials_p, CREDENTIALS_UUID_S);

							if (!uuid_p)
								{
									char *uuid_s = NULL;
									uuid_t user_id;

									uuid_generate (user_id);

									uuid_s = GetUUIDAsString (user_id);

									if (uuid_s)
										{
											if (json_object_set_new (credentials_p, CREDENTIALS_UUID_S, json_string (uuid_s)) != 0)
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add uuid string to credentials");
												}

											FreeUUIDString (uuid_s);
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid string");
										}
								}
						}

				#if SERVER_DEBUG >= STM_LEVEL_FINEST
					if (req_p)
						{
							PrintJSONToLog (req_p,"ProcessMessage - request: \n", STM_LEVEL_FINEST);
						}
				#endif


					/*
					 * Is this request for an external server?
					 */
					uri_p = json_object_get (req_p, SERVER_URI_S);
					if (uri_p)
						{
							/*
							 * Find the matching external server,
							 * Remove the server uuid and proxy
							 * the request/response
							 */
							if (json_is_string (uri_p))
								{
									const char *uuid_s = json_string_value (uri_p);
									uuid_t key;

									if (ConvertStringToUUID (uuid_s, key))
										{
											ServersManager *manager_p = GetServersManager ();

											if (manager_p)
												{
													ExternalServer *external_server_p = GetExternalServerFromServersManager (manager_p, uuid_s, NULL);

													if (external_server_p)
														{
															/* remove the server's uuid */
															if (json_object_del (req_p, SERVER_UUID_S) == 0)
																{
																	/* we can now proxy the request off to the given server */
																	json_t *response_p = MakeRemoteJSONCallToExternalServer (external_server_p, req_p);

																	if (response_p)
																		{
																			/*
																			 * We now need to add the ExternalServer's
																			 * uuid back in. Not sure if we can use uuid_s
																			 * as it may have gone out of scope when we called
																			 * json_obtject_del, so best to recreate it.
																			 */
																			char buffer [UUID_STRING_BUFFER_SIZE];

																			ConvertUUIDToString (key, buffer);

																			if (json_object_set_new (response_p, SERVER_UUID_S, json_string (buffer)) == 0)
																				{

																				}		/* if (json_object_set_new (response_p, SERVER_UUID_S, json_string (buffer) == 0)) */

																		}		/* if (response_p) */

																}		/* if (json_object_del (req_p, SERVER_UUID_S) == 0) */

														}		/* if (external_server_p)*/

												}		/* if (manager_p) */

										}		/* if (ConvertStringToUUID (uuid_s, key)) */

								}		/* if (json_is_string (uuid_p)) */

						}		/* if (uuid_p) */
					else
						{
							/* the request is for this server */
						}


					if ((op_p = json_object_get (req_p, SERVER_OPERATIONS_S)) != NULL)
						{
							Operation op = GetOperation (op_p);

							switch (op)
								{
									case OP_LIST_ALL_SERVICES:
										res_p = GetAllServices (req_p, credentials_p);
										break;

									case OP_IRODS_MODIFIED_DATA:
										{
											#if IRODS_ENABLED == 1
											res_p = GetAllModifiedData (req_p, credentials_p);
											#endif
										}
										break;

									case OP_LIST_INTERESTED_SERVICES:
										res_p = GetInterestedServices (req_p, credentials_p);
										break;

									case OP_RUN_KEYWORD_SERVICES:
										{
											json_t *keyword_json_group_p = json_object_get (req_p, KEYWORDS_QUERY_S);

											if (keyword_json_group_p)
												{
													json_t *keyword_json_value_p = json_object_get (keyword_json_group_p, KEYWORDS_QUERY_S);

													if (keyword_json_value_p)
														{
															if (json_is_string (keyword_json_value_p))
																{
																	const char *keyword_s = json_string_value (keyword_json_value_p);

																	res_p = RunKeywordServices (req_p, credentials_p, keyword_s);
																}
														}
												}
										}
										break;

									case OP_GET_NAMED_SERVICES:
										res_p = GetNamedServices (req_p, credentials_p);
										break;

									case OP_CHECK_SERVICE_STATUS:
										res_p = GetServiceStatus (req_p, credentials_p);
										break;

									case OP_GET_SERVICE_RESULTS:
										res_p = GetServiceResultsAsJSON (req_p, credentials_p);
										break;

									case OP_CLEAN_UP_JOBS:
										res_p = CleanUpJobs (req_p, credentials_p);
										break;

									case OP_GET_RESOURCE:
										res_p = GetRequestedResource (req_p, credentials_p);
										break;

									default:
										break;
								}		/* switch (op) */


							//res_p = AddExternalServerOperationsToJSON (servers_manager_p, res_p, op);

						}
					else if ((op_p = json_object_get (req_p, SERVICES_NAME_S)) != NULL)
						{
							uuid_t user_uuid;
							const char *user_uuid_s = GetUserUUIDStringFromJSON (credentials_p);

							if (user_uuid_s)
								{
									uuid_parse (user_uuid_s, user_uuid);
								}
							else
								{
									uuid_clear (user_uuid);
								}


							res_p = json_array ();

							if (res_p)
								{
									if (json_is_array (op_p))
										{
											size_t i;
											json_t *value_p;

											json_array_foreach (op_p, i, value_p)
												{
													int8 res = RunServiceFromJSON (value_p, credentials_p, res_p, user_uuid);

													if (res < 0)
														{
															char *value_s = json_dumps (value_p, JSON_INDENT (2));

															if (value_s)
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to run service from %s", value_s);
																	free (value_s);
																}
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to run service from");
																}
														}
												}		/* json_array_foreach (op_p, i, value_p) */
										}
									else
										{
											int8 res = RunServiceFromJSON (op_p, credentials_p, res_p, user_uuid);

											if (res < 0)
												{
													char *value_s = json_dumps (op_p, JSON_INDENT (2));

													if (value_s)
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to run service from %s", value_s);
															free (value_s);
														}
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to run service from");
														}
												}
										}

								}		/* if (res_p) */

						}		/* 	else if ((op_p = json_object_get (req_p, SERVICES_NAME_S)) != NULL) */


					#if SERVER_DEBUG >= STM_LEVEL_FINER
					PrintJSONToLog (res_p, "ProcessMessage - response: \n", STM_LEVEL_FINER, __FILE__, __LINE__);
					FlushLog ();
					#endif

				}		/* if (json_is_object (req_p)) */
			else
				{
					*error_ss = "Request is not an explicit json object";
				}

		}		/* if (req_p) */
	else
		{
			*error_ss = "Request is NULL";
		}
	
	return res_p;
}





/******************************/
/***** STATIC DEFINITIONS *****/
/******************************/


static int8 RunServiceFromJSON (const json_t *req_p, json_t *credentials_p, json_t *res_p, uuid_t user_uuid)
{
	/* Get the requested operation */
	json_t *op_p = json_object_get (req_p, SERVICE_RUN_S);
	int res = 0;
	char *req_s = json_dumps (req_p, JSON_PRESERVE_ORDER | JSON_INDENT (2));

	if (op_p)
		{
			if (json_is_true (op_p))
				{
					const char *service_name_s = GetServiceNameFromJSON (req_p);
					
					if (service_name_s)
						{
							LinkedList *services_p = AllocateLinkedList  (FreeServiceNode);

							if (services_p)
								{
									LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, credentials_p);

									#if SERVER_DEBUG >= STM_LEVEL_FINEST
										{
											ServiceNode * node_p = (ServiceNode *) (services_p -> ll_head_p);

											while (node_p)
												{
													Service *service_p = node_p -> sn_service_p;
													const char *name_s = GetServiceName (service_p);

													PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "matched service \"%s\"\n", name_s);

													node_p = (ServiceNode *) (node_p -> sn_node.ln_next_p);
												}
										}
									#endif

									if (services_p -> ll_size == 1)
										{
											ServiceNode *node_p = (ServiceNode *) (services_p -> ll_head_p);
											Service *service_p =  node_p -> sn_service_p;
											ParameterSet *params_p = NULL;
											char *username_s = NULL;
											char *password_s = NULL;

											AddPairedServices (service_p, username_s, password_s);

											/*
											 * Convert the json parameter set into a ParameterSet
											 * to run the Service with.
											 */
											params_p = CreateParameterSetFromJSON (req_p);

											if (params_p)
												{
													ServiceJobSet *jobs_p = NULL;

													#if SERVER_DEBUG >= STM_LEVEL_FINER
													PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "about to run service \"%s\"\n", service_name_s);
													#endif

													jobs_p = RunService (service_p, params_p, credentials_p);

													if (jobs_p)
														{
															bool keep_service_flag = false;

															if (ProcessServiceJobSet (jobs_p, res_p, &keep_service_flag))
																{
																	++ res;
																}

															#if SERVER_DEBUG >= STM_LEVEL_FINER
																{
																	PrintJSONToLog (res_p, "initial results", STM_LEVEL_FINER, __FILE__, __LINE__);
																	FlushLog ();
																	PrintJSONRefCounts (res_p, "initial results: ",  STM_LEVEL_FINER, __FILE__, __LINE__);
																}
															#endif

															if (keep_service_flag)
																{
																	/* since we've checked for a single node */
																	LinkedListRemHead (services_p);
																	node_p -> sn_service_p = NULL;
																	FreeServiceNode ((ListItem * const) node_p);
																}

														}		/* if (jobs_p) */
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "No jobs from running %s with params from %s", service_name_s, req_s);
														}

													FreeParameterSet  (params_p);
												}		/* if (params_p) */
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "Failed to get params from %s", req_s);
												}

										}		/* if (services_p -> ll_size == 1)) */
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "Failed to find matching service %s, found " UINT32_FMT, service_name_s, services_p -> ll_size);
										}

									FreeLinkedList (services_p);
								}		/* if (services_p) */
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "Failed to get allocate services list");
								}

						}		/* if (service_name_s) */
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "Failed to get service name from json %s", req_s);
						}
				}		/* if (json_is_true (op_p)) */

		}		/* if (op_p) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "Failed to get run value from json %s", req_s);
		}


	if (req_s)
		{
			free (req_s);
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (res_p, "final result = ", STM_LEVEL_FINE, __FILE__, __LINE__);
	FlushLog ();
	#endif

	#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONRefCounts (res_p, "final result: ", STM_LEVEL_FINER, __FILE__, __LINE__);
	#endif

	return res;
}


static Operation GetOperation (json_t *ops_p)
{
	Operation op = OP_NONE;
	json_t *op_p = json_object_get (ops_p, OPERATION_ID_S);

	if (!op_p)
		{
			op_p = json_object_get (ops_p, OPERATION_ID_OLD_S);
		}

	if (op_p)
		{
			if (json_is_integer (op_p))
				{
					op = json_integer_value (op_p);
				}
			else if (json_is_string (op_p))
				{
					const char *value_s = json_string_value (op_p);
					int i;
					
					if (GetValidInteger (&value_s, &i, NULL))
						{
							op = i;
						}
				}
		}
	
	return op;
}



static Resource *GetResourceOfInterest (const json_t * const req_p)
{
	Resource *resource_p = NULL;
	json_t *file_data_p = json_object_get (req_p, KEY_FILE_DATA);
	
	if (file_data_p)
		{
			json_t *protocol_p = json_object_get (file_data_p, KEY_PROTOCOL);

			if (protocol_p)
				{
					if (json_is_string (protocol_p))
						{
							/* is it a single file or a dir? */
							json_t *data_name_p = json_object_get (file_data_p, KEY_FILENAME);
							
							if (data_name_p && (json_is_string (data_name_p)))
								{
									const char *protocol_s = json_string_value (protocol_p);
									const char *data_name_s = json_string_value (data_name_p);
									
									resource_p = AllocateResource (protocol_s, data_name_s, NULL);
								}
							
						}		/* if (json_is_string (protocol_p)) */
					
				}		/* if (protocol_p) */
								
		}		/* if (file_data_p) */

	return resource_p;
}


static json_t *GetInterestedServices (const json_t * const req_p, const json_t * const credentials_p)
{
	json_t *res_p = NULL;
	Resource *resource_p = GetResourceOfInterest (req_p);

	if (resource_p)
		{
			Handler *handler_p = GetResourceHandler (resource_p, credentials_p);

			if (handler_p)
				{
					json_t *config_p = NULL;
					const char *username_s = NULL;
					const char *password_s = NULL;
												
					if (GetUsernameAndPassword (credentials_p, &username_s, &password_s))
						{
							res_p = GetServicesAsJSON (SERVICES_PATH_S, username_s, password_s, resource_p, handler_p, config_p);
						}
						
					FreeHandler (handler_p);
				}
				
			FreeResource (resource_p);
		}				
	
	return res_p;
}


static json_t *GetAllServices (const json_t * const req_p, const json_t *credentials_p)
{
	json_t *res_p = NULL;
	const char *username_s = NULL;
	const char *password_s = NULL;
												
	/* Get the local services */
	res_p = GetServicesAsJSON (SERVICES_PATH_S, username_s, password_s, NULL, NULL, credentials_p);

	return res_p;
}


static bool AddServiceDataToJSON (json_t *results_p, uuid_t job_id, const char *uuid_s, const char * const identifier_s, json_t *(*get_job_json_fn) (ServiceJob *job_p))
{
	bool success_flag = false;
	JobsManager *manager_p = GetJobsManager ();
	ServiceJob *job_p = GetServiceJobFromJobsManager (manager_p, job_id, NULL);
	json_t *job_json_p = NULL;

	if (job_p)
		{
			job_json_p = get_job_json_fn (job_p);

			if (!job_json_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job %s for \"%s\" %s", identifier_s, job_p -> sj_name_s ? job_p -> sj_name_s : "", uuid_s);
				}
		}		/* if (job_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to find %s in services table", uuid_s);

			job_json_p = json_pack ("{s:s,s:s}", JOB_UUID_S, uuid_s, ERROR_S, "Failed to find uuid in services table");

			if (!job_json_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get create error json for %s", uuid_s);
				}
		}

	if (job_json_p)
		{
			if (json_array_append_new (results_p, job_json_p) != 0)
				{
					json_decref (job_json_p);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job %s for \"%s\" %s", identifier_s, job_p -> sj_name_s ? job_p -> sj_name_s : "", uuid_s);
				}
		}

	return success_flag;
}

static bool AddServiceStatusToJSON (json_t *results_p, uuid_t job_id, const char *uuid_s)
{
	return AddServiceDataToJSON (results_p, job_id, uuid_s, "status", GetServiceJobStatusAsJSON);
}


static bool AddServiceResultsToJSON (json_t *results_p, uuid_t job_id, const char *uuid_s)
{
	return AddServiceDataToJSON (results_p, job_id, uuid_s, "results", GetServiceJobAsJSON);
}


static json_t *GetServiceData (const json_t * const req_p, const json_t *credentials_p, bool (*callback_fn) (json_t *services_p, uuid_t service_id, const char *uuid_s))
{
	json_t *results_array_p = json_array ();

	if (results_array_p)
		{
			json_t *service_uuids_json_p = json_object_get (req_p, SERVICES_NAME_S);

			if (service_uuids_json_p)
				{
					if (json_is_array (service_uuids_json_p))
						{
							size_t i;
							json_t *service_uuid_json_p;
							size_t num_successes = 0;
							size_t num_uuids = json_array_size (service_uuids_json_p);

							json_array_foreach (service_uuids_json_p, i, service_uuid_json_p)
								{
									if (json_is_string (service_uuid_json_p))
										{
											const char *uuid_s = json_string_value (service_uuid_json_p);
											uuid_t service_id;

											if (ConvertStringToUUID (uuid_s, service_id))
												{
													if (callback_fn (results_array_p, service_id, uuid_s))
														{
															++ num_successes;
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add callback json results for \"%s\"", uuid_s);
														}

												}		/* if (ConvertStringToUUID (uuid_s, service_id)) */
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to convert \"%s\" to uuid", uuid_s);
												}

										}		/* if (json_is_string (service_uuid_json_p)) */
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "service_uuid_json_p is %d not a string", json_typeof (service_uuid_json_p));
										}

								}		/* json_array_foreach (service_uuids_json_p, i, service_uuid_json_p) */


							if (num_uuids == num_successes)
								{
									//CloseService (job_p -> sj_service_p);
								}

						}		/* if (json_is_array (service_uuids_json_p)) */
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "service_uuids_json_p is %d not an array", json_typeof (service_uuids_json_p));
						}

				}		/* if (service_uuids_json_p) */

		}		/* if (results_array_p) */

	return results_array_p;
}


static json_t *GetServiceResultsAsJSON (const json_t * const req_p, const json_t *credentials_p)
{
	return GetServiceData (req_p, credentials_p, AddServiceResultsToJSON);
}



static json_t *GetServiceStatus (const json_t * const req_p, const json_t *credentials_p)
{
	return GetServiceData (req_p, credentials_p, AddServiceStatusToJSON);
}


static json_t *CleanUpJobs (const json_t * const req_p, const json_t *credentials_p)
{
	return GetServiceData (req_p, credentials_p, AddServiceCleanUpToJSON);
}


static json_t *GetRequestedResource (const json_t * const req_p, const json_t *credentials_p)
{
	json_t *res_p = NULL;

	return res_p;
}


static bool AddServiceCleanUpToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s)
{
	bool success_flag = false;
	json_t *status_p = json_object ();

	if (status_p)
		{
			if (json_object_set_new (status_p, SERVICE_UUID_S, json_string (uuid_s)) == 0)
				{
					JobsManager *manager_p = GetJobsManager ();
					ServiceJob *job_p = RemoveServiceJobFromJobsManager (manager_p, service_id, NULL);

					if (job_p)
						{
							Service *service_p = job_p -> sj_service_p;
							const char *service_name_s = GetServiceName (job_p -> sj_service_p);
							OperationStatus status = CloseServiceJob (job_p) ? OS_CLEANED_UP : OS_ERROR;

							if (!IsServiceLive (service_p))
								{
									CloseService (service_p);
								}

							success_flag = true;

							if (json_object_set_new (status_p, SERVICE_NAME_S, json_string (service_name_s)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add service name %s to status json", service_name_s);
									json_object_set_new (status_p, ERROR_S, json_string ("Failed to add service name to status json"));
									success_flag = false;
								}

							if (json_object_set_new (status_p, SERVICE_STATUS_VALUE_S, json_integer (status)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add service status for name %s to status json", service_name_s);
									json_object_set_new (status_p, ERROR_S, json_string ("Failed to add service status to status json"));
									success_flag = false;
								}

						}		/* if (service_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to find %s in services table", uuid_s);
							json_object_set_new (status_p, ERROR_S, json_string ("Failed to find uuid in services table"));
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add service uuid_s %s to status json", uuid_s);
					json_object_set_new (status_p, ERROR_S, json_string ("Failed to add uuid to status json"));
				}

			success_flag = (json_array_append_new (services_p, status_p) == 0);
		}		/* if (status_p) */

	return success_flag;
}


static json_t *GetNamedServices (const json_t * const req_p, const json_t *credentials_p)
{
	json_t *res_p = NULL;
	LinkedList *services_p = AllocateLinkedList (FreeServiceNode);

	if (services_p)
		{
			const char *service_name_s = NULL;
			json_t *service_names_p = json_object_get (req_p, SERVICES_NAME_S);

			if (service_names_p)
				{
					json_t *service_name_p = NULL;

					if (json_is_array (service_names_p))
						{
							size_t index;

							/*@TODO
							 * This is inefficient and would be better to loop through in
							 * a LoadServices.... method passing in an array of service names
							 */
							json_array_foreach (service_names_p, index, service_name_p)
								{
									if (json_is_string (service_name_p))
										{
											service_name_s = json_string_value (service_name_p);
											LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, credentials_p);
										}
								}
						}
					else
						{
							if (json_is_string (service_name_p))
								{
									service_name_s = json_string_value (service_name_p);
									LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, credentials_p);
								}
						}
				}

			if (services_p -> ll_size > 0)
				{
					const char *username_s = NULL;
					const char *password_s = NULL;

					GetUsernameAndPassword (credentials_p, &username_s, &password_s);
					AddAllPairedServices (services_p, username_s, password_s);

					res_p = GetServicesListAsJSON (services_p, NULL, credentials_p, false);
				}

			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return res_p;
}


#if IRODS_ENABLED == 1
static json_t *GetAllModifiedData (const json_t * const req_p, const json_t *credentials_p)
{
	json_t *res_p = NULL;
	const char *username_s = NULL;
	const char *password_s = NULL;
												
	if (GetUsernameAndPassword (credentials_p, &username_s, &password_s))
		{
			const char *from_s = NULL;
			const char *to_s = NULL;
			/* "from" defaults to the start of time */
			time_t from = 0;

			/* "to" defaults to now */
			time_t to = time (NULL);

			json_t *group_p = json_object_get (req_p, KEY_IRODS);
			
			if (group_p)
				{
					json_t *interval_p = json_object_get (group_p, KEY_INTERVAL);
					
					if (interval_p)
						{
							from_s = GetJSONString (interval_p, "from");
							to_s = GetJSONString (interval_p, "to");
						}
				}
																
			if (from_s)
				{
					if (!ConvertCompactStringToEpochTime (from_s, &from))
						{
							// error
						}
				}
				
			if (to_s)
				{
					if (!ConvertCompactStringToEpochTime (to_s, &to))
						{
							// error
						}
				}
			
			res_p = GetModifiedIRodsFiles (username_s, password_s, from, to);
		}
	
	return res_p;
}
#endif


static bool IsRequiredExternalOperation (const json_t *external_op_p, const char *op_name_s)
{
	bool match_flag = false;
	const char *external_op_s = GetOperationNameFromJSON (external_op_p);

	if (external_op_s)
		{
			match_flag = (strcmp (external_op_s, op_name_s) == 0);
		}

	return match_flag;
}


static LinkedList *GetServicesList (const char * const services_path_s, const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p)
{
	LinkedList *services_p = AllocateLinkedList (FreeServiceNode);

	if (services_p)
		{
			ServersManager *servers_manager_p = GetServersManager ();

			LoadMatchingServices (services_p, services_path_s, resource_p, handler_p, config_p);

			if (services_p -> ll_size > 0)
				{
					AddAllPairedServices (services_p, username_s, password_s);

					return services_p;
				}

			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return NULL;
}



static json_t *GetServicesAsJSON (const char * const services_path_s, const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p)
{
	json_t *json_p = NULL;
	LinkedList *services_p = GetServicesList (services_path_s, username_s, password_s, resource_p, handler_p, config_p);
	
	if (services_p)
		{
			json_p = GetServicesListAsJSON (services_p, resource_p, config_p, false);
			FreeLinkedList (services_p);
		}
	else
		{
			ServersManager *servers_manager_p = GetServersManager ();
			json_p = AddExternalServerOperationsToJSON (servers_manager_p, NULL, OP_LIST_ALL_SERVICES);
		}

	return json_p;
}


static json_t *RunKeywordServices (const json_t * const req_p, json_t *config_p, const char *keyword_s)
{
	json_t *res_p = json_array ();

	if (res_p)
		{
			Resource *resource_p = AllocateResource (PROTOCOL_TEXT_S, keyword_s, keyword_s);

			if (resource_p)
				{
					const char *username_s = NULL;
					const char *password_s = NULL;
					LinkedList *services_p = GetServicesList (SERVICES_PATH_S, username_s, password_s, resource_p, NULL, config_p);

					if (services_p)
						{
							ServiceMatcher *matcher_p = AllocateKeywordServiceMatcher ();

							if (matcher_p)
								{
									/* For each service, set its keyword parameter */
									ServiceNode *service_node_p = (ServiceNode *) (services_p -> ll_head_p);

									while (service_node_p)
										{
											Service *service_p = service_node_p -> sn_service_p;

											if (RunServiceMatcher (matcher_p, service_p))
												{
													ParameterSet *params_p = NULL;
													bool param_flag = true;

													params_p = GetServiceParameters (service_p, NULL, config_p);

													if (params_p)
														{
															ParameterNode *param_node_p = (ParameterNode *) params_p -> ps_params_p -> ll_head_p;

															param_flag = false;

															while (param_node_p)
																{
																	Parameter *param_p = param_node_p -> pn_parameter_p;

																	/* set the keyword parameter */
																	if (param_p -> pa_type == PT_KEYWORD)
																		{
																			if (SetParameterValue (param_p, keyword_s, true))
																				{
																					param_flag = true;
																				}
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set service param \"%s\" - \"%s\" to \"%s\"", GetServiceName (service_p), param_p -> pa_name_s, keyword_s);
																					param_flag = false;
																				}
																		}

																	param_node_p = (ParameterNode *) (param_node_p -> pn_node.ln_next_p);
																}		/* while (param_node_p) */

															/* Now run the service */
															if (param_flag)
																{
																	ServiceJobSet *jobs_set_p = RunService (service_p, params_p, config_p);

																	if (jobs_set_p)
																		{
																			bool keep_service_flag;
																			ProcessServiceJobSet (jobs_set_p, res_p, &keep_service_flag);
																		}
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to run service \"%s\" with keyword \"%s\"", GetServiceName (service_p), keyword_s);
																		}
																}

															ReleaseServiceParameters (service_p, params_p);

														}		/* if (params_p) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get parameters for service \"%s\" to run with keyword \"%s\"", GetServiceName (service_p), keyword_s);
														}

												}		/* if (RunServiceMatcher (matcher_p, service_p)) */
											else
												{
													ParameterSet *params_p = IsServiceMatch (service_p, resource_p, NULL);

													/*
													 * Does the service match for running against this keyword?
													 */
													if (params_p)
														{
															/*
															 * Add the information that the service is interested in this keyword
															 * and can be ran.
															 */
															const char *service_name_s = GetServiceName (service_p);
															json_t *interested_app_p = GetInterestedServiceJSON (service_name_s, keyword_s, params_p);

															if (interested_app_p)
																{
																	if (json_array_append_new (res_p, interested_app_p) != 0)
																		{
																			json_decref (interested_app_p);
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add interested service \"%s\" for keyword \"%s\" to results", GetServiceName (service_p), keyword_s);
																		}		/* if (json_array_append_new (res_p, interested_app_p) != 0) */

																}		/* if (interested_app_p) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create JSON for interested service \"%s\" for keyword \"%s\" to results", GetServiceName (service_p), keyword_s);
																}

															ReleaseServiceParameters (service_p, params_p);
														}		/* if (params_p) */

												}		/* /* if (RunServiceMatcher (matcher_p, service_p)) else */

											service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);
										}		/* while (service_node_p) */

									FreeServiceMatcher (matcher_p);
								}		/* if (matcher_p) */

							FreeLinkedList (services_p);
						}		/* if (services_p) */

					FreeResource (resource_p);
				}		/* if (resource_p) */


			if (json_array_size (res_p) > 0)
				{
					return res_p;
				}

			json_decref (res_p);
		}		/* if (res_p) */

	return NULL;
}


static int32 AddPairedServices (Service *internal_service_p, const char *username_s, const char *password_s)
{
	int32 num_added_services = 0;
	ServersManager *servers_manager_p = GetServersManager ();
	LinkedList *external_servers_p = GetAllExternalServersFromServersManager (servers_manager_p, DeserialiseExternalServerFromJSON);

	if (external_servers_p)
		{
			const char *internal_service_name_s = GetServiceName (internal_service_p);
			ExternalServerNode *external_server_node_p = (ExternalServerNode *) (external_servers_p -> ll_head_p);

			while (external_server_node_p)
				{
					ExternalServer *external_server_p = external_server_node_p -> esn_server_p;
					json_t *req_p = GetAvailableServicesRequest (username_s, password_s);

					if (req_p)
						{
							json_t *response_p = MakeRemoteJSONCallToExternalServer (external_server_p, req_p);

							/* We no longer need the request */
							json_decref (req_p);

							if (response_p)
								{
									/* If it has paired services try and match them up */
									if (external_server_p -> es_paired_services_p)
										{
											KeyValuePairNode *pairs_node_p = (KeyValuePairNode *) (external_server_p -> es_paired_services_p -> ll_head_p);

											while (pairs_node_p)
												{
													const char *service_name_s = pairs_node_p -> kvpn_pair_p -> kvp_key_s;

													if (strcmp (service_name_s, internal_service_name_s) == 0)
														{
															const char *op_name_s = pairs_node_p -> kvpn_pair_p -> kvp_value_s;

															/* We don't need to loop after this iteration */
															pairs_node_p = NULL;

															/*
															 * Get the required Service from the ExternalServer
															 */
															if (json_is_array (response_p))
																{
																	const size_t size = json_array_size (response_p);
																	size_t i;

																	for (i = 0; i < size; ++ i)
																		{
																			json_t *service_response_p = json_array_get (response_p, i);
																			json_t *external_operations_p = json_object_get (service_response_p, SERVER_OPERATIONS_S);

																			if (external_operations_p)
																				{
																					json_t *matching_external_op_p = NULL;

																					if (json_is_array (external_operations_p))
																						{
																							size_t j;
																							const size_t num_ops = json_array_size (external_operations_p);

																							for (j = 0; j < num_ops; ++ j)
																								{
																									json_t *external_op_p = json_array_get (external_operations_p, j);

																									if (IsRequiredExternalOperation (external_op_p, op_name_s))
																										{
																											matching_external_op_p = external_op_p;
																											j = num_ops;		/* force exit from loop */
																										}

																								}		/* for (j = 0; j < num_ops; ++ j) */

																						}		/* if (json_is_array (external_operations_json_p)) */
																					else if (json_is_object (external_operations_p))
																						{
																							if (IsRequiredExternalOperation (external_operations_p, op_name_s))
																								{
																									matching_external_op_p = external_operations_p;
																								}
																						}

																					/* Do we have our remote service definition? */
																					if (matching_external_op_p)
																						{
																							/*
																							 * Merge the external service with our own and
																							 * if successful, then remove the external one
																							 * from the json array
																							 */
																							if (CreateAndAddPairedService (internal_service_p, external_server_p, op_name_s, matching_external_op_p))
																								{
																									++ num_added_services;
																								}		/* if (CreateAndAddPairedService (matching_internal_service_p, external_server_p, matching_external_op_p)) */

																							i = size;		/* force exit from loop */
																						}		/* if (matching_external_op_p) */

																				}		/* if (external_operations_json_p) */

																		}		/* for (i = 0; i < size; ++ i) */

																}		/* if (json_is_array (response_p)) */

														}		/* if (strcmp (service_name_s, internal_service_name_s) == 0) */

													if (pairs_node_p)
														{
															pairs_node_p = (KeyValuePairNode *) (pairs_node_p -> kvpn_node.ln_next_p);
														}

												}		/* while (pairs_node_p) */

										}		/* if (external_server_p -> es_paired_services_p) */

									json_decref (response_p);
								}		/* if (response_p) */

						}		/* if (req_p) */

					external_server_node_p = (ExternalServerNode *) external_server_node_p -> esn_node.ln_next_p;
				}		/* (while (external_server_node_p) */

			FreeLinkedList (external_servers_p);
		}		/* if (external_servers_p) */

	return num_added_services;
}


static int32 AddAllPairedServices (LinkedList *internal_services_p, const char *username_s, const char *password_s)
{
	int32 num_added_services = 0;

	if (internal_services_p)
		{
			ServiceNode *internal_service_node_p = (ServiceNode *) (internal_services_p -> ll_head_p);

			/*
			 * Loop through our internal services trying to find a match
			 */
			while (internal_service_node_p)
				{
					int32 i = AddPairedServices (internal_service_node_p -> sn_service_p, username_s, password_s);

					num_added_services += i;
					internal_service_node_p = (ServiceNode *) (internal_service_node_p -> sn_node.ln_next_p);
				}		/* while (internal_service_node_p) */

		}		/* if (internal_services_p) */

	return num_added_services;
}

