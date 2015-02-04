#include <string.h>


#include "irods_search_service_library.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "string_utils.h"
#include "service.h"

#include "query.h"
#include "connect.h"


/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData issd_base_data;
	rcComm_t *issd_connection_p;
	ParameterSet *issd_params_p;
} IrodsSearchServiceData;


/*
 * STATIC PROTOTYPES
 */

static const char *GetIrodsSearchServiceName (Service *service_p);

static const char *GetIrodsSearchServiceDesciption (Service *service_p);

static ParameterSet *GetIrodsSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);


static json_t *RunIrodsSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForIrodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);


static int IrodsSearch (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p);

static bool CloseIrodsSearchService (Service *service_p);


static rcComm_t *GetIRODSConnection (const json_t *config_p);

static bool AddParam (rcComm_t *connection_p, QueryResults *(*get_metadata_fn) (rcComm_t *connection_p), ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s);

static IrodsSearchServiceData *GetIrodsSearchServiceData (const json_t *config_p);



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
							if (AddParam (connection_p, GetAllMetadataDataAttributes, params_p, "Data objects metadata", NULL, "The metadata tags available for iRODS data objects"))
								{
									if (AddParam (connection_p, GetAllMetadataCollectionAttributes, params_p, "Collections metadata", NULL, "The metadata tags available for iRODS collections"))
										{
											data_p -> issd_connection_p = connection_p;
											data_p -> issd_params_p = params_p;

											return data_p;
										}
								}

							FreeParameterSet (params_p);
						}		/* if (params_p) */

					CloseConnection (connection_p);
				}		/* if (connection_p) */

			FreeMemory (data_p);
		}		/* if (data_p) */

	return NULL;
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




static bool AddParam (rcComm_t *connection_p, QueryResults *(*get_metadata_fn) (rcComm_t *connection_p), ParameterSet *param_set_p, const char *name_s, const char *display_name_s, const char *description_s)
{
	bool success_flag = false;
	QueryResults *results_p = get_metadata_fn (connection_p);

	if (results_p)
		{
			if (results_p -> qr_num_results == 1)
				{
					QueryResult *result_p = results_p -> qr_values_p;
					int i = result_p -> qr_num_values;
					SharedType *param_options_p = (SharedType *) AllocMemoryArray (i, sizeof (SharedType));

					if (param_options_p)
						{
							char **value_ss = result_p -> qr_values_pp;
							SharedType *option_p = param_options_p;
							ParameterMultiOptionArray *options_array_p = NULL;

							for ( ; i > 0; -- i, ++ value_ss, ++ option_p)
								{
									option_p -> st_string_value_s = *value_ss;
								}


							options_array_p = AllocateParameterMultiOptionArray (result_p -> qr_num_values, NULL, param_options_p, PT_STRING);

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


static json_t *RunIrodsSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	SharedType input_resource;
	
	res_json_p = CreateServiceResponseAsJSON (GetServiceName (service_p), res, NULL);

	return res_json_p;
}


static int IrodsSearch (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p)
{
	int res = 0;

	
	return res;
}


static bool IsFileForIrodsSearchService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;
	const char *filename_s = resource_p -> re_value_s;



	return interested_flag;
}

