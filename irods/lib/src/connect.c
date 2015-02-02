#include "connect.h"
#include "json_util.h"


rcComm_t *CreateConnectionFromJSON (const json_t *credentials_json_p)
{
	rcComm_t *connection_p = NULL;
	const char *provider_s = GetJSONString (credentials_json_p, CREDENTIALS_NAME_S);

	if (provider_s)
		{
			if (strcmp ("irods", provider_s) == 0)
				{
					const char *username_s = GetJSONString (credentials_json_p, CREDENTIALS_USERNAME_S);

					if (username_s)
						{
							const char *password_s = GetJSONString (credentials_json_p, CREDENTIALS_PASSWORD_S);

							if (password_s)
								{
									connection_p = CreateConnection (username_s, password_s);
								}
						}
				}
		}

	return connection_p;
}


rcComm_t *CreateConnection (const char *username_s, const char *password_s)
{
	rodsEnv env;
	rErrMsg_t err;
	rcComm_t *connection_p = NULL;
	int status = getRodsEnv (&env);

	if (status == 0) 
		{
			if (!username_s)
				{
					username_s = env.rodsUserName;
				}
			
			connection_p = rcConnect (env.rodsHost, env.rodsPort, (char *) username_s, env.rodsZone, 0, &err);
			
			if (connection_p)
				{
					status = password_s ? clientLoginWithPassword (connection_p, (char *) password_s) : clientLogin (connection_p);
					
					if (status != 0)
						{
							rcDisconnect (connection_p);
							connection_p = NULL;
						}
				}
		}
		
	return connection_p;
}


void CloseConnection (rcComm_t *connection_p)
{
	rcDisconnect (connection_p);
}

                                                                                                      
