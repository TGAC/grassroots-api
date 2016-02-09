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

#include <arpa/inet.h>

#include <curl/curl.h>

#include "web_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "math_utils.h"
#include "filesystem_utils.h"
#include "byte_buffer.h"
#include "streams.h"
#include "curl_tools.h"
#include "web_service_util.h"
#include "service_job.h"

/*
 * STATIC PROTOTYPES
 */

static Service *GetWebService (json_t *operation_json_p, size_t i);

static const char *GetWebServiceName (Service *service_p);

static const char *GetWebServiceDesciption (Service *service_p);

static const char *GetWebServiceInformationUri (Service *service_p);

static ParameterSet *GetWebServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseWebServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunWebService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForWebService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebServiceData *AllocateWebServiceData (json_t *config_p);

static json_t *GetWebServiceResults (Service *service_p, const uuid_t job_id);


static void FreeWebServiceData (WebServiceData *data_p);

static bool CloseWebService (Service *service_p);



/*
 * API FUNCTIONS
 */
 

ServicesArray *GetServices (json_t *config_p)
{
	return GetReferenceServicesFromJSON (config_p, "web_service", GetWebService);
}



void ReleaseService (Service *service_p)
{
	FreeWebServiceData ((WebServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */



static json_t *GetWebServiceResults (Service *service_p, const uuid_t job_id)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);
	ServiceJob *job_p = GetServiceJobFromServiceJobSetById (service_p -> se_jobs_p, job_id);
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



static Service *GetWebService (json_t *operation_json_p, size_t i)
{									
	Service *web_service_p = (Service *) AllocMemory (sizeof (Service));
	
	if (web_service_p)
		{
			ServiceData *data_p = (ServiceData *) AllocateWebServiceData (operation_json_p);
			
			if (data_p)
				{
					InitialiseService (web_service_p,
						GetWebServiceName,
						GetWebServiceDesciption,
						GetWebServiceInformationUri,
						RunWebService,
						IsResourceForWebService,
						GetWebServiceParameters,
						ReleaseWebServiceParameters,
						CloseWebService,
						GetWebServiceResults,
						NULL,
						false,
						true,
						data_p);

					return web_service_p;
				}
			
			FreeMemory (web_service_p);
		}		/* if (web_service_p) */
			
	return NULL;
}


static WebServiceData *AllocateWebServiceData (json_t *op_json_p)
{
	WebServiceData *data_p = (WebServiceData *) AllocMemory (sizeof (WebServiceData));
	
	if (data_p)
		{
			if (!InitWebServiceData (data_p, op_json_p))
				{
					FreeMemory (data_p);
					data_p = NULL;
				}
		}
		
	return data_p;
}


static void FreeWebServiceData (WebServiceData *data_p)
{
	ClearWebServiceData (data_p);
	
	FreeMemory (data_p);
}


static const char *GetWebServiceName (Service *service_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);
	
	return (data_p -> wsd_name_s);
}


static const char *GetWebServiceDesciption (Service *service_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);

	return (data_p -> wsd_description_s);
}


static const char *GetWebServiceInformationUri (Service *service_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);

	return (data_p -> wsd_info_uri_s);
}



static ParameterSet *GetWebServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);

	return (data_p -> wsd_params_p);
}


static void ReleaseWebServiceParameters (Service *service_p, ParameterSet *params_p)
{
	/*
	 * As the parameters are cached, we release the parameters when the service is destroyed
	 * so we need to do anything here.
	 */
}



static bool CloseWebService (Service *service_p)
{
	bool success_flag = true;
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);

	FreeWebServiceData (data_p);
	
	return success_flag;
}


static ServiceJobSet *RunWebService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);

	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Web Service Job", NULL);

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);

			job_p -> sj_status = OS_FAILED_TO_START;

			if (param_set_p)
				{
					bool success_flag = true;

					ResetByteBuffer (data_p -> wsd_buffer_p);

					switch (data_p -> wsd_method)
						{
							case SM_POST:
								success_flag = AddParametersToPostWebService (data_p, param_set_p);
								break;

							case SM_GET:
								success_flag = AddParametersToGetWebService (data_p, param_set_p);
								break;

							case SM_BODY:
								success_flag = AddParametersToBodyWebService (data_p, param_set_p);
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

	

static bool IsResourceForWebService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;

	if (resource_p -> re_protocol_s)
		{
			interested_flag = (strcmp (resource_p -> re_protocol_s, "string") == 0);
		}

	return interested_flag;
}

