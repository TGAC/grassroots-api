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
 * version.c
 *
 *  Created on: 6 May 2016
 *      Author: tyrrells
 */

#include "schema_version.h"
#include "json_util.h"
#include "grassroots_config.h"


uint32 GetSchemaMajorVersionFromConfig (void)
{
	uint32 major = 0;
	const json_t *schema_p = GetGlobalConfigValue (SCHEMA_S);

	if (schema_p)
		{
			GetJSONInteger (schema_p, VERSION_MAJOR_S, (int *) &major);
		}

	return major;
}


uint32 GetSchemaMinorVersionFromConfig (void)
{
	uint32 minor = 9;
	const json_t *schema_p = GetGlobalConfigValue (SCHEMA_S);

	if (schema_p)
		{
			GetJSONInteger (schema_p, VERSION_MINOR_S, (int *) &minor);
		}

	return minor;
}
