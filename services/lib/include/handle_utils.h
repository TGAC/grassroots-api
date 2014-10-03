#ifndef HANDLE_UTILS_H
#define HANDLE_UTILS_H


#include "wheatis_service_library.h"
#include "handle.h"

#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_SERVICE_API Handle *GetResourceHandle (const Resource *resource_p);

#ifdef __cplusplus
}
#endif

#endif		/* #ifndef HANDLE_UTILS_H */
