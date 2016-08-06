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


typedef struct SchemaVersion
{
	uint32 sv_major;
	uint32 sv_minor;
} SchemaVersion;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the major version of the Grassroots schema to use.
 *
 * @return The major number.
 */
GRASSROOTS_UTIL_API uint32 GetSchemaMajorVersionFromConfig (void);


/**
 * Get the minor version of the Grassroots schema to use.
 *
 * @return The minor number.
 */
GRASSROOTS_UTIL_API uint32 GetSchemaMinorVersionFromConfig (void);


#ifdef __cplusplus
}
#endif


#endif /* SRC_UTIL_INCLUDE_SCHEMA_VERSION_H_ */
