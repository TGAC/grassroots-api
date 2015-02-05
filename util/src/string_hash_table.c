#include <stdio.h>

#include <string.h>

#include "memory_allocations.h"

#include "hash_table.h"
#include "string_hash_table.h"
#include "string_utils.h"

static bool FillStringHashBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p);


static void PrintStringHashBucket (const HashBucket * const bucket_p, OutputStream * const stream_p);


static bool SaveStringHashBucket (const HashBucket * const bucket_p, FILE *out_f);


static uint32 HashString (const void * const key_p);

static uint32 HashString (const void * const key_p)
{
	uint32 res = 0;

	if (key_p)
		{
			const char *c_p = (const char *) key_p;

			while (*c_p)
				{
					res = (res << 1) ^ *c_p ++;
				}
		}

	return res;
}



/**
	* Get A HashTable of HashBuckets that have strings for their keys and values.
	*
	*
	*/
HashTable *GetHashTableOfStrings (const uint32 initial_capacity, const uint8 load_percentage)
{
	return AllocateHashTable (initial_capacity, load_percentage, HashString, CreateDeepCopyHashBuckets, NULL, FillStringHashBucket, CompareStringHashBuckets, PrintStringHashBucket, SaveStringHashBucket);
}



/** Function for filling a HashBucket */
static bool FillStringHashBucket (HashBucket * const bucket_p, const void * const key_p, const void * const value_p)
{
	bool success = true;
	char *dest_p = NULL;

	FreeHashBucket (bucket_p);

	switch (bucket_p -> hb_owns_key)
		{
			case MF_DEEP_COPY:
				dest_p = CopyToNewString ((const char * const) key_p, 0, false);

				if (dest_p)
					{
						bucket_p -> hb_key_p = dest_p;
					}
				else
					{
						success = false;
					}
				break;

			case MF_SHALLOW_COPY:
			case MF_SHADOW_USE:
			case MF_ALREADY_FREED:
				bucket_p -> hb_key_p = key_p;
				break;
		}

	if (success)
		{
			switch (bucket_p -> hb_owns_value)
				{
					case MF_DEEP_COPY:
						dest_p = CopyToNewString ((const char * const) value_p, 0, false);

						if (dest_p)
							{
								bucket_p -> hb_value_p = dest_p;
							}
						else
							{
								success = false;
							}
						break;

					case MF_SHALLOW_COPY:
					case MF_SHADOW_USE:
					case MF_ALREADY_FREED:
						bucket_p -> hb_value_p = value_p;
						break;
				}
		}

	return success;
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

