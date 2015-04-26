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
	json_t *ersd_results_p;
} EnsemblRestServiceData;


/*
 * STATIC PROTOTYPES
 */

static EnsemblRestServiceData *AllocateEnsemblRestServiceData (void);

static void FreeEnsemblRestServiceData (EnsemblRestServiceData *data_p);

static const char *GetEnsemblRestServiceName (Service *service_p);

static const char *GetEnsemblRestServiceDesciption (Service *service_p);

static const char *GetEnsemblRestServiceURI (Service *service_p);

static ParameterSet *GetEnsemblRestServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseEnsemblRestServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunEnsemblRestService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForEnsemblRestService (Service *service_p, Resource *resource_p, Handler *handler_p);


static json_t *GetEnsembleServiceResults (struct Service *service_p, const uuid_t job_id);


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
					ServiceData *data_p = (ServiceData *) AllocateEnsemblRestServiceData ();
					
					if (data_p)
						{
							InitialiseService (compress_service_p,
								GetEnsemblRestServiceName,
								GetEnsemblRestServiceDesciption,
								GetEnsemblRestServiceURI,
								RunEnsemblRestService,
								IsFileForEnsemblRestService,
								GetEnsemblRestServiceParameters,
								ReleaseEnsemblRestServiceParameters,
								CloseEnsemblRestService,
								GetEnsembleServiceResults,
								NULL,
								true,
								data_p);
							
							* (services_p -> sa_services_pp) = compress_service_p;

							return services_p;
						}

					FreeServicesArray (services_p);
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
	EnsemblRestServiceData *data_p = (EnsemblRestServiceData *) service_p -> se_data_p;

	FreeEnsemblRestServiceData (data_p);
	
	return success_flag;
}

/*
 * STATIC FUNCTIONS
 */


static EnsemblRestServiceData *AllocateEnsemblRestServiceData (void)
{
	EnsemblRestServiceData *data_p = (EnsemblRestServiceData *) AllocMemory (sizeof (EnsemblRestServiceData));

	if (data_p)
		{
			json_t *json_p = json_array ();

			if (json_p)
				{
					data_p -> ersd_results_p = json_p;
				}
			else
				{
					FreeMemory (data_p);
					data_p = NULL;
				}
		}

	return data_p;
}


static void FreeEnsemblRestServiceData (EnsemblRestServiceData *data_p)
{
	json_array_clear (data_p -> ersd_results_p);
	json_decref (data_p -> ersd_results_p);

	FreeMemory (data_p);
}


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
	EnsemblRestServiceData *data_p = (EnsemblRestServiceData *) service_p -> se_data_p;

	/* We only have one task */
	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1);

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = service_p -> se_jobs_p -> sjs_jobs_p;

			job_p -> sj_status = OS_FAILED_TO_START;

			CurlTool *curl_tool_p = AllocateCurlTool ();

			if (curl_tool_p)
				{
					json_array_clear (data_p -> ersd_results_p);

					if (RunSequenceSearch (param_set_p, data_p -> ersd_results_p, curl_tool_p))
						{
							job_p -> sj_status = OS_SUCCEEDED;
						}
					else
						{
							job_p -> sj_status = OS_FAILED;
						}

					FreeCurlTool (curl_tool_p);
				}
		}
	
	return service_p -> se_jobs_p;
}


static json_t *GetEnsembleServiceResults (struct Service *service_p, const uuid_t job_id)
{
	EnsemblRestServiceData *data_p = (EnsemblRestServiceData *) service_p -> se_data_p;

	/* Check that we have the correct job */
	if (uuid_compare (service_p -> se_jobs_p -> sjs_jobs_p -> sj_id, job_id) == 0)
		{
			return (data_p -> ersd_results_p);
		}

	return NULL;
}


static bool IsFileForEnsemblRestService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;

	return interested_flag;
}

