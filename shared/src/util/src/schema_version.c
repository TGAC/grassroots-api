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

/**
 * This define will allocate the variables
 * CURRENT_SCHEMA_VERSION_MAJOR and
 * CURRENT_SCHEMA_VERSION_MINOR
 * in schema_version.h
 */
#define ALLOCATE_SCHEMA_VERSION_TAGS

#include "schema_version.h"
#include "json_util.h"
#include "streams.h"


json_t *GetSchemaVersionAsJSON (const SchemaVersion * const sv_p)
{
	json_error_t err;
	json_t *res_p = json_pack_ex (&err, 0, "{s:i,s:i}", VERSION_MAJOR_S, sv_p -> sv_major, VERSION_MINOR_S, sv_p -> sv_minor);

	if (!res_p)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create schema version json: \"%s\"\n\"%s\"\n%d %d %d\n", err.text, err.source, err.line, err.column, err.position);
		}

	return res_p;
}


SchemaVersion *GetSchemaVersionFromJSON (const json_t * const json_p)
{
	SchemaVersion *sv_p = NULL;
	int major;

	if (GetJSONInteger (json_p, VERSION_MAJOR_S, &major))
		{
			int minor;

			if (GetJSONInteger (json_p, VERSION_MINOR_S, &minor))
				{
					sv_p = AllocateSchemaVersion (major, minor);
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, json_p, "Failed to get %s", VERSION_MINOR_S);
				}
		}
	else
		{
			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, json_p, "Failed to get %s", VERSION_MAJOR_S);
		}

	return sv_p;
}


SchemaVersion *AllocateSchemaVersion (const int major, const int minor)
{
	SchemaVersion *sv_p = (SchemaVersion *) AllocMemory (sizeof (SchemaVersion));

	if (sv_p)
		{
			sv_p -> sv_major = major;
			sv_p -> sv_minor = minor;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for schema version");
		}

	return sv_p;
}


void FreeSchemaVersion (SchemaVersion *sv_p)
{
	FreeMemory (sv_p);
}
