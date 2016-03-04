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
/**
 * @file user.h
 * @addtogroup iRODS
 * @brief Routines to query user data objects and collections
 * @{
 */
#ifndef IRODS_UTIL_USER_H
#define IRODS_UTIL_USER_H

#include <time.h>

#include <jansson.h>

#include "irods_util_library.h"


#include "linked_list.h"
#include "typedefs.h"
#include "query.h"




#ifdef __cplusplus
extern "C" 
{
#endif



/**
 * Get all data objects for a given user modified within a given time interval.
 * This calls GetAllModifiedDataForUsername before constructing a json object
 * from these results.
 *
 * @param username_s The username to get the data objects for.
 * @param from The start of the time interval.
 * @param to The end of the time interval.
 * @return A json-based representation of the matching data objects.
 * @see GetAllModifiedDataForUsername
 */
IRODS_UTIL_API json_t *GetModifiedIRodsFiles  (const char * const username_s, const char * const password_s, const time_t from, const time_t to);


/**
 * Get all Services that can work on a given data object.
 *
 * @param services_path_s The directory containing the Service modules.
 * @param username_s The username to get the data objects for.
 * @param password_s The iRODS password. FIXME! This needs to change to being encrypted.
 * @param data_name_s The name of the data object.
 * @return A json-based representation of the matching Services.
 */
IRODS_UTIL_API json_t *GetInterestedServicesForIrodsDataObject (const char *services_path_s, char * const username_s, char * const password_s, const char *data_name_s);

/** @} */

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_USER_H */
