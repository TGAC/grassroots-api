#include <string.h>

#include "ensembl_rest_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service.h"
#include "service_job.h"

#include "sequence.h"

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

static ServiceJobSet *RunEnsemblRestService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForEnsemblRestService (Service *service_p, Resource *resource_p, Handler *handler_p);



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
						NULL,
						NULL,
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



const char * const GetRootRestURI (void)
{
	return "http://rest.ensemblgenomes.org/";
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
			if (AddSequenceParameters (param_set_p))
				{
					return param_set_p;
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static void ReleaseEnsemblRestServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunEnsemblRestService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	json_t *results_json_p = NULL;
	CurlTool *curl_tool_p = AllocateCurlTool ();

	if (curl_tool_p)
		{
			results_json_p = json_array ();

			if (results_json_p)
				{
					if (RunSequenceSearch (param_set_p, results_json_p, curl_tool_p))
						{
							res = OS_SUCCEEDED;
						}
				}

			FreeCurlTool (curl_tool_p);
		}
	

	res_json_p = CreateServiceResponseAsJSON (service_p, res, results_json_p, NULL);

	return service_p -> se_jobs_p;
}



static bool IsFileForEnsemblRestService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;

	return interested_flag;
}

