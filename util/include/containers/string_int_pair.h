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

typedef struct StringIntPair
{
	char *sip_string_s;
	MEM_FLAG sip_string_mem;
	uint32 sip_value;
} StringIntPair;


typedef struct StringIntPairArray
{
	StringIntPair *sipa_values_p;
	uint32 sipa_size;
} StringIntPairArray;



#ifdef __cplusplus
extern "C" {
#endif


GRASSROOTS_UTIL_API StringIntPairArray *AllocateStringIntPairArray (const uint32 size);


GRASSROOTS_UTIL_API void FreeStringIntPairArray (StringIntPairArray *array_p);


GRASSROOTS_UTIL_API bool SetStringIntPair (StringIntPair *pair_p, char *text_s, MEM_FLAG text_mem, const uint32 value);


GRASSROOTS_UTIL_API void ClearStringIntPair (StringIntPair *pair_p);


GRASSROOTS_UTIL_API void SortStringIntPairsByCount (StringIntPairArray *pairs_p);


GRASSROOTS_UTIL_API json_t *GetStringIntPairsAsResourceJSON (const StringIntPairArray *pairs_p);


#ifdef __cplusplus
}
#endif



#endif /* STRING_INT_PAIR_H_ */
