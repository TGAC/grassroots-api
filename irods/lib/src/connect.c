/*
** Copyright 2014-2015 The Genome Analysis Centre
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
#include "connect.h"
#include "json_util.h"

#ifdef _DEBUG
	#define CONNECT_DEBUG	(STM_LEVEL_FINE)
#else
	#define CONNECT_DEBUG	(STM_LEVEL_NONE)
#endif


rcComm_t *CreateIRODSConnectionFromJSON (const json_t *config_p)
{
	rcComm_t *connection_p = NULL;
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

	#if JSON_TOOLS_DEBUG >= STM_LEVEL_FINE
		{
			char *value_s = json_dumps (credentials_p, JSON_INDENT (2));
			printf ("connection credentials:\n%s\n", value_s);
			free (value_s);
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
							connection_p = CreateIRODSConnection (username_s, password_s);
						}
				}
		}

	return connection_p;
}


rcComm_t *CreateIRODSConnection (const char *username_s, const char *password_s)
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


void CloseIRODSConnection (rcComm_t *connection_p)
{
	rcDisconnect (connection_p);
}

                                                                                                      
