/*
 * wheatis_config.h
 *
 *  Created on: 3 May 2015
 *      Author: billy
 */

#ifndef WHEATIS_CONFIG_H_
#define WHEATIS_CONFIG_H_

#include "wheatis_service_manager_library.h"
#include "typedefs.h"
#include "jansson.h"

#ifdef __cplusplus
	extern "C" {
#endif


WHEATIS_SERVICE_MANAGER_LOCAL bool InitConfig (voids);


WHEATIS_SERVICE_MANAGER_LOCAL bool DestroyConfig (void);


WHEATIS_SERVICE_MANAGER_API const json_t *GetGlobalServiceConfig (const char * const service_name_s);


WHEATIS_SERVICE_MANAGER_API void ConnectToExternalServers (void);


WHEATIS_SERVICE_MANAGER_API const json_t *GetGlobalConfigValue (const char *key_s);


#ifdef __cplusplus
}
#endif



#endif /* WHEATIS_CONFIG_H_ */
