
#include "server.h"
#include "services_manager.h"
#include "linked_list.h"
#include "jansson.h"
#include "json_tools.h"
#include "service.h"
#include "time_util.h"

#include "query.h"
#include "connect.h"
#include "user.h"


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
											
											
											if (GetIrodsUsernameAndPassword (req_p, &username_s, &password_s))
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


