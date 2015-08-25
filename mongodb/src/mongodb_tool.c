/*
 * mongodb_tool.c
 *
 *  Created on: 26 Jun 2015
 *      Author: billy
 */
#include <math.h>

#define ALLOCATE_MONGODB_TAGS (1)
#include "mongodb_tool.h"
#include "memory_allocations.h"
#include "streams.h"
#include "json_tools.h"
#include "wheatis_config.h"
#include "mongodb_util.h"
#include "json_util.h"


static bool AddSimpleTypeToQuery (bson_t *query_p, const char *key_s, const json_t *value_p);

static bson_t *AddChild (const char * const key_s, const char * const sub_key_s, const json_t * const value_p);


#ifdef _DEBUG
	#define MONGODB_TOOL_DEBUG	(STM_LEVEL_FINER)
#else
	#define MONGODB_TOOL_DEBUG	(STM_LEVEL_NONE)
#endif



MongoTool *AllocateMongoTool (void)
{
	MongoTool *tool_p = (MongoTool *) AllocMemory (sizeof (MongoTool));

	if (tool_p)
		{
			mongoc_client_t *client_p = GetMongoClient ();

			if (client_p)
				{
					tool_p -> mt_client_p = client_p;
					tool_p -> mt_collection_p = NULL;
					tool_p -> mt_cursor_p = NULL;

					return tool_p;
				}		/* if (client_p) */

			FreeMemory (tool_p);
		}		/* if (tool_p) */

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

	if (tool_p -> mt_client_p)
		{
			ReleaseMongoClient (tool_p -> mt_client_p);
		}

	FreeMemory (tool_p);
}


bool SetMongoToolCollection (MongoTool *tool_p, const char *db_s, const char *collection_s)
{
	bool success_flag = false;
	mongoc_collection_t *collection_p =  mongoc_client_get_collection (tool_p -> mt_client_p, db_s, collection_s);

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

			if (json_p)
				{
					#if MONGODB_TOOL_DEBUG >= STM_LEVEL_FINE
					PrintLog (STM_LEVEL_FINE, "raw bson data:\n", value_s);
					PrintJSONToLog (json_p, "bson to json data:", STM_LEVEL_FINE);
					#endif
				}
			else
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
					id_p = (bson_oid_t *) AllocMemory (sizeof (bson_oid_t));

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

											#if MONGODB_TOOL_DEBUG >= STM_LEVEL_FINE
											char buffer_s [25];

											bson_oid_to_string (id_p, buffer_s);
											PrintLog (STM_LEVEL_FINE, "UpdateMongoDocument id %s", buffer_s);
											LogBSON (query_p, STM_LEVEL_FINE, "UpdateMongoDocument query_p");
											LogBSON (update_statement_p, STM_LEVEL_FINE, "UpdateMongoDocument update_statement_p");
											#endif


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

	return success_flag;
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


static bool AddSimpleTypeToQuery (bson_t *query_p, const char *key_s, const json_t *value_p)
{
	bool success_flag = false;

	switch (json_typeof (value_p))
		{
			/*
				JSON_OBJECT
				JSON_ARRAY
				JSON_STRING
				JSON_INTEGER
				JSON_REAL
				JSON_TRUE
				JSON_FALSE
				JSON_NULL
			 */
			case JSON_STRING:
				success_flag = BSON_APPEND_UTF8 (query_p, key_s, json_string_value (value_p));
				break;

			case JSON_INTEGER:
				#if JSON_INTEGER_IS_LONG_LONG
					success_flag = BSON_APPEND_INT64 (query_p, key_s, json_integer_value (value_p));
				#else
					success_flag = BSON_APPEND_INT32 (query_p, key_s, json_integer_value (value_p));
				#endif
				break;

			case JSON_REAL:
				success_flag = BSON_APPEND_DOUBLE (query_p, key_s, json_real_value (value_p));
				break;

			case JSON_TRUE:
				success_flag = BSON_APPEND_BOOL (query_p, key_s, true);
				break;

			case JSON_FALSE:
				success_flag = BSON_APPEND_BOOL (query_p, key_s, false);
				break;
		}

	return success_flag;
}


static bson_t *AddChild (const char * const key_s, const char * const sub_key_s, const json_t * const value_p)
{
	bson_t *parent_p = bson_new ();

	if (parent_p)
		{
			bool success_flag = false;
			bson_t *child_p = bson_new ();

			if (child_p)
				{
					if (BSON_APPEND_DOCUMENT_BEGIN (parent_p, key_s, child_p))
						{
							if (AddSimpleTypeToQuery (child_p, sub_key_s, value_p))
								{
									if (bson_append_document_end (parent_p, child_p))
										{
											success_flag = true;
										}
								}

						}		/* if (BSON_APPEND_DOCUMENT_BEGIN (parent_p, key_s, child_p)) */
					else
						{
							bson_destroy (child_p);
						}

				}		/* if (child_p) */

			if (!success_flag)
				{
					bson_destroy (parent_p);
					parent_p = NULL;
				}

		}		/* if (parent_p) */

	return parent_p;
}


bool AddToQuery (bson_t *query_p, const char *key_s, const json_t *json_clause_p)
{
	bool success_flag = false;

	switch (json_typeof (json_clause_p))
		{
			/*
				JSON_OBJECT
				JSON_ARRAY
				JSON_STRING
				JSON_INTEGER
				JSON_REAL
				JSON_TRUE
				JSON_FALSE
				JSON_NULL
			 */
			case JSON_STRING:
				success_flag = BSON_APPEND_UTF8 (query_p, key_s, json_string_value (json_clause_p));
				break;

			case JSON_INTEGER:
				#if JSON_INTEGER_IS_LONG_LONG
					success_flag = BSON_APPEND_INT64 (query_p, key_s, json_integer_value (json_clause_p));
				#else
					success_flag = BSON_APPEND_INT32 (query_p, key_s, json_integer_value (json_clause_p));
				#endif
				break;

			case JSON_REAL:
				success_flag = BSON_APPEND_DOUBLE (query_p, key_s, json_real_value (json_clause_p));
				break;

			case JSON_TRUE:
				success_flag = BSON_APPEND_BOOL (query_p, key_s, true);
				break;

			case JSON_FALSE:
				success_flag = BSON_APPEND_BOOL (query_p, key_s, false);
				break;

			case JSON_ARRAY:
				{
					bson_t *in_p = bson_new ();

					if (in_p)
						{
							if (bson_append_array_begin (query_p, key_s, -1, in_p))
								{
									const size_t size = json_array_size (json_clause_p);
									size_t i;
									uint32 buffer_length = ((uint32) (ceil (log10 ((double) size)))) + 2;

									char *buffer_s = (char *) AllocMemory (buffer_length * sizeof (char));

									if (buffer_s)
										{
											for (i = 0; i < size; ++ i)
												{
													const char *index_p;

													if (bson_uint32_to_string (i, &index_p, buffer_s, buffer_length) > 0)
														{
															json_t *element_p = json_array_get (json_clause_p, i);
															AddSimpleTypeToQuery (in_p, index_p, json_clause_p);

														}
												}

											FreeMemory (buffer_s);
										}		/* if (buffer_s) */

									if (!bson_append_array_end (query_p, in_p))
										{

										}

								}		/* if (bson_append_array_begin (query_p, key_s, -1, in_p)) */

						}		/* if (in_p) */

				}
				break;

			case JSON_OBJECT:
				{
					/*
					 * key:  one of "=", "<", "<=", ">", ">=", "in", "range", "not"
					 * value: can be single value or array. For a "range" key, it will be an array
					 * of 2 elements that are the inclusive lower and upper bounds.
					 */
					json_t *op_p = json_object_get (json_clause_p, "operator");
					json_t *op_value_p = json_object_get (json_clause_p, "value");

					if (op_p && op_value_p)
						{
							if (json_is_string (op_p))
								{
									bson_t *op_bson_p = bson_new ();


									if (op_bson_p)
										{
											const char *op_s = json_string_value (op_p);

											if (strcmp (op_s, "range") == 0)
												{
													if (json_is_array (op_value_p))
														{
															if (json_array_size (op_value_p) == 2)
																{
																	json_t *range_value_p = json_array_get (op_value_p, 0);

																	if (AddChild (key_s, "$gte", op_value_p))
																		{
																			range_value_p = json_array_get (op_value_p, 1);

																			if (AddChild (key_s, "$lte", op_value_p))
																				{
																					success_flag = true;
																				}

																		}
																}
														}
												}
											else
												{
													const char *token_s = NULL;

													if (strcmp (op_s, "=") == 0)
														{
															token_s = "$eq";
														}
													else if (strcmp (op_s, "<") == 0)
														{
															token_s = "$lt";
														}
													else if (strcmp (op_s, "<=") == 0)
														{
															token_s = "$lte";
														}
													else if (strcmp (op_s, ">") == 0)
														{
															token_s = "$gt";
														}
													else if (strcmp (op_s, ">=") == 0)
														{
															token_s = "$gte";
														}
													else if (strcmp (op_s, "!=") == 0)
														{
															token_s = "$ne";
														}
													else
														{

														}

													if (token_s)
														{
															success_flag = AddChild (key_s, token_s, op_value_p);
														}
												}

										}

								}
						}

				}
				break;

			case JSON_NULL:
				success_flag = true;
				break;

			default:
				break;
		}		/* switch (json_typeof (json_p)) */


	#if MONGODB_TOOL_DEBUG >= STM_LEVEL_FINER
	LogBSON (query_p, STM_LEVEL_FINE, "bson search query");
	#endif

	return success_flag;
}


bson_t *GenerateQuery (json_t *json_p)
{
	bson_t *query_p = NULL;

	/*
	 * loop through each of the key:value queries
	 *
	 * if value is not an object or array, treat the statement as key = value
	 * if value is an array, treat it as in.
	 * if value is an object, value can have the following fields:
	 *
	 * key:  one of "=", "<", "<=", ">", ">=", "in", "range", "not"
	 * value: can be single value or array. For a "range" key, it will be an array
	 * of 2 elements that are the inclusive lower and upper bounds.
	 */

	if (json_p)
		{
			query_p = bson_new ();

			#if MONGODB_TOOL_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (json_p, "json search query", STM_LEVEL_FINE);
			#endif


			if (query_p)
				{
					const char *key_s;
					json_t *value_p;

					json_object_foreach (json_p, key_s, value_p)
						{
							AddToQuery (query_p, key_s, value_p);
						}		/* json_object_foreach (json_p, key_p, value_p) */

					#if MONGODB_TOOL_DEBUG >= STM_LEVEL_FINE
					LogBSON (query_p, STM_LEVEL_FINE, "final bson search query");
					#endif

				}		/* if (query_p) */

		}		/* if (json_p) */



	return query_p;
}


bool FindMatchingMongoDocumentsByJSON (MongoTool *tool_p, const json_t *query_json_p, const char **fields_ss)
{
	bool success_flag = false;
	bson_t *query_p = GenerateQuery (query_json_p);  // ConvertJSONToBSON (query_json_p);

	if (query_p)
		{
			char *value_s = bson_as_json (query_p, NULL);

			success_flag = FindMatchingMongoDocumentsByBSON (tool_p, query_p, fields_ss);
			bson_destroy (query_p);
		}

	return success_flag;
}


void LogAllBSON (const bson_t *bson_p, const int level, const char * const prefix_s)
{
	bson_iter_t iter;
	bson_iter_t sub_iter;

	if (bson_iter_init (&iter, bson_p) &&
	    (BSON_ITER_HOLDS_DOCUMENT (&iter) ||
	     BSON_ITER_HOLDS_ARRAY (&iter)) &&
	    bson_iter_recurse (&iter, &sub_iter)) {
	   while (bson_iter_next (&sub_iter)) {
	      printf ("Found key \"%s\" in sub document.\n",
	              bson_iter_key (&sub_iter));
	   }
	}
}


void LogBSON (const bson_t *bson_p, const int level, const char * const prefix_s)
{
	size_t len;
	char *dump_s = bson_as_json (bson_p, &len);

	if (dump_s)
		{
			if (prefix_s)
				{
					PrintLog (level, "%s %s", prefix_s, dump_s);
				}
			else
				{
					PrintLog (level, "%s", dump_s);

				}
			bson_free (dump_s);
		}
}


void LogBSONOid (const bson_oid_t *bson_p, const int level, const char * const prefix_s)
{
	char buffer_s [25];

	bson_oid_to_string (bson_p, buffer_s);

	if (prefix_s)
		{
			PrintLog (level, "%s %s", prefix_s, buffer_s);
		}
	else
		{
			PrintLog (level, "%s", buffer_s);

		}
}


bool FindMatchingMongoDocumentsByBSON (MongoTool *tool_p, const bson_t *query_p, const char **fields_ss)
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

							while (success_flag && *field_ss)
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


			#if MONGODB_TOOL_DEBUG >= STM_LEVEL_FINE
			LogBSON (query_p, STM_LEVEL_FINE, "mongo query");
			#endif


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


bool HasMongoQueryResults (MongoTool *tool_p)
{
	bool results_flag = false;

	if (tool_p -> mt_cursor_p)
		{
			results_flag = mongoc_cursor_more (tool_p -> mt_cursor_p);
		}

	return results_flag;
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


json_t *GetAllExistingMongoResultsAsJSON (MongoTool *tool_p)
{
	json_t *results_array_p = NULL;

	if (tool_p)
		{
			results_array_p = json_array ();

			if (results_array_p)
				{
					if (!IterateOverMongoResults (tool_p, AddBSONDocumentToJSONArray, results_array_p))
						{
						}

				}		/* if (results_array_p) */

		}		/* if (tool_p) */

	return results_array_p;

}


json_t *GetAllMongoResultsAsJSON (MongoTool *tool_p, bson_t *query_p)
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
							if (json_object_set_new (result_p, "results", results_array_p) == 0)
								{
									bool success_flag = false;
									bool alloc_query_flag = false;

									if (!query_p)
										{
											query_p = bson_new ();
											alloc_query_flag = (query_p != NULL);
										}

									if (query_p)
										{
											if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL))
												{
													if (!IterateOverMongoResults (tool_p, AddBSONDocumentToJSONArray, results_array_p))
														{
														}

												}		/* if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, NULL)) */

											if (alloc_query_flag)
												{
													bson_destroy (query_p);
												}

										}		/* if (query_p) */

								}		/* if (json_object_set_new (result_p, "results", results_array_p) == 0) */

						}		/* if (results_array_p) */

				}		/* if (result_p) */

		}		/* if (tool_p) */

	return result_p;
}


bool AddBSONDocumentToJSONArray (const bson_t *document_p, void *data_p)
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

