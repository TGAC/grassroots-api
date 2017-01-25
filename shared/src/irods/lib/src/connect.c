/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include "rcConnect.h"

#include "connect.h"
#include "json_util.h"
#include "streams.h"
#include "irods_connection.h"



#ifdef _DEBUG
	#define CONNECT_DEBUG	(STM_LEVEL_FINE)
#else
	#define CONNECT_DEBUG	(STM_LEVEL_NONE)
#endif


/***********************************************/

static IRodsConnection *AllocateIRodsConnection (rcComm_t *irods_comm_p);

static bool ReleaseIRodsConnection (rcComm_t *connection_p);


/***********************************************/


void FreeIRodsConnection (struct IRodsConnection *connection_p)
{
	ReleaseIRodsConnection (connection_p -> ic_connection_p);

	FreeMemory (connection_p);
}


static bool ReleaseIRodsConnection (rcComm_t *connection_p)
{
	int status = rcDisconnect (connection_p);

	if (status == 0)
		{
			return true;
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to disconnect from iRODS: %d", status);
			return false;
		}
}


IRodsConnection *CreateIRodsConnectionFromJSON (const json_t *config_p)
{
	IRodsConnection *connection_p = NULL;
	const json_t *irods_credentials_p = NULL;
	/*
	 * The config might have the credentials child or it might be the
	 * credentials object itself so check for both
	 */
	const json_t *credentials_p = json_object_get (config_p, CREDENTIALS_S);

	if (!credentials_p)
		{
			credentials_p = config_p;
		}

	#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINER
		{
			PrintJSONToLog (credentials_p, "connection credentials:\n", STM_LEVEL_FINE, __FILE__, __LINE__);
		}
	#endif


	irods_credentials_p = json_object_get (credentials_p, "irods");

	if (irods_credentials_p)
		{
			const char *username_s = GetJSONString (irods_credentials_p, CREDENTIALS_USERNAME_S);

			if (username_s)
				{
					const char *password_s = GetJSONString (irods_credentials_p, CREDENTIALS_PASSWORD_S);

					if (password_s)
						{
							connection_p = CreateIRodsConnection (username_s, password_s);
						}
				}
		}

	return connection_p;
}


IRodsConnection *CreateIRodsConnection (const char *username_s, const char *password_s)
{
	IRodsConnection *connection_p = NULL;
	rodsEnv env;
	rErrMsg_t err;

	/**@REPLACE IRODS CALL */
	int status = getRodsEnv (&env);

	if (status == 0) 
		{
			rcComm_t *comm_p = NULL;

			if (!username_s)
				{
					username_s = env.rodsUserName;
				}
			
			/**@REPLACE IRODS CALL */
			comm_p = rcConnect (env.rodsHost, env.rodsPort, (char *) username_s, env.rodsZone, 0, &err);
			
			if (comm_p)
				{
					/**@REPLACE IRODSCALL */
					status = password_s ? clientLoginWithPassword (comm_p, (char *) password_s) : clientLogin (comm_p);
					
					if (status == 0)
						{
							connection_p = AllocateIRodsConnection (comm_p);
						}
					else
						{
							ReleaseIRodsConnection (comm_p);
						}
				}
		}
		
	return connection_p;
}


/*******************************/

static IRodsConnection *AllocateIRodsConnection (rcComm_t *irods_comm_p)
{
	IRodsConnection *connection_p = (IRodsConnection *) AllocMemory (sizeof (IRodsConnection));

	if (connection_p)
		{
			connection_p -> ic_connection_p = irods_comm_p;
		}

	return connection_p;
}

                                                                                                      
