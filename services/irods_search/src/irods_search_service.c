#include <string.h>


#include "irods_search_service_library.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "string_utils.h"
#include "service.h"

#include "query.h"
#include "connect.h"
#include "streams.h"
#include "meta_search.h"


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

/*
 * STATIC PROTOTYPES
 */

static const char *GetIrodsSearchServiceName (Service *service_p);

static const char *GetIrodsSearchServiceDesciption (Service *service_p);

static ParameterSet *GetIrodsSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);


static json_t *RunIrodsSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForIrodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);


static bool CloseIrodsSearchService (Service *service_p);


static rcComm_t *GetIRODSConnection (const json_t *config_p);

static size_t AddParams (rcComm_t *connection_p, QueryResults *(*get_metadata_fn) (rcComm_t *connection_p), QueryResults *(*get_metadata_values_fn) (rcComm_t *connection_p, const char * const name_s), ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s);

static bool AddParam (rcComm_t *connection_p, QueryResults *(*get_metadata_values_fn) (rcComm_t *connection_p, const char * const name_s), ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s);

static IrodsSearchServiceData *GetIrodsSearchServiceData (const json_t *config_p);

static void ReleaseIrodsSearchServiceParameters (Service *service_p, ParameterSet *params_p);

static void FreeIrodsSearchServiceData (IrodsSearchServiceData *data_p);



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
							if (AddParams (connection_p, GetAllMetadataDataAttributeNames, GetAllMetadataDataAttributeValues, params_p, "Data objects metadata", NULL, "The metadata tags available for iRODS data objects") >= 0)
								{
									data_p -> issd_connection_p = connection_p;
									data_p -> issd_params_p = params_p;

									return data_p;
								}

							FreeParameterSet (params_p);
						}		/* if (params_p) */

					CloseConnection (connection_p);
				}		/* if (connection_p) */

			FreeMemory (data_p);
		}		/* if (data_p) */

	return NULL;
}


static void FreeIrodsSearchServiceData (IrodsSearchServiceData *data_p)
{
	if (data_p -> issd_connection_p)
		{
			CloseConnection (data_p -> issd_connection_p);
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
								RunIrodsSearchService,
								IsFileForIrodsSearchService,
								GetIrodsSearchServiceParameters,
								ReleaseIrodsSearchServiceParameters,
								CloseIrodsSearchService,
								true,
								data_p);
							
							* (services_p -> sa_services_pp) = irods_service_p;

							return services_p;
						}		/* if (data_p) */

					FreeServicesArray (services_p);
				}

			FreeService (irods_service_p);
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
			connection_p = CreateConnectionFromJSON (config_p);
		}

	return connection_p;
}




static size_t AddParams (rcComm_t *connection_p, QueryResults *(*get_metadata_fn) (rcComm_t *connection_p), QueryResults *(*get_metadata_values_fn) (rcComm_t *connection_p, const char * const name_s), ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s)
{
	size_t res = 0;

	/*
	 * Get the attibute keys
	 */
	QueryResults *results_p = get_metadata_fn (connection_p);

	if (results_p)
		{
			if (results_p -> qr_num_results == 1)
				{
					QueryResult *result_p = results_p -> qr_values_p;
					int i = result_p -> qr_num_values;
					char **value_ss = result_p -> qr_values_pp;

					for ( ; i > 0; --i, ++ value_ss)
						{
							if (AddParam (connection_p, get_metadata_values_fn, param_set_p, *value_ss, NULL, NULL))
								{
									++ res;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to get metadata values for \"%s\"\n", *value_ss);
								}
						}

				}		/* if (results_p -> qr_num_results == 1) */

			 FreeQueryResults (results_p);
		}		/* if (results_p) */

	return res;
}


static bool AddParam (rcComm_t *connection_p, QueryResults *(*get_metadata_values_fn) (rcComm_t *connection_p, const char * const name_s), ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s)
{
	bool success_flag = false;

	/*
	 * Get the attibute values
	 */
	QueryResults *results_p = get_metadata_values_fn (connection_p, name_s);

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

											if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, name_s, display_name_s, description_s, TAG_KEYWORD, options_array_p, def, NULL, NULL, PL_BASIC, NULL))
												{
													success_flag = true;
												}

											if (!success_flag)
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

	return success_flag;
}



/*
 * STATIC FUNCTIONS
 */


static const char *GetIrodsSearchServiceName (Service *service_p)
{
	return "Irods search service";
}


static const char *GetIrodsSearchServiceDesciption (Service *service_p)
{
	return "A service to search the metadata within Irods";
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


static json_t *RunIrodsSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	IrodsSearch *search_p = AllocateIrodsSearch ();

	if (search_p)
		{
			ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
			bool success_flag = true;
			const char *clause_s = NULL;
			json_t *query_results_json_p = NULL;

			while (node_p && success_flag)
				{
					Parameter *param_p = node_p -> pn_parameter_p;

					success_flag = AddIrodsSearchTerm (search_p, clause_s, param_p -> pa_name_s, "=", param_p -> pa_current_value.st_string_value_s);

					if (success_flag)
						{
							node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);

							if (clause_s == NULL)
								{
									clause_s = "AND";
								}
						}
				}

			if (success_flag)
				{
					IrodsSearchServiceData *data_p = (IrodsSearchServiceData *) (service_p -> se_data_p);
					QueryResults *results_p = DoIrodsSearch (search_p, data_p -> issd_connection_p);

					if (results_p)
						{
							query_results_json_p = GetQueryResultAsJSON (results_p);
						}
				}

			FreeIrodsSearch (search_p);

			res_json_p = CreateServiceResponseAsJSON (GetServiceName (service_p), res, query_results_json_p);
		}		/* if (search_p) */

	return res_json_p;
}


static bool IsFileForIrodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;
	const char *filename_s = resource_p -> re_value_s;



	return interested_flag;
}

