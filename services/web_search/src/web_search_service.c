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


static const char *S_METHOD_S = "method";
static const char *S_URI_S = "uri";


/*
 * STATIC PROTOTYPES
 */

static Service *GetWebSearchService (json_t *operation_json_p);

static const char *GetWebSearchServiceName (Service *service_p);

static const char *GetWebSearchServiceDesciption (Service *service_p);

static ParameterSet *GetWebSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseWebSearchServiceParameters (Service *service_p, ParameterSet *params_p);

static json_t *RunWebSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForWebSearchService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebSearchServiceData *AllocateWebSearchServiceData (json_t *config_p);


static void FreeWebSearchServiceData (WebSearchServiceData *data_p);

static bool CloseWebSearchService (Service *service_p);


static bool AddParametersToPostWebSearchService (Service *service_p, ParameterSet *param_set_p);

static bool AddParametersToGetWebSearchService (Service *service_p, ParameterSet *param_set_p);

static bool AddParametersToBodyWebSearchService (Service *service_p, ParameterSet *param_set_p);

static bool AddPostParameter (const Parameter * const param_p, CurlTool *curl_data_p);

static bool CallCurlWebservice (WebSearchServiceData *data_p);


/*
 * API FUNCTIONS
 */
 

 
ServicesArray *GetServices (json_t *config_p)
{
	return GetReferenceServicesFromJSON (config_p, "web_search_service");
}


void ReleaseService (Service *service_p)
{
	FreeWebSearchServiceData ((WebSearchServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */




static Service *GetWebSearchService (json_t *operation_json_p)
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
							return data_p;
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
	
	return (data_p -> wssd_name_s);
}


static const char *GetWebSearchServiceDesciption (Service *service_p)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_description_s);
}


static ParameterSet *GetWebSearchServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);

	return (data_p -> wssd_params_p);
}


static void ReleaseWebSearchServiceParameters (Service *service_p, ParameterSet *params_p)
{
	/*
	 * As the parameters are cached, we release the parameters when the service is destroyed
	 * so we need to do anything here.
	 */
}



static bool AppendParameterValue (ByteBuffer *buffer_p, const Parameter *param_p)
{
	bool success_flag = false;
	char *value_s = NULL;
	const SharedType * const value_p = & (param_p -> pa_current_value);
	bool alloc_flag = false;
	
	switch (param_p -> pa_type)
		{
			case PT_BOOLEAN:
				value_s = (char *) ((value_p -> st_boolean_value == true) ? "true" : "false");
				break;

			case PT_SIGNED_INT:
				value_s = ConvertNumberToString ((double) (value_p -> st_long_value), 0);
				alloc_flag = true;
				break;

			case PT_UNSIGNED_INT:
				value_s = ConvertNumberToString ((double) (value_p -> st_ulong_value), 0);
				alloc_flag = true;
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				value_s = ConvertNumberToString (value_p -> st_data_value, 0);
				alloc_flag = true;
				break;
			
			case PT_DIRECTORY:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
				value_s = value_p -> st_resource_value_p -> re_value_s;
				break;

			case PT_STRING:
			case PT_PASSWORD:
				value_s = value_p -> st_string_value_s;
				break;

			default:
				break;
		}		/* switch (param_p -> pa_type) */

	if (value_s)
		{
			success_flag = AppendToByteBuffer (buffer_p, value_s, strlen (value_s));
			
			if (alloc_flag)
				{
					FreeCopiedString (value_s);
				}
		}

	return success_flag;
}


static bool AddParametersToGetWebSearchService (Service *service_p, ParameterSet *param_set_p)
{
	bool success_flag = true;
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	ByteBuffer *buffer_p = data_p -> wssd_buffer_p;
	char c = '?';
	ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
			
	ResetByteBuffer (data_p -> wssd_buffer_p);
						
	while (node_p && success_flag)
		{
			Parameter *param_p = node_p -> pn_parameter_p;
			success_flag = false;
			
			if (AppendToByteBuffer (buffer_p, &c, 1))
				{
					if (c == '?')
						{
							c = '&';
						}
					
					if (AppendToByteBuffer (buffer_p, param_p -> pa_name_s, strlen (param_p -> pa_name_s)))
						{
							const char * const equal_s = "=";
							
							if (AppendToByteBuffer (buffer_p, equal_s, strlen (equal_s)))
								{
									if (AppendParameterValue (buffer_p, param_p))
										{
											success_flag = true;
										}
								}
						}
					
				}		/* if (AppendToByteBuffer (buffer_p, &c, 1)) */

			if (success_flag)
				{
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
		}

	return success_flag;
}



static bool AddPostParameter (const Parameter * const param_p, CurlTool *curl_data_p)
{
	bool success_flag = false;
	bool alloc_flag = false;
	char *value_s = GetParameterValueAsString (param_p, &alloc_flag);
	
	if (value_s)
		{
			CURLFORMcode res;
			CURLformoption opt;			
			
			switch (param_p -> pa_type)
				{
					case PT_DIRECTORY:
					case PT_FILE_TO_READ:
					case PT_FILE_TO_WRITE:
						opt = CURLFORM_FILE;
						break;

					default:					
						opt = CURLFORM_COPYCONTENTS;
						break;
				}		/* switch (param_p -> pa_type) */
			
			res = curl_formadd (& (curl_data_p -> ct_form_p), & (curl_data_p -> ct_last_field_p), CURLFORM_COPYNAME, param_p -> pa_name_s, opt, value_s, CURLFORM_END);
			
			if (res == 0)
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to add post parameter %s=%s to web service\n", param_p -> pa_name_s, value_s);
				}
			
			if (alloc_flag)
				{
					FreeCopiedString (value_s);
				}
				
		}		/* if (value_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to get post parameter value for %s\n", param_p -> pa_name_s);
		}

	return success_flag;
}


static bool AddParametersToPostWebSearchService (Service *service_p, ParameterSet *param_set_p)
{
	bool success_flag = true;
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	ByteBuffer *buffer_p = data_p -> wssd_buffer_p;
	ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
			
	ResetByteBuffer (data_p -> wssd_buffer_p);
						
	while (node_p && success_flag)
		{
			success_flag = AddPostParameter (node_p -> pn_parameter_p, data_p -> wssd_curl_data_p);

			if (success_flag)
				{
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
		}

	if (success_flag)
		{
			CURLcode res = curl_easy_setopt (data_p -> wssd_curl_data_p -> ct_curl_p, CURLOPT_HTTPPOST, data_p -> wssd_curl_data_p -> ct_form_p);

			success_flag = (res == CURLE_OK);
		}

	return success_flag;
}


/*
  curl's project page on SourceForge.net

Sponsors:
Haxx
	cURL > Mailing List > Monthly Index > Single Mail
curl-library Archives

Re: HTTP Post with json body and client SSL certificate validation

    This message: [ Message body ] [ More options ]
    Related messages: [ Next message ] [ Previous message ] [ In reply to ]

From: C�dric Deltheil <cedric_at_moodstocks.com>
Date: Fri, 27 Sep 2013 14:31:48 +0200

Le 27 sept. 2013 � 13:28, Victor Dodon <dodonvictor_at_gmail.com> a �crit :

> Q1. I need to set some custom headers for each request. The list of custom headers can be freed imediately after curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist) or only after the curl_easy_perform has returned?

After. See the HTTP custom header example[1].

> Q2. How to do a post request with json HTTP body? Is something like this:
>
> char *post_body = json_dumps(json, 0);
> struct curl_slist *slist=NULL;
> slist = curl_slist_append(slist, "Content-Type: application/json");
> curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
> curl_easy_setopt(curl, CURLOPT_POST, 1);
> curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);
> curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_body);
>
> ?

Yes. You do not even need to pass the CURLOPT_POSTFIELDSIZE since by default libcurl will do the strlen() for you.

Also, I would say CURLOPT_POST is redundant: you can omit it.

> Q3. Microsoft and Apple server require client SSL certificate validation. Assuming that I have a suitable SSL certififcate, what options I need to set on curl handles to achieve this? CURLOPT_SSLCERT and CURLOPT_SSLCERTTYPE are enough?

typedef struct CurlTool
{
	struct curl_httppost *ct_form_p;
	struct curl_httppost *ct_last_field_p;
	struct curl_slist *ct_headers_list_p;
} CurlTool;


Don't know about that :) 
*/	

static bool AddParametersToBodyWebSearchService (Service *service_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	ByteBuffer *buffer_p = data_p -> wssd_buffer_p;
	json_t *json_p = GetParameterSetAsJSON (param_set_p, true);
	
	if (json_p)
		{
			char *dump_s = json_dumps (json_p, JSON_INDENT (2));
			
			if (dump_s)
				{
					if (AppendToByteBuffer (buffer_p, dump_s, strlen (dump_s)))
						{
							CurlTool *curl_data_p = data_p -> wssd_curl_data_p;
							curl_data_p -> ct_headers_list_p = curl_slist_append (curl_data_p -> ct_headers_list_p, "Content-Type: application/json");
							
							if (curl_data_p -> ct_headers_list_p)
								{
									CURLcode res = curl_easy_setopt (curl_data_p -> ct_curl_p, CURLOPT_POSTFIELDS, buffer_p -> bb_data_p);
									
									if (res == CURLE_OK)
										{
											res = curl_easy_setopt (curl_data_p -> ct_curl_p, CURLOPT_POSTFIELDSIZE, GetByteBufferSize (buffer_p));
											
											if (res == CURLE_OK)
												{
													success_flag = true;													
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to set postfield size for json call by %s, error %s\n", GetServiceName (service_p), curl_easy_strerror (res));
												}
											
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, "Failed to set postfield for json call by %s, error %s\n", GetServiceName (service_p), curl_easy_strerror (res));
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to set content type header for json call by %s\n", GetServiceName (service_p));
								}

						}
					
					free (dump_s);
				}
			
			json_decref (json_p);
		}
	
	return success_flag;
}



static bool CloseWebSearchService (Service *service_p)
{
	bool success_flag = true;
	
	return success_flag;
}


static json_t *RunWebSearchService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebSearchServiceData *data_p = (WebSearchServiceData *) (service_p -> se_data_p);
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	
	if (param_set_p)
		{
			bool success_flag = true;

			ResetByteBuffer (data_p -> wssd_buffer_p);

			switch (data_p -> wssd_method)
				{
					case SM_POST:
						success_flag = AddParametersToPostWebSearchService (service_p, param_set_p);
						break;
						
					case SM_GET:
						success_flag = AddParametersToGetWebSearchService (service_p, param_set_p);
						break;
						
					case SM_BODY:
						success_flag = AddParametersToBodyWebSearchService (service_p, param_set_p);
						break;
						
					default:
						break;
				}

			if (success_flag)
				{
					if (CallCurlWebservice (data_p))
						{
							json_error_t error;
							const char * const data_s = GetCurlToolData (data_p -> wssd_curl_data_p);
							json_t *web_service_response_json_p = GetMatchingLinksAsJSON (data_s, data_p -> wssd_css_selector_s);
							const char *service_name_s = GetServiceName (service_p);

							if (!web_service_response_json_p)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to decode response from %s, error is %s:\n", service_name_s, error.text);
								}

							res_json_p = CreateServiceResponseAsJSON (GetServiceName (service_p), res, web_service_response_json_p);

						}		/* if (CallCurlWebservice (data_p)) */

				}		/* if (success_flag) */
						
		}		/* if (param_set_p) */

	return res_json_p;
}


static bool CallCurlWebservice (WebSearchServiceData *data_p)
{
	bool success_flag = false;	
	CURLcode res = curl_easy_perform (data_p -> wssd_curl_data_p -> ct_curl_p);

	if (res == CURLE_OK)
		{
			success_flag = true;
		}
	else
		{
			const char *service_name_s = GetServiceName (data_p -> wssd_base_data.sd_service_p);
			PrintErrors (STM_LEVEL_SEVERE, "Failed to call webservice for %s, error %s\n", service_name_s, curl_easy_strerror (res));
		}
	
	return success_flag;
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

