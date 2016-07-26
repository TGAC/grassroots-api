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
 * encode_dcc_metadata.h
 *
 *  Created on: 9 May 2016
 *      Author: billy
 */

#ifndef SRC_SERVICES_LIB_INCLUDE_ENCODE_DCC_METADATA_H_
#define SRC_SERVICES_LIB_INCLUDE_ENCODE_DCC_METADATA_H_

#include "grassroots_service_library.h"
#include "typedefs.h"

#include "jansson.h"



#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_SERVICE_LOCAL bool AddEncodeDDCContext (json_t *service_json_p, json_t *contexts_p);

#ifdef __cplusplus
}
#endif


#endif /* SRC_SERVICES_LIB_INCLUDE_ENCODE_DCC_METADATA_H_ */
