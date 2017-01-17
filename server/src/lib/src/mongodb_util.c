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
/*
 * mongodb_util.c
 *
 *  Created on: 20 Jul 2015
 *      Author: billy
 */

#include "mongodb_util.h"
#include "memory_allocations.h"
#include "json_tools.h"
#include "grassroots_config.h"
#include "json_util.h"
#include "streams.h"


mongoc_client_pool_t *s_clients_p = NULL;


bool InitMongoDB (void)
{
	if (!s_clients_p)
		{
			const json_t *mongo_config_p = GetGlobalConfigValue ("mongodb");

			if (mongo_config_p)
				{
					const char *uri_s = GetJSONString (mongo_config_p, "uri");

					if (uri_s)
						{
							mongoc_uri_t *uri_p = mongoc_uri_new (uri_s);

							if (uri_p)
								{
									s_clients_p = mongoc_client_pool_new (uri_p);

									if (!s_clients_p)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create mongodb client pool for %s", uri_s);
										}

									mongoc_uri_destroy (uri_p);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get mongodb uri for %s", uri_s);
								}
						}
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, mongo_config_p, "No uri in config");
						}

				}
			else
				{
					PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "No mongodb in config");
				}
		}

	return (s_clients_p != NULL);
}


void ExitMongoDB (void)
{
	if (s_clients_p)
		{
			mongoc_client_pool_destroy (s_clients_p);
			s_clients_p = NULL;
		}
}


mongoc_client_t *GetMongoClient (void)
{
	mongoc_client_t *client_p = NULL;

	if (s_clients_p)
		{
			client_p = mongoc_client_pool_try_pop (s_clients_p);
		}

	return client_p;
}


void ReleaseMongoClient (mongoc_client_t *client_p)
{
	if (s_clients_p)
		{
			mongoc_client_pool_push (s_clients_p, client_p);
		}
}


