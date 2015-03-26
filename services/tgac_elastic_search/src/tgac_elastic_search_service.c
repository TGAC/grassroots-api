#include <string.h>

#include "tgac_elastic_search_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"


#define TAG_SEARCH_FIELD MAKE_TAG ('E', 'S', 'F', 'D')
#define TAG_SEARCH_KEYWORD MAKE_TAG ('E', 'S', 'K', 'Y')

/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData essd_base_data;
} ElasticSearchServiceData;



typedef enum
{
	SF_CENTRE_NAME,
	SF_EXPERIMENT_TITLE,
	SF_STUDY_TITLE,
	SF_NUM_FIELDS
} SearchField;


static const char *s_field_names_pp [SF_NUM_FIELDS] =
{
	"Centre Name",
	"Experiment Title",
	"Study Title"
};

/*
 * STATIC PROTOTYPES
 */

static const char *GetElasticSearchRestServiceName (Service *service_p);

static const char *GetElasticSearchRestServiceDesciption (Service *service_p);

static const char *GetElasticSearchRestServiceURI (Service *service_p);

static ParameterSet *GetElasticSearchRestServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseElasticSearchRestServiceParameters (Service *service_p, ParameterSet *params_p);

static json_t *RunElasticSearchRestService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForElasticSearchRestService (Service *service_p, Resource *resource_p, Handler *handler_p);



static bool CloseElasticSearchRestService (Service *service_p);



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
					ServiceData *data_p = NULL;
					
					InitialiseService (compress_service_p,
						GetElasticSearchRestServiceName,
						GetElasticSearchRestServiceDesciption,
						GetElasticSearchRestServiceURI,
						RunElasticSearchRestService,
						IsFileForElasticSearchRestService,
						GetElasticSearchRestServiceParameters,
						ReleaseElasticSearchRestServiceParameters,
						CloseElasticSearchRestService,
						true,
						data_p);
					
					* (services_p -> sa_services_pp) = compress_service_p;
							
					return services_p;
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
	return "http://v0214.nbi.ac.uk:8080/wis-web/wis-web/rest/searchelasticsearch/";
}


static bool CloseElasticSearchRestService (Service *service_p)
{
	bool success_flag = true;
	
	return success_flag;
}

/*
 * STATIC FUNCTIONS
 */


static const char *GetElasticSearchRestServiceName (Service *service_p)
{
	return "TGAC Elastic Search service";
}


static const char *GetElasticSearchRestServiceDesciption (Service *service_p)
{
	return "A service to access the TGAC Elastic Search data";
}


static const char *GetElasticSearchRestServiceURI (Service *service_p)
{
	return "http://v0214.nbi.ac.uk:8080/wis-web/";
}


static ParameterSet *GetElasticSearchRestServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Elastic Search service parameters", "The parameters used for the Elastic Search service");

	if (param_set_p)
		{
			SharedType def;
			ParameterMultiOptionArray *options_p = NULL;
			SharedType values [SF_NUM_FIELDS];
			uint32 i;
			const char **descriptions_pp = NULL;

			for (i = 0; i < SF_NUM_FIELDS; ++ i)
				{
					values [i].st_string_value_s = (char *) (* (s_field_names_pp + i));
				}

			options_p = AllocateParameterMultiOptionArray (SF_NUM_FIELDS, descriptions_pp, values, PT_STRING);

			if (options_p)
				{
					def.st_string_value_s = values [0].st_string_value_s;

					if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Search field", NULL, "The field to search", TAG_SEARCH_FIELD, options_p, def, NULL, NULL, PL_ALL, NULL))
						{
							if (CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, "Search term", NULL, "The term to search for in the given field", TAG_SEARCH_KEYWORD, NULL, def, NULL, NULL, PL_ALL, NULL))
								{
									return param_set_p;
								}
						}

					FreeParameterMultiOptionArray (options_p);
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static void ReleaseElasticSearchRestServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static json_t *RunElasticSearchRestService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	OperationStatus res = OS_FAILED_TO_START;
	json_t *res_json_p = NULL;
	json_t *results_json_p = NULL;
	CurlTool *curl_tool_p = AllocateCurlTool ();

	if (curl_tool_p)
		{
			SharedType field;

			if (GetParameterValueFromParameterSet (param_set_p, TAG_SEARCH_FIELD, &field, true))
				{
					SharedType keyword;

					if (GetParameterValueFromParameterSet (param_set_p, TAG_SEARCH_KEYWORD, &keyword, true))
						{
							results_json_p = json_array ();

							if (results_json_p)
								{
									if (RunSearch (param_set_p, results_json_p, curl_tool_p))
										{
											res = OS_SUCCEEDED;
										}
								}

						}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_SEARCH_KEYWORD, &keyword, true)) */

				}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_SEARCH_FIELD, &field, true)) */

			FreeCurlTool (curl_tool_p);
		}
	

	res_json_p = CreateServiceResponseAsJSON (service_p, res, results_json_p);

	return res_json_p;
}



static bool IsFileForElasticSearchRestService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;

	return interested_flag;
}

