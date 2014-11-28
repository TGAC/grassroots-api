#include <string.h>

#include <arpa/inet.h>

#include "web_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"


/*
 * STATIC PROTOTYPES
 */

static const char *GetWebServiceName (void);

static const char *GetWebServiceDesciption (void);

static ParameterSet *GetWebServiceParameters (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p);


static int RunWebService (ServiceData *service_data_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForWebService (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p);


static int Web (Resource *input_resource_p, const char * const algorithm_s, json_t *credentials_p);


/*
 * API FUNCTIONS
 */

Service *GetService (void)
{
	Service *web_service_p = (Service *) AllocMemory (sizeof (Service));
	ServiceData *data_p = NULL;

	InitialiseService (compress_service_p,
		GetWebServiceName,
		GetWebServiceDesciption,
		RunWebService,
		IsResourceForWebService,
		GetWebServiceParameters,
		data_p);

	return web_service_p;
}


void ReleaseService (Service *service_p)
{
	FreeMemory (service_p);
}


/*
 * STATIC FUNCTIONS
 */


static const char *GetWebServiceName (void)
{
	return "CerealsDB Web service";
}


static const char *GetWebServiceDesciption (void)
{
	return "A service to call the web service";
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

