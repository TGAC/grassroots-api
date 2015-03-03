#include <string.h>

#include "ensembl_rest_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"

/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData ersd_base_data;
} EnsemblRestServiceData;


/*
 * STATIC PROTOTYPES
 */

static const char *GetEnsemblRestServiceName (Service *service_p);

static const char *GetEnsemblRestServiceDesciption (Service *service_p);

static const char *GetEnsemblRestServiceURI (Service *service_p);

static ParameterSet *GetEnsemblRestServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseEnsemblRestServiceParameters (Service *service_p, ParameterSet *params_p);

static json_t *RunEnsemblRestService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForEnsemblRestService (Service *service_p, Resource *resource_p, Handler *handler_p);


static int EnsemblRest (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p);

static bool CloseEnsemblRestService (Service *service_p);



/*
 * API FUNCTIONS
 */

ServicesArray *GetServices (const json_t *config_p)
{
	Service *compress_service_p = (Service *) AllocMemory (sizeof (Service));

	if (compress_service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					ServiceData *data_p = NULL;
					
					InitialiseService (compress_service_p,
						GetEnsemblRestServiceName,
						GetEnsemblRestServiceDesciption,
						GetEnsemblRestServiceURI,
						RunEnsemblRestService,
						IsFileForEnsemblRestService,
						GetEnsemblRestServiceParameters,
						ReleaseEnsemblRestServiceParameters,
						CloseEnsemblRestService,
						true,
						data_p);
					
					* (services_p -> sa_services_pp) = compress_service_p;
							
					return services_p;
				}
				
			FreeService (compress_service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


static bool CloseEnsemblRestService (Service *service_p)
{
	bool success_flag = true;
	
	return success_flag;
}

/*
 * STATIC FUNCTIONS
 */


static const char *GetEnsemblRestServiceName (Service *service_p)
{
	return "Ensembl Plants service";
}


static const char *GetEnsemblRestServiceDesciption (Service *service_p)
{
	return "A service to access the Ensembl Plants data";
}


static const char *GetEnsemblRestServiceURI (Service *service_p)
{
	return "http://plants.ensembl.org/index.html";
}


static ParameterSet *GetEnsemblRestServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("EnsemblRest service parameters", "The parameters used for the EnsemblRest service");

	if (param_set_p)
		{
			SharedType def;
				
			def.st_resource_value_p = resource_p;		

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, S_INPUT_PARAM_NAME_S, NULL, "The input file to read", TAG_INPUT_FILE, NULL, def, NULL, NULL, PL_BASIC, NULL))
				{
					ParameterMultiOptionArray *options_p = NULL;
					const char *descriptions_pp [CA_NUM_ALGORITHMS] = { "Use Raw", "Use Zip", "Use GZip" };
					SharedType values [CA_NUM_ALGORITHMS];

					values [CA_Z].st_string_value_s = (char *) s_algorithm_names_pp [CA_Z];
					values [CA_ZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_ZIP];
					values [CA_GZIP].st_string_value_s = (char *) s_algorithm_names_pp [CA_GZIP];

					options_p = AllocateParameterMultiOptionArray (CA_NUM_ALGORITHMS, descriptions_pp, values, PT_STRING);

					if (options_p)
						{
							def.st_string_value_s = values [0].st_string_value_s;

							if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "EnsemblRestion algorithm", NULL, "The algorithm to use to compress the data with", TAG_COMPRESS_ALGORITHM, options_p, def, NULL, NULL, PL_BASIC, NULL))
								{
									return param_set_p;
								}

							FreeParameterMultiOptionArray (options_p);
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static void ReleaseEnsemblRestServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static json_t *RunEnsemblRestService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	SharedType input_resource;
	
	if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &input_resource, true))
		{
			
		}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */
	
	res_json_p = CreateServiceResponseAsJSON (service_p, res, NULL);

	return res_json_p;
}



static int EnsemblRest (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p)
{
	bool success_flag = false;
	int i;
	int algo_index = -1;
	int res = 0;
	
	/* Find the algorithm to use */
	for (i = 0; i < CA_NUM_ALGORITHMS; ++ i)
		{
			if (strcmp (algorithm_s, s_algorithm_names_pp [i]) == 0)
				{
					algo_index = i;
					i = CA_NUM_ALGORITHMS;
				}
		}
		
	if (algo_index)
		{
			bool replace_flag = false;

			#ifdef WINDOWS
			replace_flag = true;
			#endif
			
			char *output_name_s = SetFileExtension (input_resource_p -> re_value_s, s_algorithm_names_pp [algo_index], replace_flag);
			
			if (output_name_s)
				{
					Handler *input_handler_p = GetResourceHandler (input_resource_p, credentials_p);

					if (input_handler_p)
						{
							Resource output_resource;
							Handler *output_handler_p = NULL;
							
							output_resource.re_protocol_s = input_resource_p -> re_protocol_s;
							output_resource.re_value_s = output_name_s;
							
							output_handler_p = GetResourceHandler (&output_resource, credentials_p);

							if (output_handler_p)
								{
									if (OpenHandler (input_handler_p, input_resource_p -> re_value_s, "rb"))
										{											
											if (OpenHandler (output_handler_p, output_name_s, "wb"))
												{
													int level = Z_DEFAULT_COMPRESSION;	/* gzip default */
													res = s_compress_fns [algo_index] (input_handler_p, output_handler_p, level);
													
													success_flag = (res == Z_OK);
													
													CloseHandler (output_handler_p);											
												}		/* if (OpenHandler (output_handler_p, output_name_s "wb")) */
												
										}		/* if (OpenHandler (input_handler_p, input_resource_p -> re_value_s, "rb")) */
								
 								}		/* if (output_handler_p) */
							
							CloseHandler (input_handler_p);
						}		/* if (input_handler_p) */
								
					FreeCopiedString (output_name_s);
				}		/* if (output_name_s) */
						
		}		/* if (algo_index) */
	
	
	return success_flag;
}


static bool IsFileForEnsemblRestService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;

	return interested_flag;
}

