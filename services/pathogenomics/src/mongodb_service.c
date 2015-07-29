#include <string.h>

#include "jansson.h"

#include "mongodb_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "service_job.h"
#include "mongodb_tool.h"
#include "string_utils.h"
#include "json_tools.h"

#include "string_linked_list.h"

#ifdef _DEBUG
	#define MONGODB_SERVICE_DEBUG	(STM_LEVEL_FINE)
#else
	#define MONGODB_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


#define TAG_UPDATE MAKE_TAG('P', 'G', 'U', 'P')
#define TAG_QUERY MAKE_TAG('P', 'G', 'Q', 'U')
#define TAG_REMOVE MAKE_TAG('P', 'G', 'R', 'M')
#define TAG_DUMP MAKE_TAG('P', 'G', 'D', 'P')
#define TAG_COLLECTION MAKE_TAG('P', 'G', 'C', 'O')
#define TAG_DATABASE MAKE_TAG('P', 'G', 'D', 'B')
#define TAG_DELIMITER MAKE_TAG('P', 'G', 'D', 'L')
#define TAG_FILE MAKE_TAG('P', 'G', 'F', 'I')


static const char *S_DATABASE_S = "geodb";

typedef struct MongoDBServiceData
{
	MongoTool *msd_tool_p;
} MongoDBServiceData;

/*
 * STATIC PROTOTYPES
 */

static Service *GetMongoDBService (json_t *operation_json_p, size_t i);

static const char *GetMongoDBServiceName (Service *service_p);

static const char *GetMongoDBServiceDesciption (Service *service_p);

static const char *GetMongoDBServiceInformationUri (Service *service_p);

static ParameterSet *GetMongoDBServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseMongoDBServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunMongoDBService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForMongoDBService (Service *service_p, Resource *resource_p, Handler *handler_p);


static MongoDBServiceData *AllocateMongoDBServiceData (json_t *config_p);

static json_t *GetMongoDBServiceResults (Service *service_p, const uuid_t job_id);


static void FreeMongoDBServiceData (MongoDBServiceData *data_p);

static bool CloseMongoDBService (Service *service_p);

static bool CleanUpMongoDBServiceJob (ServiceJob *job_p);



static bool InsertData (MongoTool *tool_p, json_t *data_p, const char *collection_s);

static bool SearchData (MongoTool *tool_p, json_t *data_p, const char *collection_s);

static bool DeleteData (MongoTool *tool_p, json_t *data_p, const char *collection_s);

static LinkedList *GetTableRow (const char **data_ss, const char delimiter, ByteBuffer *buffer_p, bool empty_strings_allowed_flag);

static int32 UploadDelimitedTable (MongoTool *tool_p,  const char *data_s, const char delimiter);

static bool AddUploadParams (ParameterSet *param_set_p);

static bool ImportTableData (MongoTool *tool_p,  const char *collection_s, const char *data_s);

/*
 * API FUNCTIONS
 */
 

ServicesArray *GetServices (json_t *config_p)
{
	ServicesArray *services_p = AllocateServicesArray (1);

	if (services_p)
		{
			Service *service_p = GetMongoDBService (config_p, 0);

			if (service_p)
				{
					* (services_p -> sa_services_pp) = service_p;
					return services_p;
				}

			FreeServicesArray (services_p);
		}

	return NULL;
}



void ReleaseService (Service *service_p)
{
	FreeMongoDBServiceData ((MongoDBServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */



static json_t *GetMongoDBServiceResults (Service *service_p, const uuid_t job_id)
{
	MongoDBServiceData *data_p = (MongoDBServiceData *) (service_p -> se_data_p);
	ServiceJob *job_p = GetJobById (service_p -> se_jobs_p, job_id);
	json_t *res_p = NULL;

	if (job_p)
		{
			if (job_p -> sj_status == OS_SUCCEEDED)
				{
					json_error_t error;
					//const char *buffer_data_p = GetCurlToolData (data_p -> wsd_curl_data_p);
					//res_p = json_loads (buffer_data_p, 0, &error);
				}
		}		/* if (job_p) */

	return res_p;
}



static Service *GetMongoDBService (json_t *operation_json_p, size_t i)
{									
	Service *mongodb_service_p = (Service *) AllocMemory (sizeof (Service));
	
	if (mongodb_service_p)
		{
			ServiceData *data_p = (ServiceData *) AllocateMongoDBServiceData (operation_json_p);
			
			if (data_p)
				{
					InitialiseService (mongodb_service_p,
						GetMongoDBServiceName,
						GetMongoDBServiceDesciption,
						GetMongoDBServiceInformationUri,
						RunMongoDBService,
						IsResourceForMongoDBService,
						GetMongoDBServiceParameters,
						ReleaseMongoDBServiceParameters,
						CloseMongoDBService,
						GetMongoDBServiceResults,
						NULL,
						false,
						true,
						data_p);

					return mongodb_service_p;
				}
			
			FreeMemory (mongodb_service_p);
		}		/* if (mongodb_service_p) */
			
	return NULL;
}


static MongoDBServiceData *AllocateMongoDBServiceData (json_t *op_json_p)
{
	MongoTool *tool_p = AllocateMongoTool ();
	
	if (tool_p)
		{
			MongoDBServiceData *data_p = (MongoDBServiceData *) AllocMemory (sizeof (MongoDBServiceData));

			if (data_p)
				{
					data_p -> msd_tool_p = tool_p;

					return data_p;
				}

			FreeMongoTool (tool_p);
		}

	return NULL;
}


static void FreeMongoDBServiceData (MongoDBServiceData *data_p)
{
	FreeMongoTool (data_p -> msd_tool_p);

	FreeMemory (data_p);
}


static const char *GetMongoDBServiceName (Service *service_p)
{
	return "Pathogenomics Geoservice";
}


static const char *GetMongoDBServiceDesciption (Service *service_p)
{
	return "A service to analyse the spread of Wheat-related diseases both geographically and temporally";
}


static const char *GetMongoDBServiceInformationUri (Service *service_p)
{
	return NULL;
}


static ParameterSet *GetMongoDBServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	ParameterSet *params_p  = AllocateParameterSet ("Pathogenomics service parameters", "The parameters used for the Pathogenomics service");

	if (params_p)
		{
			Parameter *param_p = NULL;
			SharedType def;

			def.st_json_p = NULL;

			if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_JSON, false, "update", "Update", "Add data to the system", TAG_UPDATE, NULL, def, NULL, NULL, PL_ADVANCED, NULL)) != NULL)
				{
					if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_JSON, false, "search", "Search", "Find data to the system", TAG_QUERY, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
						{
							if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_JSON, false, "delete", "Delete", "Delete data to the system", TAG_REMOVE, NULL, def, NULL, NULL, PL_ADVANCED, NULL)) != NULL)
								{
									if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_BOOLEAN, false, "dump", "Dump", "Get all of the data in the system", TAG_DUMP, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
										{
											if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_STRING, false, "collection", "Collection", "The collection to act upon", TAG_COLLECTION, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
												{
													if (AddUploadParams (params_p))
														{
															return params_p;
														}
												}
										}
								}
						}
				}

			FreeParameterSet (params_p);
		}

	return NULL;
}



static bool AddUploadParams (ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 2;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;

	def.st_char_value = '|';

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_CHAR, false, "delimiter", "Delimiter", "The character delimiting columns", TAG_DELIMITER, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
		{
			def.st_string_value_s = NULL;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_TABLE, false, "data", "Data to upload", "The data to upload", TAG_FILE, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					const char * const group_name_s = "Spreadsheet Import Parameters";

					*grouped_param_pp = param_p;

					if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to add %s grouping", group_name_s);
							FreeMemory (grouped_params_pp);
						}

					success_flag = true;
				}
		}

	return success_flag;
}


static void ReleaseMongoDBServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}




static bool CloseMongoDBService (Service *service_p)
{
	bool success_flag = true;
	
	return success_flag;
}


static ServiceJobSet *RunMongoDBService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	MongoDBServiceData *data_p = (MongoDBServiceData *) (service_p -> se_data_p);

	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1, CleanUpMongoDBServiceJob);

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = service_p -> se_jobs_p -> sjs_jobs_p;

			job_p -> sj_status = OS_FAILED_TO_START;

			if (param_set_p)
				{
					/* get the collection to work on */
					SharedType value;

					if (GetParameterValueFromParameterSet (param_set_p, TAG_COLLECTION, &value, true))
						{
							const char *collection_s = value.st_string_value_s;

							MongoTool *tool_p = AllocateMongoTool ();

							if (tool_p)
								{
									json_t *response_p = NULL;
									Parameter *param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_DUMP);
									bool run_flag = false;

									SetMongoToolCollection (tool_p, S_DATABASE_S, collection_s);

									if (param_p && (param_p -> pa_type == PT_BOOLEAN) && (param_p -> pa_current_value.st_boolean_value == true))
										{
											response_p = GetAllMongoResultsAsJSON (tool_p, NULL);

											job_p -> sj_status = (response_p != NULL) ? OS_SUCCEEDED : OS_FAILED;
										}
									else
										{
											bool data_uploaded_flag = false;

											param_p -> GetParameterFromParameterSetByTag (param_set_p, TAG_FILE);

											if (param_p)
												{
													data_uploaded_flag = ImportTableData (tool_p, collection_s, param_p -> pa_current_value.st_string_value_s);
												}

											if (!data_uploaded_flag)
												{
													bool (*data_fn) (MongoTool *tool_p, json_t *data_p, const char *collection_s) = NULL;
													json_t *json_param_p = NULL;

													job_p -> sj_status = OS_FAILED;

													if ((GetParameterValueFromParameterSet (param_set_p, TAG_UPDATE, &value, true)) && (value.st_json_p))
														{
															data_fn = InsertData;
															json_param_p = value.st_json_p;
														}
													else if ((GetParameterValueFromParameterSet (param_set_p, TAG_QUERY, &value, true)) && (value.st_json_p))
														{
															data_fn = SearchData;
															json_param_p = value.st_json_p;
														}
													else if ((GetParameterValueFromParameterSet (param_set_p, TAG_REMOVE, &value, true)) && (value.st_json_p))
														{
															data_fn = DeleteData;
															json_param_p = value.st_json_p;
														}

													if (data_fn)
														{
															json_error_t error;
															bool success_flag = data_fn (tool_p, json_param_p, collection_s);

															#if MONGODB_SERVICE_DEBUG >= STM_LEVEL_FINE
																{
																	char *dump_s = json_dumps (json_param_p, JSON_INDENT (2));
																	PrintLog (STM_LEVEL_FINE, "mongo param: %s", dump_s);
																	free (dump_s);
																}
															#endif

															response_p = json_pack_ex (&error, 0, "{s:b,s:s,s:o}", "status", success_flag, "collection", collection_s, "results", data_p);

															if (response_p)
																{
																	job_p -> sj_status = OS_SUCCEEDED;
																}
															else
																{
																	job_p -> sj_status = OS_FAILED;
																}

														}

												}		/* if (!data_uploaded_flag) */

										}

								}		/* if (tool_p) */

						}		/* if (collection_s) */

				}		/* if (param_set_p) */

		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}


static bool ImportTableData (MongoTool *tool_p,  const char *collection_s, const char *data_s, const char *delimiter_s)
{
	bool success_flag = false;
	const char *current_line_s = data_s;
	const char *next_line_s = NULL;

	bool loop_flag = true;

	while (loop_flag)
		{
			char *value_s = NULL;
			bool alloc_flag = false;

			next_line_s = strchr (current_line_s, '\n');

			if (next_line_s)
				{
					value_s = CopyToNewString (current_line_s, next_line_s - current_line_s, false);

					if (value_s)
						{

							alloc_flag = true;
						}


					current_line_s = next_line_s + 1;
				}
			else
				{
					loop_flag = false;
				}
		}



	return success_flag;
}


static bool SearchData (MongoTool *tool_p, json_t *data_p, const char *collection_s)
{
	bool success_flag = false;
	json_t *values_p = json_object_get (data_p, MONGO_OPERATION_DATA_S);

	if (values_p)
		{
			const char **fields_ss = NULL;
			json_t *fields_p = json_object_get (data_p, MONGO_OPERATION_FIELDS_S);

			if (fields_p)
				{
					if (json_is_array (fields_p))
						{
							size_t size = json_array_size (fields_p);

							fields_ss = (const char **) AllocMemoryArray (size + 1, sizeof (const char *));

							if (fields_ss)
								{
									const char **field_ss = fields_ss;
									size_t i;
									json_t *field_p;

									json_array_foreach (fields_p, i, field_p)
										{
											if (json_is_string (field_p))
												{
													*field_ss = json_string_value (field_p);
													++ field_ss;
												}
											else
												{
													char *dump_s = json_dumps (field_p, JSON_INDENT (2));

													PrintErrors (STM_LEVEL_WARNING, "Failed to get field from %s", dump_s);
													free (dump_s);
												}
										}

								}		/* if (fields_ss) */

						}		/* if (json_is_array (fields_p)) */

				}		/* if (fields_p) */

			success_flag = FindMatchingMongoDocumentsByJSON (tool_p, values_p, fields_ss);

			if (fields_ss)
				{
					FreeMemory (fields_ss);
				}
		}		/* if (values_p) */

	return success_flag;
}


static bool InsertData (MongoTool *tool_p, json_t *values_p, const char *collection_s)
{
	bool success_flag = false;

	/**
	 * Is it an insert or an update?
	 */
	const char *id_s = GetJSONString (values_p, MONGO_ID_S);

	if (id_s)
		{
			bson_oid_t oid;

			if (bson_oid_is_valid (id_s, strlen (id_s)))
				{
					bson_oid_init_from_string (&oid, id_s);

					if (json_object_del (values_p, MONGO_ID_S) == 0)
						{
							success_flag = UpdateMongoDocument (tool_p, &oid, values_p);
						}

				}
		}
	else
		{
			bson_oid_t *id_p = InsertJSONIntoMongoCollection (tool_p, values_p);

			if (id_p)
				{
					success_flag = true;
					FreeMemory (id_p);
				}
		}

	if (success_flag)
		{
			/**
			 * Add the fields to the list of available fields for this
			 * collection
			 */
			char *fields_collection_s = ConcatenateStrings (collection_s, ".fields");

			if (fields_collection_s)
				{
					json_t *field_p = json_object ();
					bson_oid_t doc_id;

					if (field_p)
						{
							const char *fields_ss [2] = { NULL, NULL };
							const char *key_s;
							json_t *value_p;

							SetMongoToolCollection (tool_p, S_DATABASE_S, fields_collection_s);

							json_object_foreach (values_p, key_s, value_p)
								{
									bson_t *query_p = bson_new (); //BCON_NEW ("$query", "{", key_s, BCON_INT32 (1), "}");

									if (query_p)
										{
											int32 value = 0;
											json_t *update_p = NULL;
											json_error_t error;

											*fields_ss = key_s;

											if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, fields_ss))
												{
													const bson_t *doc_p = NULL;

													/* should only be one of these */
													while (mongoc_cursor_next (tool_p -> mt_cursor_p, &doc_p))
														{
															bson_iter_t iter;
															json_t *json_value_p = NULL;

															#if MONGODB_SERVICE_DEBUG >= STM_LEVEL_FINE
															LogAllBSON (doc_p, STM_LEVEL_FINE, "matched doc: ");
															#endif

															if (bson_iter_init (&iter, doc_p))
																{
																	if (bson_iter_find (&iter, "_id"))
																		{
																			const bson_oid_t *src_p = NULL;

																			if (BSON_ITER_HOLDS_OID (&iter))
																				{
																					src_p = (const bson_oid_t  *) bson_iter_oid (&iter);
																				}

																			bson_oid_copy (src_p, &doc_id);

																			#if MONGODB_SERVICE_DEBUG >= STM_LEVEL_FINE
																			LogBSONOid (src_p, STM_LEVEL_FINE, "doc id");
																			LogBSONOid (&doc_id, STM_LEVEL_FINE, "doc id");
																			#endif
																		}
																}

															json_value_p = ConvertBSONToJSON (doc_p);

															if (json_value_p)
																{
																	json_t *count_p = json_object_get (json_value_p, key_s);

																	if (count_p)
																		{
																			if (json_is_integer (count_p))
																				{
																					value = json_integer_value (count_p);
																				}
																		}

																	WipeJSON (json_value_p);
																}
														 }
												}

											++ value;

											/*
											 * Now need to set key_s = value into the collection
											 */
											update_p = json_pack_ex (&error, 0, "{s:i}", key_s, value);

											if (update_p)
												{
													if (value == 1)
														{
															bson_oid_t *fields_id_p = InsertJSONIntoMongoCollection (tool_p, update_p);

															if (fields_id_p)
																{
																	FreeMemory (fields_id_p);
																}
															else
																{
																	PrintErrors (STM_LEVEL_WARNING, "Failed to update %s in %s", key_s, fields_collection_s);
																	success_flag = false;
																}
														}
													else
														{
															if (!UpdateMongoDocument (tool_p, (bson_oid_t *) &doc_id, update_p))
																{
																	PrintErrors (STM_LEVEL_WARNING, "Failed to update %s in %s", key_s, fields_collection_s);
																}
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, "Failed to create json for updating fields collection %s,  %s", fields_collection_s, error.text);
													success_flag = false;
												}

											bson_destroy (query_p);
										}
								}		/* json_object_foreach (values_p, key_s, value_p) */


						}		/* if (field_p) */


					FreeCopiedString (fields_collection_s);
				}		/* if (fields_collection_s) */

		}

	return success_flag;
}
	


static bool DeleteData (MongoTool *tool_p, json_t *data_p, const char *collection_s)
{
	bool success_flag = false;
	json_t *selector_p = json_object_get (data_p, MONGO_OPERATION_DATA_S);

	if (selector_p)
		{
			success_flag = RemoveMongoDocuments (tool_p, selector_p, false);
		}		/* if (values_p) */

	return success_flag;
}


static bool IsResourceForMongoDBService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;

	if (resource_p -> re_protocol_s)
		{
			interested_flag = (strcmp (resource_p -> re_protocol_s, "string") == 0);
		}

	return interested_flag;
}


static bool CleanUpMongoDBServiceJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;

	return cleaned_up_flag;
}



static int32 UploadDelimitedTable (MongoTool *tool_p,  const char *data_s, const char delimiter)
{
	int32 num_lines = 0;

	if (data_s)
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);

			if (buffer_p)
				{
					json_t *json_data_p = json_object ();

					if (json_data_p)
						{
							LinkedList *headers_p = GetTableRow (&data_s, delimiter, buffer_p, false);

							if (headers_p)
								{
									uint32 line_number = 2;
									LinkedList *row_p = NULL;

									while ((row_p = GetTableRow (&data_s, delimiter, buffer_p, true)) != NULL)
										{
											if (headers_p -> ll_size == row_p -> ll_size)
												{
													StringListNode *header_node_p = (StringListNode *) (headers_p -> ll_head_p);
													StringListNode *row_node_p = (StringListNode *) (row_p -> ll_head_p);
													bson_oid_t *id_p = NULL;

													while (header_node_p)
														{
															const char *row_value_s = row_node_p -> sln_string_s;

															if (row_value_s)
																{
																	if (json_object_set_new (json_data_p, header_node_p -> sln_string_s, json_string (row_value_s)) != 0)
																		{
																			PrintErrors (STM_LEVEL_SEVERE, "Failed to insert %s: %s into json object", header_node_p -> sln_string_s, row_value_s);
																		}
																}

															header_node_p = (StringListNode *) (header_node_p -> sln_node.ln_next_p);
															row_node_p = (StringListNode *) (row_node_p -> sln_node.ln_next_p) ;
														}		/* while (header_node_p) */

													/*
													The ID header is the unique identifier so check to see if it is already
													in the collection
													*/

													#if MONGODB_SERVICE_DEBUG >= STM_LEVEL_FINE
														{
															char *dump_s = json_dumps (json_data_p, JSON_INDENT (2));

															if (dump_s)
																{
																	PrintLog (STM_LEVEL_FINE, "json row %s", dump_s);
																	free (dump_s);
																}
														}
													#endif

													id_p = InsertJSONIntoMongoCollection (tool_p, json_data_p);
													if (!id_p)
														{
															PrintErrors (STM_LEVEL_SEVERE, "Failed to insert json object into collection");
														}

													json_object_clear (json_data_p);
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, "" UINT32_FMT " headers and " UINT32_FMT "rows  for line " UINT32_FMT, headers_p -> ll_size, row_p -> ll_size, line_number);
												}

											++ line_number;
										}

								}	/* if (headers_p) */

							WipeJSON (json_data_p);
						}		/* if (json_data_p) */

					FreeByteBuffer (buffer_p);
				}		/* if (buffer_p) */

		}


	return num_lines;
}


static LinkedList *GetTableRow (const char **data_ss, const char delimiter, ByteBuffer *buffer_p, bool empty_strings_allowed_flag)
{
	bool success_flag = true;
	const char *buffer_s = NULL;
	LinkedList *row_p = AllocateLinkedList (FreeStringListNode);

	if (row_p)
		{
			bool loop_flag = (*buffer_s != '\0');

			buffer_s = *data_ss;
			ResetByteBuffer (buffer_p);

			while (loop_flag && success_flag)
				{
					if (*buffer_s == delimiter)
						{
							/* For an empty column this can be an empty string */
							const char *data_s = GetByteBufferData (buffer_p);
							StringListNode *node_p = NULL;

							if (*data_s != '\0')
								{
									node_p = AllocateStringListNode (data_s, MF_DEEP_COPY);
								}
							else if (empty_strings_allowed_flag)
								{
									node_p = AllocateStringListNode (data_s, MF_SHADOW_USE);
								}

							if (node_p)
								{
									LinkedListAddTail (row_p, (ListItem * const) node_p);
								}
							else
								{
									success_flag = false;
								}
						}
					else
						{
							switch (*buffer_s)
								{
									case '\r':
									case '\n':
										loop_flag = false;
										break;

									default:
										success_flag = AppendToByteBuffer (buffer_p, buffer_s, 1);
										break;
								}
						}

					if (loop_flag && success_flag)
						{
							++ buffer_s;
							loop_flag = (*buffer_s != '\0');
						}
				}		/*( while (loop_flag) */

		}		/* if (row_p) */

	if (success_flag)
		{
			*data_ss = buffer_s;
		}
	else
		{
			FreeLinkedList (row_p);
			row_p = NULL;
		}

	return row_p;
}
