/*
 * grassroots_config.h
 *
 *  Created on: 3 May 2015
 *      Author: billy
 */

#ifndef GRASSROOTS_CONFIG_H_
#define GRASSROOTS_CONFIG_H_

#include "grassroots_service_manager_library.h"
#include "typedefs.h"
#include "jansson.h"

#ifdef __cplusplus
	extern "C" {
#endif


GRASSROOTS_SERVICE_MANAGER_LOCAL bool InitConfig (void);


GRASSROOTS_SERVICE_MANAGER_LOCAL bool DestroyConfig (void);


GRASSROOTS_SERVICE_MANAGER_API const json_t *GetGlobalServiceConfig (const char * const service_name_s);


GRASSROOTS_SERVICE_MANAGER_API void ConnectToExternalServers (void);


GRASSROOTS_SERVICE_MANAGER_API const json_t *GetGlobalConfigValue (const char *key_s);


GRASSROOTS_SERVICE_MANAGER_API const char *GetProviderName (void);


GRASSROOTS_SERVICE_MANAGER_API const char *GetProviderDescription (void);


GRASSROOTS_SERVICE_MANAGER_API const char *GetProviderURI (void);


GRASSROOTS_SERVICE_MANAGER_API const json_t *GetProviderAsJSON (void);


GRASSROOTS_SERVICE_MANAGER_API  bool IsServiceEnabled (const char *service_name_s);


#ifdef __cplusplus
}
#endif



#endif /* GRASSROOTS_CONFIG_H_ */
