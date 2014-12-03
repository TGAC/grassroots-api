#include <string.h>

#include <arpa/inet.h>

#include "web_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "byte_buffer.h"


typedef enum SubmissionMethod
{
	SM_POST,
	SM_GET,
	SM_BODY,
	SM_NUM_METHODS
} SubmissionMethod;


typedef struct WebServiceData
{
	ServiceData wsd_base_data;
	json_t *wsd_config_p;
	const char *wsd_name_s;
	const char *wsd_description_s;
	ParameterSet *wsd_params_p;
	ByteBuffer *wsd_buffer_p;
	SubmissionMethod wsd_method;	
} WebServiceData;

/*
 * STATIC PROTOTYPES
 */

static const char *GetWebServiceName (ServiceData *service_data_p);

static const char *GetWebServiceDesciption (ServiceData *service_data_p);

static ParameterSet *GetWebServiceParameters (ServiceData *service_data_p, Resource *resource_p, const json_t *json_p);


static int RunWebService (ServiceData *service_data_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForWebService (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p);


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
			ServiceData *data_p = (ServiceData *) AllocateWebServiceData (config_p);

			if (data_p)
				{
					InitialiseService (web_service_p,
						GetWebServiceName,
						GetWebServiceDesciption,
						RunWebService,
						IsResourceForWebService,
						GetWebServiceParameters,
						false,
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
							data_p -> wsd_buffer_p = AllocateByteBuffer (1024);
							
							if (data_p -> wsd_buffer_p)
								{							
									data_p -> wsd_params_p = CreateParameterSetFromJSON (config_p);
									
									if (data_p -> wsd_params_p)
										{
											return data_p;
										}

									FreeByteBuffer (data_p -> wsd_buffer_p);
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
	FreeByteBuffer (data_p -> wsd_buffer_p);

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
			/*
			SharedType def;
				
			def.st_string_value_s = resource_p;		

			if (CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, S_INPUT_PARAM_NAME_S, "The input file to read", TAG_INPUT_FILE, NULL, def, NULL, NULL, PL_BASIC, NULL))
				{
					return param_set_p;
				}
			*/
			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}




static char *GetParameterValueAsString (const Parameter *param_p)
{
	char *value_s = NULL;
	const SharedType * const value_p = & (param_p -> pa_current_value);
	
	switch (param_type)
		{
			case PT_BOOLEAN:
				if (value_p -> st_boolean_value == true)
					{
						
					}
				else
					{
						
					}
				break;

			case PT_SIGNED_INT:
				if (json_is_integer (json_value_p))
					{
						value_p -> st_long_value = (int32) json_integer_value (json_value_p);
						success_flag = true;
					}
				break;

			case PT_UNSIGNED_INT:
				if (json_is_integer (json_value_p))
					{
						value_p -> st_ulong_value = (uint32) json_integer_value (json_value_p);
						success_flag = true;
					}
				break;

			case PT_SIGNED_REAL:
			case PT_UNSIGNED_REAL:
				if (json_is_real (json_value_p))
					{
						value_p -> st_data_value = (double64) json_real_value (json_value_p);
						success_flag = true;
					}
				break;

			
			case PT_DIRECTORY:
			case PT_FILE_TO_READ:
			case PT_FILE_TO_WRITE:
				{
					json_t *protocol_p = json_object_get (json_value_p, RESOURCE_PROTOCOL_S);
					
					if ((protocol_p) && (json_is_string (protocol_p)))
						{
							json_value_p = json_object_get (json_value_p, RESOURCE_VALUE_S);
							
							if (json_value_p && (json_is_string (json_value_p)))
								{
									const char *protocol_s = json_string_value (protocol_p);
									const char *value_s = json_string_value (json_value_p);

									value_p -> st_resource_value_p = AllocateResource (protocol_s, value_s);
									
									success_flag = (value_p -> st_resource_value_p != NULL);										
								}					
						}					
				}
				break;
			
			case PT_STRING:
			case PT_PASSWORD:
				if (json_is_string (json_value_p))
					{
						char *value_s = CopyToNewString (json_string_value (json_value_p), 0, false);

						if (value_s)
							{
								if (value_p -> st_string_value_s)
									{
										FreeCopiedString (value_p -> st_string_value_s);
									}

								value_p -> st_string_value_s = value_s;
								success_flag = true;
							}
					}
				break;

			default:
				break;
		}		/* switch (param_p -> pa_type) */

	return value_s;
}


static bool AddParameterToWebService (WebServiceData *service_data_p, Parameter *param_p)
{
	bool success_flag = true;
	
	if (AppendToByteBuffer (param_p -> pa_name_s))
		{
			if (AppendToByteBuffer ("=")
				{
					switch (param_p -> pa_type)
						{
							
							
						}
						
					if (AppendToByteBuffer (param_p -> pa_current_value))
						{
							
						}					
				}
		}
	
	switch (service_data_p -> wsd_method)
		{
			case SM_POST:
			
				break;
				
			case SM_GET:
			
				break;
				
			case SM_BODY:
			
				break;
		
			default:
				success_flag = false;
				break;			
		}
	
	return success_flag;
}


static int RunWebService (ServiceData *service_data_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebServiceData *data_p = (WebServiceData *) service_data_p;
	int result = -1;
	
	if (param_set_p)
		{
			ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
			
			ResetByteBuffer (data_p -> wsd_buffer_p);
			
			while (node_p)
				{
					AddParameterToWebService (data_p, node_p -> pn_parameter_p);
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
			
		}		/* if (param_set_p) */


	return result;
}



static bool IsResourceForWebService (ServiceData *service_data_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;

	if (resource_p -> re_protocol_s)
		{
			interested_flag = (strcmp (resource_p -> re_protocol_s, "string") == 0);
		}

	return interested_flag;
}

