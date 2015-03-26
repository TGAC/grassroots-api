#ifndef TGAC_ELASTIC_SEARCH_REST_SERVICE_H
#define TGAC_ELASTIC_SEARCH_REST_SERVICE_H

#include "service.h"
#include "library.h"
#include "tgac_elastic_search_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


TGAC_ELASTIC_SEARCH_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


TGAC_ELASTIC_SEARCH_SERVICE_API void ReleaseServices (ServicesArray *service_p);


TGAC_ELASTIC_SEARCH_SERVICE_API const char * const GetRootRestURI (void);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef TGAC_ELASTIC_SEARCH_REST_SERVICE_H */
