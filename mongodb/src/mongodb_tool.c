/*
 * mongodb_tool.c
 *
 *  Created on: 26 Jun 2015
 *      Author: billy
 */

#include "mongodb_tool.h"
#include "memory_allocations.h"
#include "streams.h"
#include "json_tools.h"

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
					tool_p -> mt_cursor_p = NULL;

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
			mongoc_collection_destroy (tool_p -> mt_collection_p);
		}

	if (tool_p -> mt_cursor_p)
		{
			mongoc_cursor_destroy (tool_p -> mt_cursor_p);
		}

	FreeMemory (tool_p);
}


bool SetMongoToolCollection (MongoTool *tool_p, const char *db_s, const char *collection_s)
{
	bool success_flag = false;
	mongoc_collection_t *collection_p =  mongoc_client_get_collection (tool_p -> mt_client_p, collection_s, collection_s);

	if (collection_p)
		{
			if (tool_p -> mt_collection_p)
				{
					mongoc_collection_destroy (tool_p -> mt_collection_p);
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

			bson_p = bson_new_from_json ((const uint8 *) value_s, -1, &error);

			if (!bson_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to convert %s to BSON, error %s\n", value_s, error.message);
				}

			free (value_s);
		}		/* if (value_s) */

	return bson_p;
}


json_t *ConvertBSONToJSON (const bson_t *bson_p)
{
	json_t *json_p = NULL;
	char *value_s = bson_as_json (bson_p, NULL);

	if (value_s)
		{
			json_error_t error;

			json_p = json_loads (value_s, 0, &error);

			if (!json_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to convert %s to JSON, error %s\n", value_s, error.text);
				}

			bson_free (value_s);
		}		/* if (value_s) */

	return json_p;
}


bson_oid_t *InsertJSONIntoMongoCollection (MongoTool *tool_p, json_t *json_p)
{
	bson_oid_t *id_p = NULL;

	if (tool_p -> mt_collection_p)
		{
			bson_t *bson_p = ConvertJSONToBSON (json_p);

			if (bson_p)
				{
					bson_oid_t *id_p = (bson_oid_t *) AllocMemory (sizeof (bson_oid_t));

					if (id_p)
						{
							bool success_flag = false;
							bson_oid_init (id_p, NULL);

							if (BSON_APPEND_OID (bson_p, MONGO_ID_S, id_p))
								{
									bson_error_t error;
									success_flag = mongoc_collection_insert (tool_p -> mt_collection_p, MONGOC_INSERT_NONE, bson_p, NULL, &error);

									if (!success_flag)
										{
											char *value_s = json_dumps (json_p, JSON_INDENT (2));

											if (value_s)
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to insert %s, error %s\n", value_s, error.message);
													free (value_s);
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to insert json doc, error %s\n", error.message);
												}

										}		/* if (!success_flag) */

								}		/* if (BSON_APPEND_OID (bson_p, MONGO_ID_S, id_p)) */

							if (!success_flag)
								{
									FreeMemory (id_p);
									id_p = NULL;
								}

						}		/* if (id_p) */

				}		/* if (bson_p) */

		}		/* if (tool_p -> mt_collection_p) */

	return id_p;
}



bool UpdateMongoDocument (MongoTool *tool_p, const bson_oid_t *id_p, json_t *json_p)
{
	bool success_flag = false;

	if (tool_p -> mt_collection_p)
		{
			bson_t *data_p = ConvertJSONToBSON (json_p);

			if (data_p)
				{
					bson_t *query_p = BCON_NEW (MONGO_ID_S, BCON_OID (id_p));

					if (query_p)
						{
							bson_t *update_statement_p = bson_new ();

							if (update_statement_p)
								{
									if (bson_append_document (update_statement_p, "$set", -1, data_p))
										{
											bson_error_t error;

									    if (mongoc_collection_update (tool_p -> mt_collection_p, MONGOC_UPDATE_NONE, query_p, update_statement_p, NULL, &error))
									    	{
									    		success_flag = true;
												}		/* if (mongoc_collection_update (tool_p -> mt_collection_p, MONGOC_UPDATE_NONE, query_p, update_statement_p, NULL, &error)) */

										}		/* if (bson_append_document (update_statement_p, "$set", -1, bson_p)) */

									bson_destroy (update_statement_p);
								}		/* if (query_p) */

							bson_destroy (query_p);
						}		/* if (query_p) */

					bson_destroy (data_p);
				}		/* if (data_p) */

		}		/* if (tool_p -> mt_collection_p) */

	return id_p;
}


bool RemoveMongoDocuments (MongoTool *tool_p, const json_t *selector_json_p, const bool remove_first_match_only_flag)
{
	bool success_flag = false;

	if (tool_p -> mt_collection_p)
		{
			bson_t *selector_p = ConvertJSONToBSON (selector_json_p);

			if (selector_p)
				{
					bson_error_t error;
					mongoc_remove_flags_t flags = remove_first_match_only_flag ? MONGOC_REMOVE_SINGLE_REMOVE : MONGOC_REMOVE_NONE;

					if (mongoc_collection_remove (tool_p -> mt_collection_p, flags, selector_p, NULL, &error))
						{
							success_flag = true;
						}		/* if (mongoc_collection_update (tool_p -> mt_collection_p, MONGOC_UPDATE_NONE, query_p, update_statement_p, NULL, &error)) */

					bson_destroy (selector_p);
				}		/* if (selector_p) */

		}		/* if (tool_p -> mt_collection_p) */

	return success_flag;
}



bool FindMatchingMongoDocumentsByJSON (MongoTool *tool_p, const json_t *query_json_p, const char **fields_ss)
{
	bool success_flag = false;
	bson_t *query_p = ConvertJSONToBSON (query_json_p);

	if (query_p)
		{
			success_flag = FindMatchingMongoDocumentsByBSON (tool_p, query_p, fields_ss);
			bson_destroy (query_p);
		}

	return success_flag;
}



bool FindMatchingMongoDocuments (MongoTool *tool_p, const bson_t *query_p, const char **fields_ss)
{
	bool success_flag = false;

	if (tool_p -> mt_collection_p)
		{
			bson_t *fields_p = NULL;
			mongoc_cursor_t *cursor_p = NULL;

			success_flag = true;

			/*
			 * Add the fields to retrieve
			 */
			if (fields_ss)
				{
					fields_p = bson_new ();

					if (fields_p)
						{
							const char **field_ss = fields_ss;

							while (success_flag && field_ss)
								{
									if (BSON_APPEND_INT32 (fields_p, *field_ss, 1))
										{
											++ field_ss;
										}
									else
										{
											success_flag = false;
										}

								}		/* while (success_flag && field_ss) */

						}		/* if (fields_p) */

				}		/* if (fields_ss) */

			cursor_p = mongoc_collection_find (tool_p -> mt_collection_p, MONGOC_QUERY_NONE, 0, 0, 0, query_p, fields_p, NULL);

			if (cursor_p)
				{
					if (tool_p -> mt_cursor_p)
						{
							mongoc_cursor_destroy (tool_p -> mt_cursor_p);
						}

					tool_p -> mt_cursor_p = cursor_p;
					success_flag = true;
				}

			if (fields_p)
				{
					bson_destroy (fields_p);
				}		/* if (fields_p) */


		}		/* if (tool_p -> mt_collection_p) */

	return success_flag;
}


bool IterateOverMongoResults (MongoTool *tool_p, bool (*process_bson_fn) (const bson_t *document_p, void *data_p), void *data_p)
{
	bool success_flag = true;

	if (tool_p -> mt_cursor_p)
		{
			const bson_t *document_p = NULL;

			while (success_flag && (mongoc_cursor_next (tool_p -> mt_cursor_p, &document_p)))
				{
					success_flag = process_bson_fn (document_p, data_p);
				}

			if (!mongoc_cursor_more (tool_p -> mt_cursor_p))
				{
					mongoc_cursor_destroy (tool_p -> mt_cursor_p);
					tool_p -> mt_cursor_p = NULL;
				}

		}		/* if (tool_p -> mt_cursor_p) */
	else
		{
			success_flag = false;
		}

	return success_flag;
}



json_t *GetAllMongoResultsAsJSON (MongoTool *tool_p)
{
	json_t *result_p = NULL;

	if (tool_p)
		{
			result_p = json_object ();

			if (result_p)
				{
					json_t *results_array_p = json_array ();

					if (results_array_p)
						{

						}		/* if (results_array_p) */

				}		/* if (result_p) */

		}		/* if (tool_p) */

	return result_p;
}


bool AddBSONDocumentToJSONArray (bson_t *document_p, void *data_p)
{
	bool success_flag = false;
	json_t *json_p = (json_t *) data_p;
	json_t *row_p = ConvertBSONToJSON (document_p);

	if (row_p)
		{
			if (json_array_append_new (json_p, row_p) == 0)
				{
					success_flag = true;
				}		/* if (json_array_append_new (json_p, row_p) == 0) */
			else
				{
					WipeJSON (row_p);
				}
		}		/* if (row_p) */

	return success_flag;

}

