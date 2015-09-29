#include <string.h>

#include <arpa/inet.h>

#include <curl/curl.h>

#include "web_search_json_service.h"
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


typedef struct WebSearchJSONServiceData
{
	WebServiceData wssjd_base_data;
	const char *wssd_link_selector_s;
	const char *wssd_title_selector_s;
} WebSearchJSONServiceData;

/*
 * STATIC PROTOTYPES
 */


static Service *GetWebSearchJSONService (json_t *operation_json_p, size_t i);

static const char *GetWebSearchJSONServiceName (Service *service_p);

static const char *GetWebSearchJSONServiceDesciption (Service *service_p);


static const char *GetWebSearchJSONServiceInformationUri (Service *service_p);


static ParameterSet *GetWebSearchJSONServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseWebSearchJSONServiceParameters (Service *service_p, ParameterSet *params_p);

ServiceJobSet *RunWebSearchJSONService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);


static bool IsResourceForWebSearchJSONService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebSearchJSONServiceData *AllocateWebSearchJSONServiceData (json_t *config_p);

static json_t *GetWebSearchJSONServiceResults (Service *service_p, const uuid_t job_id);

static void FreeWebSearchJSONServiceData (WebSearchJSONServiceData *data_p);

static bool CloseWebSearchJSONService (Service *service_p);

static bool CleanUpWebSearchJSONServiceJob (ServiceJob *job_p);

static json_t *CreateWebSearchJSONServiceResults (WebSearchJSONServiceData *data_p);

/*
 * API FUNCTIONS
 */
 

 
ServicesArray *GetServices (json_t *config_p)
{
	return GetReferenceServicesFromJSON (config_p, "web_search_service", GetWebSearchJSONService);
}


void ReleaseService (Service *service_p)
{
	FreeWebSearchJSONServiceData ((WebSearchJSONServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */




static Service *GetWebSearchJSONService (json_t *operation_json_p, size_t i)
{									
	Service *web_service_p = (Service *) AllocMemory (sizeof (Service));
	
	if (web_service_p)
		{
			ServiceData *data_p = (ServiceData *) AllocateWebSearchJSONServiceData (operation_json_p);
			
			if (data_p)
				{
					InitialiseService (web_service_p,
						GetWebSearchJSONServiceName,
						GetWebSearchJSONServiceDesciption,
						GetWebSearchJSONServiceInformationUri,
						RunWebSearchJSONService,
						IsResourceForWebSearchJSONService,
						GetWebSearchJSONServiceParameters,
						ReleaseWebSearchJSONServiceParameters,
						CloseWebSearchJSONService,
						GetWebSearchJSONServiceResults,
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


static WebSearchJSONServiceData *AllocateWebSearchJSONServiceData (json_t *op_json_p)
{
	WebSearchJSONServiceData *service_data_p = (WebSearchJSONServiceData *) AllocMemory (sizeof (WebSearchJSONServiceData));
	
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


static void FreeWebSearchJSONServiceData (WebSearchJSONServiceData *data_p)
{
	ClearWebServiceData (& (data_p -> wssd_base_data));
	
	FreeMemory (data_p);
}


static const char *GetWebSearchJSONServiceName (Service *service_p)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);
	
	return (data_p -> wssd_base_data.wsd_name_s);
}


static const char *GetWebSearchJSONServiceDesciption (Service *service_p)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_base_data.wsd_description_s);
}


static const char *GetWebSearchJSONServiceInformationUri (Service *service_p)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_base_data.wsd_info_uri_s);
}


static ParameterSet *GetWebSearchJSONServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_base_data.wsd_params_p);
}


static void ReleaseWebSearchJSONServiceParameters (Service *service_p, ParameterSet *params_p)
{
	/*
	 * As the parameters are cached, we release the parameters when the service is destroyed
	 * so we need to do anything here.
	 */
}




static bool CloseWebSearchJSONService (Service *service_p)
{
	bool success_flag = true;
	
	return success_flag;
}


ServiceJobSet *RunWebSearchJSONService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebSearchJSONServiceData *service_data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);
	WebServiceData *data_p = & (service_data_p -> wssd_base_data);
	
	/* We only have one task */
	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1, CleanUpWebSearchJSONServiceJob);

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

									job_p -> sj_result_p = CreateWebSearchJSONServiceResults (service_data_p);
								}		/* if (CallCurlWebservice (data_p)) */

						}		/* if (success_flag) */

				}		/* if (param_set_p) */

		}

	return service_p -> se_jobs_p;
}


static json_t *CreateWebSearchJSONServiceResults (WebSearchJSONServiceData *data_p)
{
	json_t *res_p = NULL;
	const char * const data_s = GetCurlToolData (data_p -> wssd_base_data.wsd_curl_data_p);

	if (data_s && *data_s)
		{
			res_p = GetMatchingLinksAsJSON (data_s, data_p -> wssd_link_selector_s, data_p -> wssd_title_selector_s, data_p -> wssd_base_data.wsd_base_uri_s);
		}

	return res_p;
}


static json_t *GetWebSearchJSONServiceResults (Service *service_p, const uuid_t job_id)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);
	ServiceJob *job_p = GetJobById (service_p -> se_jobs_p, job_id);
	json_t *res_p = NULL;

	if (job_p)
		{
			if (job_p -> sj_status == OS_SUCCEEDED)
				{
					if (!job_p -> sj_result_p)
						{
							job_p -> sj_result_p = CreateWebSearchJSONServiceResults (data_p);
						}

					res_p = job_p -> sj_result_p;
				}
		}		/* if (job_p) */

	return res_p;
}
	

static bool IsResourceForWebSearchJSONService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;

	if (resource_p -> re_protocol_s)
		{
			interested_flag = (strcmp (resource_p -> re_protocol_s, "string") == 0);
		}

	return interested_flag;
}


static bool CleanUpWebSearchJSONServiceJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;

	return cleaned_up_flag;
}
