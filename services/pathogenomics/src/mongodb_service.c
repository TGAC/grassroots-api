#include <string.h>

#include "mongodb_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "service_job.h"
#include "mongo_tool.h"


#define TAG_UPDATE MAKE_TAG('P', 'G', 'U', 'P')
#define TAG_QUERY MAKE_TAG('P', 'G', 'Q', 'U')
#define TAG_REMOVE MAKE_TAG('P', 'G', 'R', 'M')

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
					const char *buffer_data_p = GetCurlToolData (data_p -> wsd_curl_data_p);
					res_p = json_loads (buffer_data_p, 0, &error);
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
	MongoDBServiceData *data_p = (MongoDBServiceData *) AllocMemory (sizeof (MongoDBServiceData));
	
	if (data_p)
		{
			if (!InitMongoDBServiceData (data_p, op_json_p))
				{
					FreeMemory (data_p);
					data_p = NULL;
				}
		}
		
	return data_p;
}


static void FreeMongoDBServiceData (MongoDBServiceData *data_p)
{
	ClearMongoDBServiceData (data_p);
	
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
									return params_p;
								}
						}
				}

			FreeParameterSet (params_p);
		}

	return NULL;
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
					bool success_flag = true;

					ResetByteBuffer (data_p -> wsd_buffer_p);

					switch (data_p -> wsd_method)
						{
							case SM_POST:
								success_flag = AddParametersToPostMongoDBService (data_p, param_set_p);
								break;

							case SM_GET:
								success_flag = AddParametersToGetMongoDBService (data_p, param_set_p);
								break;

							case SM_BODY:
								success_flag = AddParametersToBodyMongoDBService (data_p, param_set_p);
								break;

							default:
								break;
						}

					if (success_flag)
						{
							job_p -> sj_status = (CallCurlWebservice (data_p)) ? OS_SUCCEEDED : OS_FAILED;
						}		/* if (success_flag) */

				}		/* if (param_set_p) */

		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}



static bool ProcessRequest (MongoTool *tool_p, json_t *data_p)
{
	bool success_flag = false;
	const char *collection_s = GetJSONString (data_p, MONGO_COLLECTION_S);

	if (collection_s)
		{
			const char *operation_s = GetJSONString (data_p, MONGO_OPERATION_S);

			if (operation_s)
				{
					if (strcmp (operation_s, MONGO_OPERATION_INSERT_S))
						{
							json_t *values_p = json_object_get (data_p, MONGO_OPERATION_DATA_S);

							if (values_p)
								{
									/**
									 * Is it an insert or an update?
									 */
									const char *id_s = GetJSONString (values_p, MONGO_ID_S);

									if (id_s)
										{
											bson_oid_t oid;

											if (bson_oid_is_valid (id_s, strlen (id_s)))
												{
													bson_t *query_p = NULL;
													bson_t *update_p = NULL;

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
													json_t *field_p = json_object_new ();

													if (field_p)
														{
															const char *fields_ss [2] = { NULL, NULL };
															const char *key_s;


															SetMongoToolCollection (tool_p, PATHOGENOMICS_DB_S, fields_collection_s);

															json_object_foreach (values_p, key_s, value_p)
																{
																	bson_t *query_p = BCON_NEW ("$query", "{", key_s, BCON_INT32 (1), "}");

																	if (query_p)
																		{
																			int32 value = 0;
																			*fields_ss = key_s;

																			if (FindMatchingMongoDocumentsByBSON (tool_p, query_p, fields_ss))
																				{
																					const bson_t *doc_p = NULL;

																					/* should only be one of these */
																					while (mongoc_cursor_next (tool_p -> mt_cursor_p, &doc_p))
																						{
																							json_t *json_value_p = ConvertBSONToJSON (doc_p);

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

																			bson_destroy (query_p);
																		}
																}		/* json_object_foreach (values_p, key_s, value_p) */


														}		/* if (field_p) */


													FreeCopiedString (fields_collection_s);
												}		/* if (fields_collection_s) */

										}

								}		/* if (values_p) */
						}
				}		/* if (operation_s) */

		}		/* if (collection_s) */

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


