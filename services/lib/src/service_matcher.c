#include <string.h>

#include "service_matcher.h"


void InitServiceMatcher (ServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p))
{
	matcher_p -> sm_match_fn = match_fn;	
}


void InitResourceServiceMatcher (ResourceServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p), Resource *resource_p, Handler *handler_p)
{
	InitServiceMatcher (& (matcher_p -> rsm_base_matcher), match_fn);
	
	matcher_p -> rsm_resource_p = resource_p;
	matcher_p -> rsm_handler_p = handler_p;	
}


void InitNameServiceMatcher (NameServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p), const char *name_s)
{
	InitServiceMatcher (& (matcher_p -> nsm_base_matcher), match_fn);

	matcher_p -> nsm_service_name_s = name_s;
}


void InitPluginNameServiceMatcher (PluginNameServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p), const char *name_s)
{
	InitServiceMatcher (& (matcher_p -> pnsm_base_matcher), match_fn);

	matcher_p -> pnsm_plugin_name_s = name_s;
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
