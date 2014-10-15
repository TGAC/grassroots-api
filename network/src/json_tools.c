
#include "json_tools.h"
#include "request_tools.h"
#include "server.h"
#include "json_util.h"




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



bool AddCredentialsToJson (json_t *root_p, const char * const username_s, const char * const password_s)
{
	bool success_flag = false;
	json_error_t error;
	json_t *credentials_p = json_pack_ex (&error, 0, "{s:s,s:s}", CREDENTIALS_USERNAME_S, username_s, CREDENTIALS_PASSWORD_S, password_s);

	if (credentials_p)
		{
			if (json_object_set_new (root_p, CREDENTIALS_S, credentials_p) == 0)
				{
					success_flag = true;
				}								
			else
				{
					json_decref (credentials_p);
				}
		}

	return success_flag;
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



json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, json_t * const protocol_p)
{
	json_t *root_p = GetOperationAsJSON (OP_LIST_INTERESTED_SERVICES);
	bool success_flag = false;
	
	if (root_p)
		{
			if (AddCredentialsToJson (root_p, username_s, password_s))
				{
					if (json_object_set (root_p, KEY_FILE_DATA, protocol_p) == 0)
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
	json_t *json_p = json_pack_ex (&error, 0, "{s:{s:i}}", SERVER_OPERATIONS_S, SERVER_OPERATION_S, op);

	if (!json_p)
		{
			
		}
	
	return json_p;
}



