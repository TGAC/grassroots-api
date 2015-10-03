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
#include "wheatis_util_library.h"


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


WHEATIS_UTIL_API StringIntPairArray *AllocateStringIntPairArray (const uint32 size);


WHEATIS_UTIL_API void FreeStringIntPairArray (StringIntPairArray *array_p);


WHEATIS_UTIL_API bool SetStringIntPair (StringIntPair *pair_p, char *text_s, MEM_FLAG text_mem, const uint32 value);


WHEATIS_UTIL_API void ClearStringIntPair (StringIntPair *pair_p);


#ifdef __cplusplus
}
#endif



#endif /* STRING_INT_PAIR_H_ */
