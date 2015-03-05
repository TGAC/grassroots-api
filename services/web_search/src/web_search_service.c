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


typedef struct WebSearchServiceData
{
	WebServiceData wssd_base_data;
	const char *wssd_css_selector_s;
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


static bool IsResourceForWebSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebSearchServiceData *AllocateWebSearchServiceData (json_t *config_p);


static void FreeWebSearchServiceData (WebSearchServiceData *data_p);

static bool CloseWebSearchService (Service *service_p);

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
						false,
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
					service_data_p -> wssd_css_selector_s = GetJSONString (op_json_p, "selector");

					if (service_data_p -> wssd_css_selector_s)
						{
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
	
	return success_flag;
}


json_t *RunWebSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebSearchServiceData *service_data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	WebServiceData *data_p = & (service_data_p -> wssd_base_data);
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	
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
							json_error_t error;
							const char * const data_s = GetCurlToolData (data_p -> wsd_curl_data_p);
							json_t *web_service_response_json_p = GetMatchingLinksAsJSON (data_s, service_data_p -> wssd_css_selector_s, data_p -> wsd_base_uri_s);
							const char *service_name_s = GetServiceName (service_p);

							if (!web_service_response_json_p)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to decode response from %s, error is %s:\ndata:\n%s\n", service_name_s, error.text, data_s);
								}

							res_json_p = CreateServiceResponseAsJSON (service_p, res, web_service_response_json_p);

						}		/* if (CallCurlWebservice (data_p)) */

				}		/* if (success_flag) */
						
		}		/* if (param_set_p) */

	return res_json_p;
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

