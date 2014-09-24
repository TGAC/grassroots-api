
#include "server.h"
#include "services_manager.h"
#include "linked_list.h"
#include "jansson.h"
#include "json_tools.h"
#include "json_util.h"
#include "service.h"
#include "time_util.h"

#include "query.h"
#include "connect.h"
#include "user.h"

#include "irods_stream.h"


/*****************************/
/***** STATIC PROTOTYPES *****/
/*****************************/

#define SERVICES_PATH		("services")


static json_t *GetAllModifiedData (const json_t * const req_p);

static json_t *GetInterestedServices (const json_t * const req_p);

static json_t *GetAllServices (const json_t * const req_p);

static json_t *GetServices (const char * const services_path_s, const char * const username_s, const char * const password_s, const char * const filename_s, Stream *stream_p);


/***************************/
/***** API DEFINITIONS *****/
/***************************/


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


static json_t *GetInterestedServices (const json_t * const req_p)
{
	json_t *res_p = NULL;
	const char *username_s = NULL;
	const char *password_s = NULL;
												
	if (GetIrodsUsernameAndPassword (req_p, &username_s, &password_s))
		{
			json_t *file_data_p = json_object_get (req_p, KEY_FILE_DATA);
			
			if (file_data_p)
				{
					/* is it an irods file object? */
					json_t *group_p = json_object_get (file_data_p, KEY_IRODS);

					if (group_p)
						{
							/* is it a single file or a dir? */
							json_t *data_name_p = json_object_get (group_p, KEY_FILENAME);
					
							if (data_name_p)
								{
									if (json_is_string (data_name_p))
										{
											const char *data_name_s = json_string_value (data_name_p);
											Stream *stream_p = GetIRodsStream (username_s, password_s);
											
											if (stream_p)
												{
													res_p = GetServices (SERVICES_PATH, username_s, password_s, data_name_s, stream_p);
													FreeIRodsStream (stream_p);
												}
										}
								}									
																							
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
												
	if (GetIrodsUsernameAndPassword (req_p, &username_s, &password_s))
		{
			res_p = GetServices (SERVICES_PATH, username_s, password_s, NULL, NULL);
		}

	return res_p;
}


static json_t *GetAllModifiedData (const json_t * const req_p)
{
	json_t *res_p = NULL;
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
	
	return res_p;
}



static json_t *GetServices (const char * const services_path_s, const char * const username_s, const char * const password_s, const char * const filename_s, Stream *stream_p)
{
	json_t *json_p = NULL;
	LinkedList *services_list_p = LoadMatchingServices (services_path_s, filename_s, stream_p);
	
	json_p = GetServicesListAsJSON (services_list_p);

	if (services_list_p)
		{
			FreeLinkedList (services_list_p);
		}
	
	return json_p;
}


