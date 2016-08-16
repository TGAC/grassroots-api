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
 * schema_version.h
 *
 *  Created on: 6 May 2016
 *      Author: tyrrells
 */

#ifndef SRC_UTIL_INCLUDE_SCHEMA_VERSION_H_
#define SRC_UTIL_INCLUDE_SCHEMA_VERSION_H_

#include "grassroots_util_library.h"
#include "typedefs.h"
#include "jansson.h"


/**
 * This is a datatype to store the versioning details
 * for the Grassroots JSON schema that is being used.
 * This allows backward compatibility code to be used
 * as the schema is upgraded.
 */
typedef struct SchemaVersion
{
	/** The major revision of the schema. */
	uint32 sv_major;

	/** The minor revision of the schema. */
	uint32 sv_minor;
} SchemaVersion;



/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_SCHEMA_VERSION_TAGS is defined then it will
 * become
 *
 * 		const char *ALLOCATE_SCHEMA_VERSION_TAGS = "path";
 *
 * ALLOCATE_SCHEMA_VERSION_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * schema_version.c.
 */
#ifdef ALLOCATE_SCHEMA_VERSION_TAGS
	#define SV_PREFIX GRASSROOTS_UTIL_API
	#define SV_VAL(x)	= x
#else
	#define SV_PREFIX extern
	#define SV_VAL(x)
#endif

SV_PREFIX const uint32 CURRENT_SCHEMA_VERSION_MAJOR SV_VAL(0);
SV_PREFIX const uint32 CURRENT_SCHEMA_VERSION_MINOR SV_VAL(9);

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Create a new SchemaVersion.
 *
 * @param major The major revision of the SchemaVersion.
 * @param minor The minor revision of the SchemaVersion.
 * @return The newly-allocated SchemaVersion which will need to be freed
 * when finished with or <code>NULL</code> upon error.
 * @see FreeSchemaVersion
 * @memberof SchemaVersion
 */
GRASSROOTS_UTIL_API SchemaVersion *AllocateSchemaVersion (const int major, const int minor);


/**
 * Free the memory and resources for a given SchemaVersion.
 *
 * @param sv_p The SchemaVersion to free.
 * @memberof SchemaVersion
 */
GRASSROOTS_UTIL_API void FreeSchemaVersion (SchemaVersion *sv_p);


/**
 * Get the JSON representation for a SchemaVersion.
 *
 * @param sv_p The SchemaVersion to get the JSON representation of.
 * @return The newly-allocated JSON representation or <code>NULL</code> upon error.
 * @memberof SchemaVersion
 */
GRASSROOTS_UTIL_API json_t *GetSchemaVersionAsJSON (const SchemaVersion * const sv_p);


/**
 * Create a SchemaVersion from a JSON representation.
 *
 * @param json_p The JSON representation to create a SchemaVersion from.
 * @return The newly-allocated SchemaVersion which will need to be freed
 * when finished with or <code>NULL</code> upon error.
 * @see FreeSchemaVersion
 * @memberof SchemaVersion
 */
GRASSROOTS_UTIL_API SchemaVersion *GetSchemaVersionFromJSON (const json_t * const json_p);




#ifdef __cplusplus
}
#endif


#endif /* SRC_UTIL_INCLUDE_SCHEMA_VERSION_H_ */
