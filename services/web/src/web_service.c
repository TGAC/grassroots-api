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


/*
 * STATIC PROTOTYPES
 */

static Service *GetWebService (json_t *operation_json_p, size_t i);

static const char *GetWebServiceName (Service *service_p);

static const char *GetWebServiceDesciption (Service *service_p);

static const char *GetWebServiceInformationUri (Service *service_p);

static ParameterSet *GetWebServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseWebServiceParameters (Service *service_p, ParameterSet *params_p);

static json_t *RunWebService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForWebService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebServiceData *AllocateWebServiceData (json_t *config_p);


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
						NULL,
						false,
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
	
	return success_flag;
}


static json_t *RunWebService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);
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
					json_error_t error;
					json_t *web_service_response_json_p = NULL;
					const char *service_name_s = GetServiceName (service_p);
					
					res = (CallCurlWebservice (data_p)) ? OS_SUCCEEDED : OS_FAILED;	
					
					if (res == OS_SUCCEEDED)
						{
							const char *buffer_data_p = GetCurlToolData (data_p -> wsd_curl_data_p);
							web_service_response_json_p = json_loads (buffer_data_p, 0, &error);

							if (!web_service_response_json_p)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to decode response from %s, error is %s:\n%s\n", service_name_s, error.text, buffer_data_p);
								}

							res_json_p = CreateServiceResponseAsJSON (service_p, res, web_service_response_json_p);

						}		/* if (res == OS_SUCCEEDED) */

				}		/* if (success_flag) */
						
		}		/* if (param_set_p) */

	return res_json_p;
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




