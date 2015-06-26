/*
 * mongodb_tool.c
 *
 *  Created on: 26 Jun 2015
 *      Author: billy
 */

#include "mongodb_tool.h"
#include "memory_allocations.h"

mongoc_client_pool_t *s_clients_p = NULL;


bool InitMongo (const char *uri_s)
{
//	mongoc_uri_t *uri_p = mongoc_uri_new ("mongodb://mdb1.example.com/?minPoolSize=16");
	mongoc_uri_t *uri_p = mongoc_uri_new (uri_s);

	if (uri_p)
		{
			s_clients_p = mongoc_client_pool_new (uri_p);

			mongoc_uri_destroy (uri_p);
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


MongoTool *AllocateMongoTool (void)
{
	mongo_client_t *client_p = mongoc_client_pool_pop (s_clients_p);

	if (client_p)
		{
			MongoTool *tool_p = (MongoTool *) AllocMemory (sizeof (MongoTool));

			if (tool_p)
				{
					tool_p -> mt_client_p = client_p;
					tool_p -> mt_collection_p = NULL;

					return tool_p;
				}		/* if (tool_p) */

			mongoc_client_pool_push (s_clients_p, client_p);
		}		/* if (client_p) */

	return NULL;
}


void FreeMongoTool (MongoTool *tool_p)
{
	if (tool_p -> mt_collection_p)
		{
			mongoc_colletion_destoy (tool_p -> mt_collection_p);
		}

	FreeMemory (tool_p);
}


bool GetMongoCollection (MongoTool *tool_p, const char *db_s, const char *collection_s)
{
	bool success_flag = false;
	mongoc_collection_t *collection_p =  mongoc_client_get_collection (tool_p -> mt_client_p, collection_s, collection_s);

	if (collection_p)
		{
			if (tool_p -> mt_collection_p)
				{
					mongoc_colletion_destoy (tool_p -> mt_collection_p);
				}

			tool_p -> mt_collection_p = collection_p;
			success_flag = true;
		}

	return success_flag;
}
