/*
 * drmaa_util.h
 *
 *  Created on: 28 Apr 2015
 *      Author: tyrrells
 */

#ifndef DRMAA_UTIL_H_
#define DRMAA_UTIL_H_


#include "grassroots_service_manager_library.h"
#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif

GRASSROOTS_SERVICE_MANAGER_API bool InitDrmaa (void);

GRASSROOTS_SERVICE_MANAGER_API bool ExitDrmaa (void);

#ifdef __cplusplus
}
#endif


#endif /* DRMAA_UTIL_H_ */
