#ifndef SYSTEM_UTIL_H
#define SYSTEM_UTIL_H

#include "wheatis_service_manager_library.h"
#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif

WHEATIS_SERVICE_MANAGER_API bool InitInformationSystem (void);

WHEATIS_SERVICE_MANAGER_API bool DestroyInformationSystem (void);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SYSTEM_UTIL_H */
