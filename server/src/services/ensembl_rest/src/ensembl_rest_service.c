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

#include "ensembl_rest_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service.h"
#include "service_job.h"
#include "json_tools.h"


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

static ParameterSet *GetEnsemblRestServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static void ReleaseEnsemblRestServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunEnsemblRestService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static ParameterSet *IsFileForEnsemblRestService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseEnsemblRestService (Service *service_p);


/*
 * API FUNCTIONS
 */

ServicesArray *GetServices (UserDetails *user_p)
{
	Service *service_p = (Service *) AllocMemory (sizeof (Service));

	if (service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					ServiceData *data_p = (ServiceData *) AllocateEnsemblRestServiceData ();
					
					if (data_p)
						{
							InitialiseService (service_p,
								GetEnsemblRestServiceName,
								GetEnsemblRestServiceDesciption,
								GetEnsemblRestServiceURI,
								RunEnsemblRestService,
								IsFileForEnsemblRestService,
								GetEnsemblRestServiceParameters,
								ReleaseEnsemblRestServiceParameters,
								CloseEnsemblRestService,
								NULL,
								true,
								true,
								data_p);
							
							* (services_p -> sa_services_pp) = service_p;

							return services_p;
						}

					FreeServicesArray (services_p);
				}
				
			FreeService (service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}



const char * GetRootRestURI (void)
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
	WipeJSON (data_p -> ersd_results_p);

	FreeMemory (data_p);
}


static const char *GetEnsemblRestServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Ensembl Plants service";
}


static const char *GetEnsemblRestServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service to access the Ensembl Plants data";
}


static const char *GetEnsemblRestServiceURI (Service * UNUSED_PARAM (service_p))
{
	return "http://plants.ensembl.org/index.html";
}


static ParameterSet *GetEnsemblRestServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("EnsemblRest service parameters", "The parameters used for the EnsemblRest service");

	if (param_set_p)
		{
			if (AddSequenceParameters (service_p -> se_data_p, param_set_p))
				{
					return param_set_p;
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static void ReleaseEnsemblRestServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunEnsemblRestService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	EnsemblRestServiceData *data_p = (EnsemblRestServiceData *) service_p -> se_data_p;

	/* We only have one task */
	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Ensembl result");

	if (service_p -> se_jobs_p)
		{
			CurlTool *curl_tool_p = AllocateCurlTool (CM_MEMORY);
			ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);

			job_p -> sj_status = OS_FAILED_TO_START;

			if (curl_tool_p)
				{
					if (SetCurlToolForJSONPost (curl_tool_p))
						{
							json_t *result_p = NULL;

							json_array_clear (data_p -> ersd_results_p);

							result_p = RunSequenceSearch (param_set_p,  curl_tool_p);

							if (result_p)
								{
									if (AddResultToServiceJob (job_p, result_p))
										{
											job_p -> sj_status = OS_SUCCEEDED;
										}
									else
										{
											char uuid_s [UUID_STRING_BUFFER_SIZE];

											job_p -> sj_status = OS_ERROR;
											AddErrorToServiceJob (job_p, ERROR_S, "Failed to store result");

											ConvertUUIDToString (job_p -> sj_id, uuid_s);

											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add result for %s", uuid_s);
										}
								}
							else
								{
									job_p -> sj_status = OS_FAILED;
								}

						}		/* if (SetCurlToolForJSONPost (curl_tool_p)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set curl tool for HTTP POST request");
						}

					FreeCurlTool (curl_tool_p);
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate curl tool for %s", GetServiceName (service_p));
				}
		}
	else
		{

		}
	
	return service_p -> se_jobs_p;
}



static ParameterSet *IsFileForEnsemblRestService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}

