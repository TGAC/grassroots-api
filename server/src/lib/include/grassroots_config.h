/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
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


GRASSROOTS_SERVICE_MANAGER_API json_t *GetGlobalServiceConfig (const char * const service_name_s, bool *alloc_flag_p);


GRASSROOTS_SERVICE_MANAGER_API void ConnectToExternalServers (void);


GRASSROOTS_SERVICE_MANAGER_API void DisconnectFromExternalServers (void);


GRASSROOTS_SERVICE_MANAGER_API json_t *GetGlobalConfigValue (const char *key_s);


GRASSROOTS_SERVICE_MANAGER_API const char *GetServerProviderName (void);


GRASSROOTS_SERVICE_MANAGER_API const char *GetServerProviderDescription (void);


GRASSROOTS_SERVICE_MANAGER_API const char *GetServerProviderURI (void);


GRASSROOTS_SERVICE_MANAGER_API const json_t *GetProviderAsJSON (void);


GRASSROOTS_SERVICE_MANAGER_API  bool IsServiceEnabled (const char *service_name_s);


#ifdef __cplusplus
}
#endif



#endif /* GRASSROOTS_CONFIG_H_ */