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
 * provider.c
 *
 *  Created on: 15 Sep 2015
 *      Author: tyrrells
 */

#include "provider.h"

#include "json_tools.h"
#include "json_util.h"


const char *GetProviderName (const json_t * const data_p)
{
	return GetJSONString (data_p, PROVIDER_NAME_S);
}


const char *GetProviderURI (const json_t * const data_p)
{
	return GetJSONString (data_p, PROVIDER_URI_S);
}


const char *GetProviderDescription (const json_t * const data_p)
{
	return GetJSONString (data_p, PROVIDER_DESCRIPTION_S);
}
