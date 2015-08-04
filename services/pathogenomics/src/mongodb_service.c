#include <string.h>

#include "jansson.h"

#define ALLOCATE_PATHOGENOMICS_TAGS
#include "mongodb_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "service_job.h"
#include "mongodb_tool.h"
#include "string_utils.h"
#include "json_tools.h"
#include "wheatis_config.h"
#include "country_codes.h"

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

static const char s_default_column_delimiter =  '|';

typedef struct MongoDBServiceData
{
	ServiceData bsd_base_data;

	MongoTool *msd_tool_p;

	const char *msd_geocoding_uri_s;
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


static uint32 InsertData (MongoTool *tool_p, json_t *values_p, const char *collection_s);

static bool InsertSingleItem (MongoTool *tool_p, json_t *values_p, const char *collection_s);

static uint32 SearchData (MongoTool *tool_p, json_t *data_p, const char *collection_s);

static uint32 DeleteData (MongoTool *tool_p, json_t *data_p, const char *collection_s);

static LinkedList *GetTableRow (const char **data_ss, const char delimiter, ByteBuffer *buffer_p, bool empty_strings_allowed_flag);

static int32 UploadDelimitedTable (MongoTool *tool_p,  const char *data_s, const char delimiter);

static bool AddUploadParams (ParameterSet *param_set_p);

static json_t *GetLocationData (const json_t *row_p, MongoDBServiceData *data_p);

static bool ConvertDate (json_t *row_p);

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
			MongoDBServiceData *data_p = AllocateMongoDBServiceData (operation_json_p);
			
			if (data_p)
				{
					const json_t *config_p = NULL;

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
						(ServiceData *) data_p);

					if ((config_p = GetGlobalServiceConfig (GetMongoDBServiceName (mongodb_service_p))) != NULL)
						{
							data_p -> msd_geocoding_uri_s = GetJSONString (config_p, "geocoding_uri");

							if (data_p -> msd_geocoding_uri_s)
								{
									return mongodb_service_p;
								}
						}

					FreeMongoDBServiceData (data_p);
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
					data_p -> msd_geocoding_uri_s = NULL;

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

							if (collection_s)
								{
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
													uint32 (*data_fn) (MongoTool *tool_p, json_t *data_p, const char *collection_s) = NULL;
													json_t *json_param_p = NULL;
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
															json_param_p = ConvertTabularDataToJSON (param_p -> pa_current_value.st_string_value_s, delimiter, '\n');

															if (json_param_p)
																{
																	#if MONGODB_SERVICE_DEBUG >= STM_LEVEL_FINE
																	PrintJSONToLog (json_param_p, "table", STM_LEVEL_FINE);
																	#endif

																	data_fn = InsertData;
																}
														}

													if (json_param_p)
														{
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
																	uint32 num_successes = data_fn (tool_p, json_param_p, collection_s);

																	#if MONGODB_SERVICE_DEBUG >= STM_LEVEL_FINE
																		{
																			char *dump_s = json_dumps (json_param_p, JSON_INDENT (2));
																			PrintLog (STM_LEVEL_FINE, "mongo param: %s", dump_s);
																			free (dump_s);
																		}
																	#endif

																	response_p = json_pack_ex (&error, 0, "{s:i,s:s,s:o}", "status", num_successes, "collection", collection_s, "results", data_p);

																	if (response_p)
																		{
																			job_p -> sj_status = OS_SUCCEEDED;
																		}
																	else
																		{
																			job_p -> sj_status = OS_FAILED;
																		}

																}

															if (json_param_p != value.st_json_p)
																{
																	WipeJSON (json_param_p);
																}

														}		/* if (json_param_p) */

												}

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



static uint32 SearchData (MongoTool *tool_p, json_t *data_p, const char *collection_s)
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

 	return success_flag ? 1 : 0;;
}


static char *CheckDataIsValid (const json_t *row_p, MongoDBServiceData *data_p)
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


			/*
			 * Convert the date into YYYYMMDD format to allow us to get ranges using them as
			 * numbers
			 */


		}		/* if (buffer_p) */



	return errors_s;
}


static bool ConvertDate (json_t *row_p)
{
	bool success_flag = false;
	const char *date_s = GetJSONString (row_p, PG_DATE_S);
	char buffer_s [9];

	if (date_s)
		{
			/* Is it DD/MM/YYYY */
			if ((strlen (date_s) == 10) && (* (date_s + 2) == '/') && (* (date_s + 5) == '/'))
				{
					memcpy (buffer_s, date_s + 6, 4 * sizeof (char));
					memcpy (buffer_s + 4, date_s + 3, 2 * sizeof (char));
					memcpy (buffer_s + 6, date_s, 2 * sizeof (char));

					* (buffer_s + 8) = '\0';
					success_flag = true;
				}
			else
				{
					#define NUM_MONTHS (12)
					const char *months_ss [NUM_MONTHS] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };

					bool loop_flag = true;
					uint month_index = 0;
					uint8 i = 0;

					while (loop_flag)
						{
							const char *month_s = * (months_ss + i);

							if (Strnicmp (month_s, date_s, strlen (month_s)) == 0)
								{
									month_index = i;
									loop_flag = false;
								}
							else
								{
									++ i;

									if (i == NUM_MONTHS)
										{
											loop_flag = false;
										}
								}

						}		/* while (loop_flag) */

					if (month_index > 0)
						{
							const char *value_p = date_s + strlen (* (months_ss + month_index));

							loop_flag = (*value_p != '\0');

							while (loop_flag)
								{
									if (isdigit (*value_p))
										{
											loop_flag = false;
										}
									else
										{
											++ value_p;

											loop_flag = (*value_p != '\0');
										}
								}

							if (*value_p != '\0')
								{
									/* Do we have a number? */
									if (isdigit (* (value_p + 1)))
										{
											bool match_flag = false;

											/* Do we have a 2 digit year... */
											if (* (value_p + 2) == '\0')
												{
													*buffer_s = '2';
													* (buffer_s + 1) = '0';

													memcpy (buffer_s + 2, value_p, 2 * sizeof (char));
													match_flag = true;
												}
											/* ... or a 4 digit one? */
											else if ((isdigit (* (value_p + 2))) && (isdigit (* (value_p + 3))))
												{
													memcpy (buffer_s, value_p, 4 * sizeof (char));
													match_flag = true;
												}

											if (match_flag)
												{
													char month_s [3];

													sprintf (month_s, "%02d", month_index + 1);
													memcpy (buffer_s + 4, month_s, 2 * sizeof (char));

													* (buffer_s + 6) = '0';
													* (buffer_s + 7) = '1';
													* (buffer_s + 8) = '\0';
													success_flag = true;
												}

										}
								}
						}		/* if (month_index > 0) */
				}

			if (success_flag)
				{
					if (json_object_set_new (row_p, PG_DATE_S, json_string (buffer_s)) != 0)
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to set date to %s", buffer_s);
							success_flag = false;
						}
				}

			if (!success_flag)
				{
					char *dump_s = json_dumps (row_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

					PrintErrors (STM_LEVEL_WARNING, "Failed to convert date from %s for %s", date_s, dump_s);

					free (dump_s);
				}
		}		/* if (date_s) */
	else
		{
			char *dump_s = json_dumps (row_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

			PrintErrors (STM_LEVEL_WARNING, "No date for %s", date_s, dump_s);

			free (dump_s);
		}






	return success_flag;
}


static bool InsertLocationData (MongoTool *tool_p, const json_t *row_p, MongoDBServiceData *data_p)
{
	bool success_flag = false;
	const char *id_s = GetJSONString (row_p, PG_ID_S);

	if (id_s)
		{
			json_t *location_data_p = GetLocationData (row_p, data_p);

			if (location_data_p)
				{
					json_error_t error;
					json_t *row_json_p = json_pack_ex (&error, 0, "{s:s,s:o}", PG_ID_S, id_s, PG_GEOJSON_S, location_data_p);

					if (row_json_p)
						{
							bson_oid_t *id_p = InsertJSONIntoMongoCollection (tool_p, row_json_p);

							if (id_p)
								{

									FreeMemory (id_p);
								}

							WipeJSON (location_data_p);
						}
					WipeJSON (location_data_p);
				}		/* if (location_data_p) */

		}		/* if (id_s) */

	return success_flag;
}


static json_t *GetLocationData (const json_t *row_p, MongoDBServiceData *data_p)
{
	json_t *res_p = NULL;

	const char *gps_s = GetJSONString (row_p, PG_GPS_S);

	if (gps_s)
		{

		}
	else
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);

			if (buffer_p)
				{
					if (AppendStringToByteBuffer (buffer_p, data_p -> msd_geocoding_uri_s))
						{
							const char *value_s = GetJSONString (row_p, PG_TOWN_S);
							bool success_flag = true;

							/* town */
							if (value_s)
								{
									success_flag = AppendStringToByteBuffer (buffer_p, value_s);
								}

							/* county */
							if (success_flag)
								{
									value_s = GetJSONString (row_p, PG_COUNTY_S);

									if (value_s)
										{
											success_flag = AppendStringsToByteBuffer (buffer_p, ", ", value_s, NULL);
										}		/* if (value_s) */

								}		/* if (success_flag) */


							/* country */
							if (success_flag)
								{
									const char *country_code_s = NULL;
									value_s = GetJSONString (row_p, PG_COUNTRY_S);

									country_code_s = GetCountryCodeFromName (value_s);

									if (country_code_s)
										{
											success_flag = AppendStringsToByteBuffer (buffer_p, "&countrycode=", country_code_s, NULL);
										}

								}		/* if (success_flag) */


							if (success_flag)
								{
									CurlTool *curl_tool_p = AllocateCurlTool ();

									if (curl_tool_p)
										{
											const char *uri_s = GetByteBufferData (buffer_p);

											if (SetUriForCurlTool (curl_tool_p, uri_s))
												{
													CURLcode c = RunCurlTool (curl_tool_p);

													if (c == CURLE_OK)
														{
															const char *response_s = GetCurlToolData (curl_tool_p);

															if (response_s)
																{
																	json_error_t error;

																	res_p = json_loads (response_s, 0, &error);

																	if (!res_p)
																		{

																		}
																}
														}
													else
														{

														}
												}


											FreeCurlTool (curl_tool_p);
										}		/* if (curl_tool_p) */

								}

						}		/* if (AppendStringToByteBuffer (buffer_p, data_p -> msd_geocoding_uri_s)) */

					FreeByteBuffer (buffer_p);
				}		/* if (buffer_p) */

		}


	return res_p;
}


static uint32 InsertData (MongoTool *tool_p, json_t *values_p, const char *collection_s)
{
	uint32 num_imports = 0;

	if (json_is_array (values_p))
		{
			json_t *value_p;
			size_t i;

			json_array_foreach (values_p, i, value_p)
				{
					if (ConvertDate (value_p))
						{
							if (InsertSingleItem (tool_p, value_p, collection_s))
								{
									++ num_imports;
								}
						}
				}
		}
	else
		{
			if (ConvertDate (values_p))
				{
					if (InsertSingleItem (tool_p, values_p, collection_s))
						{
							++ num_imports;
						}
				}
		}

	return num_imports;
}


static bool InsertSingleItem (MongoTool *tool_p, json_t *values_p, const char *collection_s)
{
	bool success_flag = false;
	bool add_fields_flag = false;

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

	if (success_flag && add_fields_flag)
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
	


static uint32 DeleteData (MongoTool *tool_p, json_t *data_p, const char *collection_s)
{
	bool success_flag = false;
	json_t *selector_p = json_object_get (data_p, MONGO_OPERATION_DATA_S);

	if (selector_p)
		{
			success_flag = RemoveMongoDocuments (tool_p, selector_p, false);
		}		/* if (values_p) */

	return success_flag ? 1 : 0;
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
