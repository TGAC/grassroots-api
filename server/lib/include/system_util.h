#ifndef SYSTEM_UTIL_H
#define SYSTEM_UTIL_H

#include "grassroots_service_manager_library.h"
#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif

GRASSROOTS_SERVICE_MANAGER_API bool InitInformationSystem (void);

GRASSROOTS_SERVICE_MANAGER_API bool DestroyInformationSystem (void);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SYSTEM_UTIL_H */
