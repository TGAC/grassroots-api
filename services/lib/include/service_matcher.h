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


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_SERVICE_API void InitServiceMatcher (ServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p));

WHEATIS_SERVICE_API void InitResourceServiceMatcher (ResourceServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p), Resource *resource_p, Handler *handler_p);

WHEATIS_SERVICE_API void InitNameServiceMatcher (NameServiceMatcher *matcher_p, bool (*match_fn) (ServiceMatcher *matcher_p, Service *service_p), const char *name_s);

WHEATIS_SERVICE_API bool RunServiceMatcher (ServiceMatcher *matcher_p, Service *service_p);

WHEATIS_SERVICE_API bool MatchServiceByName (ServiceMatcher *matcher_p, Service *service_p);

WHEATIS_SERVICE_API bool MatchServiceByResource (ServiceMatcher *matcher_p, Service *service_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SERVICE_MATCHER_H */
