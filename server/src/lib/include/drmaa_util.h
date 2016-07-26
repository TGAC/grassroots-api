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

GRASSROOTS_SERVICE_MANAGER_API bool InitDrmaaEnvironment (void);

GRASSROOTS_SERVICE_MANAGER_API bool ExitDrmaaEnvironment (void);

#ifdef __cplusplus
}
#endif


#endif /* DRMAA_UTIL_H_ */
