#include <string.h>

#include "jansson.h"

#define ALLOCATE_PATHOGENOMICS_TAGS
#include "pathogenomics_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "service_job.h"
#include "mongodb_tool.h"
#include "string_utils.h"
#include "json_tools.h"
#include "wheatis_config.h"
#include "country_codes.h"
#include "sample_metadata.h"
#include "string_linked_list.h"
#include "math_utils.h"

#ifdef _DEBUG
	#define PATHOGENOMICS_SERVICE_DEBUG	(STM_LEVEL_FINE)
#else
	#define PATHOGENOMICS_SERVICE_DEBUG	(STM_LEVEL_NONE)
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

static const char s_default_column_delimiter =  '|';


/*
 * STATIC PROTOTYPES
 */

static Service *GetPathogenomicsService (json_t *operation_json_p, size_t i);

static const char *GetPathogenomicsServiceName (Service *service_p);

static const char *GetPathogenomicsServiceDesciption (Service *service_p);

static const char *GetPathogenomicsServiceInformationUri (Service *service_p);

static ParameterSet *GetPathogenomicsServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleasePathogenomicsServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunPathogenomicsService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForPathogenomicsService (Service *service_p, Resource *resource_p, Handler *handler_p);


static PathogenomicsServiceData *AllocatePathogenomicsServiceData (json_t *config_p);

static json_t *GetPathogenomicsServiceResults (Service *service_p, const uuid_t job_id);

static bool ConfigurePathogenomicsService (PathogenomicsServiceData *data_p, const json_t *service_config_p);


static void FreePathogenomicsServiceData (PathogenomicsServiceData *data_p);

static bool ClosePathogenomicsService (Service *service_p);

static bool CleanUpPathogenomicsServiceJob (ServiceJob *job_p);


static uint32 InsertData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *service_data_p, json_t *errors_p);

static const char *InsertSingleItem (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *service_data_p);

static json_t *SearchData (MongoTool *tool_p, json_t *data_p, const char *collection_s, PathogenomicsServiceData *service_data_p, json_t *errors_p);

static uint32 DeleteData (MongoTool *tool_p, json_t *data_p, const char *collection_s, PathogenomicsServiceData *service_data_p, json_t *errors_p);

static bool AddUploadParams (ParameterSet *param_set_p);

/*
 * API FUNCTIONS
 */
 

ServicesArray *GetServices (json_t *config_p)
{
	ServicesArray *services_p = AllocateServicesArray (1);

	if (services_p)
		{
			Service *service_p = GetPathogenomicsService (config_p, 0);

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
	FreePathogenomicsServiceData ((PathogenomicsServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */



static json_t *GetPathogenomicsServiceResults (Service *service_p, const uuid_t job_id)
{
	PathogenomicsServiceData *data_p = (PathogenomicsServiceData *) (service_p -> se_data_p);
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



static Service *GetPathogenomicsService (json_t *operation_json_p, size_t i)
{									
	Service *mongodb_service_p = (Service *) AllocMemory (sizeof (Service));
	
	if (mongodb_service_p)
		{
			PathogenomicsServiceData *data_p = AllocatePathogenomicsServiceData (operation_json_p);
			
			if (data_p)
				{
					const json_t *config_p = NULL;

					InitialiseService (mongodb_service_p,
						GetPathogenomicsServiceName,
						GetPathogenomicsServiceDesciption,
						GetPathogenomicsServiceInformationUri,
						RunPathogenomicsService,
						IsResourceForPathogenomicsService,
						GetPathogenomicsServiceParameters,
						ReleasePathogenomicsServiceParameters,
						ClosePathogenomicsService,
						GetPathogenomicsServiceResults,
						NULL,
						false,
						true,
						(ServiceData *) data_p);

					if ((config_p = GetGlobalServiceConfig (GetPathogenomicsServiceName (mongodb_service_p))) != NULL)
						{
							if (ConfigurePathogenomicsService (data_p, config_p))
								{
									return mongodb_service_p;
								}
						}

					FreePathogenomicsServiceData (data_p);
				}
			
			FreeMemory (mongodb_service_p);
		}		/* if (mongodb_service_p) */
			
	return NULL;
}


static bool ConfigurePathogenomicsService (PathogenomicsServiceData *data_p, const json_t *service_config_p)
{
	bool success_flag = false;
	const char *value_s = GetJSONString (service_config_p, "default_geocoder");

	data_p -> psd_geocoder_uri_s = NULL;

	if (value_s)
		{
			json_t *geocoders_p = json_object_get (service_config_p, "geocoders");

			if (geocoders_p)
				{
					data_p -> psd_geocoder_uri_s = NULL;

					if (json_is_array (geocoders_p))
						{
							const size_t size = json_array_size (geocoders_p);
							size_t i = 0;

							while (i < size)
								{
									json_t *geocoder_p = json_array_get (geocoders_p, i);
									const char *name_s = GetJSONString (geocoder_p, "name");

									if (name_s && (strcmp (name_s, value_s) == 0))
										{
											data_p -> psd_geocoder_uri_s = GetJSONString (geocoder_p, "uri");
											i = size;
										}
									else
										{
											++ i;
										}
								}
						}
					else
						{
							const char *name_s = GetJSONString (geocoders_p, "name");

							if (name_s && (strcmp (name_s, value_s) == 0))
								{
									data_p -> psd_geocoder_uri_s = GetJSONString (geocoders_p, "uri");
								}
						}

					if (data_p -> psd_geocoder_uri_s)
						{
							if (strcmp (value_s, "google") == 0)
								{
									data_p -> psd_geocoder_fn = GetLocationDataByGoogle;
									data_p -> psd_refine_location_fn = RefineLocationDataForGoogle;
								}
							else if (strcmp (value_s, "opencage") == 0)
								{
									data_p -> psd_geocoder_fn = GetLocationDataByOpenCage;
									data_p -> psd_refine_location_fn = RefineLocationDataForOpenCage;
								}
							else
								{
									data_p -> psd_geocoder_fn = NULL;
								}
						}
				}
		}

	if (data_p -> psd_geocoder_fn)
		{
			data_p -> psd_samples_collection_s = GetJSONString (service_config_p, "samples_collection");

			if (data_p -> psd_samples_collection_s)
				{
					data_p -> psd_locations_collection_s = GetJSONString (service_config_p, "locations_collection");

					if (data_p -> psd_locations_collection_s)
						{
							success_flag = true;
						}
				}
		}


	return success_flag;
}

static PathogenomicsServiceData *AllocatePathogenomicsServiceData (json_t *op_json_p)
{
	MongoTool *tool_p = AllocateMongoTool ();
	
	if (tool_p)
		{
			PathogenomicsServiceData *data_p = (PathogenomicsServiceData *) AllocMemory (sizeof (PathogenomicsServiceData));

			if (data_p)
				{
					data_p -> psd_tool_p = tool_p;
					data_p -> psd_geocoder_fn = NULL;
					data_p -> psd_refine_location_fn = NULL;
					data_p -> psd_geocoder_uri_s = NULL;
					data_p -> psd_locations_collection_s = NULL;
					data_p -> psd_samples_collection_s = NULL;

					return data_p;
				}

			FreeMongoTool (tool_p);
		}

	return NULL;
}


static void FreePathogenomicsServiceData (PathogenomicsServiceData *data_p)
{
	FreeMongoTool (data_p -> psd_tool_p);

	FreeMemory (data_p);
}


static const char *GetPathogenomicsServiceName (Service *service_p)
{
	return "Pathogenomics Geoservice";
}


static const char *GetPathogenomicsServiceDesciption (Service *service_p)
{
	return "A service to analyse the spread of Wheat-related diseases both geographically and temporally";
}


static const char *GetPathogenomicsServiceInformationUri (Service *service_p)
{
	return NULL;
}


static ParameterSet *GetPathogenomicsServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
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
											def.st_string_value_s = "samples";

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

	def.st_char_value = s_default_column_delimiter;

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


static void ReleasePathogenomicsServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}




static bool ClosePathogenomicsService (Service *service_p)
{
	bool success_flag = true;
	
	return success_flag;
}


static json_type GetPathogenomicsJSONFieldType (const char *name_s, const void *data_p)
{
	json_type t = JSON_STRING;
	PathogenomicsServiceData *service_data_p = (PathogenomicsServiceData *) (data_p);
	const json_t *config_p = GetGlobalServiceConfig (GetPathogenomicsServiceName (service_data_p -> psd_base_data.sd_service_p));

	if (config_p)
		{
			const json_t *field_defs_p = json_object_get (config_p, "field_defs");

			if (field_defs_p)
				{
					const char *field_def_s = GetJSONString (field_defs_p, name_s);

					if (field_def_s)
						{
							if (strcmp (field_def_s, "int") == 0)
								{
									t = JSON_INTEGER;
								}
							else if (strcmp (field_def_s, "real") == 0)
								{
									t = JSON_REAL;
								}
							else if (strcmp (field_def_s, "bool") == 0)
								{
									t = JSON_TRUE;
								}
						}
				}
		}

	return t;
}



static ServiceJobSet *RunPathogenomicsService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	PathogenomicsServiceData *data_p = (PathogenomicsServiceData *) (service_p -> se_data_p);

	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1, CleanUpPathogenomicsServiceJob);

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

							if (collection_s)
								{
									MongoTool *tool_p = AllocateMongoTool ();

									if (tool_p)
										{
											Parameter *param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_DUMP);
											bool run_flag = false;

											SetMongoToolCollection (tool_p, S_DATABASE_S, collection_s);

											if (param_p && (param_p -> pa_type == PT_BOOLEAN) && (param_p -> pa_current_value.st_boolean_value == true))
												{
													json_t *response_p = GetAllMongoResultsAsJSON (tool_p, NULL);

													if (response_p)
														{
															job_p -> sj_status = OS_SUCCEEDED;
															job_p -> sj_result_p = response_p;
														}
													else
														{
															job_p -> sj_status = OS_FAILED;
															job_p -> sj_result_p = NULL;
														}
												}
											else
												{
													json_t *json_param_p = NULL;
													bool update_flag = false;
													char delimiter = s_default_column_delimiter;

													/* Get the current delimiter */
													param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_DELIMITER);
													if (param_p)
														{
															delimiter = param_p -> pa_current_value.st_char_value;
														}


													param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_FILE);

													if (param_p)
														{
															json_param_p = ConvertTabularDataToJSON (param_p -> pa_current_value.st_string_value_s, delimiter, '\n', GetPathogenomicsJSONFieldType, data_p);

															if (json_param_p)
																{
																	#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
																	PrintJSONToLog (json_param_p, "table", STM_LEVEL_FINE);
																	#endif

																	update_flag = true;
																}
														}

													if (json_param_p)
														{
															uint32 num_successes = 0;
															json_error_t error;
															json_t *errors_p = json_array ();
															json_t *metadata_p = NULL;
															uint32 size = 1;

															if (json_is_array (json_param_p))
																{
																	size = json_array_size (json_param_p);
																}

															job_p -> sj_status = OS_FAILED;

															if (update_flag)
																{
																	num_successes = InsertData (tool_p, json_param_p, collection_s, data_p, errors_p);
																}
															else if ((GetParameterValueFromParameterSet (param_set_p, TAG_UPDATE, &value, true)) && (value.st_json_p))
																{
																	num_successes = InsertData (tool_p, value.st_json_p, collection_s, data_p, errors_p);
																}
															else if ((GetParameterValueFromParameterSet (param_set_p, TAG_QUERY, &value, true)) && (value.st_json_p))
																{
																	json_t *results_p = SearchData (tool_p,  value.st_json_p, collection_s, data_p, errors_p);

																	if (results_p)
																		{
																			if (json_is_array (results_p))
																				{
																					num_successes = json_array_size (results_p);
																					job_p -> sj_result_p = results_p;
																				}

																		}

																	if (!metadata_p)
																		{
																			WipeJSON (errors_p);
																			errors_p = NULL;
																		}

																}
															else if ((GetParameterValueFromParameterSet (param_set_p, TAG_REMOVE, &value, true)) && (value.st_json_p))
																{
																	num_successes = DeleteData (tool_p,  value.st_json_p, collection_s, data_p, errors_p);
																}

															metadata_p = json_pack_ex (&error, 0, "{s:i,s:s,s:o}", "status", num_successes, "collection", collection_s, "errors", errors_p);


															#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
																{
																	char *dump_s = json_dumps (json_param_p, JSON_INDENT (2));
																	PrintLog (STM_LEVEL_FINE, "mongo param: %s", dump_s);
																	free (dump_s);
																}
															#endif


															if (metadata_p)
																{
																	if (errors_p)
																		{
																			if (json_object_set (metadata_p, "errors", errors_p) != 0)
																				{

																				}
																		}


																	job_p -> sj_metadata_p = metadata_p;
																}

															if (num_successes == 0)
																{
																	job_p -> sj_status = OS_FAILED;
																}
															else if (num_successes == size)
																{
																	job_p -> sj_status = OS_SUCCEEDED;
																}
															else
																{
																	job_p -> sj_status = OS_PARTIALLY_SUCCEEDED;
																}


															if (errors_p)
																{
																	WipeJSON (errors_p);
																}


															if (json_param_p != value.st_json_p)
																{
																	WipeJSON (json_param_p);
																}

														}		/* if (json_param_p) */

												}		/* if (param_p && (param_p -> pa_type == PT_BOOLEAN) && (param_p -> pa_current_value.st_boolean_value == true)) else */

										}		/* if (tool_p) */
								}		/* if (collection_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, "no collection specified");
								}

						}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_COLLECTION, &value, true)) */

				}		/* if (param_set_p) */

		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}



static json_t *SearchData (MongoTool *tool_p, json_t *data_p, const char *collection_s, PathogenomicsServiceData *service_data_p, json_t *errors_p)
{
	json_t *results_p = NULL;
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

			if (FindMatchingMongoDocumentsByJSON (tool_p, values_p, fields_ss))
				{
					json_t *raw_results_p = GetAllExistingMongoResultsAsJSON (tool_p);

					if (raw_results_p)
						{
							if (json_is_array (raw_results_p))
								{
									results_p = json_array ();

									if (results_p)
										{
											const size_t size = json_array_size (raw_results_p);
											size_t i = 0;
											json_t *raw_result_p = NULL;

											for (i = 0; i < size; ++ i)
												{
													raw_result_p = json_array_get (raw_results_p, i);
													char *title_s = ConvertNumberToString ((double) i, -1);

													if (title_s)
														{
															json_t *resource_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, title_s, raw_result_p);

															if (resource_p)
																{
																	if (json_array_append_new (results_p, resource_p) != 0)
																		{

																		}
																}

															FreeCopiedString (title_s);
														}
												}
										}
								}
						}
				}

			if (fields_ss)
				{
					FreeMemory (fields_ss);
				}
		}		/* if (values_p) */

 	return results_p;
}


static char *CheckDataIsValid (const json_t *row_p, PathogenomicsServiceData *data_p)
{
	char *errors_s = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			/*
			 * Each row the pathogenmics must contain an ID field
			 * not the mongo _id, and the ability to get a geojson
			 * stub and a date.
			 */

			if (!json_object_get (row_p, PG_ID_S))
				{
					if (!AppendStringsToByteBuffer (buffer_p, "The row does not have an ",  PG_ID_S,  " field", NULL))
						{
						}
				}

			// http://api.opencagedata.com/geocode/v1/geojson?pretty=1&key=1a9d04b5e924fd52d1f306d924d023a5&query=Osisek,+Croatia'



			if (GetByteBufferSize (buffer_p) > 0)
				{
					errors_s = DetachByteBufferData (buffer_p);
				}
		}		/* if (buffer_p) */


	return errors_s;
}



static bool InsertLocationData (MongoTool *tool_p, const json_t *row_p, PathogenomicsServiceData *data_p, const char *id_s)
{
	bool success_flag = false;
	json_t *location_data_p = data_p -> psd_geocoder_fn (data_p, row_p, id_s);

	if (location_data_p)
		{
			json_error_t error;
			json_t *row_json_p = json_pack_ex (&error, 0, "{s:s,s:o}", PG_ID_S, id_s, PG_LOCATION_S, location_data_p);

			if (row_json_p)
				{
					bson_oid_t *id_p = NULL;

					#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
					PrintJSONToLog (row_json_p, "location data:", STM_LEVEL_FINE);
					#endif

					if (SetMongoToolCollection (tool_p, S_DATABASE_S, data_p -> psd_locations_collection_s))
						{
							json_t *results_p = NULL;
							bson_t query;

							bson_init (&query);
							bson_append_utf8 (&query, PG_ID_S, -1, id_s, -1);

							results_p = GetAllMongoResultsAsJSON (tool_p, &query);

							/* does the id already exist? */
							if (results_p && ((json_is_array (results_p)) && (json_array_size (results_p) >= 0)))
								{
									bson_oid_t oid;

									#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
									PrintJSONToLog (results_p, "results:", STM_LEVEL_FINE);
									#endif

									success_flag = UpdateMongoDocument (tool_p, &oid, row_json_p);
									WipeJSON (results_p);
								}		/* if (results_p) */
							else
								{
									id_p = InsertJSONIntoMongoCollection (tool_p, row_json_p);

									if (id_p)
										{
											success_flag = true;
											FreeMemory (id_p);
										}
								}

						}		/* if (SetMongoToolCollection (tool_p, S_DATABASE_S, data_p -> psd_geojson_collection_s)) */

					WipeJSON (row_json_p);
				}		/* if (row_json_p) */
			else
				{
					WipeJSON (location_data_p);
				}

		}		/* if (location_data_p) */


	return success_flag;
}



static bool AddErrorMessage (json_t *errors_p, const json_t *values_p)
{
	bool success_flag = false;
	const char *pathogenomics_id_s = GetJSONString (values_p, PG_ID_S);

	if (pathogenomics_id_s)
		{
			json_t *error_p = json_object ();

			if (error_p)
				{
					if (json_object_set_new (error_p, "ID", json_string (pathogenomics_id_s)) == 0)
						{
							if (json_array_append_new (errors_p, error_p) == 0)
								{
									success_flag = true;
								}
						}
				}
		}

	return success_flag;
}


static uint32 InsertData (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p, json_t *errors_p)
{
	uint32 num_imports = 0;

	if (json_is_array (values_p))
		{
			json_t *value_p;
			size_t i;

			json_array_foreach (values_p, i, value_p)
				{
					const char *error_s = InsertSingleItem (tool_p, value_p, collection_s, data_p);

					if (error_s)
						{
							if (!AddErrorMessage (errors_p, values_p))
								{

								}
							PrintErrors (STM_LEVEL_WARNING, "%s", error_s);
						}
					else
						{
							++ num_imports;
						}
				}
		}
	else
		{
			const char *error_s = InsertSingleItem (tool_p, values_p, collection_s, data_p);

			if (error_s)
				{
					if (!AddErrorMessage (errors_p, values_p))
						{

						}
					PrintErrors (STM_LEVEL_WARNING, "%s", error_s);
				}
			else
				{
					++ num_imports;
				}
		}

	return num_imports;
}




static const char *InsertSingleItem (MongoTool *tool_p, json_t *values_p, const char *collection_s, PathogenomicsServiceData *data_p)
{
	const char *error_s = NULL;
	bool add_fields_flag = false;

	const char *pathogenomics_id_s = GetJSONString (values_p, PG_ID_S);

	if (pathogenomics_id_s)
		{
			if (ConvertDate (values_p))
				{

					if (InsertLocationData (tool_p, values_p, data_p, pathogenomics_id_s))
						{
							/**
							 * Is it an insert or an update?
							 */
							const char *id_s = GetJSONString (values_p, MONGO_ID_S);

							if (SetMongoToolCollection (tool_p, S_DATABASE_S, data_p -> psd_samples_collection_s))
								{
									if (id_s)
										{
											bson_oid_t oid;

											if (bson_oid_is_valid (id_s, strlen (id_s)))
												{
													bson_oid_init_from_string (&oid, id_s);

													if (json_object_del (values_p, MONGO_ID_S) == 0)
														{
															if (!UpdateMongoDocument (tool_p, &oid, values_p))
																{
																	error_s = "Failed to update data";
																}
														}

												}
										}
									else
										{
											bson_oid_t *id_p = InsertJSONIntoMongoCollection (tool_p, values_p);

											if (id_p)
												{
													FreeMemory (id_p);
												}
											else
												{
													error_s = "Failed to insert data";
												}
										}

								}


							if ((error_s == NULL) && add_fields_flag)
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

																					#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
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

																									#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
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
																							error_s = "Failed to insert data into fields collection";
																						}
																				}
																			else
																				{
																					if (!UpdateMongoDocument (tool_p, (bson_oid_t *) &doc_id, update_p))
																						{
																							PrintErrors (STM_LEVEL_WARNING, "Failed to update %s in %s", key_s, fields_collection_s);
																							error_s = "Failed to update data in fields collection";
																						}
																				}
																		}
																	else
																		{

																			PrintErrors (STM_LEVEL_WARNING, "Failed to create json for updating fields collection %s,  %s", fields_collection_s, error.text);
																			error_s = "Failed to create JSON data to update fields collection";
																		}

																	bson_destroy (query_p);
																}
														}		/* json_object_foreach (values_p, key_s, value_p) */


												}		/* if (field_p) */


											FreeCopiedString (fields_collection_s);
										}		/* if (fields_collection_s) */

								}

						}		/* if (InsertLocationData (tool_p, values_p, data_p)) */
					else
						{
							error_s = "Failed to add location data into system";
						}

				}		/* if (ConvertDate (values_p)) */
			else
				{
					error_s = "Could not get date";
				}

		}		/* if (pathogenomics_id_s) */
	else
		{
			error_s = "Could not get pathogenomics id";
		}

	return error_s;
}
	


static uint32 DeleteData (MongoTool *tool_p, json_t *data_p, const char *collection_s, PathogenomicsServiceData *service_data_p, json_t *errors_p)
{
	bool success_flag = false;
	json_t *selector_p = json_object_get (data_p, MONGO_OPERATION_DATA_S);

	if (selector_p)
		{
			success_flag = RemoveMongoDocuments (tool_p, selector_p, false);
		}		/* if (values_p) */

	return success_flag ? 1 : 0;
}


static bool IsResourceForPathogenomicsService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;

	if (resource_p -> re_protocol_s)
		{
			interested_flag = (strcmp (resource_p -> re_protocol_s, "string") == 0);
		}

	return interested_flag;
}


static bool CleanUpPathogenomicsServiceJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;

	return cleaned_up_flag;
}


