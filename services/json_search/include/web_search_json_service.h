#ifndef WEB_SEARH_JSON_SERVICE_H
#define WEB_SEARH_JSON_SERVICE_H

#include "service.h"
#include "library.h"
#include "web_search_json_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


WEB_SEARCH_JSON_SERVICE_API ServicesArray *GetServices (json_t *config_p);


WEB_SEARCH_JSON_SERVICE_API void ReleaseServices (ServicesArray *services_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef WEB_SEARH_JSON_SERVICE_H */
