/*
 * mongodb_util.c
 *
 *  Created on: 20 Jul 2015
 *      Author: billy
 */

#include "mongodb_util.h"
#include "memory_allocations.h"
#include "json_tools.h"
#include "wheatis_config.h"

mongoc_client_pool_t *s_clients_p = NULL;


bool InitMongo (const char *uri_s)
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

									mongoc_uri_destroy (uri_p);
								}

						}
				}
		}

	return (s_clients_p != NULL);
}


void ExitMongo (void)
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
			client_p = mongoc_client_pool_pop (s_clients_p);
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


