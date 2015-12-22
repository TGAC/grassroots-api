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

#include "web_search_service.h"
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
#include "selector.hpp"
#include "web_service_util.h"
#include "service_job.h"


typedef struct WebSearchServiceData
{
	WebServiceData wssd_base_data;
	const char *wssd_link_selector_s;
	const char *wssd_title_selector_s;
} WebSearchServiceData;

/*
 * STATIC PROTOTYPES
 */


static Service *GetWebSearchService (json_t *operation_json_p, size_t i);

static const char *GetWebSearchServiceName (Service *service_p);

static const char *GetWebSearchServiceDesciption (Service *service_p);


static const char *GetWebSearchServiceInformationUri (Service *service_p);


static ParameterSet *GetWebSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseWebSearchServiceParameters (Service *service_p, ParameterSet *params_p);

ServiceJobSet *RunWebSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);


static bool IsResourceForWebSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebSearchServiceData *AllocateWebSearchServiceData (json_t *config_p);

static json_t *GetWebSearchServiceResults (Service *service_p, const uuid_t job_id);

static void FreeWebSearchServiceData (WebSearchServiceData *data_p);

static bool CloseWebSearchService (Service *service_p);

static bool CleanUpWebSearchServiceJob (ServiceJob *job_p);

static json_t *CreateWebSearchServiceResults (WebSearchServiceData *data_p);

/*
 * API FUNCTIONS
 */
 

 
ServicesArray *GetServices (json_t *config_p)
{
	return GetReferenceServicesFromJSON (config_p, "web_search_service", GetWebSearchService);
}


void ReleaseService (Service *service_p)
{
	FreeWebSearchServiceData ((WebSearchServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */




static Service *GetWebSearchService (json_t *operation_json_p, size_t i)
{									
	Service *web_service_p = (Service *) AllocMemory (sizeof (Service));
	
	if (web_service_p)
		{
			ServiceData *data_p = (ServiceData *) AllocateWebSearchServiceData (operation_json_p);
			
			if (data_p)
				{
					InitialiseService (web_service_p,
						GetWebSearchServiceName,
						GetWebSearchServiceDesciption,
						GetWebSearchServiceInformationUri,
						RunWebSearchService,
						IsResourceForWebSearchService,
						GetWebSearchServiceParameters,
						ReleaseWebSearchServiceParameters,
						CloseWebSearchService,
						GetWebSearchServiceResults,
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


static WebSearchServiceData *AllocateWebSearchServiceData (json_t *op_json_p)
{
	WebSearchServiceData *service_data_p = (WebSearchServiceData *) AllocMemory (sizeof (WebSearchServiceData));
	
	if (service_data_p)
		{
			WebServiceData *data_p = & (service_data_p -> wssd_base_data);

			if (InitWebServiceData (data_p, op_json_p))
				{
					service_data_p -> wssd_link_selector_s = GetJSONString (op_json_p, "link_selector");

					if (service_data_p -> wssd_link_selector_s)
						{
							service_data_p -> wssd_title_selector_s = GetJSONString (op_json_p, "title_selector");

							return service_data_p;
						}

					ClearWebServiceData (data_p);
				}

			FreeMemory (service_data_p);
		}
		
	return NULL;
}


static void FreeWebSearchServiceData (WebSearchServiceData *data_p)
{
	ClearWebServiceData (& (data_p -> wssd_base_data));
	
	FreeMemory (data_p);
}


static const char *GetWebSearchServiceName (Service *service_p)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	
	return (data_p -> wssd_base_data.wsd_name_s);
}


static const char *GetWebSearchServiceDesciption (Service *service_p)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_base_data.wsd_description_s);
}


static const char *GetWebSearchServiceInformationUri (Service *service_p)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_base_data.wsd_info_uri_s);
}


static ParameterSet *GetWebSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_base_data.wsd_params_p);
}


static void ReleaseWebSearchServiceParameters (Service *service_p, ParameterSet *params_p)
{
	/*
	 * As the parameters are cached, we release the parameters when the service is destroyed
	 * so we need to do anything here.
	 */
}




static bool CloseWebSearchService (Service *service_p)
{
	bool success_flag = true;
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);

	FreeWebSearchServiceData (data_p);
	
	/*
	if (service_p -> se_jobs_p)
		{
			FreeServiceJobSet (service_p -> se_jobs_p);
			service_p -> se_jobs_p = 0;
		}
	 */
	return success_flag;
}


ServiceJobSet *RunWebSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebSearchServiceData *service_data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	WebServiceData *data_p = & (service_data_p -> wssd_base_data);
	
	/* We only have one task */
	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1, CleanUpWebSearchServiceJob);

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
							if (CallCurlWebservice (data_p))
								{
									job_p -> sj_status = OS_SUCCEEDED;
									job_p -> sj_result_p = CreateWebSearchServiceResults (service_data_p);
								}		/* if (CallCurlWebservice (data_p)) */

						}		/* if (success_flag) */

				}		/* if (param_set_p) */

		}

	return service_p -> se_jobs_p;
}


static json_t *CreateWebSearchServiceResults (WebSearchServiceData *data_p)
{
	json_t *res_p = NULL;
	const char * const data_s = GetCurlToolData (data_p -> wssd_base_data.wsd_curl_data_p);

	if (data_s && *data_s)
		{
			res_p = GetMatchingLinksAsJSON (data_s, data_p -> wssd_link_selector_s, data_p -> wssd_title_selector_s, data_p -> wssd_base_data.wsd_base_uri_s);
		}

	return res_p;
}


static json_t *GetWebSearchServiceResults (Service *service_p, const uuid_t job_id)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	ServiceJob *job_p = GetJobById (service_p -> se_jobs_p, job_id);
	json_t *res_p = NULL;

	if (job_p)
		{
			if (job_p -> sj_status == OS_SUCCEEDED)
				{
					if (!job_p -> sj_result_p)
						{
							job_p -> sj_result_p = CreateWebSearchServiceResults (data_p);
						}

					res_p = job_p -> sj_result_p;
				}
		}		/* if (job_p) */

	return res_p;
}
	

static bool IsResourceForWebSearchService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;

	if (resource_p -> re_protocol_s)
		{
			interested_flag = (strcmp (resource_p -> re_protocol_s, "string") == 0);
		}

	return interested_flag;
}


static bool CleanUpWebSearchServiceJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;

	return cleaned_up_flag;
}
