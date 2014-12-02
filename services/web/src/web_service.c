#include <string.h>

#include <arpa/inet.h>

#include "web_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"


typedef struct WebServiceData
{
	ServiceData wsd_base_data;
	json_t *wsd_config_p;
	const char *wsd_name_s;
	const char *wsd_description_s;
	ParameterSet *wsd_params_p;
} WebServiceData;

/*
 * STATIC PROTOTYPES
 */

static const char *GetWebServiceName (void);

static const char *GetWebServiceDesciption (void);

static ParameterSet *GetWebServiceParameters (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p);


static int RunWebService (ServiceData *service_data_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForWebService (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p);


static int Web (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p);



static WebServiceData *AllocateWebServiceData (json_t *config_p);


static void FreeWebServiceData (WebServiceData *data_p);


/*
 * API FUNCTIONS
 */

Service *GetService (json_t *config_p)
{
	Service *web_service_p = (Service *) AllocMemory (sizeof (Service));
	
	if (web_service_p)
		{
			ServiceData *data_p = AllocateWebServiceData (config_p);

			if (data_p)
				{
					InitialiseService (compress_service_p,
						GetWebServiceName,
						GetWebServiceDesciption,
						RunWebService,
						IsResourceForWebService,
						GetWebServiceParameters,
						data_p);

					
					return web_service_p;
				}
			
			FreeMemory (web_service_p);
		}
	
	return NULL;
}


void ReleaseService (Service *service_p)
{
	FreeWebServiceData ((WebServiceData *) (service_p -> se_data_p));
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */


static WebServiceData *AllocateWebServiceData (json_t *config_p)
{
	WebServiceData *data_p = (WebServiceData *) AllocMemory (sizeof (WebServiceData));
	
	if (data_p)
		{
			data_p -> wsd_config_p = config_p;
			data_p -> wsd_name_s = GetServiceNameFromJSON (config_p);
			
			if (data_p -> wsd_name_s)
				{
					data_p -> wsd_description_s = GetServiceDescriptionFromJSON (config_p);
					
					if (data_p -> wsd_description_s)
						{
							data_p -> wsd_params_p = CreateParameterSetFromJSON (config_p);
							
							if (data_p -> wsd_params_p)
								{
									return data_p;
								}
						}
					
				}
				
			FreeMemory (data_p);
		}
		
	return NULL;
}


static void FreeWebServiceData (WebServiceData *data_p)
{
	FreeParameterSet (data_p -> wsd_params_p);

	json_decref (data_p -> wsd_config_p);
	
	FreeMemory (data_p);
}


static const char *GetWebServiceName (ServiceData *service_data_p)
{
	WebServiceData *data_p = (WebServiceData *) service_data_p;
	
	return (data_p -> wsd_name_s);
}


static const char *GetWebServiceDesciption (ServiceData *service_data_p)
{
	WebServiceData *data_p = (WebServiceData *) service_data_p;

	return (data_p -> wsd_description_s);
}


static ParameterSet *GetWebServiceParameters (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("web service parameters", "The parameters used for the web service");

	if (param_set_p)
		{
			SharedType def;
				
			def.st_string_value_s = resource_p;		

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, S_INPUT_PARAM_NAME_S, "The input file to read", TAG_INPUT_FILE, NULL, def, NULL, NULL, PL_BASIC, NULL))
				{
					return param_set_p;
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}




static int RunWebService (ServiceData *service_data_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	int result = -1;
	SharedType input_resource;
	
	if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &input_resource, true))
		{
			Resource *input_resource_p = input_resource.st_resource_value_p;
			SharedType value;
			
			if (GetParameterValueFromParameterSet (param_set_p, TAG_COMPRESS_ALGORITHM, &value, true))
				{
					const char * const algorithm_s = value.st_string_value_s;
					
					if (algorithm_s)
						{
							result = Web (input_resource_p, algorithm_s, credentials_p);
								
						}		/* if (algorithm_s) */
										
				}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */			
								
		}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_INPUT_FILE, &value)) */
	

	return result;
}



static bool IsResourceForWebService (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = true;
	const char *filename_s = resource_p -> re_value_s;

	if (resource_p -> re_re_protocol_s)
		{
			interested_flag = (strcmp (resource_p -> re_re_protocol_s, "string") == 0);
		}

	return interested_flag;
}

