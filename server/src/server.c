
#include "server.h"
#include "linked_list.h"
#include "jansson.h"
#include "json_tools.h"
#include "json_util.h"
#include "service.h"
#include "time_util.h"

#include "query.h"
#include "connect.h"
#include "user.h"

#include "handler.h"
#include "handler_utils.h"

#include "parameter_set.h"

//#include "irods_handle.h"


#ifdef _DEBUG
	#define SERVER_DEBUG	(DL_INFO)
#else
	#define SERVER_DEBUG	(DL_NONE)
#endif


/*****************************/
/***** STATIC PROTOTYPES *****/
/*****************************/

#define SERVICES_PATH		("services")


static json_t *GetAllModifiedData (const json_t * const req_p);

static json_t *GetInterestedServices (const json_t * const req_p);

static json_t *GetAllServices (const json_t * const req_p);

static json_t *GetServices (const char * const services_path_s, const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p);

static json_t *ProcessJSONRequest (json_t *req_p);


/***************************/
/***** API DEFINITIONS *****/
/***************************/


json_t *ProcessMessage (const char * const request_s)
{
	json_error_t error;
	json_t *req_p = json_loads (request_s, JSON_PRESERVE_ORDER, &error);
	json_t *res_p = NULL;
	
	if (req_p)
		{
			if (json_is_array (req_p))
				{
					res_p = json_array ();
					
					if (res_p)
						{
							size_t i;
							json_t *value_p;
														
							json_array_foreach (req_p, i, value_p) 
								{
									json_t *json_p = ProcessJSONRequest (value_p);
									
									if (json_p)
										{
											if (json_array_append_new (res_p, json_p) != 0)
												{
													// error
													
												}
										}
									else
										{
											// error
										}
								}				
							
						}
					else
						{
							
						}
					
				}
			else
				{
					res_p = ProcessJSONRequest (req_p);

					if (!res_p)
						{
							// error
						}
				}
		}
	else
		{
			/* error decoding the request */
			
		}	
	
	
	return res_p;
}


/******************************/
/***** STATIC DEFINITIONS *****/
/******************************/

static json_t *ProcessJSONRequest (json_t *req_p)
{
	/* Get the requested operation */
	json_t *op_p = json_object_get (req_p, KEY_OPERATIONS);
	json_t *res_p = NULL;
	
	#if SERVER_DEBUG >= DL_INFO
	char *req_s = json_dumps (req_p, JSON_PRESERVE_ORDER | JSON_INDENT (2));
	#endif
	
	if (op_p)
		{
			if (json_is_integer (op_p))
				{
					json_int_t operation_id = json_integer_value (op_p);
					
					switch (operation_id)
						{
							case OP_LIST_ALL_SERVICES:
								res_p = GetAllServices (req_p);
								break;
							
							case OP_IRODS_MODIFIED_DATA:
								res_p = GetAllModifiedData (req_p);
								break;
						
							case OP_LIST_INTERESTED_SERVICES:
								res_p = GetInterestedServices (req_p);
								break;
								
							default:
								break;
						}
				}
		}
	else 
		{
			/*
			 * Is there a request to run a service?
			 */
			op_p = json_object_get (req_p, SERVICE_RUN_S);
			
			if (op_p && json_is_true (op_p))
				{
					const char *service_name_s = GetServiceNameFromJSON (req_p);
					
					if (service_name_s)
						{
							LinkedList *services_list_p = LoadMatchingServicesByName (SERVICES_PATH, service_name_s);
							
							if (services_list_p)
								{
									if (services_list_p -> ll_size == 1)
										{	
											Service *service_p = ((ServiceNode *) (services_list_p -> ll_head_p)) -> sn_service_p;
									
											if (service_p)
												{
													/* 
													 * Convert the json parameter set into a ParameterSet
													 * to run the Service with.
													 */
													ParameterSet *params_p = CreateParameterSetFromJSON (req_p);

													if (params_p)
														{
															int res = RunService (service_p, params_p);
														}		/* if (params_p) */

												}		/* if (service_p) */

										}		/* if (services_list_p -> ll_size == 1)) */
						
									FreeLinkedList (services_list_p);
								}		/* if (services_list_p) */
						
						}		/* if (service_name_s) */
				
				}		/* if (op_p && json_is_true (op_p)) */
				
		}	

	#if SERVER_DEBUG >= DL_INFO
	if (req_s)
		{
			free (req_s);
		}
	#endif
		
	return res_p;				
}


static json_t *GetInterestedServices (const json_t * const req_p)
{
	json_t *res_p = NULL;
	const char *username_s = NULL;
	const char *password_s = NULL;

	if (GetUsernameAndPassword (req_p, &username_s, &password_s))
		{
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
											
											Resource *resource_p = AllocateResource (protocol_s, data_name_s);

											if (resource_p)
												{
													json_t *credentials_p = json_object_get (req_p, CREDENTIALS_S);
													Handler *handler_p = GetResourceHandler (resource_p, credentials_p);

													if (handler_p)
														{
															json_t *config_p = NULL;
															res_p = GetServices (SERVICES_PATH, username_s, password_s, resource_p, handler_p, config_p);
															
															FreeHandler (handler_p);
														}
														
													FreeResource (resource_p);
												}
										}									
									
								}		/* if (json_is_string (protocol_p)) */
							
						}					
				}
		}
				
	
	return res_p;
}


static json_t *GetAllServices (const json_t * const req_p)
{
	json_t *res_p = NULL;
	const char *username_s = NULL;
	const char *password_s = NULL;
												
	if (GetUsernameAndPassword (req_p, &username_s, &password_s))
		{
			res_p = GetServices (SERVICES_PATH, username_s, password_s, NULL, NULL, NULL);
		}

	return res_p;
}


static json_t *GetAllModifiedData (const json_t * const req_p)
{
	json_t *res_p = NULL;
	const char *username_s = NULL;
	const char *password_s = NULL;
												
	if (GetUsernameAndPassword (req_p, &username_s, &password_s))
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
					if (!ConvertStringToEpochTime (from_s, &from))
						{
							// error
						}
				}
				
			if (to_s)
				{
					if (!ConvertStringToEpochTime (to_s, &to))
						{
							// error
						}
				}
			
			res_p = GetModifiedIRodsFiles (username_s, password_s, from, to);
		}
	
	return res_p;
}



static json_t *GetServices (const char * const services_path_s, const char * const username_s, const char * const password_s, Resource *resource_p, Handler *handler_p, const json_t *config_p)
{
	json_t *json_p = NULL;
	LinkedList *services_list_p = LoadMatchingServices (services_path_s, resource_p, handler_p);
	
	json_p = GetServicesListAsJSON (services_list_p, resource_p, config_p);

	if (services_list_p)
		{
			FreeLinkedList (services_list_p);
		}
	
	return json_p;
}


