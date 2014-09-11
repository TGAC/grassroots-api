
#include "json_tools.h"
#include "request_tools.h"
#include "server.h"

static const char * const S_IRODS_KEY = "irods";
static const char * const S_CREDENTIALS_KEY = "credentials";
static const char * const S_USERNAME_KEY = "user";
static const char * const S_PASSWORD_KEY = "pass";


static bool FillInJSONString (const json_t *json_p, const char * const key_s, const char **value_ss);
static bool GetIrodsUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss);


int SendJsonRequest (int socket_fd, uint32 id, json_t *json_p)
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
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetLoginJsonRequest (const char * const username_s, const char *password_s)
{
	json_error_t error;
	json_t *json_p = json_pack_ex (&error, 0, "{s:{s:{s:s,s:s}}}", S_IRODS_KEY, S_CREDENTIALS_KEY, S_USERNAME_KEY, username_s, S_PASSWORD_KEY, password_s);
	
	if (!json_p)
		{
			
		}
	
	return json_p;
}

/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetModifiedFilesRequest (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s)
{
	json_t *json_p = json_pack ("s{s{ssss}s{ssss}}", S_IRODS_KEY, S_CREDENTIALS_KEY, S_USERNAME_KEY, username_s, S_PASSWORD_KEY, password_s, "interval", "from", from_s, "to", to_s);
	
	return json_p;
}


/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetAvailableServicesRequest (const char * const username_s, const char * const password_s)
{
	json_t *json_p = json_pack ("{s:i}", KEY_OPERATIONS, OP_LIST_SERVICES);
	return json_p;
}



bool GetIrodsUsernameAndPassword (const json_t * const root_p, const char **username_ss, const char **password_ss)
{
	bool success_flag = false;

	json_t *group_p = json_object_get (root_p, S_IRODS_KEY);
	
	if (group_p)
		{
			group_p = json_object_get (group_p, S_CREDENTIALS_KEY);
			
			if (group_p)
				{
					if ((*username_ss = FillInJSONString (group_p, S_USERNAME_KEY)) != NULL)
						{
							if ((*password_ss = FillInJSONString (group_p, S_PASSWORD_KEY)) != NULL)
								{
									success_flag = true;
								}
						}
				}
		}
	

	return success_flag;
}


const char *GetJSONString (const json_t *json_p, const char * const key_s)
{
	const char *value_s = NULL;
	json_t *value_p = json_object_get (json_p, key_s);

	if (value_p)
		{
			if (json_is_string (value_p))
				{
					value_s = json_string_value (value_p);
				}
		}
	
	return value_s;
}

