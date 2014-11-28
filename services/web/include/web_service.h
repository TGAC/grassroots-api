#ifndef WEB_SERVICE_H
#define WEB_SERVICE_H

#include "service.h"
#include "library.h"
#include "web_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


WEB_SERVICE_API Service *GetService (void);


WEB_SERVICE_API void ReleaseService (Service *service_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef WEB_SERVICE_H */
