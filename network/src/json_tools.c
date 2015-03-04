
#include "json_tools.h"
#include "request_tools.h"
#include "server.h"
#include "json_util.h"
#include "streams.h"


#ifdef _DEBUG
	#define JSON_TOOLS_DEBUG	(STM_LEVEL_FINE)
#else
	#define JSON_TOOLS_DEBUG	(STM_LEVEL_NONE)
#endif


json_t *LoadConfig (const char *path_s);


static bool AddKeyAndStringValue (json_t *json_p, const char * const key_s, const char * const value_s);

int SendJsonRequest (int socket_fd, uint32 id, const json_t *json_p)
{
	int res = -1;
	char *req_s = json_dumps (json_p, 0);
	
	if (req_s)
		{
			res = AtomicSendString (socket_fd, id, req_s);
			free (req_s);
		}
		
	return res;
}



/*
 {
	credentials:
		{
			dropbox:
				{
					token_key:    onr78fxbbne0gzfy,
					token_secret: bnr6bfxwgy995su
				}
		}
}
* */

bool AddCredentialsToJson (json_t *root_p, const char * const username_s, const char * const password_s)
{
	bool success_flag = false;
	json_error_t error;
	json_t *credentials_p = json_pack_ex (&error, 0, "{s:s,s:s}", CREDENTIALS_USERNAME_S, username_s, CREDENTIALS_PASSWORD_S, password_s);

	if (credentials_p)
		{
			json_t *config_p = LoadConfig (".wheatis");
			
			if (config_p)
				{
					json_t *loaded_credentials_p = json_object_get (config_p, CREDENTIALS_S);
					
					if (loaded_credentials_p)
						{
							const char *key_s;
							json_t *value_p;

							json_object_foreach (loaded_credentials_p, key_s, value_p) 
								{
									json_object_set (credentials_p, key_s, value_p);
									json_object_del (config_p, key_s);
								}
						}		
						
					json_decref (config_p);
				}
			
			
			if (json_object_set_new (root_p, CREDENTIALS_S, credentials_p) == 0)
				{
					success_flag = true;
				}								
			else
				{
					json_decref (credentials_p);
				}
		}


	#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
		{
			char *value_s = json_dumps (root_p, JSON_INDENT (2));
			free (value_s);
		}
	#endif

	return success_flag;
}


json_t *LoadConfig (const char *path_s)
{
	bool success_flag = true;
	json_error_t error;
	json_t *config_json_p = json_load_file (path_s, 0, &error);	

	#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
	char *value_s = json_dumps (config_json_p, JSON_INDENT (2));
	#endif


	if (!config_json_p)
		{
			success_flag = false;
		}		/* if (config_json_p) */

	#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
	free (value_s);
	#endif


	return config_json_p;
}



/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetModifiedFilesRequest (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s)
{
	bool success_flag = false;
	json_t *root_p = GetOperationAsJSON (OP_IRODS_MODIFIED_DATA);

	if (root_p)
		{
			if (AddCredentialsToJson (root_p, username_s, password_s))
				{
					json_t *interval_p = json_object ();
					
					if (interval_p)
						{
							json_t *irods_p = json_object_get (root_p, KEY_IRODS);
							
							if (json_object_set_new (irods_p, "interval", interval_p) == 0)
								{
									success_flag = true;
									
									if (from_s)
										{
											success_flag = AddKeyAndStringValue (interval_p, "from", from_s);
										}

									if (success_flag && to_s)
										{
											success_flag = AddKeyAndStringValue (interval_p, "to", to_s);
										}

								}	
							else
								{
									json_decref (interval_p);
								}
						}
				}
			
			if (!success_flag)
				{
					json_object_clear (root_p);
					json_decref (root_p);
					root_p = NULL;
				}
		}		/* if (root_p) */
		
	return root_p;
}


static bool AddKeyAndStringValue (json_t *json_p, const char * const key_s, const char * const value_s)
{
	bool success_flag = false;
	json_t *value_p = json_string (value_s);
	
	if (value_p)
		{
			if (json_object_set_new (json_p, key_s, value_p) == 0)
				{
					success_flag = true;
				}
			else
				{
					json_decref (value_p);
				}
		}

	return success_flag;
}




/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetAvailableServicesRequest (const char * const username_s, const char * const password_s)
{	
	json_t *json_p = GetOperationAsJSON (OP_LIST_ALL_SERVICES);
	
	return json_p;
}


json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, const char * const protocol_s, const char * const filename_s)
{

	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "{s:s, s:s}", KEY_PROTOCOL, protocol_s, KEY_FILENAME, filename_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (username_s, password_s, OP_LIST_INTERESTED_SERVICES, KEY_FILE_DATA, op_data_p);
		}
	else
		{

		}

	return res_p;
}


json_t *GetKeywordServicesRequest (const char * const username_s, const char * const password_s, const char * const keyword_s)
{
	json_t *res_p = NULL;
	json_error_t error;
	json_t *op_data_p = json_pack_ex (&error, 0, "{s:s}", KEY_QUERY, keyword_s);

	if (op_data_p)
		{
			res_p = GetServicesRequest (username_s, password_s, OP_RUN_KEYWORD_SERVICES, KEY_QUERY, op_data_p);
		}
	else
		{

		}

	return res_p;
}


json_t *GetServicesRequest (const char * const username_s, const char * const password_s, const Operation op, const char * const op_key_s, json_t * const op_data_p)
{
	json_t *root_p = GetOperationAsJSON (op);
	bool success_flag = false;

	if (root_p)
		{
			if (AddCredentialsToJson (root_p, username_s, password_s))
				{
					if (json_object_set (root_p, op_key_s, op_data_p) == 0)
						{
							success_flag = true;
						}
				}
		}

	if (!success_flag)
		{
			json_object_clear (root_p);
			json_decref (root_p);
			root_p = NULL;
		}

	return root_p;
}


bool GetUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss)
{
	bool success_flag = false;
	char *dump_s = json_dumps (root_p, 0);

	/* 
	 * Take care of whether we have been passed the credentials group
	 * or its parent.
	 */
	const json_t *group_p = json_object_get (root_p, CREDENTIALS_S);
	if (!group_p)
		{
			group_p = root_p;
		}

	if ((*username_ss = GetJSONString (group_p, CREDENTIALS_USERNAME_S)) != NULL)
		{
			if ((*password_ss = GetJSONString (group_p, CREDENTIALS_PASSWORD_S)) != NULL)
				{
					success_flag = true;
				}
		}	

	free (dump_s);

	return success_flag;
}


json_t *GetOperationAsJSON (Operation op)
{
	json_error_t error;
	json_t *json_p = json_pack_ex (&error, 0, "{s:{s:i}}", SERVER_OPERATIONS_S, OPERATION_ID_S, op);

	if (!json_p)
		{
			PrintErrors (STM_LEVEL_WARNING, "Failed to get Operation %d as JSON, %s\n", op, error.text);
		}
	
	return json_p;
}


