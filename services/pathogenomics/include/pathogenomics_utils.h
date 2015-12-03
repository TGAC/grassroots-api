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
 * pathogenomics_utils.h
 *
 *  Created on: 2 Dec 2015
 *      Author: tyrrells
 */

#ifndef PATHOGENOMICS_UTILS_H_
#define PATHOGENOMICS_UTILS_H_

#include "pathogenomics_service_library.h"
#include "jansson.h"

#ifdef __cplusplus
extern "C"
{
#endif


PATHOGENOMICS_SERVICE_LOCAL bool AddPublishDateToJSON (json_t *json_p, const char * const key_s);


#ifdef __cplusplus
}
#endif

#endif /* PATHOGENOMICS_UTILS_H_ */
