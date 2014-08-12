#ifndef PLATFORM_H
#define PLATFORM_H

#include "typedefs.h"
#include "runner_api.h"

#ifdef __cplusplus
	extern "C" {
#endif

RUNNER_API BOOLEAN InitPlatform (void);
RUNNER_API void ExitPlatform (void);

#ifdef __cplusplus
}
#endif



#endif



