
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




static json_t *GetAllModifiedData (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetInterestedServices (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetAllServices (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServices (const char * const services_path_s, const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p);

static int8 RunServiceFromJSON (const json_t *req_p, json_t *credentials_p, json_t *res_p, uuid_t user_uuid);

static json_t *RunKeywordServices (const json_t * const req_p, json_t *config_p, const char *keyword_s);

static Operation GetOperation (json_t *ops_p);

static json_t *GetNamedServices (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServiceStatus (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServiceResultsAsJSON (const json_t * const req_p, const json_t *credentials_p);

static json_t *GetServiceData (const json_t * const req_p, const json_t *credentials_p, bool (*callback_fn) (json_t *services_p, uuid_t service_id, const char *uuid_s));

static bool AddServiceStatusToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static bool AddServiceResultsToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static bool AddServiceCleanUpToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s);

static json_t *CleanUpJobs (const json_t * const req_p, const json_t *credentials_p);

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
			res_p = ProcessServerJSONMessage (req_p, socket_fd);
		}
	else
		{
			/* error decoding the request */
			
		}	
	

	
	return res_p;
}


json_t *ProcessServerJSONMessage (json_t *req_p, const int socket_fd)
{
	json_t *res_p = NULL;
	json_t *op_p = NULL;
	json_t *credentials_p = json_object_get (req_p, CREDENTIALS_S);
	json_t *uuid_p = NULL;

	if (!credentials_p)
		{
			credentials_p = json_object ();

			if (credentials_p)
				{
					if (json_object_set_new (req_p, CREDENTIALS_S, credentials_p) != 0)
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to add credentials json");
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to create credentials json");
				}
		}


	/* add a unique id if not already there */
	if (credentials_p)
		{
			uuid_p = json_object_get (credentials_p, CREDENTIALS_UUID_S);

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
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add uuid string to credentials");
								}

							FreeUUIDString (uuid_s);
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to get uuid string");
						}
				}
		}

	#if SERVER_DEBUG >= STM_LEVEL_FINE
		{
			if (req_p)
				{
					char *dump_s = json_dumps (req_p, JSON_INDENT (2));
					
					if (dump_s)
						{
							printf ("ProcessMessage - request: \n%s\n\n", dump_s);
							free (dump_s);
						}
				}
		}
	#endif


	/*
	 * Is this request for an external server?
	 */
	uuid_p = json_object_get (req_p, SERVER_UUID_S);
	if (uuid_p)
		{
			/*
			 * Find the matching external server,
			 * Remove the server uuid and proxy
			 * the request/response
			 */
			if (json_is_string (uuid_p))
				{
					const char *uuid_s = json_string_value (uuid_p);
					uuid_t key;

					if (ConvertStringToUUID (uuid_s, key))
						{
							ServersManager *manager_p = GetServersManager ();

							if (manager_p)
								{
									ExternalServer *external_server_p = GetExternalServerFromServersManager (manager_p, key);

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
			ServersManager *servers_manager_p = GetServersManager ();

			switch (op)
				{
					case OP_LIST_ALL_SERVICES:
						res_p = GetAllServices (req_p, credentials_p);
						break;
					
					case OP_IRODS_MODIFIED_DATA:
						res_p = GetAllModifiedData (req_p, credentials_p);
						break;
				
					case OP_LIST_INTERESTED_SERVICES:
						res_p = GetInterestedServices (req_p, credentials_p);
						break;
						
					case OP_RUN_KEYWORD_SERVICES:
						{
							json_t *keyword_json_group_p = json_object_get (req_p, KEY_QUERY);

							if (keyword_json_group_p)
								{
									json_t *keyword_json_value_p = json_object_get (keyword_json_group_p, KEY_QUERY);

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

					default:
						break;
				}		/* switch (op) */
				

//			res_p = AddExternalServerOperationsToJSON (servers_manager_p, res_p, op_p);

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
													PrintErrors (STM_LEVEL_SEVERE, "Failed to run service from %s", value_s);
													free (value_s);
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to run service from");
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
											PrintErrors (STM_LEVEL_SEVERE, "Failed to run service from %s", value_s);
											free (value_s);
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, "Failed to run service from");
										}
								}
						}

				}		/* if (res_p) */

		}		/* 	else if ((op_p = json_object_get (req_p, SERVICES_NAME_S)) != NULL) */

		
	#if SERVER_DEBUG >= STM_LEVEL_FINE
		{
			if (res_p)
				{
					char *dump_s = json_dumps (res_p, JSON_INDENT (2));
					
					if (dump_s)
						{
							printf ("ProcessMessage - response: \n%s\n\n", dump_s);
							free (dump_s);
						}
				}
		}
	#endif
	
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
	
	#if SERVER_DEBUG >= STM_LEVEL_INFO
	char *req_s = json_dumps (req_p, JSON_PRESERVE_ORDER | JSON_INDENT (2));
	printf ("RunServiceFromJSON:\n%s\n", req_s);
	#endif
	
	if (op_p && json_is_true (op_p))
		{
			const char *service_name_s = GetServiceNameFromJSON (req_p);
			
			if (service_name_s)
				{
					LinkedList *services_p = AllocateLinkedList  (FreeServiceNode);
					
					if (services_p)
						{
							LoadMatchingServicesByName (services_p, SERVICES_PATH_S, service_name_s, credentials_p);

							#if SERVER_DEBUG >= STM_LEVEL_FINE
								{
									ServiceNode * node_p = (ServiceNode *) (services_p -> ll_head_p);

									while (node_p)
										{
											Service *service_p = node_p -> sn_service_p;
											const char *name_s = GetServiceName (service_p);

											printf ("matched service \"%s\"\n", name_s);

											node_p = (ServiceNode *) (node_p -> sn_node.ln_next_p);
										}
								}
							#endif

							if (services_p -> ll_size == 1)
								{	
									ServiceNode *node_p = (ServiceNode *) (services_p -> ll_head_p);
									Service *service_p =  node_p -> sn_service_p;
							
									if (service_p)
										{
											/* 
											 * Convert the json parameter set into a ParameterSet
											 * to run the Service with.
											 */
											ParameterSet *params_p = CreateParameterSetFromJSON (req_p);

											if (params_p)
												{
													ServiceJobSet *jobs_p = RunService (service_p, params_p, credentials_p);
													bool keep_service_flag = false;

													if (jobs_p)
														{
															const size_t num_jobs = jobs_p -> sjs_num_jobs;
															size_t i;
															ServiceJob *job_p = jobs_p -> sjs_jobs_p;
															JobsManager *manager_p = GetJobsManager ();

															res = 1;

															for (i = 0; i < num_jobs; ++ i, ++ job_p)
																{
																	json_t *job_json_p = NULL;

																	if (job_p -> sj_status == OS_SUCCEEDED)
																		{
																			/* add the result directly */
																			job_json_p = GetServiceJobAsJSON (job_p);
																		}
																	else
																		{
																			job_json_p = GetServiceJobStatusAsJSON (job_p);
																		}

																	if (job_json_p)
																		{
																			if (json_array_append_new (res_p, job_json_p) != 0)
																				{

																				}
																		}

																	if (job_p -> sj_status == OS_STARTED || job_p -> sj_status == OS_SUCCEEDED)
																		{
																			keep_service_flag = true;

																			if (!AddServiceJobToJobsManager (manager_p, job_p -> sj_id, job_p))
																				{

																				}

																		}
																}

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

														}

												}		/* if (params_p) */

										}		/* if (service_p) */

								}		/* if (services_p -> ll_size == 1)) */
				
							FreeLinkedList (services_p);
						}		/* if (services_p) */
				
				}		/* if (service_name_s) */
		
		}		/* if (op_p && json_is_true (op_p)) */

	#if SERVER_DEBUG >= STM_LEVEL_INFO
	if (req_s)
		{
			free (req_s);
		}
	#endif
		
	return res;
}


static Operation GetOperation (json_t *ops_p)
{
	Operation op = OP_NONE;
	json_t *op_p = json_object_get (ops_p, OPERATION_ID_S);
		
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
			json_t *credentials_p = json_object_get (req_p, CREDENTIALS_S);
			Handler *handler_p = GetResourceHandler (resource_p, credentials_p);

			if (handler_p)
				{
					json_t *config_p = NULL;
					const char *username_s = NULL;
					const char *password_s = NULL;
												
					if (GetUsernameAndPassword (credentials_p, &username_s, &password_s))
						{
							res_p = GetServices (SERVICES_PATH_S, username_s, password_s, resource_p, handler_p, config_p);
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
	res_p = GetServices (SERVICES_PATH_S, username_s, password_s, NULL, NULL, credentials_p);

	return res_p;
}


static bool AddServiceStatusToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s)
{
	bool success_flag = false;
	json_t *status_p = json_object ();

	if (status_p)
		{
			if (json_object_set_new (status_p, SERVICE_UUID_S, json_string (uuid_s)) == 0)
				{
					JobsManager *manager_p = GetJobsManager ();
					ServiceJob *job_p = GetServiceJobFromJobsManager (manager_p, service_id);

					if (job_p)
						{
							OperationStatus status = GetServiceJobStatus (job_p);
							const char *service_name_s = GetServiceName (job_p -> sj_service_p);

							success_flag = true;

							if (json_object_set_new (status_p, SERVICE_NAME_S, json_string (service_name_s)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add service name %s to status json", service_name_s);
									json_object_set_new (status_p, ERROR_S, json_string ("Failed to add service name to status json"));
									success_flag = false;
								}

							if (json_object_set_new (status_p, SERVICE_STATUS_S, json_integer (status)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add service status for name %s to status json", service_name_s);
									json_object_set_new (status_p, ERROR_S, json_string ("Failed to add service status to status json"));
									success_flag = false;
								}

						}		/* if (service_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to find %s in services table", uuid_s);
							json_object_set_new (status_p, ERROR_S, json_string ("Failed to fine uuid in services table"));
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to add service uuid_s %s to status json", uuid_s);
					json_object_set_new (status_p, ERROR_S, json_string ("Failed to add uuid to status json"));
				}

			success_flag = (json_array_append_new (services_p, status_p ) == 0);
		}		/* if (status_p) */

	return success_flag;
}


static bool AddServiceResultsToJSON (json_t *results_p, uuid_t job_id, const char *uuid_s)
{
	bool success_flag = false;
	JobsManager *manager_p = GetJobsManager ();
	ServiceJob *job_p = GetServiceJobFromJobsManager (manager_p, job_id);
	json_t *service_result_p = NULL;

	if (job_p)
		{
			const OperationStatus status = GetServiceJobStatus (job_p);
			const char *service_name_s = GetServiceName (job_p -> sj_service_p);

			/* Check that the service has finished */
			if (status == OS_SUCCEEDED || status == OS_FINISHED)
				{
					service_result_p = GetServiceResults (job_p -> sj_service_p, job_id);
					success_flag = true;
				}		/* if (status == OS_SUCCEEDED || status == OS_FINISHED) */
			else
				{
					service_result_p = json_pack ("{s:s,s:s,s:i}", SERVICE_NAME_S, service_name_s, SERVICE_UUID_S, uuid_s, SERVICE_STATUS_S, status);
				}

			if (service_result_p)
				{
					if (json_is_array (service_result_p))
						{
							size_t i;
							json_t *result_item_p;

							json_array_foreach (service_result_p, i, result_item_p)
								{
									if (json_array_append_new (results_p, result_item_p) != 0)
										{
											json_object_clear (result_item_p);
											json_decref (result_item_p);
											PrintErrors (STM_LEVEL_SEVERE, "Failed to add service result for %s %s", service_name_s, uuid_s);
										}


								}		/* json_array_foreach (service_result_p, i, result_item_p) */

						}
					else if (json_is_object (service_result_p))
						{
							if (json_array_append_new (results_p, service_result_p) != 0)
								{
									json_object_clear (service_result_p);
									json_decref (service_result_p);
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add service result for %s %s", service_name_s, uuid_s);
								}
						}
				}
		}		/* if (service_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to find %s in services table", uuid_s);

			service_result_p = json_pack ("{s:s,s:s}", SERVICE_UUID_S, uuid_s, ERROR_S, "Failed to fine uuid in services table");
		}

	return success_flag;
}


static json_t *GetServiceData (const json_t * const req_p, const json_t *credentials_p, bool (*callback_fn) (json_t *services_p, uuid_t service_id, const char *uuid_s))
{
	json_t *res_p = json_object ();

	if (res_p)
		{
			json_t *res_services_p = json_array ();

			if (res_services_p)
				{
					if (json_object_set (res_p, SERVICES_NAME_S, res_services_p) == 0)
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
																	if (callback_fn (res_services_p, service_id, uuid_s))
																		{
																			++ num_successes;
																		}
																}		/* if (ConvertStringToUUID (uuid_s, service_id)) */

														}		/* if (json_is_string (service_uuid_json_p)) */

												}		/* json_array_foreach (service_uuids_json_p, i, service_uuid_json_p) */


											if (num_uuids == num_successes)
												{
													//CloseService (job_p -> sj_service_p);
												}

										}
									else
										{

										}

								}		/* if (service_uuids_json_p) */
						}
					else
						{
							const char error_s [] = "Failed to add services array to services status json";

							PrintErrors (STM_LEVEL_SEVERE, error_s);
							json_object_set_new (res_p, ERROR_S, json_string (error_s));
						}

				}		/* if (res_services_p) */
			else
				{
					const char error_s [] = "Failed to get services array to services status json";

					PrintErrors (STM_LEVEL_SEVERE, error_s);
					json_object_set_new (res_p, ERROR_S, json_string (error_s));
				}

		}		/* if (res_p) */

	return res_p;

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


static bool AddServiceCleanUpToJSON (json_t *services_p, uuid_t service_id, const char *uuid_s)
{
	bool success_flag = false;
	json_t *status_p = json_object ();

	if (status_p)
		{
			if (json_object_set_new (status_p, SERVICE_UUID_S, json_string (uuid_s)) == 0)
				{
					JobsManager *manager_p = GetJobsManager ();
					ServiceJob *job_p = RemoveServiceJobFromJobsManager (manager_p, service_id);

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
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add service name %s to status json", service_name_s);
									json_object_set_new (status_p, ERROR_S, json_string ("Failed to add service name to status json"));
									success_flag = false;
								}

							if (json_object_set_new (status_p, SERVICE_STATUS_S, json_integer (status)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add service status for name %s to status json", service_name_s);
									json_object_set_new (status_p, ERROR_S, json_string ("Failed to add service status to status json"));
									success_flag = false;
								}

						}		/* if (service_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to find %s in services table", uuid_s);
							json_object_set_new (status_p, ERROR_S, json_string ("Failed to fine uuid in services table"));
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to add service uuid_s %s to status json", uuid_s);
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
					res_p = GetServicesListAsJSON (services_p, NULL, credentials_p, false);
				}

			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return res_p;
}


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



//static json_t GetRemoteServices (const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p)



static json_t *GetServices (const char * const services_path_s, const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p)
{
	json_t *json_p = NULL;
	
	LinkedList *services_p = AllocateLinkedList (FreeServiceNode);
	
	if (services_p)
		{
			LoadMatchingServices (services_p, services_path_s, resource_p, handler_p, config_p);

			if (services_p -> ll_size > 0)
				{
					json_p = GetServicesListAsJSON (services_p, resource_p, config_p, false);
				}
				
			FreeLinkedList (services_p);
		}		/* if (services_p) */
		
	return json_p;
}


static json_t *RunKeywordServices (const json_t * const req_p, json_t *config_p, const char *keyword_s)
{
	json_t *res_p = NULL;

	LinkedList *services_p = AllocateLinkedList (FreeServiceNode);

	if (services_p)
		{
			LoadKeywordServices (services_p, SERVICES_PATH_S, config_p);

			if (services_p -> ll_size > 0)
				{
					res_p = json_array ();

					if (res_p)
						{
							/* For each service, set its keyword parameter */
							ServiceNode *service_node_p = (ServiceNode *) (services_p -> ll_head_p);

							while (service_node_p)
								{
									ParameterSet *params_p = NULL;
									Service *service_p = service_node_p -> sn_service_p;
									json_t *service_res_p = NULL;
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
															if (SetParameterValue (param_p, keyword_s))
																{
																	param_flag = true;
																}
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, "Failed to set service param \"%s\" - \"%s\" to \"%s\"", GetServiceName (service_p), param_p -> pa_name_s, keyword_s);
																	param_flag = false;
																}
														}

													param_node_p = (ParameterNode *) (param_node_p -> pn_node.ln_next_p);
												}		/* while (param_node_p) */

											/* Now run the service */
											if (param_flag)
												{
													ServiceJobSet *jobs_set_p = RunService (service_p, params_p, config_p);
												}

											ReleaseServiceParameters (service_p, params_p);

										}		/* if (params_p) */

									if (service_res_p)
										{
											if (json_array_append_new (res_p, service_res_p) != 0)
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to add results from service \"%s\" to results list", GetServiceName (service_p));
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, "Failed to run service \"%s\" with keyword \"%s\"", GetServiceName (service_p), keyword_s);
										}

									service_node_p = (ServiceNode *) (service_node_p -> sn_node.ln_next_p);

								}		/* while (service_node_p) */

						}		/* if (res_p) */

				}		/* if (services_p -> ll_size > 0) */

			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return res_p;
}

