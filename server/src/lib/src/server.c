/*
 ** Copyright 2014-2016 The Earlham Institute
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
#include "service_config.h"

#include "handler.h"
#include "handler_utils.h"

#include "math_utils.h"
#include "string_utils.h"
#include "jobs_manager.h"
#include "parameter_set.h"

#include "servers_pool.h"
#include "uuid/uuid.h"

//#include "irods_handle.h"
#include "providers_state_table.h"
#include "grassroots_config.h"



#ifdef _DEBUG
#define SERVER_DEBUG	(STM_LEVEL_FINE)
#else
#define SERVER_DEBUG	(STM_LEVEL_NONE)
#endif


/*****************************/
/***** STATIC PROTOTYPES *****/
/*****************************/



#if IRODS_ENABLED == 1
static json_t *GetAllModifiedData (const json_t * const req_p, UserDetails *user_p);
#endif


static json_t *GetInterestedServices (const json_t * const req_p, UserDetails *user_p);

static json_t *GetAllServices (const json_t * const req_p, UserDetails *user_p);

static json_t *GetServicesAsJSON (const char * const services_path_s, UserDetails *user_p, Resource *resource_p, Handler *handler_p, ProvidersStateTable *providers_p);

static int8 RunServiceFromJSON (const json_t *service_req_p, const json_t *paired_servers_req_p, UserDetails *user_p, json_t *res_p, uuid_t user_uuid);

static json_t *RunKeywordServices (const json_t * const req_p, UserDetails *user_p, const char *keyword_s);

static Operation GetOperation (json_t *ops_p);

static json_t *GetNamedServices (const json_t * const req_p, UserDetails *user_p);

static json_t *GetServiceStatus (const json_t * const req_p, UserDetails *user_p);

static json_t *GetServiceResultsAsJSON (const json_t * const req_p, UserDetails *user_p);

static json_t *GetServiceData (const json_t * const req_p, UserDetails *user_p, bool (*callback_fn) (json_t *services_p, uuid_t service_id, const char *uuid_s));

static bool AddServiceDataToJSON (json_t *results_p, uuid_t job_id, const char *uuid_s, const char * const identifier_s, json_t *(*get_job_json_fn) (ServiceJob *job_p));

static bool AddServiceStatusToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static bool AddServiceResultsToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static bool AddServiceCleanUpToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static json_t *CleanUpJobs (const json_t * const req_p, UserDetails *user_p);

static json_t *GetRequestedResource (const json_t * const req_p, UserDetails *user_p);

static int32 AddPairedServices (Service *internal_service_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static int32 AddAllPairedServices (LinkedList *internal_services_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static json_t *GetServerStatus (const json_t * const req_p, UserDetails *user_p);

static LinkedList *GetServicesList (const char * const services_path_s, UserDetails *user_p, Resource *resource_p, Handler *handler_p, ProvidersStateTable *providers_p);





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


json_t *ProcessServerJSONMessage (json_t *req_p, const int UNUSED_PARAM (socket_fd), const char **error_ss)
{
	json_t *res_p = NULL;

	if (req_p)
		{
			if (json_is_object (req_p))
				{
					json_t *op_p = NULL;
					UserDetails *user_p = NULL;
					json_t *uri_p = NULL;
					json_t *config_p = json_object_get (req_p, CONFIG_S);

					if (config_p)
						{
							json_t *credentials_p = json_object_get (config_p, CREDENTIALS_S);

							/*
							 * Create the UserDetails from the JSON details.
							 */
							if (credentials_p)
								{
									user_p = AllocateUserDetails (credentials_p);

									if (!user_p)
										{
											PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, credentials_p, "Failed to create UserDetails");
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
										res_p = GetAllServices (req_p, user_p);
										break;

//									case OP_IRODS_MODIFIED_DATA:
//										{
//											#if IRODS_ENABLED == 1
//											res_p = GetAllModifiedData (req_p, user_p);
//											#endif
//										}
//										break;

									case OP_LIST_INTERESTED_SERVICES:
										res_p = GetInterestedServices (req_p, user_p);
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

																	res_p = RunKeywordServices (req_p, user_p, keyword_s);
																}
															else
																{
																	PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, keyword_json_value_p, "Keyword not a string");
																}
														}
													else
														{
															PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, keyword_json_group_p, "Failed to get query keyword");
														}
												}
											else
												{
													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, req_p, "Failed to get query group");
												}
										}
										break;

									case OP_GET_NAMED_SERVICES:
										res_p = GetNamedServices (req_p, user_p);
										break;

									case OP_CHECK_SERVICE_STATUS:
										res_p = GetServiceStatus (req_p, user_p);
										break;

									case OP_GET_SERVICE_RESULTS:
										res_p = GetServiceResultsAsJSON (req_p, user_p);
										break;

									case OP_CLEAN_UP_JOBS:
										res_p = CleanUpJobs (req_p, user_p);
										break;

									case OP_GET_RESOURCE:
										res_p = GetRequestedResource (req_p, user_p);
										break;

									case OP_SERVER_STATUS:
										res_p = GetServerStatus (req_p, user_p);
										break;


									default:
										break;
								}		/* switch (op) */


							//res_p = AddExternalServerOperationsToJSON (servers_manager_p, res_p, op);

						}
					else if ((op_p = json_object_get (req_p, SERVICES_NAME_S)) != NULL)
						{
							json_t *service_results_p = json_array ();

							if (service_results_p)
								{
									uuid_t user_uuid;

									uuid_clear (user_uuid);

									res_p = GetInitialisedResponseOnServer (req_p, SERVICE_RESULTS_S, service_results_p);

									if (res_p)
										{
											const json_t *external_servers_req_p = json_object_get (req_p, SERVERS_S);

											if (json_is_array (op_p))
												{
													size_t i;
													json_t *value_p;

													json_array_foreach (op_p, i, value_p)
														{
															int8 res = RunServiceFromJSON (value_p, external_servers_req_p, user_p, service_results_p, user_uuid);

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
													int8 res = RunServiceFromJSON (op_p, external_servers_req_p, user_p, service_results_p, user_uuid);

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
									else
										{
											*error_ss = "Failed to create service response";
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create response");
										}

								}		/* if (service_results_p) */
							else
								{
									*error_ss = "Failed to create service response";
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create service results array");
								}

						}		/* 	else if ((op_p = json_object_get (req_p, SERVICES_NAME_S)) != NULL) */


#if SERVER_DEBUG >= STM_LEVEL_FINER
					PrintJSONToLog (res_p, "ProcessMessage - response: \n", STM_LEVEL_FINER, __FILE__, __LINE__);
					FlushLog ();
#endif


					if (user_p)
						{
							FreeUserDetails (user_p);
						}

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



json_t *GetInitialisedResponseOnServer (const json_t *req_p, const char *key_s, json_t *value_p)
{
	const SchemaVersion * const sv_p = GetSchemaVersion ();
	json_t *res_p = GetInitialisedMessage (sv_p);

	if (res_p)
		{
			if (req_p)
				{
					bool verbose_flag = false;

					GetJSONBoolean (req_p, REQUEST_VERBOSE_S, &verbose_flag);

					if (verbose_flag)
						{
							json_t *copied_req_p = json_deep_copy (req_p);

							if (copied_req_p)
								{
									if (json_object_set_new (res_p, REQUEST_S, copied_req_p) != 0)
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add request to response header");
											json_decref (copied_req_p);
										}

								}		/* if (copied_req_p) */
							else
								{
									PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, req_p, "Failed to add request to response header");
								}

						}		/* if (verbose_flag) */

				}		/* if (req_p) */

			if (key_s && value_p)
				{
					if (json_object_set_new (res_p, key_s, value_p) != 0)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s to the response header", key_s);
							json_decref (res_p);
						}
				}		/* if (key_s && value_p) */

		}		/* if (res_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get initialised message");
		}

	return res_p;
}




/******************************/
/***** STATIC DEFINITIONS *****/
/******************************/


static int8 RunServiceFromJSON (const json_t *service_req_p, const json_t *paired_servers_req_p, UserDetails *user_p, json_t *res_p, uuid_t user_uuid)
{
	/* Get the requested operation */
	json_t *op_p = json_object_get (service_req_p, SERVICE_RUN_S);
	int res = 0;
	char *req_s = json_dumps (service_req_p, JSON_PRESERVE_ORDER | JSON_INDENT (2));

	if (op_p)
		{
			if (json_is_true (op_p))
				{
					const char *service_name_s = GetServiceNameFromJSON (service_req_p);

					if (service_name_s)
						{
							LinkedList *services_p = AllocateLinkedList  (FreeServiceNode);

							if (services_p)
								{
									LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, user_p);

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
											const char *server_uri_s = GetServerProviderURI ();
											ProvidersStateTable *providers_p = GetInitialisedProvidersStateTableForSingleService (paired_servers_req_p, server_uri_s, service_name_s);

											if (providers_p)
												{
													ServiceNode *node_p = (ServiceNode *) (services_p -> ll_head_p);
													Service *service_p =  node_p -> sn_service_p;
													ParameterSet *params_p = NULL;

													AddPairedServices (service_p, user_p, providers_p);

													/*
													 * Convert the json parameter set into a ParameterSet
													 * to run the Service with.
													 */
													params_p = CreateParameterSetFromJSON (service_req_p, true);

													if (params_p)
														{
															ServiceJobSet *jobs_p = NULL;

															#if SERVER_DEBUG >= STM_LEVEL_FINER
															PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "about to run service \"%s\"\n", service_name_s);
															#endif

															/*
															 * Now we reset the providers table back to its initial state
															 */
															if (ReinitProvidersStateTable (providers_p, paired_servers_req_p, server_uri_s, service_name_s))
																{
																	jobs_p = RunService (service_p, params_p, user_p, providers_p);

																	if (jobs_p)
																		{
																			if (ProcessServiceJobSet (jobs_p, res_p))
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


																		}		/* if (jobs_p) */
																	else
																		{
																			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "No jobs from running %s with params from %s", service_name_s, req_s);
																		}

																}		/* if (ReinitProvidersStateTable (providers_p, req_p, server_uri_s, service_name_s)) */


															FreeParameterSet  (params_p);
														}		/* if (params_p) */
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__,  "Failed to get params from %s", req_s);
														}

													FreeProvidersStateTable (providers_p);
												}		/* if (providers_p) */


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
	PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, res_p, "final result = ");
	FlushLog ();
#endif

#if SERVER_DEBUG >= STM_LEVEL_FINER
	PrintJSONRefCounts (STM_LEVEL_FINER, __FILE__, __LINE__, res_p, "final result: ");
#endif

	return res;
}


static Operation GetOperation (json_t *ops_p)
{
	Operation op = OP_NONE;
	json_t *op_p = json_object_get (ops_p, OPERATION_S);

	if (op_p)
		{
			if (json_is_string (op_p))
				{
					const char *op_s = json_string_value (op_p);

					if (op_s)
						{
							op = GetOperationFromString (op_s);

							if (op == OP_NONE)
								{
									PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, op_p, "Failed to get valid operation value");
								}
						}
					else
						{
							PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, ops_p, "%s value is NULL", OPERATION_S);
						}
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, ops_p, "%s value is not a string", OPERATION_S);
				}
		}

	if (op == OP_NONE)
		{
			op_p = json_object_get (ops_p, OPERATION_ID_S);

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

							if (GetValidInteger (&value_s, &i))
								{
									op = i;
								}
						}
				}

		}

	return op;
}



static Resource *GetResourceOfInterest (const json_t * const req_p)
{
	Resource *resource_p = NULL;
	json_t *file_data_p = json_object_get (req_p, RESOURCE_S);

	if (file_data_p)
		{
			json_t *protocol_p = json_object_get (file_data_p, RESOURCE_PROTOCOL_S);

			if (protocol_p)
				{
					if (json_is_string (protocol_p))
						{
							json_t *data_name_p = json_object_get (file_data_p, RESOURCE_VALUE_S);

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


static json_t *GetInterestedServices (const json_t * const req_p, UserDetails *user_p)
{
	json_t *res_p = NULL;
	Resource *resource_p = GetResourceOfInterest (req_p);

	if (resource_p)
		{
			Handler *handler_p = GetResourceHandler (resource_p, user_p);

			if (handler_p)
				{
					json_t *paired_servers_p = (req_p != NULL) ? json_object_get (req_p, SERVERS_S) : NULL;
					ProvidersStateTable *providers_p = AllocateProvidersStateTable (paired_servers_p);

					if (providers_p)
						{
							json_t *services_p = GetServicesAsJSON (SERVICES_PATH_S, user_p, resource_p, handler_p, providers_p);

							if (services_p)
								{
									res_p = GetInitialisedResponseOnServer (req_p, SERVICES_NAME_S, services_p);

									if (!res_p)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetInitialisedResponse failed for adding services");

											json_decref (services_p);
										}
								}
							else
								{
									const SchemaVersion *sv_p = GetSchemaVersion ();
									res_p = GetInitialisedMessage (sv_p);

									if (!res_p)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetInitialisedMessage failed");
										}
								}

							FreeProvidersStateTable (providers_p);
						}

					FreeHandler (handler_p);
				}

			FreeResource (resource_p);
		}				

	return res_p;
}


static json_t *GetAllServices (const json_t * const req_p, UserDetails *user_p)
{
	json_t *paired_servers_p = (req_p != NULL) ? json_object_get (req_p, SERVERS_S) : NULL;
	ProvidersStateTable *providers_p = AllocateProvidersStateTable (paired_servers_p);

	if (providers_p)
		{
			/* Get the local services */
			json_t *services_p = GetServicesAsJSON (SERVICES_PATH_S, user_p, NULL, NULL, providers_p);

			FreeProvidersStateTable (providers_p);

			if (services_p)
				{
					json_t *res_p = GetInitialisedResponseOnServer (req_p, SERVICES_NAME_S, services_p);

					if (res_p)
						{
							return res_p;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetInitialisedResponse failed for adding services");
						}

					json_decref (services_p);
				}
			else
				{
					PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "GetServicesAsJSON returned no services");
				}

		}		/* if (providers_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate ProvidersStateTable");
		}

	return NULL;
}


static bool AddServiceDataToJSON (json_t *results_p, uuid_t job_id, const char *uuid_s, const char * const identifier_s, json_t *(*get_job_json_fn) (ServiceJob *job_p))
{
	bool success_flag = false;
	JobsManager *manager_p = GetJobsManager ();
	ServiceJob *job_p = GetServiceJobFromJobsManager (manager_p, job_id);
	json_t *job_json_p = NULL;

	if (job_p)
		{
			OperationStatus old_status = GetCachedServiceJobStatus (job_p);
			OperationStatus current_status = GetServiceJobStatus (job_p);

			/* Has the ServiceJob changed its status since the last check? */
			if (old_status != current_status)
				{
					switch (current_status)
						{

							/*
							 * If the job has finished, then remove it from the jobs manager.
							 */
							case OS_FINISHED:
							case OS_SUCCEEDED:
							case OS_PARTIALLY_SUCCEEDED:
								{
									RemoveServiceJobFromJobsManager (manager_p, job_id, false);
								}
								break;


							case OS_ERROR:
							case OS_FAILED_TO_START:
							case OS_FAILED:
								{
									RemoveServiceJobFromJobsManager (manager_p, job_id, false);
								}
								break;

								/*
								 * If the job has updated its status but not yet finished running,
								 * then update the value stored in the jobs manager
								 */
							case OS_PENDING:
							case OS_STARTED:
								{
									if (!AddServiceJobToJobsManager (manager_p, job_id, job_p))
										{
											char job_uuid_s [UUID_STRING_BUFFER_SIZE];

											ConvertUUIDToString (job_id, job_uuid_s);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to update job %s from status %d to %d", job_uuid_s, old_status, current_status);
										}
								}
								break;

						}		/* switch (current_status) */

				}		/* if (old_status != current_status) */

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


static json_t *GetServiceData (const json_t * const req_p, UserDetails *user_p, bool (*callback_fn) (json_t *services_p, uuid_t service_id, const char *uuid_s))
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


static json_t *GetServiceResultsAsJSON (const json_t * const req_p, UserDetails *user_p)
{
	return GetServiceData (req_p, user_p, AddServiceResultsToJSON);
}



static json_t *GetServiceStatus (const json_t * const req_p, UserDetails *user_p)
{
	return GetServiceData (req_p, user_p, AddServiceStatusToJSON);
}


static json_t *CleanUpJobs (const json_t * const req_p, UserDetails *user_p)
{
	return GetServiceData (req_p, user_p, AddServiceCleanUpToJSON);
}


static json_t *GetRequestedResource (const json_t * const req_p, UserDetails *user_p)
{
	json_t *res_p = NULL;

	return res_p;
}


static json_t *GetServerStatus (const json_t * const req_p, UserDetails *user_p)
{
	json_t *res_p = json_object ();

	if (res_p)
		{
			JobsManager *jobs_manager_p = GetJobsManager ();

			if (jobs_manager_p)
				{
					LinkedList *jobs_list_p = GetAllServiceJobsFromJobsManager (jobs_manager_p);

					if (jobs_list_p)
						{
							if (jobs_list_p -> ll_size > 0)
								{
									json_t *jobs_array_p = json_array ();

									if (jobs_array_p)
										{
											ServiceJobNode *job_node_p = (ServiceJobNode *) (jobs_list_p -> ll_head_p);
											bool success_flag = true;

											while ((success_flag == true) && (job_node_p != NULL))
												{
													json_t *job_json_p = GetServiceJobAsJSON (job_node_p -> sjn_job_p);

													if (job_json_p)
														{
															if (json_array_append_new (jobs_array_p, job_json_p) == 0)
																{
																	job_node_p = (ServiceJobNode *) (job_node_p -> sjn_node.ln_next_p);
																}
															else
																{
																	success_flag = false;
																	json_decref (job_json_p);
																}

														}		/* if (job_json_p) */
													else
														{
															success_flag = false;
														}

												}		/* while ((success_flag == true) && (job_node_p != NULL)) */

											if (success_flag)
												{
													if (json_object_set (res_p, SERVICE_JOBS_S, jobs_array_p) != 0)
														{
															success_flag = false;
															json_decref (jobs_array_p);
														}
												}
											else
												{
													json_decref (jobs_array_p);
												}

										}		/* if (jobs_array_p) */

								}		/* if (jobs_list_p -> ll_size > 0) */

							FreeLinkedList (jobs_list_p);
						}		/* if (jobs_list_p) */

				}		/* if (jobs_manager_p) */

		}		/* if (res_p) */



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
					ServiceJob *job_p = RemoveServiceJobFromJobsManager (manager_p, service_id, true);

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


static json_t *GetNamedServices (const json_t * const req_p, UserDetails *user_p)
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
										LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, user_p);
									}
							}
						}
					else
						{
							if (json_is_string (service_name_p))
								{
									service_name_s = json_string_value (service_name_p);
									LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, user_p);
								}
						}
				}

			if (services_p -> ll_size > 0)
				{
					ProvidersStateTable *providers_p = GetInitialisedProvidersStateTable (req_p, services_p);

					if (providers_p)
						{
							json_t *services_json_p = NULL;

							//GetUsernameAndPassword (credentials_p, &username_s, &password_s);
							AddAllPairedServices (services_p, user_p, providers_p);

							services_json_p = GetServicesListAsJSON (services_p, NULL, user_p, false, providers_p);

							if (services_json_p)
								{
									res_p = GetInitialisedResponseOnServer (req_p, SERVICES_NAME_S, services_json_p);

									if (!res_p)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__,  "Failed to create response for the services array");
											json_decref (services_json_p);
										}

								}		/* if (services_json_p) */

							FreeProvidersStateTable (providers_p);
						}		/* if (providers_p) */


				}		/* if (services_p -> ll_size > 0) */

			FreeLinkedList (services_p);
		}		/* if (services_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__,  "Failed to create services list");
		}

	return res_p;
}


#if IRODS_ENABLED == 1
static json_t *GetAllModifiedData (const json_t * const req_p, UserDetails *user_p)
{
	json_t *res_p = NULL;
	const char *username_s = NULL;
	const char *password_s = NULL;
	const char *token_s = NULL;
	const UserAuthentication *user_auth_p = GetUserAuthenticationForSystem (user_p, PROTOCOL_IRODS_S);


	if (user_auth_p)
		{
			const char *from_s = NULL;
			const char *to_s = NULL;
			/* "from" defaults to the start of time */
			time_t from = 0;

			/* "to" defaults to now */
			time_t to = time (NULL);

			json_t *group_p = json_object_get (req_p, "");

			if (group_p)
				{
					json_t *interval_p = json_object_get (group_p, "");

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


static LinkedList *GetServicesList (const char * const services_path_s, UserDetails *user_p, Resource *resource_p, Handler *handler_p, ProvidersStateTable *providers_p)
{
	LinkedList *services_p = AllocateLinkedList (FreeServiceNode);

	if (services_p)
		{
			LoadMatchingServices (services_p, services_path_s, resource_p, handler_p, user_p);

			if (services_p -> ll_size > 0)
				{
					if (AddServicesListToProvidersStateTable (providers_p, services_p))
						{
							AddAllPairedServices (services_p, user_p, providers_p);

							return services_p;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "AddServicesListToProvidersStateTable failed");
						}
				}

			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return NULL;
}



static json_t *GetServicesAsJSON (const char * const services_path_s, UserDetails *user_p, Resource *resource_p, Handler *handler_p, ProvidersStateTable *providers_p)
{
	json_t *json_p = NULL;
	LinkedList *services_p = GetServicesList (services_path_s, user_p, resource_p, handler_p, providers_p);

	if (services_p)
		{
			json_p = GetServicesListAsJSON (services_p, resource_p, user_p, false, providers_p);
			FreeLinkedList (services_p);
		}
	else
		{
			ServersManager *servers_manager_p = GetServersManager ();
			json_p = AddExternalServerOperationsToJSON (servers_manager_p, OP_LIST_ALL_SERVICES);
		}

	return json_p;
}


static json_t *RunKeywordServices (const json_t * const req_p, UserDetails *user_p, const char *keyword_s)
{
	json_t *res_p = NULL;
	json_t *results_p = json_array ();

	if (results_p)
		{
			res_p = GetInitialisedResponseOnServer (req_p, SERVICE_RESULTS_S, results_p);

			if (res_p)
				{
					Resource *resource_p = AllocateResource (PROTOCOL_TEXT_S, keyword_s, keyword_s);

					if (resource_p)
						{
							json_t *paired_servers_p = (req_p != NULL) ? json_object_get (req_p, SERVERS_S) : NULL;
							ProvidersStateTable *providers_p = AllocateProvidersStateTable (paired_servers_p);

							if (providers_p)
								{
									LinkedList *services_p = GetServicesList (SERVICES_PATH_S, user_p, resource_p, NULL, providers_p);

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

																	params_p = GetServiceParameters (service_p, NULL, user_p);

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
																					ServiceJobSet *jobs_set_p = RunService (service_p, params_p, user_p, providers_p);

																					if (jobs_set_p)
																						{
																							ProcessServiceJobSet (jobs_set_p, results_p);
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
																			json_t *interested_app_p = GetInterestedServiceJSON (service_name_s, keyword_s, params_p, true);

																			if (interested_app_p)
																				{
																					if (json_array_append_new (results_p, interested_app_p) != 0)
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

																}		/* if (RunServiceMatcher (matcher_p, service_p)) else */

															service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);
														}		/* while (service_node_p) */

													FreeServiceMatcher (matcher_p);
												}		/* if (matcher_p) */

											FreeLinkedList (services_p);
										}		/* if (services_p) */

									FreeProvidersStateTable (providers_p);
								}		/* if (providers_p) */

							if (user_p)
								{
									FreeUserDetails (user_p);
								}

							FreeResource (resource_p);
						}		/* if (resource_p) */

				}		/* if (res_p) */

		}		/* if (results_p) */

	return res_p;
}


static int32 AddPairedServices (Service *internal_service_p, UserDetails *user_p, ProvidersStateTable *providers_p)
{
	int32 num_added_services = 0;
	ServersManager *servers_manager_p = GetServersManager ();
	LinkedList *external_servers_p = GetAllExternalServersFromServersManager (servers_manager_p, DeserialiseExternalServerFromJSON);

	if (external_servers_p)
		{
			const SchemaVersion *sv_p = GetSchemaVersion ();
			const char *internal_service_name_s = GetServiceName (internal_service_p);
			ExternalServerNode *external_server_node_p = (ExternalServerNode *) (external_servers_p -> ll_head_p);

			while (external_server_node_p)
				{
					ExternalServer *external_server_p = external_server_node_p -> esn_server_p;

					/* If it has paired services try and match them up */
					if (external_server_p -> es_paired_services_p)
						{
							KeyValuePairNode *pairs_node_p = (KeyValuePairNode *) (external_server_p -> es_paired_services_p -> ll_head_p);

							while (pairs_node_p)
								{
									const char *external_service_name_s = pairs_node_p -> kvpn_pair_p -> kvp_key_s;

									if (strcmp (external_service_name_s, internal_service_name_s) == 0)
										{
											if (!IsServiceInProvidersStateTable (providers_p, external_server_p -> es_uri_s, external_service_name_s))
												{
													const char *op_name_s = pairs_node_p -> kvpn_pair_p -> kvp_value_s;
													json_t *req_p = GetAvailableServicesRequestForAllProviders (providers_p, user_p, sv_p);

													/* We don't need to loop after this iteration */
													pairs_node_p = NULL;

													if (req_p)
														{
															json_t *response_p = MakeRemoteJSONCallToExternalServer (external_server_p, req_p);

															if (response_p)
																{
																	json_t *services_p = json_object_get (response_p, SERVICES_NAME_S);

																	if (services_p)
																		{
																			/*
																			 * Get the required Service from the ExternalServer
																			 */
																			if (json_is_array (services_p))
																				{
																					const size_t size = json_array_size (services_p);
																					size_t i;

																					for (i = 0; i < size; ++ i)
																						{
																							json_t *service_response_p = json_array_get (services_p, i);
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
																											const json_t *provider_p = json_object_get (service_response_p, SERVER_PROVIDER_S);

																											if (CreateAndAddPairedService (internal_service_p, external_server_p, op_name_s, matching_external_op_p, provider_p))
																												{
																													++ num_added_services;

																													if (!AddToProvidersStateTable (providers_p, external_server_p -> es_uri_s, external_service_name_s))
																														{
																															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add external service %s:%s to providers table", external_server_p -> es_name_s, external_service_name_s);
																														}
																												}		/* if (CreateAndAddPairedService (matching_internal_service_p, external_server_p, matching_external_op_p)) */

																											i = size;		/* force exit from loop */
																										}		/* if (matching_external_op_p) */

																								}		/* if (external_operations_json_p) */

																						}		/* for (i = 0; i < size; ++ i) */

																				}		/* if (json_is_array (services_p)) */
																			else
																				{
																					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, services_p, "services is not a json array");
																				}

																		}		/* if (services_p) */
																	else
																		{
																			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, response_p, "Failed to get services from response");
																		}

																	json_decref (response_p);
																}		/* if (response_p) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get response from %s at %s", external_server_p -> es_name_s, external_server_p -> es_uri_s);
																}

															json_decref (req_p);
														}		/* if (req_p) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to build request for %s at %s", external_server_p -> es_name_s, external_server_p -> es_uri_s);
														}

												}		/* if (!IsServiceInProvidersStateTable (providers_p, external_server_p -> es_name_s, external_service_name_s)) */

										}		/* if (strcmp (service_name_s, internal_service_name_s) == 0) */

									if (pairs_node_p)
										{
											pairs_node_p = (KeyValuePairNode *) (pairs_node_p -> kvpn_node.ln_next_p);
										}

								}		/* while (pairs_node_p) */

						}		/* if (external_server_p -> es_paired_services_p) */

					external_server_node_p = (ExternalServerNode *) external_server_node_p -> esn_node.ln_next_p;
				}		/* (while (external_server_node_p) */

			FreeLinkedList (external_servers_p);
		}		/* if (external_servers_p) */

	return num_added_services;
}


static int32 AddAllPairedServices (LinkedList *internal_services_p, UserDetails *user_p, ProvidersStateTable *providers_p)
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
					int32 i = AddPairedServices (internal_service_node_p -> sn_service_p, user_p, providers_p);

					num_added_services += i;
					internal_service_node_p = (ServiceNode *) (internal_service_node_p -> sn_node.ln_next_p);
				}		/* while (internal_service_node_p) */

		}		/* if (internal_services_p) */

	return num_added_services;
}

