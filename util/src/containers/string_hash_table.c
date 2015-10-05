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
#include <stdio.h>

#include <string.h>

#include "memory_allocations.h"

#include "hash_table.h"
#include "string_hash_table.h"
#include "string_utils.h"


static bool FillStringValue (const void *src_p, const void **dest_pp, const MEM_FLAG mf);


static bool FillStringHashBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p);


static void PrintStringHashBucket (const HashBucket * const bucket_p, OutputStream * const stream_p);


static bool SaveStringHashBucket (const HashBucket * const bucket_p, FILE *out_f);


static bool FillStringIntHashBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p);


static void PrintStringIntHashBucket (const HashBucket * const bucket_p, OutputStream * const stream_p);


static bool SaveStringIntHashBucket (const HashBucket * const bucket_p, FILE *out_f);




/**
	* Get A HashTable of HashBuckets that have strings for their keys and values.
	*
	*
	*/
HashTable *GetHashTableOfStrings (const uint32 initial_capacity, const uint8 load_percentage)
{
	return AllocateHashTable (initial_capacity, load_percentage, HashString, CreateDeepCopyHashBuckets, NULL, FillStringHashBucket, CompareStringHashBuckets, PrintStringHashBucket, SaveStringHashBucket);
}


/**
	* Get A HashTable of HashBuckets that have strings for their keys and ints for their values.
	*
	*
	*/
HashTable *GetHashTableOfStringInts (const uint32 initial_capacity, const uint8 load_percentage)
{
	return AllocateHashTable (initial_capacity, load_percentage, HashString, CreateDeepCopyHashBuckets, NULL, FillStringIntHashBucket, CompareStringHashBuckets, PrintStringIntHashBucket, SaveStringIntHashBucket);
}



/** Function for filling a HashBucket */
static bool FillStringHashBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p)
{
	bool success_flag = false;

	if (FillStringValue (key_p, & (bucket_p -> hb_key_p), bucket_p -> hb_owns_key))
		{
			if (FillStringValue (value_p, & (bucket_p -> hb_value_p), bucket_p -> hb_owns_value))
				{
					success_flag = true;
				}
		}

	return success_flag;
}



/** Function for comparing the keys of two HashBuckets */
bool CompareStringHashBuckets (const void * const bucket_key_p, const void * const key_p)
{
	if (bucket_key_p && key_p)
		{
			const char * const bucket_key_string_p = (const char * const) bucket_key_p;
			const char * const key_string_p = (const char * const) key_p;

			return ((strcmp (bucket_key_string_p, key_string_p)) == 0);
		}

	return false;
}


int CompareKeysAlphabetically (const void *v0_p, const void *v1_p)
{
	const char ** const key0_pp = (const char ** const) (v0_p);
	const char ** const key1_pp = (const char ** const) (v1_p);

	return strcmp (*key0_pp, *key1_pp);
}



static void PrintStringHashBucket (const HashBucket * const bucket_p, OutputStream * const stream_p)
{
	const char * const key_p = (const char * const) (bucket_p -> hb_key_p);
	const char * const value_p = (const char * const) (bucket_p -> hb_value_p);

	PrintToOutputStream (stream_p, "key: \"%s\" (%sowned) value: \"%s\" (%sowned)\n", (key_p != NULL) ? key_p : "NULL", bucket_p -> hb_owns_key ? "" : "not ", (value_p != NULL) ? value_p : "NULL", bucket_p -> hb_owns_value ? "" : "not ");
}


static bool SaveStringHashBucket (const HashBucket * const bucket_p, FILE *out_f)
{
	return (fprintf (out_f, "%s = %s\n", (const char * const) (bucket_p -> hb_key_p), (const char * const) (bucket_p -> hb_value_p)) > 0);
}


static bool FillStringValue (const void *src_p, const void **dest_pp, const MEM_FLAG mf)
{
	bool success_flag = true;

	switch (mf)
		{
			case MF_DEEP_COPY:
				{
					char *dest_p = CopyToNewString ((const char * const) src_p, 0, false);

					if (dest_p)
						{
							*dest_pp = dest_p;
						}
					else
						{
							success_flag = false;
						}
				}
				break;

			case MF_SHALLOW_COPY:
			case MF_SHADOW_USE:
			case MF_ALREADY_FREED:
				*dest_pp = src_p;
				break;
		}

	return success_flag;
}


/** Function for filling a HashBucket */
static bool FillStringIntHashBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p)
{
	bool success_flag = false;

	if (FillStringValue (key_p, & (bucket_p -> hb_key_p), bucket_p -> hb_owns_key))
		{
			int *dest_value_p = (int *) (bucket_p -> hb_value_p);

			if (!dest_value_p)
				{
					dest_value_p = (int *) AllocMemory (sizeof (int));
				}

			if (dest_value_p)
				{
					*dest_value_p = * ((const int *) value_p);

					bucket_p -> hb_value_p = dest_value_p;
					success_flag = true;
				}
		}

	return success_flag;
}



static void PrintStringIntHashBucket (const HashBucket * const bucket_p, OutputStream * const stream_p)
{
	const char * const key_p = (const char * const) (bucket_p -> hb_key_p);
	const int value = * ((const int * const) (bucket_p -> hb_value_p));

	PrintToOutputStream (stream_p, "key: \"%s\" (%sowned) value: " UINT32_FMT " (%sowned)\n", (key_p != NULL) ? key_p : "NULL", bucket_p -> hb_owns_key ? "" : "not ", value, bucket_p -> hb_owns_value ? "" : "not ");
}



static bool SaveStringIntHashBucket (const HashBucket * const bucket_p, FILE *out_f)
{
	return (fprintf (out_f, "%s = " UINT32_FMT "\n", (const char * const) (bucket_p -> hb_key_p), * ((const int * const) (bucket_p -> hb_value_p))) > 0);
}
