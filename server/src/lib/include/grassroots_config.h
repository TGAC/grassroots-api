/*
** Copyright 2014-2016 The Earlham Institute
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

/**
 * @file
 * @brief
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
#include "schema_version.h"

#ifdef __cplusplus
	extern "C" {
#endif



/**
 * Load the global configuration and set the default
 * SchemaVersion to use.
 *
 * @return <code>true</code> if the configuration was successfully
 * loaded, <code>false</code> otherwise.
 */
GRASSROOTS_SERVICE_MANAGER_LOCAL bool InitConfig (void);


/**
 * Free the global configuration and the default
 * SchemaVersion.
 *
 * @return <code>true</code> if the data was successfully
 * freed, <code>false</code> otherwise.
 */
GRASSROOTS_SERVICE_MANAGER_LOCAL bool DestroyConfig (void);


/**
 * Get a configuration value for a named Service.
 *
 * The system will initially check the "config/<service_name>" folder
 * within the Grassroots directory e.g. "config/BlastN Service" for the
 * Service called "BlastN Service". If this file exists it will be loaded
 * and produce the configuration value. If it does not exist or cannot be
 * loaded then, the system will attempt to use an object specified at
 * "services.<service_name>" in the global configuration file instead.
 *
 * @param service_name_s The name of the Service to get the configuration data for.
 * @param alloc_flag_p If the returned JSON fragment has been newly-allocated then
 * the value that this points to will be set to <code>true</code> to indicate that
 * the returned value will need to have json_decref called upon it to avoid a
 * memory leak.
 * @return The JSON fragment containing the configuration data or <code>
 * NULL</code> if it could not be loaded.
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *GetGlobalServiceConfig (const char * const service_name_s, bool *alloc_flag_p);


/**
 * Connect to any defined separate Grassroots servers.
 */
GRASSROOTS_SERVICE_MANAGER_API void ConnectToExternalServers (void);


/**
 * Disconnect to any defined separate Grassroots servers.
 */
GRASSROOTS_SERVICE_MANAGER_API void DisconnectFromExternalServers (void);


/**
 * Get a configuration value from the global Grassroots configuration file.
 *
 * @param key_s The key to get the associated value for.
 * @return The JSON fragment containing the configuration data or <code>
 * NULL</code> if it could not be found.
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *GetGlobalConfigValue (const char *key_s);


/**
 * Get the Provider name for this Grassroots Server.
 *
 * @return The Provider name.
 */
GRASSROOTS_SERVICE_MANAGER_API const char *GetServerProviderName (void);


/**
 * Get the Provider description for this Grassroots Server.
 *
 * @return The Provider description.
 */
GRASSROOTS_SERVICE_MANAGER_API const char *GetServerProviderDescription (void);


/**
 * Get the Provider URI for this Grassroots Server.
 *
 * @return The Provider URI.
 */
GRASSROOTS_SERVICE_MANAGER_API const char *GetServerProviderURI (void);


/**
 * Get the JSON fragment with all of the details of the Provider
 * for this Grassroots Server.
 *
 * @return A read-only JSON representation of the Provider
 * or <code>NULL</code> upon error.
 */
GRASSROOTS_SERVICE_MANAGER_API const json_t *GetProviderAsJSON (void);


/**
 * Check if a named Service is enabled upon this Grassroots server.
 *
 * @param service_name_s The name of the Service to check.
 * @return <code>true</code> if the named Service is enabled, <code>false</code> otherwise.
 */
GRASSROOTS_SERVICE_MANAGER_API bool IsServiceEnabled (const char *service_name_s);




#ifdef __cplusplus
}
#endif



#endif /* GRASSROOTS_CONFIG_H_ */
