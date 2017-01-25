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
/**
 * @file
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include "typedefs.h"
#include "grassroots_util_library.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Initialise the Grassroots system for the operating system
 * that it is running on.
 *
 * @return <code>true</code> if the value was converted successfully, <code>false</code> otherwise.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API bool InitPlatform (void);


/**
 * Release any operating system-specific resources.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API void ExitPlatform (void);


/**
 * Sleep the current process.
 *
 * @param milliseconds The number of milliseconds to sleep for.
 * @return 0 if the process slept for the required time, or the
 * number of milliseconds remaining if it was interrupted.
 *
 * @ingroup utility_group
 */
GRASSROOTS_UTIL_API uint32 Snooze (uint32 milliseconds);


#ifdef __cplusplus
}
#endif



#endif



