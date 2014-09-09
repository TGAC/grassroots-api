
#include "json_tools.h"
#include "request_tools.h"


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
json_t *GetLoginJson (const char * const username_s, const char *password_s)
{
	json_error_t error;
	json_t *json_p = json_pack_ex (&error, 0, "{s:{s:{s:s,s:s}}}", "irods", "credentials", "user", username_s, "password", password_s);
	
	if (!json_p)
		{
			
		}
	
	return json_p;
}

/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetModifiedFiles (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s)
{
	json_t *json_p = json_pack ("s{s{ssss}s{ssss}}", "irods", "credentials", "user", username_s, "password", password_s, "interval", "from", from_s, "to", to_s);
	
	return json_p;
}


/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetAvailableServices (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s)
{
	json_t *json_p = json_pack ("s", "services");
	return json_p;
}

