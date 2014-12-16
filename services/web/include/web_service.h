#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include "service.h"
#include "library.h"
#include "web_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


WEB_SERVICE_API ServicesArray *GetServices (json_t *config_p);


WEB_SERVICE_API void ReleaseServices (ServicesArray *services_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef WEB_SERVICE_H */
