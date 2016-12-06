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
 * provider.h
 *
 *  Created on: 15 Sep 2015
 *      Author: tyrrells
 */

#ifndef PROVIDER_H_
#define PROVIDER_H_

#include "grassroots_service_library.h"
#include "jansson.h"


#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_SERVICE_API const char *GetProviderName (const json_t * const data_p);

GRASSROOTS_SERVICE_API const char *GetProviderURI (const json_t * const data_p);

GRASSROOTS_SERVICE_API const char *GetProviderDescription (const json_t * const data_p);


#ifdef __cplusplus
}
#endif


#endif /* PROVIDER_H_ */
