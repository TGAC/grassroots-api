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
#ifndef SERVICE_CONFIG_H
#define SERVICE_CONFIG_H

#include "grassroots_service_library.h"
#include "typedefs.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Free any internal Server resources.
 *
 * If a new type of Server instance has been written,
 * this should be called when one of these Server instances
 * is closed.
 */
GRASSROOTS_SERVICE_API void FreeServerResources (void);


/**
 * Set the path to the where the Grassroots is installed.
 *
 * @param path_s The path to Grassroots. A deep copy will be made of this
 * so this value does not need to stay in scope.
 * @return <code>true></code> if the path was set successfully,
 * <code>false</code> otherwise.
 */
GRASSROOTS_SERVICE_API bool SetServerRootDirectory (const char * const path_s);


/**
 * Get the path to where Grassroots is installed.
 *
 * @return The path to Grassroots
 */
GRASSROOTS_SERVICE_API const char *GetServerRootDirectory (void);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SERVICE_CONFIG_H */

