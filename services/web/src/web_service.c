#include <string.h>

#include <arpa/inet.h>

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

static Service *GetService (json_t *operation_json_p);

static const char *GetWebServiceName (Service *service_p);

static const char *GetWebServiceDesciption (Service *service_p);

static ParameterSet *GetWebServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);


static int RunWebService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsResourceForWebService (Service *service_p, Resource *resource_p, Handler *handler_p);


static WebServiceData *AllocateWebServiceData (json_t *config_p);


static void FreeWebServiceData (WebServiceData *data_p);

static bool CloseWebService (Service *service_p);

/*
 * API FUNCTIONS
 */
 

 
ServicesArray *GetServices (json_t *config_p)
{
	if (config_p)
		{
			/* Make sure that the config refers to our service */
			const char * const plugin_name_s = "web_service";
			json_t *value_p = json_object_get (config_p, PLUGIN_NAME_S);

			if (value_p)
				{
					if (json_is_string (value_p))
						{
							const char *value_s = json_string_value (value_p);
							
							if (strcmp (value_s, plugin_name_s) == 0)
								{
									json_t *ops_p = json_object_get (config_p, SERVER_OPERATIONS_S);
									
									if (ops_p)
										{
											size_t num_ops = json_is_array (ops_p) ? json_array_size (ops_p) : 1;											
											ServicesArray *services_p = AllocateServicesArray (num_ops);
											
											if (services_p)
												{
													size_t i = 0;
													Service **service_pp = services_p -> sa_services_pp;
													
													while (i < num_ops)
														{
															json_t *op_p =  json_array_get (ops_p, i);															
															Service *service_p = GetService (op_p);
															
															if (service_p)
																{
																	*service_pp = service_p;
																}
															else
																{
																	char *dump_s = json_dumps (op_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);
																	
																	if (dump_s)
																		{
																			PrintErrors (STM_LEVEL_SEVERE, "Failed to create servce %lu from:\n%s\n", i, dump_s);
																			free (dump_s);
																		}
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, "Failed to create servce %lu\n", i);																			
																		}
																	
																}
																														
															++ i;
															++ service_pp;
														}
														
														return services_p;
												}
											
										}

								}		/* if (strcmp (value_s, plugin_name_s) == 0) */						
								
						}		/* if (json_is_string (value_p)) */

				}		/* if (value_p) */
			
		}		/* if (config_p) */
						
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




static Service *GetService (json_t *operation_json_p)
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
						RunWebService,
						IsResourceForWebService,
						GetWebServiceParameters,
						CloseWebService,
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
			data_p -> wsd_config_p = op_json_p;
			data_p -> wsd_name_s = GetOperationNameFromJSON (op_json_p);
			
			if (data_p -> wsd_name_s)
				{
					data_p -> wsd_description_s = GetOperationDescriptionFromJSON (op_json_p);
					
					if (data_p -> wsd_description_s)
						{
							data_p -> wsd_buffer_p = AllocateByteBuffer (1024);
							
							if (data_p -> wsd_buffer_p)
								{							
									json_t *params_p = json_object_get (op_json_p, PARAM_SET_PARAMS_S);

									if (params_p)
										{
											data_p -> wsd_params_p = CreateParameterSetFromJSON (op_json_p);
											
											if (data_p -> wsd_params_p)
												{
													return data_p;
												}
																								
										}		/* if (params_p) */
																								
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


static ParameterSet *GetWebServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);

	return (data_p -> wsd_params_p);
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


static bool AddParameterToWebService (Service *service_p, Parameter *param_p)
{
	bool success_flag = false;
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);
	ByteBuffer *buffer_p = data_p -> wsd_buffer_p;
	
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

	return success_flag;
}


static bool CloseWebService (Service *service_p)
{
	bool success_flag = true;
	
	return success_flag;
}


static int RunWebService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	WebServiceData *data_p = (WebServiceData *) (service_p -> se_data_p);
	int result = -1;
	
	if (param_set_p)
		{
			ParameterNode *node_p = (ParameterNode *) (param_set_p -> ps_params_p -> ll_head_p);
			
			ResetByteBuffer (data_p -> wsd_buffer_p);
			
			while (node_p)
				{
					AddParameterToWebService (service_p, node_p -> pn_parameter_p);
					node_p = (ParameterNode *) (node_p -> pn_node.ln_next_p);
				}
			
		}		/* if (param_set_p) */


	return result;
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

