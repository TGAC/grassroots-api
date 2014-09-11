#include "connect.h"


rcComm_t *CreateConnection (char *username_s, char *password_s)
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
			
			connection_p = rcConnect (env.rodsHost, env.rodsPort, username_s, env.rodsZone, 0, &err);
			
			if (connection_p)
				{
					status = password_s ? clientLoginWithPassword (connection_p, password_s) : clientLogin (connection_p);
					
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

                                                                                                      
