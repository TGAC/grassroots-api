
#include "server.h"
#include "services_manager.h"
#include "linked_list.h"
#include "jansson.h"
#include "json_tools.h"
#include "service.h"
#include "time_util.h"

static json_t *GetServices (const char * const path_s, const char * const username_s, const char * const password_s);


json_t *ProcessMessage (const char * const request_s)
{
	json_error_t error;
	json_t *req_p = json_loads (request_s, 0, &error);
	json_t *res_p = NULL;
	
	if (req_p)
		{
			/* Get the requested operation */
			json_t *op_p = json_object_get (req_p, KEY_OPERATIONS);
			
			if (op_p)
				{
					if (json_is_integer (op_p))
						{
							json_int_t operation_id = json_integer_value (op_p);
							
							switch (operation_id)
								{
									case OP_LIST_SERVICES:
										{
											res_p = GetServices ("services", "username", "password");
											
										}
										break;
									
									case OP_IRODS_MODIFIED_DATA:
										{
											const char *username_s = NULL;
											const char *password_s = NULL;
											
											if (GetIrodsUsernameAndPassword (op_p, &username_s, &password_s))
												{
													/* "from" defaults to the start of time */
													time_t from = 0;

													/* "to" defaults to now */
													time_t to = time (NULL);
													
													const char *date_s = GetJSONString (op_p, "from");
													if (date_s)
														{
															if (!ConvertStringToEpochTime (date_s, &from))
																{
																	// error
																}
														}

													date_s = GetJSONString (op_p, "to");
													if (date_s)
														{
															if (!ConvertStringToEpochTime (date_s, &to))
																{
																	// error
																}
														}
													
													
													
												}
										}
										break;
									
									default:
										break;
								}
						}
				}
		}
	else
		{
			/* error decoding the request */
			
		}
	
	
	return res_p;
}






static json_t *GetModifiedFiles  (const char * const username_s, const char * const password_s, const time_t from, const time_t to)
{
	json_t *json_p = NULL;
	rcComm_t *connection_p = CreateConnection (username, password_s);
	
	if (connection_p)
		{
			QueryResults *qr_p = GetAllModifiedDataForUsername (connection_p, username_s, from, to);
			
			if (qr_p)
				{
					
					FreeQueryResults (qr_p);
				}
			
			CloseConnection (connection_p);
		}
	
	return json_p;
}



static json_t *GetServices (const char * const path_s, const char * const username_s, const char * const password_s)
{
	json_t *json_p = NULL;
	LinkedList *services_list_p = LoadMatchingServices (path_s, NULL);
	
	if (services_list_p)
		{
			json_p = GetServicesListAsJSON (services_list_p);
			FreeLinkedList (services_list_p);
		}
	
	return json_p;
}


IRODS_UTIL_API QueryResults *GetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from, const time_t to);


