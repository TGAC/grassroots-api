#include <string.h>

#include "service_matcher.h"
#include "memory_allocations.h"




ServiceMatcher *AllocateServiceMatcher (void)
{
	ServiceMatcher *matcher_p = (ServiceMatcher *) AllocMemory (sizeof (ServiceMatcher));
	
	if (matcher_p)
		{
			InitServiceMatcher (matcher_p, NULL);
		}
		
	return matcher_p;
}


ServiceMatcher *AllocateResourceServiceMatcher (Resource *resource_p, Handler *handler_p)
{
	ResourceServiceMatcher *matcher_p = (ResourceServiceMatcher *) AllocMemory (sizeof (ResourceServiceMatcher));
	
	if (matcher_p)
		{
			InitResourceServiceMatcher (matcher_p, resource_p, handler_p);
		}
		
	return (ServiceMatcher *) matcher_p;
}



ServiceMatcher *AllocateOperationNameServiceMatcher (const char *name_s)
{
	NameServiceMatcher *matcher_p = (NameServiceMatcher *) AllocMemory (sizeof (NameServiceMatcher));
	
	if (matcher_p)
		{
			InitOperationNameServiceMatcher (matcher_p, name_s);
		}
		
	return (ServiceMatcher *) matcher_p;
}



ServiceMatcher *AllocatePluginNameServiceMatcher (const char *name_s)
{
	PluginNameServiceMatcher *matcher_p = (PluginNameServiceMatcher *) AllocMemory (sizeof (PluginNameServiceMatcher));
	
	if (matcher_p)
		{
			InitPluginNameServiceMatcher (matcher_p, name_s);
		}
		
	return (ServiceMatcher *) matcher_p;
}


ServiceMatcher *AllocatePluginOperationNameServiceMatcher (const char *plugin_name_s, const char *operation_name_s)
{
	PluginOperationNameServiceMatcher *matcher_p = (PluginOperationNameServiceMatcher *) AllocMemory (sizeof (PluginOperationNameServiceMatcher));
	
	if (matcher_p)
		{
			InitPluginOperationNameServiceMatcher (matcher_p, plugin_name_s, operation_name_s);
		}
		
	return (ServiceMatcher *) matcher_p;
}



ServiceMatcher *AllocateKeywordServiceMatcher (void)
{
	KeywordServiceMatcher *matcher_p = (KeywordServiceMatcher *) AllocMemory (sizeof (KeywordServiceMatcher));

	if (matcher_p)
		{
			InitServiceMatcher (& (matcher_p -> ksm_base_matcher), MatchServiceByKeyword);
			matcher_p -> ksm_base_matcher.sm_destroy_fn = FreeKeywordServiceMatcher;
		}

	return (ServiceMatcher *) matcher_p;
}


void InitServiceMatcher (ServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p))
{
	matcher_p -> sm_match_fn = match_fn;	
	matcher_p -> sm_destroy_fn = FreeServiceMatcher;
}


void InitResourceServiceMatcher (ResourceServiceMatcher *matcher_p, Resource *resource_p, Handler *handler_p)
{
	InitServiceMatcher (& (matcher_p -> rsm_base_matcher), MatchServiceByResource);
	matcher_p -> rsm_base_matcher.sm_destroy_fn = FreeResourceServiceMatcher;
	
	matcher_p -> rsm_resource_p = resource_p;
	matcher_p -> rsm_handler_p = handler_p;	
}


void InitOperationNameServiceMatcher (NameServiceMatcher *matcher_p, const char *name_s)
{
	InitServiceMatcher (& (matcher_p -> nsm_base_matcher), MatchServiceByName);
	matcher_p -> nsm_base_matcher.sm_destroy_fn = FreeNameServiceMatcher;

	matcher_p -> nsm_service_name_s = name_s;
}


void InitPluginNameServiceMatcher (PluginNameServiceMatcher *matcher_p, const char *name_s)
{
	InitServiceMatcher (& (matcher_p -> pnsm_base_matcher), MatchServiceByPluginName);
	matcher_p -> pnsm_base_matcher.sm_destroy_fn = FreePluginNameServiceMatcher;

	matcher_p -> pnsm_plugin_name_s = name_s;

}


void InitPluginOperationNameServiceMatcher (PluginOperationNameServiceMatcher *matcher_p, const char *plugin_name_s, const char *operation_name_s)
{
	InitPluginNameServiceMatcher (& (matcher_p -> ponsm_base_matcher), plugin_name_s);

	matcher_p -> ponsm_base_matcher.pnsm_base_matcher.sm_match_fn = MatchServiceByPluginAndOperationsName;
	matcher_p -> ponsm_base_matcher.pnsm_base_matcher.sm_destroy_fn = FreePluginOperationNameServiceMatcher;

	matcher_p -> ponsm_operation_name_s = operation_name_s;

}


bool RunServiceMatcher (ServiceMatcher *matcher_p, Service *service_p)
{
	return (matcher_p -> sm_match_fn (matcher_p, service_p));
}


bool MatchServiceByName (ServiceMatcher *matcher_p, Service *service_p)
{
	NameServiceMatcher *name_matcher_p = (NameServiceMatcher *) matcher_p;	
	const char * const service_name_s = GetServiceName (service_p);
	
	bool match_flag = (strcmp (service_name_s, name_matcher_p -> nsm_service_name_s) == 0);
		
	return match_flag;
}


bool MatchServiceByPluginName (ServiceMatcher *matcher_p, Service *service_p)
{
	PluginNameServiceMatcher *name_matcher_p = (PluginNameServiceMatcher *) matcher_p;	
	
	return (strcmp (service_p -> se_plugin_p -> pl_name_s, name_matcher_p -> pnsm_plugin_name_s) == 0);
}


bool MatchServiceByPluginAndOperationsName (ServiceMatcher *matcher_p, Service *service_p)
{
	bool match_flag = false;
	PluginOperationNameServiceMatcher *name_matcher_p = (PluginOperationNameServiceMatcher *) matcher_p;	

	if (MatchServiceByPluginName (matcher_p, service_p))
		{
			const char *service_name_s = GetServiceName (service_p);
			match_flag = (strcmp (service_name_s, name_matcher_p -> ponsm_operation_name_s) == 0);
		}

	return match_flag;
}



bool MatchServiceByResource (ServiceMatcher *matcher_p, Service *service_p)
{
	bool match_flag = true;
	ResourceServiceMatcher *resource_matcher_p = (ResourceServiceMatcher *) matcher_p;
	
	if ((resource_matcher_p -> rsm_resource_p) && (resource_matcher_p -> rsm_handler_p))
		{
			match_flag = IsServiceMatch (service_p, resource_matcher_p -> rsm_resource_p, resource_matcher_p -> rsm_handler_p);
		}
		
	return match_flag;
}


bool MatchServiceByKeyword (ServiceMatcher *matcher_p, Service *service_p)
{
	bool match_flag = false;
	KeywordServiceMatcher *keyword_matcher_p = (KeywordServiceMatcher *) matcher_p;

	ParameterSet *params_p = GetServiceParameters (service_p, NULL, NULL);

	if (params_p)
		{
			ParameterNode *param_node_p = (ParameterNode *) (params_p -> ps_params_p -> ll_head_p);

			while (param_node_p)
				{
					Parameter *param_p = param_node_p -> pn_parameter_p;

					if (param_p -> pa_type == PT_KEYWORD)
						{
							match_flag = true;
							param_node_p = NULL;
						}
					else
						{
							param_node_p = (ParameterNode *) (param_node_p -> pn_node.ln_next_p);
						}

				}		/* while (param_node_p && (match_flag == false)) */

			ReleaseServiceParameters (service_p, params_p);
		}		/* if (params_p) */

	return match_flag;
}


void SetPluginNameForServiceMatcher (PluginNameServiceMatcher *matcher_p, const char *plugin_name_s)
{
	matcher_p -> pnsm_plugin_name_s = plugin_name_s;
}


void FreeServiceMatcher (ServiceMatcher *matcher_p)
{
	matcher_p -> sm_destroy_fn (matcher_p);
	matcher_p = NULL;
}


void FreeResourceServiceMatcher (ServiceMatcher *matcher_p)
{
	ResourceServiceMatcher *resource_matcher_p = (ResourceServiceMatcher *) matcher_p;
	
	FreeMemory (resource_matcher_p);
	matcher_p = NULL;
}


void FreeNameServiceMatcher (ServiceMatcher *matcher_p)
{
	NameServiceMatcher *name_matcher_p = (NameServiceMatcher *) matcher_p;
	
	FreeMemory (name_matcher_p);
	matcher_p = NULL;	
}


void FreePluginNameServiceMatcher (ServiceMatcher *matcher_p)
{
	PluginNameServiceMatcher *name_matcher_p = (PluginNameServiceMatcher *) matcher_p;
	
	FreeMemory (name_matcher_p);
	matcher_p = NULL;	
}


void FreePluginOperationNameServiceMatcher (ServiceMatcher *matcher_p)
{
	PluginOperationNameServiceMatcher *name_matcher_p = (PluginOperationNameServiceMatcher *) matcher_p;
	
	FreeMemory (name_matcher_p);
	matcher_p = NULL;	
}


void FreeKeywordServiceMatcher (ServiceMatcher *matcher_p)
{
	KeywordServiceMatcher *keyword_matcher_p = (KeywordServiceMatcher *) matcher_p;

	FreeMemory (keyword_matcher_p);
	matcher_p = NULL;
}
