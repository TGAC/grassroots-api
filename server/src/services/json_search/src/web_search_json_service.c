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
#include "web_service_util.h"
#include "service_job.h"
#include "json_tools.h"

typedef struct WebSearchJSONServiceData
{
	WebServiceData wssjd_base_data;
	const char *wssjd_results_selector_s;
	const char *wssjd_title_selector_s;
	const char *wssjd_link_selector_s;
} WebSearchJSONServiceData;

/*
 * STATIC PROTOTYPES
 */


static Service *GetWebSearchJSONService (json_t *operation_json_p, size_t i);

static const char *GetWebSearchJSONServiceName (Service *service_p);

static const char *GetWebSearchJSONServiceDesciption (Service *service_p);


static const char *GetWebSearchJSONServiceInformationUri (Service *service_p);


static ParameterSet *GetWebSearchJSONServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static void ReleaseWebSearchJSONServiceParameters (Service *service_p, ParameterSet *params_p);

ServiceJobSet *RunWebSearchJSONService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);


static  ParameterSet *IsResourceForWebSearchJSONService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebSearchJSONServiceData *AllocateWebSearchJSONServiceData (json_t *config_p);


static void FreeWebSearchJSONServiceData (WebSearchJSONServiceData *data_p);

static bool CloseWebSearchJSONService (Service *service_p);


static json_t *CreateWebSearchJSONServiceResults (WebSearchJSONServiceData *data_p);

/*
 * API FUNCTIONS
 */
 

 
ServicesArray *GetServices (UserDetails *user_p, json_t *config_p)
{
	return GetReferenceServicesFromJSON (config_p, "web_search_json_service", GetWebSearchJSONService);
}


void ReleaseService (Service *service_p)
{
	FreeWebSearchJSONServiceData ((WebSearchJSONServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */




static Service *GetWebSearchJSONService (json_t *operation_json_p, size_t UNUSED_PARAM (i))
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
			WebServiceData *data_p = & (service_data_p -> wssjd_base_data);

			if (InitWebServiceData (data_p, op_json_p))
				{
					service_data_p -> wssjd_results_selector_s = GetJSONString (op_json_p, "results_selector");

					if (service_data_p -> wssjd_results_selector_s)
						{
							service_data_p -> wssjd_title_selector_s = GetJSONString (op_json_p, "title_selector");

							service_data_p -> wssjd_link_selector_s = GetJSONString (op_json_p, "link_selector");

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
	ClearWebServiceData (& (data_p -> wssjd_base_data));
	
	FreeMemory (data_p);
}


static const char *GetWebSearchJSONServiceName (Service *service_p)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);
	
	return (data_p -> wssjd_base_data.wsd_name_s);
}


static const char *GetWebSearchJSONServiceDesciption (Service *service_p)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);

	return (data_p -> wssjd_base_data.wsd_description_s);
}


static const char *GetWebSearchJSONServiceInformationUri (Service *service_p)
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);

	return (data_p -> wssjd_base_data.wsd_info_uri_s);
}


static ParameterSet *GetWebSearchJSONServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	WebSearchJSONServiceData *data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);

	return (data_p -> wssjd_base_data.wsd_params_p);
}


static void ReleaseWebSearchJSONServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet * UNUSED_PARAM (params_p))
{
	/*
	 * As the parameters are cached, we release the parameters when the service is destroyed
	 * so we need to do anything here.
	 */
}




static bool CloseWebSearchJSONService (Service *service_p)
{
	bool success_flag = true;
	WebSearchJSONServiceData *service_data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);

	FreeWebSearchJSONServiceData (service_data_p);
	
	return success_flag;
}


ServiceJobSet *RunWebSearchJSONService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	WebSearchJSONServiceData *service_data_p = (WebSearchJSONServiceData *) (service_p -> se_data_p);
	WebServiceData *data_p = & (service_data_p -> wssjd_base_data);
	
	/* We only have one task */
	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Web json-search");

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
							if (CallCurlWebservice (data_p))
								{
									json_t *results_p = CreateWebSearchJSONServiceResults (service_data_p);

									if (results_p)
										{
											if (ReplaceServiceJobResults (job_p, results_p))
												{
													job_p -> sj_status = OS_SUCCEEDED;
												}
											else
												{
													json_decref (results_p);
													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, results_p, "Failed to set job results");
												}

										}		/* if (results_p) */
								}		/* if (CallCurlWebservice (data_p)) */

						}		/* if (success_flag) */

				}		/* if (param_set_p) */

		}

	return service_p -> se_jobs_p;
}


static json_t *CreateWebSearchJSONServiceResults (WebSearchJSONServiceData *data_p)
{
	json_t *res_p = NULL;
	const char * const data_s = GetCurlToolData (data_p -> wssjd_base_data.wsd_curl_data_p);

	if (data_s && *data_s)
		{
			/* get the matching json tag for the selector */
			char *selector_s = CopyToNewString (data_p -> wssjd_results_selector_s, 0, false);

			if (selector_s)
				{
					json_error_t error;
					json_t *parent_p = json_loads (data_s, 0, &error);

					if (parent_p)
						{
							bool loop_flag = true;
							bool success_flag = true;
							char *start_p = selector_s;

							while (loop_flag && success_flag)
								{
									json_t *child_p;
									char *full_stop_p = strchr (start_p, '.');

									if (full_stop_p)
										{
											*full_stop_p = '\0';
										}
									else
										{
											loop_flag = false;
										}

									child_p = json_object_get (parent_p, start_p);

									if (child_p)
										{
											parent_p = child_p;
										}
									else
										{
											success_flag = false;
										}

									if (full_stop_p)
										{
											start_p = full_stop_p + 1;
											loop_flag = (*start_p != '\0');
										}
								}

							if (success_flag)
								{
									res_p = json_array ();

									if (res_p)
										{
											if (json_is_array (parent_p))
												{
													size_t i;
													json_t *doc_p;

													json_array_foreach (parent_p, i, doc_p)
														{
															json_t *title_p = json_object_get (doc_p, data_p -> wssjd_title_selector_s);

															if (title_p)
																{
																	if (json_is_array (title_p))
																		{
																			const size_t size = json_array_size (title_p);

																			if (size > 1)
																				{

																				}
																			else
																				{
																					json_t *item_p = json_array_get (title_p, 0);
																					const char *title_s = NULL;
																					const char *path_s = NULL;

																					if (json_is_string (item_p))
																						{
																							title_s = json_string_value (item_p);
																						}


																					if (data_p -> wssjd_link_selector_s)
																						{

																						}

																					if (title_s)
																						{
																							json_t *link_p = GetResourceAsJSONByParts (PROTOCOL_HTTP_S, path_s, title_s, NULL);

																							if (link_p)
																								{
																									json_array_append_new (res_p, link_p);
																								}
																						}
																				}
																		}
																	else
																		{

																		}
																}		/* if (title_p) */

														}		/* json_array_foreach (parent_p, i, doc_p) */

												}		/* if (json_is_array (parent_p)) */

										}		/* if (res_p) */

								}

							WipeJSON (parent_p);
						}


					FreeCopiedString (selector_s);
				}

		}

	return res_p;
}

	

static  ParameterSet *IsResourceForWebSearchJSONService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}

