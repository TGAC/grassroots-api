
#include "json_tools.h"
#include "request_tools.h"
#include "server.h"

static const char * const S_CREDENTIALS_KEY = "credentials";
static const char * const S_USERNAME_KEY = "user";
static const char * const S_PASSWORD_KEY = "pass";


static bool AddKeyAndStringValue (json_t *json_p, const char * const key_s, const char * const value_s);

static bool AddIrodsCredentials (json_t *root_p, const char * const username_s, const char * const password_s);


int SendJsonRequest (int socket_fd, uint32 id, const json_t *json_p)
{
	int res = -1;
	const char *req_s = json_dumps (json_p, 0);
	
	if (req_s)
		{
			res = AtomicSendString (socket_fd, id, req_s);
			free (req_s);
		}
		
	return res;
}


/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetLoginJsonRequest (const char * const username_s, const char *password_s)
{
	json_error_t error;
	json_t *json_p = json_pack_ex (&error, 0, "{s:{s:{s:s,s:s}}}", KEY_IRODS, S_CREDENTIALS_KEY, S_USERNAME_KEY, username_s, S_PASSWORD_KEY, password_s);
	
	if (!json_p)
		{
			
		}
	
	return json_p;
}


static bool AddIrodsCredentials (json_t *root_p, const char * const username_s, const char * const password_s)
{
	bool success_flag = false;
	json_error_t error;
	json_t *irods_p = json_object ();

	if (irods_p)
		{
			json_t *credentials_p = json_pack_ex (&error, 0, "{s:s,s:s}", S_USERNAME_KEY, username_s, S_PASSWORD_KEY, password_s);

			if (credentials_p)
				{
					if (json_object_set_new (irods_p, S_CREDENTIALS_KEY, credentials_p) == 0)
						{
							if (json_object_set_new (root_p, KEY_IRODS, irods_p) == 0)
								{	
									success_flag = true;
								}								
						}
					else
						{
							json_decref (credentials_p);
						}
				}

			if (!success_flag)
				{
					json_object_clear (irods_p);
					json_decref (irods_p);
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
	json_error_t error;	
	json_t *root_p = json_pack_ex (&error, 0, "{s:i}", KEY_OPERATIONS, OP_IRODS_MODIFIED_DATA);


	if (root_p)
		{
			if (AddIrodsCredentials (root_p, username_s, password_s))
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
	json_error_t error;
	json_t *json_p = json_pack_ex (&error, 0, "{s:i, s:{s}", KEY_OPERATIONS, OP_LIST_ALL_SERVICES);

	if (!json_p)
		{
			
		}
	
	return json_p;
}



json_t *GetInterestedServicesRequest (const char * const username_s, const char * const password_s, json_t * const file_data_p)
{
	json_error_t error;
	json_t *root_p = json_pack_ex (&error, 0, "{s:i}", KEY_OPERATIONS, OP_LIST_INTERESTED_SERVICES);
	bool success_flag = false;
	
	if (root_p)
		{
			if (AddIrodsCredentials (root_p, username_s, password_s))
				{
					if (json_object_set (root_p, KEY_FILE_DATA, file_data_p) == 0)
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


bool GetIrodsUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss)
{
	bool success_flag = false;

	json_t *group_p = json_object_get (root_p, KEY_IRODS);
	
	if (group_p)
		{
			group_p = json_object_get (group_p, S_CREDENTIALS_KEY);
			
			if (group_p)
				{
					if ((*username_ss = GetJSONString (group_p, S_USERNAME_KEY)) != NULL)
						{
							if ((*password_ss = GetJSONString (group_p, S_PASSWORD_KEY)) != NULL)
								{
									success_flag = true;
								}
						}
				}
		}
	

	return success_flag;
}




