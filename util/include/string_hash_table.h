#ifndef STRING_HASH_TABLE_H
#define STRING_HASH_TABLE_H

#include "hash_table.h"
#include "wheatis_util_library.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Create a HashTable where both the keys and values are strings.
 *
 * @param initital_capacity The initial number of HashBuckets for the HashTable.
 * @param load_percentage The percentage value for how full the HashTable should
 * be allowed to become before it is extended.
 * @return The HashTable or NULL is there was an error.
 */
WHEATIS_UTIL_API HashTable *GetHashTableOfStrings (const uint32 initial_capacity, const uint8 load_percentage);



WHEATIS_UTIL_API bool CompareStringHashBuckets (const void * const bucket_key_p, const void * const key_p);



WHEATIS_UTIL_API int CompareKeysAlphabetically (const void *v0_p, const void *v1_p);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef STRING_HASH_TABLE_H */
