#include "connect.h"


rcComm_t *CreateConnection (void)
{
	rodsEnv env;
	rErrMsg_t err;
	rcComm_t *connection_p = NULL;
	int status = getRodsEnv (&env);

	if (status == 0) 
		{
			connection_p = rcConnect (env.rodsHost, env.rodsPort, env.rodsUserName, env.rodsZone, 0, &err);
			
			if (connection_p)
				{
					status = clientLogin (connection_p);
					
					if (status != 0)
						{
							rcDisconnect (connection_p);
						}
				}
		}
		
	return connection_p;
}


void CloseConnection (rcComm_t *connection_p)
{
	rcDisconnect (connection_p);
}

