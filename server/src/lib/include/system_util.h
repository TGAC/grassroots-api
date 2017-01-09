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
#ifndef SYSTEM_UTIL_H
#define SYSTEM_UTIL_H

#include "grassroots_service_manager_library.h"
#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Initialise the various base components of the Grassroots
 * system.
 *
 * @return <code>true</code> if the system was successfully
 * initialised, <code>false</code> otherwise.
 * @ingroup server_group
 */
GRASSROOTS_SERVICE_MANAGER_API bool InitInformationSystem (void);


/**
 * Perform any pre-termination tasks of the Grassroots
 * system prior to it exiting.
 *
 * @return <code>true</code> if the tasks were successfully
 * completed, <code>false</code> otherwise.
 * @ingroup server_group
 */
GRASSROOTS_SERVICE_MANAGER_API bool DestroyInformationSystem (void);


/**
 * Set the Grassroots software running on a multi-process system
 * e.g. Apache httpd with multiple processes or single process
 * e.g. Apache httpd ran with the "-X" option or with StartServers
 * and ServerLimit set to 1 when using the event mpm module.
 *
 * @param b <code>true</code> if the system is potentially running
 * within multiple processes, <code>false</code> if definitely
 * running within a single process.
 * @ingroup server_group
 */
GRASSROOTS_SERVICE_MANAGER_API void SetMultiProcessSystem (bool b);


/**
 * Is the Grassroots software running on a multi-process system
 * e.g. Apache httpd with multiple processes or single process
 * e.g. Apache httpd ran with the "-X" option or with StartServers
 * and ServerLimit set to 1 when using the event mpm module.
 *
 * @return <code>true</code> if the system is potentially running
 * within multiple processes, <code>false</code> if definitely
 * running within a single process.
 * @ingroup server_group
 */
GRASSROOTS_SERVICE_MANAGER_API bool IsMultiProcessSystem (void);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef SYSTEM_UTIL_H */
