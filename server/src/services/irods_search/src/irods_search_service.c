/*
** Copyright 2014-2016 The Earlham Institute
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
#include "irods_connection.h"

#include "parameter_set.h"


/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData issd_base_data;
	IRodsConnection *issd_connection_p;
	ParameterSet *issd_params_p;
} IRodsSearchServiceData;


static const char S_UNSET_VALUE_S [] = "<NONE>";

static const char S_KEY_ID_S [] = "key column id";
static const char S_VALUE_ID_S [] = "value column id";


static const char * const S_IRODS_KEYWORD_S = "text";

/*
 * STATIC PROTOTYPES
 */

static const char *GetIRodsSearchServiceName (Service *service_p);

static const char *GetIRodsSearchServiceDesciption (Service *service_p);

static ParameterSet *GetIRodsSearchServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);


static ServiceJobSet *RunIRodsSearchService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static ParameterSet *IsFileForIRodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);


static bool CloseIRodsSearchService (Service *service_p);


static Parameter *AddParam (ServiceData *service_data_p, IRodsConnection *connection_p, ParameterSet *param_set_p, ParameterGroup *group_p, const char *name_s, const char *display_name_s, const char *description_s);


static int AddParams (ServiceData *data_p, IRodsConnection *connection_p, ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s);

static IRodsSearchServiceData *GetIRodsSearchServiceData (const json_t *config_p);

static void ReleaseIRodsSearchServiceParameters (Service *service_p, ParameterSet *params_p);

static void FreeIRodsSearchServiceData (IRodsSearchServiceData *data_p);

static bool GetColumnId (const Parameter * const param_p, const char *key_s, int *id_p);

static QueryResults *DoIrodsMetaSearch (IRodsSearch *search_p, IRodsSearchServiceData *data_p);

static bool AddIdToParameterStore (Parameter *param_p, const char * const key_s, int val);


static OperationStatus DoKeywordSearch (const char *keyword_s, ServiceJob *job_p, IRodsSearchServiceData *data_p);



static IRodsSearchServiceData *GetIRodsSearchServiceData (UserDetails *user_p)
{
	IRodsSearchServiceData *data_p = (IRodsSearchServiceData *) AllocMemory (sizeof (IRodsSearchServiceData));

	if (data_p)
		{
			IRodsConnection *connection_p = NULL;

			memset (& (data_p -> issd_base_data), 0, sizeof (ServiceData));

			connection_p = CreateIRodsConnectionFromJSON (user_p);


			if (connection_p)
				{
					ParameterSet *params_p = AllocateParameterSet ("iRODS search service parameters", "The parameters used for an iRODS metadata search service");

					if (params_p)
						{

							if (AddParams (& (data_p -> issd_base_data), connection_p, params_p, "Data objects metadata", NULL, "The metadata tags available for iRODS data objects") >= 0)
								{
									/*
									 * Although we'd like to add the column metadata values separately, it seems that for irods 3.3.1 any keys added via imeta
									 * get added to both the collection and data object keys
									 */
									//if (AddParams (connection_p, COL_META_COLL_ATTR_NAME, COL_META_COLL_ATTR_VALUE, params_p, "Collections metadata", NULL, "The metadata tags available for iRODS collections") >= 0)
									//	{
									SharedType def;
									def.st_string_value_s = NULL;

									if (CreateAndAddParameterToParameterSet (NULL, params_p, NULL, PT_KEYWORD, false, S_IRODS_KEYWORD_S, "Search term", "Search for matching metadata values", NULL, def, NULL, NULL, PL_ALL, NULL))
										{

											data_p -> issd_connection_p = connection_p;
											data_p -> issd_params_p = params_p;

											return data_p;
										}
									//	}
								}

							FreeParameterSet (params_p);
						}		/* if (params_p) */

					FreeIRodsConnection (connection_p);
				}		/* if (connection_p) */

			FreeMemory (data_p);
		}		/* if (data_p) */

	return NULL;
}


static void FreeIRodsSearchServiceData (IRodsSearchServiceData *data_p)
{
	if (data_p -> issd_connection_p)
		{
			FreeIRodsConnection (data_p -> issd_connection_p);
			data_p -> issd_connection_p = NULL;
		}

	if (data_p -> issd_params_p)
		{
			FreeParameterSet (data_p -> issd_params_p);
		}

	FreeMemory (data_p);
}


/*
 * API FUNCTIONS
 */

ServicesArray *GetServices (UserDetails *user_p)
{
	Service *irods_service_p = (Service *) AllocMemory (sizeof (Service));

	if (irods_service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);

			if (services_p)
				{
					ServiceData *data_p = (ServiceData *) GetIRodsSearchServiceData (user_p);
					
					if (data_p)
						{
							InitialiseService (irods_service_p,
								GetIRodsSearchServiceName,
								GetIRodsSearchServiceDesciption,
								NULL,
								RunIRodsSearchService,
								IsFileForIRodsSearchService,
								GetIRodsSearchServiceParameters,
								ReleaseIRodsSearchServiceParameters,
								CloseIRodsSearchService,
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


static bool CloseIRodsSearchService (Service *service_p)
{
	bool success_flag = true;
	IRodsSearchServiceData *data_p = (IRodsSearchServiceData *) (service_p -> se_data_p);

	FreeIRodsSearchServiceData (data_p);
	
	return success_flag;
}





static OperationStatus DoKeywordSearch (const char *keyword_s, ServiceJob *job_p, IRodsSearchServiceData *data_p)
{
	OperationStatus result = OS_FAILED;
	json_t *res_p = NULL;
	HashTable *store_p = GetHashTableOfStringInts (100, 75);

	if (store_p)
		{
			IRodsSearch *search_p = AllocateIRodsSearch ();

			if (search_p)
				{
					/*
					 * Get all attribute names
					 */
					QueryResults *attribute_names_p = GetAllMetadataDataAttributeNames (data_p -> issd_connection_p);

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
											char *value_s = ConcatenateStrings (keyword_s, "%");

											if (value_s)
												{
													if (AddMetadataDataAttributeSearchTerm (search_p, NULL, *attribute_name_ss, "like", value_s))
														{
															QueryResults *attr_search_results_p = DoIrodsMetaSearch (search_p, data_p);

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

														}		/* if (AddMetadataDataAttributeSearchTerm (search_p, NULL, *attribute_name_ss, "like", keyword_s)) */

													FreeCopiedString (value_s);
												}		/* if (value_s) */


											ClearIRodsSearch (search_p);
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

												}		/* for ( ; i > 0; -- i, ++ key_pp, ++ pair_p) */

											SortStringIntPairsByCount (results_p);

											for (i = store_p -> ht_size, key_pp = keys_pp, pair_p = results_p -> sipa_values_p; i > 0; -- i, ++ key_pp, ++ pair_p)
												{
													res_p = GetStringIntPairAsResourceJSON (pair_p, PROTOCOL_IRODS_S);

													if (res_p)
														{
															if (!AddResultToServiceJob (job_p, res_p))
																{
																	char uuid_s [UUID_STRING_BUFFER_SIZE];

																	ConvertUUIDToString (job_p -> sj_id, uuid_s);
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add result %s to job %s", pair_p -> sip_string_s, uuid_s);
																}

														}		/* if (res_p) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get resource json for " UINT32_FMT " = %s", pair_p -> sip_value, pair_p -> sip_string_s);
														}
												}		/* for ( ; i > 0; -- i, ++ key_pp, ++ pair_p) */


											/* If we couldn't add all results, set the OperationStatus for the ServiceJob accordingly */
											i = GetNumberOfServiceJobResults (job_p);
											result = (i == store_p -> ht_size) ? OS_SUCCEEDED : OS_PARTIALLY_SUCCEEDED;

											FreeStringIntPairArray (results_p);
										}		/* if (results_p) */

									FreeKeysIndex ((void **) keys_pp);
								}		/* if (keys_pp) */

						}		/* if (store_p -> ht_size > 0) */

				}		/* if (search_p) */


			FreeHashTable (store_p);
		}		/* if (store_p) */


	SetServiceJobStatus (job_p, result);

	return result;
}



static int AddParams (ServiceData *data_p, IRodsConnection *connection_p, ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s)
{
	int res = 0;

	/*
	 * Get the attibute keys
	 */
	QueryResults *results_p = GetAllMetadataDataAttributeNames (connection_p);

	if (results_p)
		{
			if (results_p -> qr_num_results == 1)
				{
					QueryResult *result_p = results_p -> qr_values_p;
					int i = result_p -> qr_num_values;
					char **value_ss = result_p -> qr_values_pp;
					const char *heading_s = display_name_s;
					ParameterGroup *group_p = NULL;

					if (!heading_s)
						{
							heading_s = name_s;
						}


					group_p = CreateAndAddParameterGroupToParameterSet (heading_s, NULL, data_p, param_set_p);


					for ( ; i > 0; --i, ++ value_ss)
						{
							Parameter *param_p = AddParam (data_p, connection_p, param_set_p, group_p, *value_ss, NULL, display_name_s);

							if (param_p)
								{
									++ res;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get metadata values for \"%s\"\n", *value_ss);
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


static Parameter *AddParam (ServiceData *service_data_p, IRodsConnection *connection_p, ParameterSet *param_set_p, ParameterGroup *group_p, const char *name_s, const char *display_name_s, const char *description_s)
{
	bool success_flag = false;
	Parameter *param_p = NULL;

	/*
	 * Get the attibute values
	 */
	QueryResults *results_p = GetAllMetadataDataAttributeValues (connection_p, name_s);

	if (results_p)
		{
			if (results_p -> qr_num_results == 1)
				{
					LinkedList *options_p = CreateProgramOptionsList ();

					if (options_p)
						{
							QueryResult *result_p = results_p -> qr_values_p;
							const int num_opts = (result_p -> qr_num_values) + 1;
							char **value_ss = result_p -> qr_values_pp;
							SharedType def;
							int i = num_opts;
							bool added_options_flag = false;

							success_flag = true;
							InitSharedType (&def);

							/* Add all of the values into our options list */
							while (success_flag && (i > 1))
								{
									def.st_string_value_s = CopyToNewString (*value_ss, 0, false);

									if (CreateAndAddParameterOption (options_p, def, NULL, PT_STRING))
										{
											-- i;
											++ value_ss;
										}
									else
										{
											success_flag = false;
										}
								}		/* while (success_flag && (i > 1)) */

							/* Add the empty option */
							if (success_flag)
								{
									def.st_string_value_s = CopyToNewString (S_UNSET_VALUE_S, 0, false);

									if (CreateAndAddParameterOption (options_p, def, NULL, PT_STRING))
										{
											ParameterOption *first_option_p = ((ParameterOptionNode *) options_p -> ll_head_p) -> pon_option_p;

											param_p = AllocateParameter (service_data_p, PT_KEYWORD, false, name_s, display_name_s, description_s, options_p, first_option_p -> po_value, NULL, NULL, PL_ALL, NULL);

											if (param_p)
												{
													added_options_flag = true;

													if (!AddParameterToParameterSet (param_set_p, param_p))
														{
															FreeParameter (param_p);
															param_p = NULL;
															success_flag = false;
														}

												}		/* if (param_p) */

										}		/* if (CreateAndAddParameterOptionToParameter (param_p, def, NULL)) */

								}		/* if (success_flag) */

							if (!added_options_flag)
								{
									FreeLinkedList (options_p);
								}

						}		/* if (options_p) */

				}		/* if (results_p -> qr_num_results == 1) */

			 FreeQueryResults (results_p);
		}		/* if (results_p) */

	return param_p;
}



/*
 * STATIC FUNCTIONS
 */


static const char *GetIRodsSearchServiceName (Service *service_p)
{
	return "iRods search service";
}


static const char *GetIRodsSearchServiceDesciption (Service *service_p)
{
	return "A service to search the metadata within iRods";
}


static ParameterSet *GetIRodsSearchServiceParameters (Service *service_p, Resource *resource_p, UserDetails * UNUSED_PARAM (user_p))
{
	IRodsSearchServiceData *data_p = (IRodsSearchServiceData *) (service_p -> se_data_p);

	return data_p -> issd_params_p;
}


static void ReleaseIRodsSearchServiceParameters (Service *service_p, ParameterSet *params_p)
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

			if (GetValidInteger (&column_s, &i))
				{
					*id_p = i;
					success_flag = true;
				}

		}		/* if (column_s) */

	return success_flag;
}


static ServiceJobSet *RunIRodsSearchService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p)
{
	IRodsSearch *search_p = AllocateIRodsSearch ();

	if (search_p)
		{
			/* We only have one task */
			service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "iRods search");

			if (service_p -> se_jobs_p)
				{
					IRodsSearchServiceData *data_p = (IRodsSearchServiceData *) (service_p -> se_data_p);
					ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);
					SharedType def;

					job_p -> sj_status = OS_FAILED;

					/* is it a keyword search? */
					if ((GetParameterValueFromParameterSet (param_set_p, S_IRODS_KEYWORD_S, &def, true)) && (!IsStringEmpty (def.st_string_value_s)))
						{
							OperationStatus status = DoKeywordSearch (def.st_string_value_s, job_p, data_p);
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

									if (strcmp (param_p -> pa_name_s, S_IRODS_KEYWORD_S) == 0)
										{
											const char *value_s = param_p -> pa_current_value.st_string_value_s;

											if (strcmp (S_UNSET_VALUE_S, value_s) != 0)
												{
													if (AddMetadataDataAttributeSearchTerm (search_p, clause_s, param_p -> pa_name_s, "=", value_s))
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

												}		/* if (strcmp (S_UNSET_VALUE_S, value_s) != 0) */

										}		/* if (param_p -> pa_tag != TAG_IRODS_KEYWORD) */

									if (success_flag)
										{
											node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
										}
								}		/* while (node_p && success_flag) */

							if (success_flag)
								{
									QueryResults *results_p = DoIrodsMetaSearch (search_p, data_p);

									job_p -> sj_status = OS_FAILED;

									if (results_p)
										{
											//PrintQueryResults (stdout, results_p);
											//fflush (stdout);

											query_results_json_p = GetQueryResultAsResourcesJSON (results_p);

											if (query_results_json_p)
												{
													if (ReplaceServiceJobResults (job_p, query_results_json_p))
														{
															job_p -> sj_status = OS_SUCCEEDED;
														}
												}

										}		/* if (results_p) */

								}		/* if (success_flag) */
						}

				}		/* if (service_p -> se_jobs_p) */


			FreeIRodsSearch (search_p);

			//res_json_p = CreateServiceResponseAsJSON (service_p, res, query_results_json_p, NULL);
		}		/* if (search_p) */

	return service_p -> se_jobs_p;
}


static QueryResults *DoIrodsMetaSearch (IRodsSearch *search_p, IRodsSearchServiceData *data_p)
{
	bool upper_case_flag = false;
	char *zone_s = NULL;
	QueryResults *results_p = DoMetaSearchForAllDataAndCollections (search_p, data_p -> issd_connection_p, upper_case_flag, zone_s);

	return results_p;
}


static ParameterSet *IsFileForIRodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return NULL;
}

