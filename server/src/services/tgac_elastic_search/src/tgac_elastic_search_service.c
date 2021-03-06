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

#include "tgac_elastic_search_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "curl_tools.h"
#include "service_job.h"


/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData essd_base_data;
	CurlTool *essd_curl_tool_p;
} ElasticSearchServiceData;



typedef enum
{
	SF_STUDY_ACCESSION,
	SF_SCIENTIFIC_NAME,
	SF_CENTRE_NAME,
	SF_EXPERIMENT_TITLE,
	SF_STUDY_TITLE,
	SF_NUM_FIELDS
} SearchField;


static const char *s_field_names_pp [SF_NUM_FIELDS] =
{
	"study_accession",
	"scientific_name",
	"center_name",
	"experiment_title",
	"study_title"
};


static NamedParameterType TES_SEARCH_FIELD = { "Search field", PT_STRING };
static NamedParameterType TES_SEARCH_TERM = { "Search term", PT_STRING };


/*
 * STATIC PROTOTYPES
 */

static const char *GetElasticSearchRestServiceName (Service *service_p);

static const char *GetElasticSearchRestServiceDesciption (Service *service_p);

static const char *GetElasticSearchRestServiceURI (Service *service_p);

static ParameterSet *GetElasticSearchRestServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static void ReleaseElasticSearchRestServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunElasticSearchRestService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static ParameterSet *IsFileForElasticSearchRestService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseElasticSearchRestService (Service *service_p);




static json_t *GetElasticSearchRestServiceResults (struct Service *service_p, const uuid_t joob_id);

static ElasticSearchServiceData *AllocateElasticSearchServiceData (void);

static void FreeElasticSearchServiceData (ElasticSearchServiceData *data_p);


static ElasticSearchServiceData *AllocateElasticSearchServiceData (void)
{
	ElasticSearchServiceData *data_p = (ElasticSearchServiceData *) AllocMemory (sizeof (ElasticSearchServiceData));

	if (data_p)
		{
			data_p -> essd_curl_tool_p = AllocateCurlTool (CM_MEMORY);

			if (data_p -> essd_curl_tool_p)
				{
					return data_p;
				}

			FreeMemory (data_p);
		}

	return NULL;
}


static void FreeElasticSearchServiceData (ElasticSearchServiceData *data_p)
{
	FreeCurlTool (data_p -> essd_curl_tool_p);
	FreeMemory (data_p);
}


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
					ServiceData *data_p = (ServiceData *) AllocateElasticSearchServiceData ();
					
					if (data_p)
						{
							InitialiseService (service_p,
								GetElasticSearchRestServiceName,
								GetElasticSearchRestServiceDesciption,
								GetElasticSearchRestServiceURI,
								RunElasticSearchRestService,
								IsFileForElasticSearchRestService,
								GetElasticSearchRestServiceParameters,
								ReleaseElasticSearchRestServiceParameters,
								CloseElasticSearchRestService,
								NULL,
								true,
								true,
								data_p);
							
							* (services_p -> sa_services_pp) = service_p;

							return services_p;
						}

					FreeServicesArray (services_p);
				}
				
			FreeMemory (service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}



static const char *GetRootRestURI (void)
{
	return "http://v0214.nbi.ac.uk:8080/wis-web/wis-web/rest/searchelasticsearch/";
}


static bool CloseElasticSearchRestService (Service *service_p)
{
	bool success_flag = true;
	ElasticSearchServiceData *data_p = (ElasticSearchServiceData *) (service_p -> se_data_p);

	FreeElasticSearchServiceData (data_p);
	
	return success_flag;
}

/*
 * STATIC FUNCTIONS
 */


static const char *GetElasticSearchRestServiceName (Service * UNUSED_PARAM (service_p))
{
	return "TGAC Elastic Search service";
}


static const char *GetElasticSearchRestServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service to access the TGAC Elastic Search data";
}


static const char *GetElasticSearchRestServiceURI (Service * UNUSED_PARAM (service_p))
{
	return "http://v0214.nbi.ac.uk:8080/wis-web/";
}


static ParameterSet *GetElasticSearchRestServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Elastic Search service parameters", "The parameters used for the Elastic Search service");

	if (param_set_p)
		{
			Parameter *param_p;
			bool success_flag = true;
			SharedType def;
			uint32 i;
			const char *descriptions_pp [SF_NUM_FIELDS] =
				{
					"Study Accession",
					"Scientific Name",
					"Centre Name",
					"Experiment Title",
					"Study Title"
				};


			def.st_string_value_s = (char *) (*s_field_names_pp);

			if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, TES_SEARCH_FIELD.npt_type, TES_SEARCH_FIELD.npt_name_s, NULL, "The field to search", def, PL_ALL)) != NULL)
				{
					for (i = 0; i < SF_NUM_FIELDS; ++ i)
						{
							def.st_string_value_s = (char *) (* (s_field_names_pp + i));

							if (!CreateAndAddParameterOptionToParameter (param_p, def, * (descriptions_pp + i)))
								{
									i = SF_NUM_FIELDS;
									success_flag = false;
								}
						}

					if (success_flag)
						{
							def.st_string_value_s = "";

							if (EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, TES_SEARCH_TERM.npt_type, TES_SEARCH_TERM.npt_name_s, NULL, "The term to search for in the given field", def, PL_ALL))
								{
									return param_set_p;
								}
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static void ReleaseElasticSearchRestServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunElasticSearchRestService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	ElasticSearchServiceData *data_p = (ElasticSearchServiceData *) (service_p -> se_data_p);
	/* We only have one task */
	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "TGAC Elastic Search");

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);

			job_p -> sj_status = OS_FAILED_TO_START;

					SharedType field;

					if (GetParameterValueFromParameterSet (param_set_p, TES_SEARCH_FIELD.npt_name_s, &field, true))
						{
							SharedType keyword;

							if (GetParameterValueFromParameterSet (param_set_p, TES_SEARCH_TERM.npt_name_s, &keyword, true))
								{
									/* Is keyword valid? */
									if (!IsStringEmpty (keyword.st_string_value_s))
										{
											ByteBuffer *buffer_p = AllocateByteBuffer (1024);

											if (buffer_p)
												{
													/* build the uri to call */
													if (AppendStringsToByteBuffer (buffer_p, GetRootRestURI (), field.st_string_value_s, "/", keyword.st_string_value_s, NULL))
														{
															const char * const uri_s = GetByteBufferData (buffer_p);

															if (SetUriForCurlTool (data_p -> essd_curl_tool_p, uri_s))
																{
																	CURLcode curl_res = RunCurlTool (data_p -> essd_curl_tool_p);

																	job_p -> sj_status = (curl_res == CURLE_OK) ? OS_SUCCEEDED : OS_FAILED;
																}		/* if (SetUriForCurlTool (curl_tool_p, uri_s)) */

														}		/* if (AppendStringsToByteBuffer (buffer_p, field.st_string_value_s, "/", keyword.st_string_value_s, NULL)) */

													FreeByteBuffer (buffer_p);
												}		/* if (buffer_p) */

										}		/* if (!IsStringEmpty (keyword.st_string_value_s)) */

								}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_SEARCH_KEYWORD, &keyword, true)) */

						}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_SEARCH_FIELD, &field, true)) */

		}		/* if (service_p -> se_jobs_p) */
	
	return service_p -> se_jobs_p;
}



static ParameterSet *IsFileForElasticSearchRestService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}


static json_t *GetElasticSearchRestServiceResults (struct Service * UNUSED_PARAM (service_p), const uuid_t  UNUSED_PARAM (job_id))
{
	return NULL;
}

