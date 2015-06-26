/*
 * mongodb_tool.c
 *
 *  Created on: 26 Jun 2015
 *      Author: billy
 */

#include "mongodb_tool.h"
#include "memory_allocations.h"
#include "streams.h"

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
	mongoc_client_t *client_p = mongoc_client_pool_pop (s_clients_p);

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
			mongoc_colletion_destroy (tool_p -> mt_collection_p);
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
					mongoc_colletion_destroy (tool_p -> mt_collection_p);
				}

			tool_p -> mt_collection_p = collection_p;
			success_flag = true;
		}

	return success_flag;
}


bson_t *ConvertJSONToBSON (const json_t *json_p)
{
	bson_t *bson_p = NULL;
	char *value_s = json_dumps (json_p, JSON_INDENT (2));

	if (value_s)
		{
			bson_error_t error;

			bson_p = bson_new_from_json (value_s, &error);

			if (!bson_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to convert %s to BSON, error %s\n", value_s, error.message);
				}

			free (value_s);
		}		/* if (value_s) */

	return bson_p;
}


bool InsertJSONIntoMongoCollection (MongoTool *tool_p, json_t *json_p)
{
	bool success_flag = false;

	if (tool_p -> mt_collection_p)
		{
			bson_t *bson_p = ConvertJSONToBSON (json_p);

			if (bson_p)
				{
					bson_error_t error;

					bool b = mongoc_collection_insert (mongoc_collection_t          *collection,
					                          mongoc_insert_flags_t         flags,
					                          const bson_t                 *document,
					                          const mongoc_write_concern_t *write_concern,
					                          bson_error_t                 *error);
				}		/* if (bson_p) */

		}		/* if (tool_p -> mt_collection_p) */

	return success_flag;
}


