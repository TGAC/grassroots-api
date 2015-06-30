#ifndef MONGODB_SERVICE_H
#define MONGODB_SERVICE_H

#include "service.h"
#include "library.h"
#include "mongodb_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


MONGODB_SERVICE_API ServicesArray *GetServices (json_t *config_p);


MONGODB_SERVICE_API void ReleaseServices (ServicesArray *services_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef MONGODB_SERVICE_H */
