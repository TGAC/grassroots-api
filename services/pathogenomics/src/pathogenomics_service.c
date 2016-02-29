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
#include "grassroots_config.h"
#include "country_codes.h"
#include "sample_metadata.h"
#include "phenotype_metadata.h"
#include "genotype_metadata.h"
#include "string_linked_list.h"
#include "math_utils.h"
#include "search_options.h"
#include "time_util.h"


#ifdef _DEBUG
	#define PATHOGENOMICS_SERVICE_DEBUG	(STM_LEVEL_FINER)
#else
	#define PATHOGENOMICS_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


#define TAG_UPDATE MAKE_TAG('P', 'G', 'U', 'P')
#define TAG_QUERY MAKE_TAG('P', 'G', 'Q', 'U')
#define TAG_REMOVE MAKE_TAG('P', 'G', 'R', 'M')
#define TAG_DUMP MAKE_TAG('P', 'G', 'D', 'P')
#define TAG_PREVIEW MAKE_TAG('P', 'G', 'P', 'V')
#define TAG_COLLECTION MAKE_TAG('P', 'G', 'C', 'O')
#define TAG_DATABASE MAKE_TAG('P', 'G', 'D', 'B')
#define TAG_DELIMITER MAKE_TAG('P', 'G', 'D', 'L')
#define TAG_FILE MAKE_TAG('P', 'G', 'F', 'I')



typedef enum
{
	PD_SAMPLE,
	PD_PHENOTYPE,
	PD_GENOTYPE,
	PD_NUM_TYPES
} PathogenomicsData;


static const char *s_data_names_pp [PD_NUM_TYPES] =
{
	PG_SAMPLE_S,
	PG_PHENOTYPE_S,
	PG_GENOTYPE_S
};



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

static  ParameterSet *IsResourceForPathogenomicsService (Service *service_p, Resource *resource_p, Handler *handler_p);


static PathogenomicsServiceData *AllocatePathogenomicsServiceData (json_t *config_p);

static json_t *GetPathogenomicsServiceResults (Service *service_p, const uuid_t job_id);

static bool ConfigurePathogenomicsService (PathogenomicsServiceData *data_p, const json_t *service_config_p);


static void FreePathogenomicsServiceData (PathogenomicsServiceData *data_p);

static bool ClosePathogenomicsService (Service *service_p);


static uint32 InsertData (MongoTool *tool_p, json_t *values_p, const PathogenomicsData collection_type, PathogenomicsServiceData *service_data_p, json_t *errors_p);


static json_t *SearchData (MongoTool *tool_p, json_t *data_p, const PathogenomicsData collection_type, PathogenomicsServiceData *service_data_p, json_t *errors_p, const bool preview_flag);


static uint32 DeleteData (MongoTool *tool_p, json_t *data_p, const PathogenomicsData collection_type, PathogenomicsServiceData *service_data_p, json_t *errors_p);

static bool AddUploadParams (ParameterSet *param_set_p);

static bool GetCollectionName (ParameterSet *param_set_p, PathogenomicsServiceData *data_p, const char **collection_name_ss, PathogenomicsData *collection_type_p);

static bool AddLiveDateFiltering (json_t *record_p, const char * const date_s);


static json_t *ConvertToResource (const size_t i, json_t *src_record_p);

static json_t *CopyValidRecord (const size_t i, json_t *src_record_p);

static json_t *FilterResultsByDate (json_t *src_results_p, const bool preview_flag, json_t *(convert_record_fn) (const size_t i, json_t *src_record_p));



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
	ClosePathogenomicsService (service_p);
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */



static json_t *GetPathogenomicsServiceResults (Service *service_p, const uuid_t job_id)
{
	PathogenomicsServiceData *data_p = (PathogenomicsServiceData *) (service_p -> se_data_p);
	ServiceJob *job_p = GetServiceJobFromServiceJobSetById (service_p -> se_jobs_p, job_id);
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


	data_p -> psd_database_s = GetJSONString (service_config_p, "database");

	if (data_p -> psd_database_s)
		{
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
										}
									else if (strcmp (value_s, "opencage") == 0)
										{
											data_p -> psd_geocoder_fn = GetLocationDataByOpenCage;
										}
									else
										{
											data_p -> psd_geocoder_fn = NULL;
										}
								}
						}
				}		/* if (value_s) */

			if (data_p -> psd_geocoder_fn)
				{
					data_p -> psd_samples_collection_s = GetJSONString (service_config_p, "samples_collection");

					if (data_p -> psd_samples_collection_s)
						{
							data_p -> psd_locations_collection_s = GetJSONString (service_config_p, "locations_collection");

							if (data_p -> psd_locations_collection_s)
								{
									data_p -> psd_phenotype_collection_s = GetJSONString (service_config_p, "phenotypes_collection");

									if (data_p -> psd_phenotype_collection_s)
										{
											data_p -> psd_genotype_collection_s = GetJSONString (service_config_p, "genotypes_collection");

											if (data_p -> psd_genotype_collection_s)
												{
													success_flag = true;
												}
										}
								}
						}
				}

		} /* if (data_p -> psd_database_s) */

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
					data_p -> psd_geocoder_uri_s = NULL;
					data_p -> psd_database_s = NULL;
					data_p -> psd_locations_collection_s = NULL;
					data_p -> psd_samples_collection_s = NULL;
					data_p -> psd_phenotype_collection_s = NULL;

					return data_p;
				}

			FreeMongoTool (tool_p);
		}

	return NULL;
}


static bool AddIndexes (PathogenomicsServiceData *data_p)
{
	bool success_flag = true;
	bson_t keys;
	bson_error_t error;
	mongoc_index_opt_t opt;

	mongoc_index_opt_init (&opt);

	bson_init (&keys);
	BSON_APPEND_UTF8 (&keys, PG_VARIETY_S, "text");
	mongoc_collection_create_index (data_p -> psd_tool_p -> mt_collection_p, &keys, &opt, &error);

	bson_reinit (&keys);
	BSON_APPEND_UTF8 (&keys, PG_DISEASE_S, "text");
	mongoc_collection_create_index (data_p -> psd_tool_p -> mt_collection_p, &keys, &opt, &error);

	return success_flag;
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
									def.st_boolean_value = false;

									if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_BOOLEAN, false, "dump", "Dump", "Get all of the data in the system", TAG_DUMP, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
										{
											if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_BOOLEAN, false, "preview", "Preview", "Ignore the live dates", TAG_PREVIEW, NULL, def, NULL, NULL, PL_ADVANCED, NULL)) != NULL)
												{
													ParameterMultiOptionArray *options_p = NULL;
													SharedType values [PD_NUM_TYPES];
													uint32 i;

													for (i = 0; i < PD_NUM_TYPES; ++ i)
														{
															values [i].st_string_value_s = (char *) s_data_names_pp [i];
														}

													options_p = AllocateParameterMultiOptionArray (PD_NUM_TYPES, s_data_names_pp, values, PT_STRING, true);

													if (options_p)
														{
															def.st_string_value_s = values [0].st_string_value_s;

															if ((param_p = CreateAndAddParameterToParameterSet (params_p, PT_STRING, false, "collection", "Collection", "The collection to act upon", TAG_COLLECTION, options_p, def, NULL, NULL, PL_ALL, NULL)) != NULL)
																{
																	if (AddUploadParams (params_p))
																		{
																			return params_p;
																		}
																}

															FreeParameterMultiOptionArray (options_p);
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

					if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, NULL, grouped_params_pp, num_group_params))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
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

	FreePathogenomicsServiceData ((PathogenomicsServiceData *) (service_p -> se_data_p));;

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


static bool GetCollectionName (ParameterSet *param_set_p, PathogenomicsServiceData *data_p, const char **collection_name_ss, PathogenomicsData *collection_type_p)
{
	bool success_flag = false;
	SharedType value;

	if (GetParameterValueFromParameterSet (param_set_p, TAG_COLLECTION, &value, true))
		{
			const char *collection_s = value.st_string_value_s;

			if (collection_s)
				{
					if (strcmp (collection_s, s_data_names_pp [PD_SAMPLE]) == 0)
						{
							*collection_name_ss = data_p -> psd_samples_collection_s;
							*collection_type_p = PD_SAMPLE;
							success_flag = true;
						}
					else if (strcmp (collection_s, s_data_names_pp [PD_PHENOTYPE]) == 0)
						{
							*collection_name_ss = data_p -> psd_phenotype_collection_s;
							*collection_type_p = PD_PHENOTYPE;
							success_flag = true;
						}
					else if (strcmp (collection_s, s_data_names_pp [PD_GENOTYPE]) == 0)
						{
							*collection_name_ss = data_p -> psd_genotype_collection_s;
							*collection_type_p = PD_GENOTYPE;
							success_flag = true;
						}
				}
		}

	return success_flag;
}


static ServiceJobSet *RunPathogenomicsService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	PathogenomicsServiceData *data_p = (PathogenomicsServiceData *) (service_p -> se_data_p);

	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Pathogenomics");

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);

			job_p -> sj_status = OS_FAILED_TO_START;

			if (param_set_p)
				{
					/* get the collection to work on */
					const char *collection_name_s = NULL;
					bool preview_flag = false;
					PathogenomicsData collection_type = PD_NUM_TYPES;
					Parameter *param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_PREVIEW);

					if (param_p && (param_p -> pa_type == PT_BOOLEAN))
						{
							preview_flag = param_p -> pa_current_value.st_boolean_value;
						}

					if (GetCollectionName (param_set_p, data_p, &collection_name_s, &collection_type))
						{
							MongoTool *tool_p = data_p -> psd_tool_p;

							if (tool_p)
								{
									param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_DUMP);

									SetMongoToolCollection (tool_p, data_p -> psd_database_s, collection_name_s);

									/* Do we want to get a dump of the entire collection? */
									if (param_p && (param_p -> pa_type == PT_BOOLEAN) && (param_p -> pa_current_value.st_boolean_value == true))
										{
											json_t *errors_p = json_array ();
											json_t *raw_results_p = GetAllMongoResultsAsJSON (tool_p, NULL);

											if (raw_results_p)
												{
													if (!preview_flag)
														{
															raw_results_p = FilterResultsByDate (raw_results_p, preview_flag, CopyValidRecord);
														}

													PrintJSONToLog (raw_results_p, "dump: ", STM_LEVEL_FINER, __FILE__, __LINE__);

													if (raw_results_p)
														{
															json_t *dest_results_p = json_array ();

															if (dest_results_p)
																{
																	size_t i;
																	json_t *src_record_p;

																	json_array_foreach (raw_results_p, i, src_record_p)
																		{
																			char *title_s = ConvertNumberToString ((double) i, -1);
																			json_t *dest_record_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, title_s, src_record_p);

																			if (dest_record_p)
																				{
																					if (json_array_append_new (dest_results_p, dest_record_p) != 0)
																						{
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json_array_append_new failed");

																							if (errors_p)
																								{
																									if (json_array_append_new (errors_p, json_string ("Failed to add document to result")) != 0)
																										{
																											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set job error data");
																										}
																								}
																							json_decref (dest_record_p);
																						}

																				}		/* if (dest_record_p) */
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetResourceAsJSONByParts failed");

																					if (errors_p)
																						{
																							if (json_array_append_new (errors_p, json_string ("Failed to get document to add to result")) != 0)
																								{
																									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set job error data");
																								}
																						}
																				}

																			if (title_s)
																				{
																					FreeCopiedString (title_s);
																				}

																		}		/* json_array_foreach (raw_results_p, i, src_record_p) */


																	i = json_array_size (dest_results_p);

																	if (i == json_array_size (raw_results_p))
																		{
																			job_p -> sj_status = OS_SUCCEEDED;
																			job_p -> sj_result_p = dest_results_p;
																		}
																	else if (i > 0)
																		{
																			job_p -> sj_status = OS_PARTIALLY_SUCCEEDED;
																			job_p -> sj_result_p = dest_results_p;
																		}
																	else
																		{
																			job_p -> sj_status = OS_FAILED;
																			json_decref (dest_results_p);
																		}

																	PrintJSONToLog (dest_results_p, "dest_results_p: ", STM_LEVEL_FINER, __FILE__, __LINE__);

																}		/* if (dest_results_p) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create results array");

																	if (errors_p)
																		{
																			if (json_array_append_new (errors_p, json_string ("Failed to create results")) != 0)
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set job error data");
																				}
																		}
																}

															json_decref (raw_results_p);
														}		/* if (raw_results_p) */
													else
														{
															/* The call succeeded but all results were filtered out */
															job_p -> sj_status = OS_SUCCEEDED;
														}


												}		/* if (dump_p) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetAllMongoResultsAsJSON for \"%s\".\"%s\" failed", data_p -> psd_database_s, collection_name_s);

													if (errors_p)
														{
															if (json_array_append_new (errors_p, json_string ("Failed to export data collection")) != 0)
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set job error data");
																}
														}
												}

											if (errors_p)
												{
													if (json_array_size (errors_p) > 0)
														{
															job_p -> sj_errors_p = errors_p;
														}
													else
														{
															json_decref (errors_p);
														}

												}		/* if (errors_p) */

										}		/* if (param_p && (param_p -> pa_type == PT_BOOLEAN) && (param_p -> pa_current_value.st_boolean_value == true)) */
									else
										{
											json_t *json_param_p = NULL;
											char delimiter = s_default_column_delimiter;
											const char *param_name_s = NULL;
											json_t *errors_p = json_array ();
											uint32 num_successes = 0;
											bool free_json_param_flag = false;

											/* Get the current delimiter */
											param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_DELIMITER);
											if (param_p)
												{
													delimiter = param_p -> pa_current_value.st_char_value;
												}

											/*
											 * Data can be updated either from the table or as a json insert statement
											 *
											 * Has a tabular dataset been uploaded...
											 */
											param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_FILE);
											if (param_p && (!IsStringEmpty (param_p -> pa_current_value.st_string_value_s)))
												{
													json_param_p = ConvertTabularDataToJSON (param_p -> pa_current_value.st_string_value_s, delimiter, '\n', GetPathogenomicsJSONFieldType, data_p);

													if (json_param_p)
														{
															#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
															PrintJSONToLog (json_param_p, "table", STM_LEVEL_FINE, __FILE__, __LINE__);
															#endif

															param_name_s = param_p -> pa_name_s;
															free_json_param_flag = true;
														}
												}
											/* ... or do we have an insert statement? */
											else if (((param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_UPDATE)) != NULL) && (!IsJSONEmpty (param_p -> pa_current_value.st_json_p)))
												{
													json_param_p = param_p -> pa_current_value.st_json_p;
													param_name_s = param_p -> pa_name_s;
												}

											/* Are we doing an update? */
											if (json_param_p)
												{
													uint32 size = 1;

													if (json_is_array (json_param_p))
														{
															size = json_array_size (json_param_p);
														}

													num_successes = InsertData (tool_p, json_param_p, collection_type, data_p, errors_p);


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

													if (free_json_param_flag)
														{
															WipeJSON (json_param_p);
														}
												}
											else if (((param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_QUERY)) != NULL) && (!IsJSONEmpty (param_p -> pa_current_value.st_json_p)))
												{
													json_t *results_p = NULL;

													json_param_p = param_p -> pa_current_value.st_json_p;

													results_p = SearchData (tool_p, json_param_p, collection_type, data_p, errors_p, preview_flag);

													if (results_p)
														{
															if (json_is_array (results_p))
																{
																	#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINER
																	PrintJSONToLog (results_p, "initial results", STM_LEVEL_FINER, __FILE__, __LINE__);
																	PrintJSONRefCounts (results_p, "initial results", STM_LEVEL_FINER, __FILE__, __LINE__);
																	#endif

																	num_successes = json_array_size (results_p);
																	job_p -> sj_result_p = results_p;
																}
															else
																{
																	char *json_s = json_dumps (results_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

																	if (json_s)
																		{
																			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "SearchData results \"%s\" is not an array", json_s);
																			free (json_s);
																		}
																	else
																		{
																			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "SearchData results is not an array");
																		}

																	json_decref (results_p);
																}
														}

													if (json_array_size (errors_p) == 0)
														{
															WipeJSON (errors_p);
															errors_p = NULL;
														}

													job_p -> sj_status = OS_SUCCEEDED;
													param_name_s = param_p -> pa_name_s;
												}
											else if (((param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_REMOVE)) != NULL) && (!IsJSONEmpty (param_p -> pa_current_value.st_json_p)))
												{
													uint32 size = 1;

													if (json_is_array (json_param_p))
														{
															size = json_array_size (json_param_p);
														}

													json_param_p = param_p -> pa_current_value.st_json_p;
													num_successes = DeleteData (tool_p, json_param_p, collection_type, data_p, errors_p);

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

													param_name_s = param_p -> pa_name_s;
												}

											if (json_param_p)
												{
													json_error_t error;
													json_t *metadata_p = NULL;

													metadata_p = json_pack_ex (&error, 0, "{s:i}", "successful entries", num_successes);


													#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
														{
															PrintJSONToLog (errors_p, "errors: ", STM_LEVEL_FINE, __FILE__, __LINE__);
															PrintJSONToLog (metadata_p, "metadata 1: ", STM_LEVEL_FINE, __FILE__, __LINE__);
														}
													#endif


												if (errors_p)
													{
														bool added_errors_flag = false;
														json_t *errors_array_p = json_array ();

														if (errors_array_p)
															{
																json_t *error_obj_p = json_pack_ex (&error, 0, "{s:s,s:o}", PARAM_NAME_S, param_name_s, JOB_ERRORS_S, errors_p);

																if (error_obj_p)
																	{
																		if (json_array_append_new (errors_array_p, error_obj_p) == 0)
																			{
																				job_p -> sj_errors_p = errors_array_p;
																				added_errors_flag = true;
																			}
																		else
																			{
																				WipeJSON (error_obj_p);
																			}
																	}
															}		/* if (errors_array_p) */

														if (!added_errors_flag)
															{
																WipeJSON (errors_p);

																if (errors_array_p)
																	{
																		WipeJSON (errors_array_p);
																	}
															}
													}

													if (metadata_p)
														{
															job_p -> sj_metadata_p = metadata_p;
														}

													#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
													PrintJSONToLog (job_p -> sj_errors_p, "job errors: ", STM_LEVEL_FINE, __FILE__, __LINE__);
													PrintJSONToLog (job_p -> sj_metadata_p, "job metadata: ", STM_LEVEL_FINE, __FILE__, __LINE__);
													PrintJSONToLog (job_p -> sj_result_p, "job results: ", STM_LEVEL_FINE, __FILE__, __LINE__);
													PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "job status %d", job_p -> sj_status);
													#endif


													/*
													if (json_param_p != value.st_json_p)
														{
															WipeJSON (json_param_p);
														}
													*/
												}		/* if (json_param_p) */

										}		/* if (param_p && (param_p -> pa_type == PT_BOOLEAN) && (param_p -> pa_current_value.st_boolean_value == true)) else */

								}		/* if (tool_p) */
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "no collection specified");
						}

				}		/* if (param_set_p) */

			#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (job_p -> sj_metadata_p, "metadata 3: ", STM_LEVEL_FINE, __FILE__, __LINE__);
			#endif

		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}


static bool AddLiveDateFiltering (json_t *record_p, const char * const date_s)
{
	bool success_flag = false;
	uint32 i;

	for (i = 0; i < PD_NUM_TYPES; ++ i)
		{
			const char * const group_name_s = * (s_data_names_pp + i);
			char *key_s = ConcatenateStrings (group_name_s, PG_LIVE_DATE_SUFFIX_S);

			if (key_s)
				{
					json_t *date_p = json_object_get (record_p, key_s);

					if (date_p)
						{
							const char *live_date_s = GetJSONString (date_p, "date");

							if (live_date_s)
								{
									/*
									 * Although both values are dates as strings, we don't need to convert
									 * them as they are both in YYYY-MM-DD format. So strcmp is enough to
									 * see if the "go live" date has been met.
									 */
									if (strcmp (live_date_s, date_s) > 0)
										{
											if (json_object_del (record_p, group_name_s) == 0)
												{
													success_flag = true;
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to remove %s from record", group_name_s);
												}
										}
									else
										{
											success_flag = true;
										}
								}

							/* Remove the "_live_date" object as any */
							if (json_object_del (record_p, key_s) != 0)
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to remove %s from record", key_s);
								}
						}		/* if (date_p) */

					FreeCopiedString (key_s);
				}		/* if (key_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create key from  \"%s\" and \"%s\"", group_name_s, PG_LIVE_DATE_SUFFIX_S);
				}

		}		/* for (i = 0; i < PD_NUM_TYPES; ++ i) */

	return success_flag;
}


static char *GetCurrentDateAsString (void)
{
	char *date_s = NULL;
	struct tm current_time;

	if (GetCurrentTime (&current_time))
		{
			date_s = GetTimeAsString (&current_time);

			if (!date_s)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert time to a string");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get current time");
		}

	return date_s;
}


static json_t *ConvertToResource (const size_t i, json_t *src_record_p)
{
	json_t *resource_p = NULL;
	char *title_s = ConvertNumberToString ((double) i, -1);

	if (title_s)
		{
			resource_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, title_s, src_record_p);

			FreeCopiedString (title_s);
		}		/* if (raw_result_p) */

	return resource_p;
}


static json_t *CopyValidRecord (const size_t i, json_t *src_record_p)
{
	return json_deep_copy (src_record_p);
}


static json_t *FilterResultsByDate (json_t *src_results_p, const bool preview_flag, json_t *(convert_record_fn) (const size_t i, json_t *src_record_p))
{
	json_t *results_p = json_array ();

	if (results_p)
		{
			char *date_s = GetCurrentDateAsString ();

			if (date_s)
				{
					const size_t size = json_array_size (src_results_p);
					size_t i = 0;

					for (i = 0; i < size; ++ i)
						{
							json_t *src_result_p = json_array_get (src_results_p, i);
							json_t *dest_result_p = NULL;

							/* We don't need to return the internal mongo id so remove it */
							json_object_del (src_result_p, MONGO_ID_S);

							if (!preview_flag)
								{
									if (AddLiveDateFiltering (src_result_p, date_s))
										{
											/*
											 * If the result is non-trivial i.e. has at least one of the sample,
											 * genotype or phenotype, then keep it
											 */
											if ((json_object_get (src_result_p, PG_SAMPLE_S) != NULL) ||
													(json_object_get (src_result_p, PG_PHENOTYPE_S) != NULL) ||
													(json_object_get (src_result_p, PG_GENOTYPE_S) != NULL))
												{
													dest_result_p = convert_record_fn (i, src_result_p);

													if (!dest_result_p)
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert src json \"%s\" " SIZET_FMT " to results array", i);
														}

												}
											else
												{
													#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
													const char *id_s = GetJSONString (src_result_p, PG_ID_S);
													PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Discarding %s after filtering", id_s ? id_s : "");
													#endif
												}
										}
									else
										{
											const char *id_s = GetJSONString (src_result_p, PG_ID_S);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add date filtering for %s", id_s ? id_s : "");
										}		/* if (!AddLiveDateFiltering (values_p, collection_type)) */

								}		/* if (!private_view_flag) */
							else
								{
									dest_result_p = convert_record_fn (i, src_result_p);

									if (!dest_result_p)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert src json \"%s\" " SIZET_FMT " to results array", i);
										}
								}


							if (dest_result_p)
								{
									if (json_array_append_new (results_p, dest_result_p) != 0)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json resource for " SIZET_FMT " to results array", i);
											json_decref (dest_result_p);
										}
								}

						}		/* for (i = 0; i < size; ++ i) */

					FreeCopiedString (date_s);
				}		/* if (date_s) */

		}		/* if (results_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create results object");
		}

	return results_p;
}


static json_t *SearchData (MongoTool *tool_p, json_t *data_p, const PathogenomicsData collection_type, PathogenomicsServiceData *service_data_p, json_t *errors_p, const bool preview_flag)
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

													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get field from %s", dump_s);
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
											char *date_s = NULL;
											bool success_flag = true;

											/*
											 * If we are on the public view, we need to filter
											 * out entries that don't meet the live dates.
											 */
											if (!preview_flag)
												{
													struct tm current_time;

													if (GetCurrentTime (&current_time))
														{
															date_s = GetTimeAsString (&current_time);

															if (!date_s)
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert time to a string");
																	success_flag = false;
																}
														}
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get current time");
															success_flag = false;
														}

												}		/* if (!private_view_flag) */

											if (success_flag)
												{
													for (i = 0; i < size; ++ i)
														{
															raw_result_p = json_array_get (raw_results_p, i);
															char *title_s = ConvertNumberToString ((double) i + 1, -1);

															if (!title_s)
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert " SIZET_FMT " to string for title", i + 1);
																}

															/* We don't need to return the internal mongo id so remove it */
															json_object_del (raw_result_p, MONGO_ID_S);

															if (!preview_flag)
																{
																	if (AddLiveDateFiltering (raw_result_p, date_s))
																		{
																			/*
																			 * If the result is non-trivial i.e. has at least one of the sample,
																			 * genotype or phenotype, then keep it
																			 */
																			if ((json_object_get (raw_result_p, PG_SAMPLE_S) == NULL) &&
																					(json_object_get (raw_result_p, PG_PHENOTYPE_S) == NULL) &&
																					(json_object_get (raw_result_p, PG_GENOTYPE_S) == NULL))
																				{
																					#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
																					const char *id_s = GetJSONString (raw_result_p, PG_ID_S);
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Discarding %s after filtering", id_s ? id_s : "");
																					#endif

																					raw_result_p = NULL;
																				}
																		}
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add date filtering for %d", collection_type);
																			raw_result_p = NULL;
																		}		/* if (!AddLiveDateFiltering (values_p, collection_type)) */
																}		/* if (!private_view_flag) */

															if (raw_result_p)
																{
																	json_t *resource_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, title_s, raw_result_p);

																	if (resource_p)
																		{
																			if (json_array_append_new (results_p, resource_p) == 0)
																				{

																				}
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json resource for " SIZET_FMT " to results array", i);
																					json_decref (resource_p);
																				}
																		}		/* if (resource_p) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create json resource for " SIZET_FMT, i);
																		}

																}		/* if (raw_result_p) */

															if (title_s)
																{
																	FreeCopiedString (title_s);
																}
														}		/* for (i = 0; i < size; ++ i) */

												}		/* if (success_flag) */

											if (date_s)
												{
													FreeCopiedString (date_s);
												}

										}		/* if (results_p) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create results object");
										}
								}		/* if (json_is_array (raw_results_p)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Search results is not an array");
								}

							json_decref (raw_results_p);
						}		/* if (raw_results_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get raw results from search");
						}
				}		/* if (FindMatchingMongoDocumentsByJSON (tool_p, values_p, fields_ss)) */
			else
				{
					#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
					PrintJSONToLog (values_p, "No results found for ", STM_LEVEL_SEVERE, __FILE__, __LINE__);
					#endif
				}

			if (fields_ss)
				{
					FreeMemory (fields_ss);
				}
		}		/* if (values_p) */


	#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (results_p, "results_p: ", STM_LEVEL_FINE, __FILE__, __LINE__);
	#endif

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


bool AddErrorMessage (json_t *errors_p, const json_t *values_p, const size_t row, const char * const error_s)
{
	bool success_flag = false;
	const char *pathogenomics_id_s = GetJSONString (values_p, PG_ID_S);

	if (pathogenomics_id_s)
		{
			json_error_t error;
			json_t *error_p = json_pack_ex (&error, 0, "{s:s,s:i,s:s}", "ID", pathogenomics_id_s, "row", row, "error", error_s);

			if (error_p)
				{
					if (json_array_append_new (errors_p, error_p) == 0)
						{
							success_flag = true;
						}
				}
		}

	#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (errors_p, "errors data: ", STM_LEVEL_FINE, __FILE__, __LINE__);
	#endif

	return success_flag;
}



static uint32 InsertData (MongoTool *tool_p, json_t *values_p, const PathogenomicsData collection_type, PathogenomicsServiceData *data_p, json_t *errors_p)
{
	uint32 num_imports = 0;
	const char *(*insert_fn) (MongoTool *tool_p, json_t *values_p, PathogenomicsServiceData *data_p) = NULL;

	#if PATHOGENOMICS_SERVICE_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (values_p, "values_p: ", PATHOGENOMICS_SERVICE_DEBUG, __FILE__, __LINE__);
	#endif

	switch (collection_type)
		{
			case PD_SAMPLE:
				insert_fn = InsertSampleData;
				break;

			case PD_PHENOTYPE:
				insert_fn = InsertPhenotypeData;
				break;

			case PD_GENOTYPE:
				insert_fn = InsertGenotypeData;
				break;

			default:
				break;

		}


	if (insert_fn)
		{
			if (json_is_array (values_p))
				{
					json_t *value_p;
					size_t i;

					json_array_foreach (values_p, i, value_p)
						{
							const char *error_s = insert_fn (tool_p, value_p, data_p);

							if (error_s)
								{
									char *dump_s = json_dumps (value_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

									if (!AddErrorMessage (errors_p, value_p, i + 1, error_s))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "failed to add %s to client feedback messsage", error_s);
										}


									if (dump_s)
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to import \"%s\": error=%s", dump_s, error_s);
											free (dump_s);
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to import error=%s", dump_s, error_s);
										}
								}
							else
								{
									++ num_imports;
								}
						}
				}
			else
				{
					const char *error_s = insert_fn (tool_p, values_p, data_p);

					if (error_s)
						{
							if (!AddErrorMessage (errors_p, values_p, 1, error_s))
								{

								}
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "%s", error_s);
						}
					else
						{
							++ num_imports;
						}
				}
		}


	return num_imports;
}



static uint32 DeleteData (MongoTool *tool_p, json_t *data_p, const PathogenomicsData collection_type, PathogenomicsServiceData *service_data_p, json_t *errors_p)
{
	bool success_flag = false;
	json_t *selector_p = json_object_get (data_p, MONGO_OPERATION_DATA_S);

	if (selector_p)
		{
			success_flag = RemoveMongoDocuments (tool_p, selector_p, false);
		}		/* if (values_p) */

	return success_flag ? 1 : 0;
}


static  ParameterSet *IsResourceForPathogenomicsService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return NULL;
}

