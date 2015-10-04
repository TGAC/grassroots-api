#include <string.h>


#include "irods_search_service_library.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "string_utils.h"
#include "math_utils.h"
#include "service.h"
#include "data_resource.h"

#include "query.h"
#include "connect.h"
#include "streams.h"
#include "meta_search.h"
#include "service_job.h"
#include "string_int_pair.h"
#include "string_hash_table.h"


/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData issd_base_data;
	rcComm_t *issd_connection_p;
	ParameterSet *issd_params_p;
} IrodsSearchServiceData;


static const char S_UNSET_VALUE_S [] = "<NONE>";

static const char S_KEY_ID_S [] = "key column id";
static const char S_VALUE_ID_S [] = "value column id";


#define TAG_IRODS_KEYWORD MAKE_TAG ('I', 'R', 'K', 'W')

/*
 * STATIC PROTOTYPES
 */

static const char *GetIrodsSearchServiceName (Service *service_p);

static const char *GetIrodsSearchServiceDesciption (Service *service_p);

static ParameterSet *GetIrodsSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);


static ServiceJobSet *RunIrodsSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForIrodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);


static bool CloseIrodsSearchService (Service *service_p);


static rcComm_t *GetIRODSConnection (const json_t *config_p);

static Parameter *AddParam (rcComm_t *connection_p, int key_col_id, int value_col_id, ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s);

static size_t AddParams (rcComm_t *connection_p, int key_col_id, int value_col_id, ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s);

static IrodsSearchServiceData *GetIrodsSearchServiceData (const json_t *config_p);

static void ReleaseIrodsSearchServiceParameters (Service *service_p, ParameterSet *params_p);

static void FreeIrodsSearchServiceData (IrodsSearchServiceData *data_p);

static bool GetColumnId (const Parameter * const param_p, const char *key_s, int *id_p);

static QueryResults *DoIrodsMetaSearch (IrodsSearch *search_p, IrodsSearchServiceData *data_p);

static bool AddIdToParameterStore (Parameter *param_p, const char * const key_s, int val);

static bool CleanUpIrodsSearchJob (ServiceJob *job_p);

static json_t *DoKeywordSearch (const char *keyword_s, int key_col_id, int value_col_id, IrodsSearchServiceData *data_p);



static IrodsSearchServiceData *GetIrodsSearchServiceData (const json_t *config_p)
{
	IrodsSearchServiceData *data_p = (IrodsSearchServiceData *) AllocMemory (sizeof (IrodsSearchServiceData));

	if (data_p)
		{
			rcComm_t *connection_p = GetIRODSConnection (config_p);

			if (connection_p)
				{
					ParameterSet *params_p = AllocateParameterSet ("iRODS search service parameters", "The parameters used for an iRODS metadata search service");

					if (params_p)
						{
							if (AddParams (connection_p, COL_META_DATA_ATTR_NAME, COL_META_DATA_ATTR_VALUE, params_p, "Data objects metadata", NULL, "The metadata tags available for iRODS data objects") >= 0)
								{
									/*
									 * Although we'd like to add the column metadata values separately, it seems that for irods 3.3.1 any keys added via imeta
									 * get added to both the collection and data object keys
									 */
									//if (AddParams (connection_p, COL_META_COLL_ATTR_NAME, COL_META_COLL_ATTR_VALUE, params_p, "Collections metadata", NULL, "The metadata tags available for iRODS collections") >= 0)
									//	{
									SharedType def;
									def.st_string_value_s = NULL;

									if (CreateAndAddParameterToParameterSet (params_p, PT_KEYWORD, false, "text", "Search term", "Search for matching metadata values", TAG_IRODS_KEYWORD, NULL, def, NULL, NULL, PL_ALL, NULL))
										{

											data_p -> issd_connection_p = connection_p;
											data_p -> issd_params_p = params_p;

											return data_p;
										}
									//	}
								}

							FreeParameterSet (params_p);
						}		/* if (params_p) */

					CloseIRODSConnection (connection_p);
				}		/* if (connection_p) */

			FreeMemory (data_p);
		}		/* if (data_p) */

	return NULL;
}


static void FreeIrodsSearchServiceData (IrodsSearchServiceData *data_p)
{
	if (data_p -> issd_connection_p)
		{
			CloseIRODSConnection (data_p -> issd_connection_p);
			data_p -> issd_connection_p = NULL;
		}

	if (data_p -> issd_params_p)
		{
			FreeParameterSet (data_p -> issd_params_p);
		}
}


/*
 * API FUNCTIONS
 */

ServicesArray *GetServices (const json_t *config_p)
{
	Service *irods_service_p = (Service *) AllocMemory (sizeof (Service));

	if (irods_service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);

			if (services_p)
				{
					ServiceData *data_p = (ServiceData *) GetIrodsSearchServiceData (config_p);
					
					if (data_p)
						{
							InitialiseService (irods_service_p,
								GetIrodsSearchServiceName,
								GetIrodsSearchServiceDesciption,
								NULL,
								RunIrodsSearchService,
								IsFileForIrodsSearchService,
								GetIrodsSearchServiceParameters,
								ReleaseIrodsSearchServiceParameters,
								CloseIrodsSearchService,
								NULL,
								NULL,
								true,
								true,
								data_p);
							
							* (services_p -> sa_services_pp) = irods_service_p;

							return services_p;
						}		/* if (data_p) */

					FreeServicesArray (services_p);
				}

			FreeMemory (irods_service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


static bool CloseIrodsSearchService (Service *service_p)
{
	bool success_flag = true;
	IrodsSearchServiceData *data_p = (IrodsSearchServiceData *) (service_p -> se_data_p);

	FreeIrodsSearchServiceData (data_p);
	
	return success_flag;
}


static rcComm_t *GetIRODSConnection (const json_t *config_p)
{
	rcComm_t *connection_p = NULL;

	if (config_p)
		{
			connection_p = CreateIRODSConnectionFromJSON (config_p);
		}

	return connection_p;
}



static json_t *DoKeywordSearch (const char *keyword_s, int key_col_id, int value_col_id, IrodsSearchServiceData *data_p)
{
	json_t *res_p = NULL;
	HashTable *store_p = GetHashTableOfStringInts (100, 75);

	if (store_p)
		{
			IrodsSearch *search_p = AllocateIrodsSearch ();

			if (search_p)
				{
					/*
					 * Get all attribute names
					 */
					QueryResults *attribute_names_p = GetAllMetadataAttributeNames (data_p -> issd_connection_p, key_col_id);

					if (attribute_names_p)
						{
							if (attribute_names_p -> qr_num_results == 1)
								{
									QueryResult *result_p = attribute_names_p -> qr_values_p;
									int i = result_p -> qr_num_values;
									char **attribute_name_ss = result_p -> qr_values_pp;

									/*
									 * For each attribute name, test for the keyword value and store any hits
									 */
									for ( ; i > 0; --i, ++ attribute_name_ss)
										{
											if (AddIrodsSearchTerm (search_p, NULL, *attribute_name_ss, key_col_id, "=", keyword_s, value_col_id))
												{
													QueryResults *attr_search_results_p = DoIrodsSearch (search_p, data_p -> issd_connection_p);

													if (attr_search_results_p)
														{
															LinkedList *matching_results_p = GetQueryResultsPaths (attr_search_results_p);

															if (matching_results_p)
																{
																	StringListNode *node_p = (StringListNode *) (matching_results_p -> ll_head_p);

																	while (node_p)
																		{
																			int count = 1;
																			char *key_s = node_p -> sln_string_s;
																			const int *count_p = (const int *) GetFromHashTable (store_p, key_s);

																			if (count_p)
																				{
																					count = 1 + (*count_p);
																				}

																			PutInHashTable (store_p, key_s, &count);

																			node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
																		}

																	FreeLinkedList (matching_results_p);
																}		/* if (matching_results_p) */


															FreeQueryResults (attr_search_results_p);
														}		/* if (attr_search_results_p) */
												}

											ClearIrodsSearch (search_p);
										}		/* for ( ; i > 0; --i, ++ attribute_name_ss) */

								}		/* if (attribute_names_p -> qr_num_results == 1) */

							FreeQueryResults (attribute_names_p);
						}		/* if (attribute_names_p) */


					/*
					 * Get all of the hits and rank them by occurrence
					 */
					if (store_p -> ht_size > 0)
						{
							char **keys_pp = (char **) GetKeysIndexFromHashTable (store_p);

							if (keys_pp)
								{
									uint32 i = store_p -> ht_size;
									StringIntPairArray *results_p = AllocateStringIntPairArray (i);

									if (results_p)
										{
											char **key_pp = keys_pp;
											StringIntPair *pair_p = results_p -> sipa_values_p;

											for ( ; i > 0; -- i, ++ key_pp, ++ pair_p)
												{
													char *key_s = *key_pp;
													const int *count_p = (const int *) GetFromHashTable (store_p, key_s);

													if (count_p)
														{
															if (!SetStringIntPair (pair_p, key_s, MF_DEEP_COPY, *count_p))
																{

																}
														}
												}

											SortStringIntPairsByCount (results_p);
											res_p = GetStringIntPairsAsResourceJSON (results_p);

											FreeStringIntPairArray (results_p);
										}		/* if (results_p) */

									FreeKeysIndex ((void **) keys_pp);
								}		/* if (keys_pp) */

						}		/* if (store_p -> ht_size > 0) */

				}		/* if (search_p) */


			FreeHashTable (store_p);
		}		/* if (store_p) */

	return res_p;
}




static size_t AddParams (rcComm_t *connection_p, int key_col_id, int value_col_id, ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s)
{
	size_t res = 0;

	/*
	 * Get the attibute keys
	 */
	QueryResults *results_p = GetAllMetadataAttributeNames (connection_p, key_col_id);

	if (results_p)
		{
			if (results_p -> qr_num_results == 1)
				{
					QueryResult *result_p = results_p -> qr_values_p;
					int i = result_p -> qr_num_values;
					char **value_ss = result_p -> qr_values_pp;
					Parameter **params_pp = (Parameter **) AllocMemoryArray (i, sizeof (Parameter *));
					Parameter **param_pp = params_pp;

					for ( ; i > 0; --i, ++ value_ss)
						{
							Parameter *param_p = AddParam (connection_p, key_col_id, value_col_id, param_set_p, *value_ss, NULL, display_name_s);

							if (param_p)
								{
									if (param_pp)
										{
											*param_pp = param_p;
											++ param_pp;
										}

									++ res;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to get metadata values for \"%s\"\n", *value_ss);
								}
						}

					if (params_pp)
						{
							const char *heading_s = display_name_s;

							if (!heading_s)
								{
									heading_s = name_s;
								}

							if (!AddParameterGroupToParameterSet (param_set_p, heading_s, params_pp, result_p -> qr_num_values))
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to add parameter group for \"%s\"\n", name_s);
								}
						}

				}		/* if (results_p -> qr_num_results == 1) */

			 FreeQueryResults (results_p);
		}		/* if (results_p) */

	return res;
}


static bool AddIdToParameterStore (Parameter *param_p, const char * const key_s, int val)
{
	bool success_flag = false;
	char *val_s = ConvertIntegerToString (val);

	if (val_s)
		{
			if (AddParameterKeyValuePair (param_p, key_s, val_s))
				{
					success_flag = true;
				}

			FreeCopiedString (val_s);
		}

	return success_flag;
}


static Parameter *AddParam (rcComm_t *connection_p, int key_col_id, int value_col_id, ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s)
{
	bool success_flag = false;
	Parameter *param_p = NULL;

	/*
	 * Get the attibute values
	 */
	QueryResults *results_p = GetAllMetadataAttributeValues (connection_p, key_col_id, name_s, value_col_id);

	if (results_p)
		{
			if (results_p -> qr_num_results == 1)
				{
					QueryResult *result_p = results_p -> qr_values_p;
					const int num_opts = (result_p -> qr_num_values) + 1;
					SharedType *param_options_p = (SharedType *) AllocMemoryArray (num_opts, sizeof (SharedType));

					if (param_options_p)
						{
							char **value_ss = result_p -> qr_values_pp;
							SharedType *option_p = param_options_p;
							ParameterMultiOptionArray *options_array_p = NULL;
							int i = num_opts;
							success_flag = true;

							/* Copy all of the values into our options array */
							while (success_flag && (i > 1))
								{
									option_p -> st_string_value_s = CopyToNewString (*value_ss, 0, false);

									if (option_p -> st_string_value_s)
										{
											-- i;
											++ value_ss;
											++ option_p;
										}
									else
										{
											success_flag = false;
										}
								}

							/* Add the empty option */
							if (success_flag)
								{
									option_p -> st_string_value_s = CopyToNewString (S_UNSET_VALUE_S, 0, false);

									success_flag = (option_p -> st_string_value_s != NULL);
								}


							if (success_flag)
								{
									success_flag = false;
									options_array_p = AllocateParameterMultiOptionArray (num_opts, NULL, param_options_p, PT_STRING);

									if (options_array_p)
										{
											SharedType def;

											def.st_string_value_s = param_options_p -> st_string_value_s;

											param_p = AllocateParameter (PT_KEYWORD, false, name_s, display_name_s, description_s, 0, options_array_p, def, NULL, NULL, PL_ALL, NULL);

											if (param_p)
												{
													if (AddIdToParameterStore (param_p, S_KEY_ID_S, key_col_id))
														{
															if (AddIdToParameterStore (param_p, S_VALUE_ID_S, value_col_id))
																{
																	if (AddParameterToParameterSet (param_set_p, param_p))
																		{
																			success_flag = true;
																		}
																}
														}

													if (!success_flag)
														{
															FreeParameter (param_p);
															param_p = NULL;
														}
												}		/* if (param_p) */
											else
												{
													FreeParameterMultiOptionArray (options_array_p);
												}
										}

									if (!success_flag)
										{
											FreeMemory (param_options_p);
										}
								}
							else
								{
									/* If we failed to fill the array, clean up */
									for ( ; i > 0; -- i, -- option_p)
										{
											FreeCopiedString (option_p -> st_string_value_s);
										}
								}

						}		/* if (param_options_p) */

				}		/* if (results_p -> qr_num_results == 1) */

			 FreeQueryResults (results_p);
		}		/* if (results_p) */

	return param_p;
}



/*
 * STATIC FUNCTIONS
 */


static const char *GetIrodsSearchServiceName (Service *service_p)
{
	return "iRods search service";
}


static const char *GetIrodsSearchServiceDesciption (Service *service_p)
{
	return "A service to search the metadata within iRods";
}


static ParameterSet *GetIrodsSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	IrodsSearchServiceData *data_p = (IrodsSearchServiceData *) (service_p -> se_data_p);

	return data_p -> issd_params_p;
}


static void ReleaseIrodsSearchServiceParameters (Service *service_p, ParameterSet *params_p)
{
	/*
	 * As the parameters are cached, we release the parameters when the service is destroyed
	 * so we need to do anything here.
	 */
}


static bool GetColumnId (const Parameter * const param_p, const char *key_s, int *id_p)
{
	bool success_flag = false;
	const char *column_s = GetParameterKeyValue (param_p, key_s);

	if (column_s)
		{
			int i;

			if (GetValidInteger (&column_s, &i, NULL))
				{
					*id_p = i;
					success_flag = true;
				}

		}		/* if (column_s) */

	return success_flag;
}


static ServiceJobSet *RunIrodsSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	IrodsSearch *search_p = AllocateIrodsSearch ();

	if (search_p)
		{
			/* We only have one task */
			service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1, NULL);

			if (service_p -> se_jobs_p)
				{
					IrodsSearchServiceData *data_p = (IrodsSearchServiceData *) (service_p -> se_data_p);
					ServiceJob *job_p = service_p -> se_jobs_p -> sjs_jobs_p;
					SharedType def;

					job_p -> sj_status = OS_FAILED;

					/* is it a keyword search? */
					if (GetParameterValueFromParameterSet (param_set_p, TAG_IRODS_KEYWORD, &def, true))
						{
							json_t *res_p = DoKeywordSearch (def.st_string_value_s, COL_META_DATA_ATTR_NAME, COL_META_DATA_ATTR_VALUE, data_p);

							if (res_p)
								{
									job_p -> sj_result_p = res_p;
									job_p -> sj_status = OS_SUCCEEDED;
								}
						}
					else
						{
							ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
							bool success_flag = true;
							const char *clause_s = NULL;
							json_t *query_results_json_p = NULL;

							while (node_p && success_flag)
								{
									Parameter *param_p = node_p -> pn_parameter_p;

									if (param_p -> pa_tag != TAG_IRODS_KEYWORD)
										{
											int key_id;

											if (GetColumnId (param_p, S_KEY_ID_S, &key_id))
												{
													int value_id;

													if (GetColumnId (param_p, S_VALUE_ID_S, &value_id))
														{
															const char *value_s = param_p -> pa_current_value.st_string_value_s;

															if (strcmp (S_UNSET_VALUE_S, value_s) != 0)
																{
																	if (AddIrodsSearchTerm (search_p, clause_s, param_p -> pa_name_s, key_id, "=", value_s, value_id))
																		{
																			if (clause_s == NULL)
																				{
																					clause_s = "AND";
																				}
																		}
																	else
																		{
																			success_flag = false;
																		}
																}
															else
																{
																	success_flag = true;
																}

														}		/* if (GetColumnId (param_p, S_VALUE_ID_S, &value_id)) */
													else
														{
															success_flag = false;
														}
												}		/* if (GetColumnId (param_p, S_KEY_ID_S, &key_id)) */
											else
												{
													success_flag = false;
												}

										}		/* if (param_p -> pa_tag != TAG_IRODS_KEYWORD) */

									if (success_flag)
										{
											node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
										}
								}		/* while (node_p && success_flag) */

							if (success_flag)
								{
									QueryResults *results_p = NULL;

									//results_p = DoIrodsSearch (search_p, data_p -> issd_connection_p);
									results_p = DoIrodsMetaSearch (search_p, data_p);

									job_p -> sj_status = OS_FAILED;

									if (results_p)
										{
											PrintQueryResults (stdout, results_p);
											fflush (stdout);

											query_results_json_p = GetQueryResultAsResourcesJSON (results_p);

											if (query_results_json_p)
												{
													job_p -> sj_status = OS_SUCCEEDED;
													job_p -> sj_result_p = query_results_json_p;
												}

										}		/* if (results_p) */

								}		/* if (success_flag) */
						}

				}		/* if (service_p -> se_jobs_p) */


			FreeIrodsSearch (search_p);

			//res_json_p = CreateServiceResponseAsJSON (service_p, res, query_results_json_p, NULL);
		}		/* if (search_p) */

	return service_p -> se_jobs_p;
}


static QueryResults *DoIrodsMetaSearch (IrodsSearch *search_p, IrodsSearchServiceData *data_p)
{
	bool upper_case_flag = false;
	char *zone_s = NULL;
	int columns [] = { COL_COLL_NAME, COL_DATA_NAME };
	QueryResults *results_p = DoMetaSearch (search_p, data_p -> issd_connection_p, columns, 2, upper_case_flag, zone_s);

	return results_p;
}


static bool IsFileForIrodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;
	const char *filename_s = resource_p -> re_value_s;



	return interested_flag;
}



static bool CleanUpIrodsSearchJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;

	return cleaned_up_flag;
}
