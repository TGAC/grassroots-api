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
 * string_int_pair.h
 *
 *  Created on: 3 Oct 2015
 *      Author: billy
 */

#ifndef STRING_INT_PAIR_H_
#define STRING_INT_PAIR_H_

#include "typedefs.h"
#include "memory_allocations.h"
#include "grassroots_util_library.h"
#include "jansson.h"


/**
 * A datatype for holding a key-value pair where the
 * key is a c-style string and the value is an unsigned
 * integer.
 */
typedef struct StringIntPair
{
	/** The key. */
	char *sip_string_s;

	/** The memory flag for the key */
	MEM_FLAG sip_string_mem;

	/** The value. */
	uint32 sip_value;
} StringIntPair;


/**
 * A datatype for storing a set of StringIntPairs.
 */
typedef struct StringIntPairArray
{
	/** An array of StringIntPairs. */
	StringIntPair *sipa_values_p;

	/** The number of StringIntPairs in the array. */
	uint32 sipa_size;
} StringIntPairArray;



#ifdef __cplusplus
extern "C" {
#endif


/**
 * Allocate a StringIntPairArray of a given size.
 *
 * @param size The number of entries in the StringIntPairArray.
 * @return If successful, the newly-allocated StringIntPairArray with all of its
 * keys set to <code>NULL</code> and all of its values set to 0. Upon failure
 * <code>NULL</code> is returned.
 * @memberof StringIntPairArray
 * @see StringIntPair
 */
GRASSROOTS_UTIL_API StringIntPairArray *AllocateStringIntPairArray (const uint32 size);


/**
 * Free a StringIntPairArray and any memory associated
 * from its keys where appropriate.
 *
 * @param array_p The StringIntPairArray to free.
 * @memberof StringIntPairArray
 */
GRASSROOTS_UTIL_API void FreeStringIntPairArray (StringIntPairArray *array_p);


/**
 * Set the data for a StringIntPair.
 *
 * @param pair_p The StringIntPair to set the values for.
 * @param text_s The new key value.
 * @param text_mem The MEM_FLAG determining how to store the key value.
 * @param value The new value.
 * @return Upon success this will free, if required, any memory associated with the old key and update the
 * key and value as appropriate and return <code>true</code>. Upon failure, this will return <code>false</code>
 * and the data of the StringIntPair will remain unaltered.
 * @memberof StringIntPair
 */
GRASSROOTS_UTIL_API bool SetStringIntPair (StringIntPair *pair_p, char *text_s, MEM_FLAG text_mem, const uint32 value);


/**
 * Clear the entries for a StringIntPair and freeing any memory associated with its value
 * if appropriate.
 *
 * @param pair_p The StringIntPair to clear.
 * @memberof StringIntPair
 */
GRASSROOTS_UTIL_API void ClearStringIntPair (StringIntPair *pair_p);


/**
 * Sort the entries of a StringIntPairArray into ascending order of their values.
 *
 * @param pairs_p The StringIntPairArray to sort.
 * @memberof StringIntPairArray
 */
GRASSROOTS_UTIL_API void SortStringIntPairsByCount (StringIntPairArray *pairs_p);


/**
 * Get the JSON fragment representing a StringIntPairArray.
 *
 * This will call GetStringIntPairAsResourceJSON for each item in the array.
 *
 * @param pairs_p The StringIntPairArray to get the JSON fragment for.
 * @param protocol_s The Protocol for the Resource.
 * @return The JSON fragment or <code>NULL</code> upon error.
 * @see PROTOCOL_IRODS_S
 * @see PROTOCOL_HTTP_S
 * @see PROTOCOL_HTTPS_S
 * @see PROTOCOL_FILE_S
 * @see PROTOCOL_INLINE_S
 * @see GetStringIntPairAsResourceJSON
 */
GRASSROOTS_UTIL_API json_t *GetStringIntPairsAsResourceJSON (const StringIntPairArray *pairs_p, const char * const protocol_s);


/**
 * Get the JSON fragment representing a StringIntPair.
 *
 * @param pair_p The StringIntPairArray to get the JSON fragment for.
 * @param protocol_s The Protocol for the Resource.
 * @return The JSON fragment or <code>NULL</code> upon error.
 * @see PROTOCOL_IRODS_S
 * @see PROTOCOL_HTTP_S
 * @see PROTOCOL_HTTPS_S
 * @see PROTOCOL_FILE_S
 * @see PROTOCOL_INLINE_S
 */
GRASSROOTS_UTIL_API json_t *GetStringIntPairAsResourceJSON (const StringIntPair *pair_p, const char * const protocol_s);


#ifdef __cplusplus
}
#endif



#endif /* STRING_INT_PAIR_H_ */
