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
#ifndef STRING_HASH_TABLE_H
#define STRING_HASH_TABLE_H

#include "hash_table.h"
#include "grassroots_util_library.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Create a HashTable where both the keys and values are strings.
 *
 * @param initial_capacity The initial number of HashBuckets for the HashTable.
 * @param load_percentage The percentage value for how full the HashTable should
 * be allowed to become before it is extended.
 * @return The HashTable or <code>NULL</code> is there was an error.
 */
GRASSROOTS_UTIL_API HashTable *GetHashTableOfStrings (const uint32 initial_capacity, const uint8 load_percentage);



/**
 * Create a HashTable where the keys are strings and the values are uint32.
 *
 * @param initial_capacity The initial number of HashBuckets for the HashTable.
 * @param load_percentage The percentage value for how full the HashTable should
 * be allowed to become before it is extended.
 * @return The HashTable or <code>NULL</code> is there was an error.
 */
GRASSROOTS_UTIL_API HashTable *GetHashTableOfStringInts (const uint32 initial_capacity, const uint8 load_percentage);


GRASSROOTS_UTIL_API bool CompareStringHashBuckets (const void * const bucket_key_p, const void * const key_p);


GRASSROOTS_UTIL_API int CompareKeysAlphabetically (const void *v0_p, const void *v1_p);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef STRING_HASH_TABLE_H */
