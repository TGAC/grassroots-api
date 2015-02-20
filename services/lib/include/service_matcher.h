#ifndef SERVICE_MATCHER_H
#define SERVICE_MATCHER_H

#include "wheatis_service_library.h"
#include "resource.h"
#include "handler.h"
#include "service.h"

struct ServiceMatcher;


typedef struct ServiceMatcher
{
	bool (*sm_match_fn) (struct ServiceMatcher *matcher_p, Service *service_p);
	void (*sm_destroy_fn) (struct ServiceMatcher *matcher_p);
} ServiceMatcher;


typedef struct ResourceServiceMatcher
{
	ServiceMatcher rsm_base_matcher;
	Resource *rsm_resource_p;
	Handler *rsm_handler_p;
} ResourceServiceMatcher;


typedef struct NameServiceMatcher
{
	ServiceMatcher nsm_base_matcher;
	const char *nsm_service_name_s;
} NameServiceMatcher;


typedef struct PluginNameServiceMatcher
{
	ServiceMatcher pnsm_base_matcher;
	const char *pnsm_plugin_name_s;
} PluginNameServiceMatcher;


typedef struct PluginOperationNameServiceMatcher
{
	PluginNameServiceMatcher ponsm_base_matcher;
	const char *ponsm_operation_name_s;
} PluginOperationNameServiceMatcher;


typedef struct KeywordServiceMatcher
{
	ServiceMatcher ksm_base_matcher;
} KeywordServiceMatcher;


#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_SERVICE_API ServiceMatcher *AllocateServiceMatcher (void);

WHEATIS_SERVICE_API ServiceMatcher *AllocateResourceServiceMatcher (Resource *resource_p, Handler *handler_p);

WHEATIS_SERVICE_API ServiceMatcher *AllocateOperationNameServiceMatcher (const char *name_s);

WHEATIS_SERVICE_API ServiceMatcher *AllocatePluginNameServiceMatcher (const char *plugin_name_s);

WHEATIS_SERVICE_API ServiceMatcher *AllocatePluginOperationNameServiceMatcher (const char *plugin_name_s, const char *service_name_s);

WHEATIS_SERVICE_API ServiceMatcher *AllocateKeywordServiceMatcher (void);


WHEATIS_SERVICE_API void InitServiceMatcher (ServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p));

WHEATIS_SERVICE_API void InitResourceServiceMatcher (ResourceServiceMatcher *matcher_p, Resource *resource_p, Handler *handler_p);

WHEATIS_SERVICE_API void InitOperationNameServiceMatcher (NameServiceMatcher *matcher_p, const char *name_s);

WHEATIS_SERVICE_API void InitPluginNameServiceMatcher (PluginNameServiceMatcher *matcher_p, const char *plugin_name_s);


WHEATIS_SERVICE_API void InitPluginOperationNameServiceMatcher (PluginOperationNameServiceMatcher *matcher_p, const char *plugin_name_s, const char *operation_name_s);


WHEATIS_SERVICE_API void InitKeywordServiceMatcher (KeywordServiceMatcher *matcher_p);


WHEATIS_SERVICE_API void SetPluginNameForServiceMatcher (PluginNameServiceMatcher *matcher_p, const char *plugin_name_s);



WHEATIS_SERVICE_API bool RunServiceMatcher (ServiceMatcher *matcher_p, Service *service_p);

WHEATIS_SERVICE_API bool MatchServiceByName (ServiceMatcher *matcher_p, Service *service_p);

WHEATIS_SERVICE_API bool MatchServiceByResource (ServiceMatcher *matcher_p, Service *service_p);

WHEATIS_SERVICE_API bool MatchServiceByPluginName (ServiceMatcher *matcher_p, Service *service_p);

WHEATIS_SERVICE_API bool MatchServiceByPluginAndOperationsName (ServiceMatcher *matcher_p, Service *service_p);

WHEATIS_SERVICE_API bool MatchServiceByKeyword (ServiceMatcher *matcher_p, Service *service_p);


WHEATIS_SERVICE_LOCAL void FreeServiceMatcher (ServiceMatcher *matcher_p);

WHEATIS_SERVICE_LOCAL void FreeResourceServiceMatcher (ServiceMatcher *matcher_p);

WHEATIS_SERVICE_LOCAL void FreeNameServiceMatcher (ServiceMatcher *matcher_p);

WHEATIS_SERVICE_LOCAL void FreePluginNameServiceMatcher (ServiceMatcher *matcher_p);

WHEATIS_SERVICE_LOCAL void FreePluginOperationNameServiceMatcher (ServiceMatcher *matcher_p);

WHEATIS_SERVICE_LOCAL void FreeKeywordServiceMatcher (ServiceMatcher *matcher_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SERVICE_MATCHER_H */
