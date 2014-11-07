#ifndef SERVICE_CONFIG_H
#define SERVICE_CONFIG_H

#include "wheatis_service_library.h"
#include "typedefs.h"


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_SERVICE_API void FreeServerResources (void);


WHEATIS_SERVICE_API bool SetServerRootDirectory (const char * const path_s);


WHEATIS_SERVICE_API const char *GetServerRootDirectory (void);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SERVICE_CONFIG_H */

