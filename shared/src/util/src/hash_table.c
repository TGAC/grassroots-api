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
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "memory_allocations.h"

#include "hash_table.h"
#include "math_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"


#ifdef _DEBUG
	#define HASH_TABLE_DEBUG (STM_LEVEL_FINER)
#else
	#define HASH_TABLE_DEBUG (STM_LEVEL_NONE)
#endif

static const char S_INCLUDE_DIRECTIVE_S [] = "IncludeFile";


/**
 * Increase the capacity of the HashTable.
 *
 * @param hash_table_p The HashTable to extend.
 */
static void ExtendHashTable (HashTable * const hash_table_p);

static HashBucket *GetMatchingBucket (const HashTable * const hash_table_p, const void * const key_p, const bool get_empty_matching_slot);

static bool SaveUnorderedHashTable (const HashTable * const table_p, FILE *out_f);

static bool SaveOrderedHashTable (const HashTable * const table_p, FILE *out_f, int (*compare_fn) (const void *v0_p, const void *v1_p));



HashBucket *CreateDeepCopyHashBuckets (const uint32 num_buckets)
{
	return CreateHashBuckets (num_buckets, MF_DEEP_COPY, MF_DEEP_COPY);
}


HashBucket *CreateShallowCopyHashBuckets (const uint32 num_buckets)
{
	return CreateHashBuckets (num_buckets, MF_SHALLOW_COPY, MF_SHALLOW_COPY);
}


HashBucket *CreateShadowUseHashBuckets (const uint32 num_buckets)
{
	return CreateHashBuckets (num_buckets, MF_SHADOW_USE, MF_SHADOW_USE);
}


HashBucket *CreateDeepCopyKeysShallowCopyValueHashBuckets (const uint32 num_buckets)
{
	return CreateHashBuckets (num_buckets, MF_DEEP_COPY, MF_SHALLOW_COPY);
}



/** Function for creating the HashBuckets */
HashBucket *CreateHashBuckets (const uint32 num_buckets, const MEM_FLAG key_mem_flag, const MEM_FLAG value_mem_flag)
{
	HashBucket *buckets_p = ((HashBucket *) AllocMemoryArray (num_buckets, sizeof (HashBucket)));

	if (buckets_p)
		{
			uint32 i = 0;
			HashBucket *bucket_p = buckets_p;

			while (i < num_buckets)
				{
					bucket_p -> hb_owns_key = key_mem_flag;
					bucket_p -> hb_owns_value = value_mem_flag;

					++ bucket_p;
					++ i;
				}
		}

	return buckets_p;
}



/**
 * Function for freeing a HashBucket
 *
 * @param bucket_p The HashBucket to free.
 */
void FreeHashBucket (HashBucket * const bucket_p)
{
	switch (bucket_p -> hb_owns_key)
		{
			case MF_DEEP_COPY:
			case MF_SHALLOW_COPY:
				if (bucket_p -> hb_key_p)
					{
						FreeMemory ((void *) (bucket_p -> hb_key_p));
					}
				break;

			case MF_SHADOW_USE:
			default:
				break;
		}
	bucket_p -> hb_key_p = NULL;

	switch (bucket_p -> hb_owns_value)
		{
			case MF_DEEP_COPY:
			case MF_SHALLOW_COPY:
				if (bucket_p -> hb_value_p)
					{
						FreeMemory ((void *) (bucket_p -> hb_value_p));
					}
				break;

			case MF_SHADOW_USE:
			default:
				break;
		}
	bucket_p -> hb_value_p = NULL;
}


/**
 * Allocate a new HashTable.
 *
 * @return The new HashTable or NULL if there was an error.
 */
HashTable *AllocateHashTable (const uint32 initial_capacity,
	const uint8 load_percentage,
	uint32 (*hash_fn) (const void *key_p),
	HashBucket *(*create_buckets_fn) (const uint32 num_buckets),
	void (*free_bucket_fn) (HashBucket * const bucket_p),
	bool (*fill_bucket_fn) (HashBucket * const bucket_p, const void * const key_p, const void * const value_p),
	bool (*compare_keys_fn) (const void * const bucket_key_p, const void * const key_p),
	void (*print_bucket_fn) (const HashBucket * const bucket_p, OutputStream * const stream_p),
	bool (*save_bucket_fn) (const HashBucket * const bucket_p, FILE *out_f))
{
	HashTable *hash_table_p = (HashTable *) AllocMemory (sizeof (HashTable));

	if (hash_table_p != NULL)
		{
			HashBucket *buckets_p = NULL;

			if (create_buckets_fn)
				{
					hash_table_p -> ht_create_buckets_fn = create_buckets_fn;
				}
			else
				{
					hash_table_p -> ht_create_buckets_fn = CreateDeepCopyHashBuckets;
				}

			buckets_p = hash_table_p -> ht_create_buckets_fn (initial_capacity);

			if (buckets_p)
				{
					hash_table_p -> ht_buckets_p = buckets_p;

					hash_table_p -> ht_hash_fn = hash_fn;

					if (free_bucket_fn)
						{
							hash_table_p -> ht_free_bucket_fn = free_bucket_fn;
						}
					else
						{
							hash_table_p -> ht_free_bucket_fn = FreeHashBucket;
						}

					hash_table_p -> ht_fill_bucket_fn = fill_bucket_fn;
					hash_table_p -> ht_compare_keys_fn = compare_keys_fn;
					hash_table_p -> ht_print_bucket_fn = print_bucket_fn;
					hash_table_p -> ht_save_bucket_fn = save_bucket_fn;

					hash_table_p -> ht_capacity = initial_capacity;
					hash_table_p -> ht_size = 0;

					hash_table_p -> ht_load = (load_percentage < 100) ? load_percentage : 100;
					hash_table_p -> ht_load_limit = (uint32) (((double) (hash_table_p -> ht_load)) * ((double) (hash_table_p -> ht_capacity)) * 0.010);

					return hash_table_p;
				}

			FreeMemory (hash_table_p);
		}

	return NULL;
}


/**
 * Free a HashTable
 *
 * @param hash_table_p The HashTable to free.
 */
void FreeHashTable (HashTable * const hash_table_p)
{
	ClearHashTable (hash_table_p);

	FreeMemory (hash_table_p -> ht_buckets_p);
	FreeMemory (hash_table_p);
}


/**
 * Clear a HashTable.
 *
 * @param hash_table_p The HashTable to clear.
 */
void ClearHashTable (HashTable * const hash_table_p)
{
	HashBucket *bucket_p = hash_table_p -> ht_buckets_p;
	int i;

	for (i = hash_table_p -> ht_capacity; i > 0; -- i, ++ bucket_p)
		{
			if (IsValidHashBucket (bucket_p))
				{
					hash_table_p -> ht_free_bucket_fn (bucket_p);
				}
		}
}



/**
 * Put a key-value pair in the HashTable. If a bucket already contains this key,
 * its value will be overwritten. If the HashTable is getting beyond its
 * designated load, its capacity will be extended.
 *
 * @param hash_table_p The HashTable to put for the key-value pair in.
 * @param key_p The key.
 * @param value_p The value.
 */
bool PutInHashTable (HashTable * const hash_table_p, const void *key_p, const void *value_p)
{
	bool success_flag  = false;
	HashBucket *bucket_p = NULL;
	if (hash_table_p -> ht_size == hash_table_p -> ht_load_limit)
		{
			ExtendHashTable (hash_table_p);
		}

	bucket_p = GetMatchingBucket (hash_table_p, key_p, true);
	if (!IsValidHashBucket (bucket_p))
		{
			++ (hash_table_p -> ht_size);
		}

	success_flag = hash_table_p -> ht_fill_bucket_fn (bucket_p, key_p, value_p);

	if (success_flag)
		{
			bucket_p -> hb_hashed_key = hash_table_p -> ht_hash_fn (key_p);
		}
	
	return success_flag;
}


/**
 * For a given key, get the value from a HashTable. If the key
 * is not in the HashTable, this will return NULL
 *
 * @param hash_table_p The HashTable to search for the value in.
 * @param key_p The key.
 * @return The value or NULL if there is not a matching value for the
 * given key.
 */
const void *GetFromHashTable (const HashTable * const hash_table_p, const void * const key_p)
{
	const HashBucket * const bucket_p = GetMatchingBucket (hash_table_p, key_p, false);
	return ((bucket_p != NULL) ? bucket_p -> hb_value_p : NULL);
}


void **GetKeysIndexFromHashTable (const HashTable * const hash_table_p)
{
	uint32 j = hash_table_p -> ht_size;

	void **keys_pp = (void **) AllocMemoryArray (j, sizeof (void *));
	if (keys_pp)
		{
			uint32 i = hash_table_p -> ht_capacity;
			const HashBucket *bucket_p = hash_table_p -> ht_buckets_p;
			const void **key_pp = (const void **) keys_pp;

			while (i > 0)
				{
					if (bucket_p -> hb_key_p)
						{
							*key_pp = bucket_p -> hb_key_p;

							-- j;

							if (j)
								{
									++ key_pp;
								}
							else
								{
									break;		/* all keys have been found */
								}
						}

					++ bucket_p;
					-- i;
				}

			return keys_pp;
		}

	return NULL;
}


void FreeKeysIndex (void **index_pp)
{
	FreeMemory (index_pp);
}


/**
 * For a given key, empty the corresponding bucket in a HashTable. If the key
 * is not in the HashTable, this will do nothing
 *
 * @param hash_table_p The HashTable to search for the in.
 * @param key_p The key.
 */
void RemoveFromHashTable (HashTable * const hash_table_p, const void * const key_p)
{
	HashBucket * const bucket_p = GetMatchingBucket (hash_table_p, key_p, false);

	if (bucket_p)
		{
			hash_table_p -> ht_free_bucket_fn (bucket_p);
			-- (hash_table_p -> ht_size);
		}
}


uint32 GetHashTableSize (const HashTable * const table_p)
{
	return table_p -> ht_size;
}


/**
 * Find the HashBucket for the given key in a HashTable.
 *
 * @param hash_table_p The HashTable.
 * @param key_p The key that we are looking up.
 * @param get_empty_matching_slot If this is set to true and the key is not in the HashTable,
 * then the empty HashBucket where the key would go will get returned. If this is set to false,
 * only valid existing HashBuckets will be returned.
 * @return The matching existing HashBucket with the given key or if it's not in the HashTable either NULL
 * or the empty slot where the key would go depending upon whether get_empty_matching_slot is true or false.
 */
static HashBucket *GetMatchingBucket (const HashTable * const hash_table_p, const void * const key_p, const bool get_empty_matching_slot)
{
	const uint32 hashed_key = (hash_table_p -> ht_hash_fn) (key_p);
	const uint32 capacity = hash_table_p -> ht_capacity;

	uint32 i = hashed_key % capacity;
	HashBucket *bucket_p = (hash_table_p -> ht_buckets_p) + i;

	/* count determines if we've gone all the way through the table */
	uint32 count = capacity;

	/* cache the function pointer */
	bool (*compare_keys_fn) (const void * const bucket_key_p, const void * const key_p) = hash_table_p -> ht_compare_keys_fn;

 	/* find the nearest bucket that we hash to */
	bool looping = true;
	HashBucket *matching_bucket_p = NULL;

	while (looping)
		{
			if (IsValidHashBucket (bucket_p))
				{
					if (compare_keys_fn (bucket_p -> hb_key_p, key_p))
						{
							matching_bucket_p = bucket_p;
							looping = false;
						}
				}
			else
				{
					if (get_empty_matching_slot)
						{
							matching_bucket_p = bucket_p;
						}

					looping = false;
				}

			if (looping)
				{
					if (++ i == capacity)
						{
							i = 0;
							bucket_p = hash_table_p -> ht_buckets_p;
						}
					else
						{
							++ bucket_p;
						}

					/* check if we have searched every bucket in the table */
					if (-- count == 0)
						{
							looping = false;
						}
				}
		}

	return matching_bucket_p;
}


/**
 * Increase the capacity of the HashTable.
 *
 * @param hash_table_p The HashTable to extend.
 */
static void ExtendHashTable (HashTable * const hash_table_p)
{
	const uint32 new_capacity = (hash_table_p -> ht_capacity << 1) + 1;
	HashBucket *new_buckets_p = hash_table_p -> ht_create_buckets_fn (new_capacity);

	/* reinsert the old bucket values */
	HashBucket *old_buckets_p = hash_table_p -> ht_buckets_p;
	uint32 i = hash_table_p -> ht_capacity;
	uint32 j = hash_table_p -> ht_size;
	HashBucket *old_bucket_p = old_buckets_p;

	hash_table_p -> ht_buckets_p = new_buckets_p;
	hash_table_p -> ht_capacity = new_capacity;
	hash_table_p -> ht_size = 0;
	hash_table_p -> ht_load_limit = (uint32) (0.010 * new_capacity * (hash_table_p -> ht_load));

	for ( ; i > 0; -- i)
		{
			if (IsValidHashBucket (old_bucket_p))
				{
					PutInHashTable (hash_table_p, old_bucket_p -> hb_key_p, old_bucket_p -> hb_value_p);

					/* if keys and values are copied by address only, make sure we don't free them */
					if (old_bucket_p -> hb_owns_key == MF_SHALLOW_COPY)
						{
							old_bucket_p -> hb_owns_key  = MF_SHADOW_USE;
						}

					if (old_bucket_p -> hb_owns_value == MF_SHALLOW_COPY)
						{
							old_bucket_p -> hb_owns_value  = MF_SHADOW_USE;
						}

					hash_table_p -> ht_free_bucket_fn (old_bucket_p);

					-- j;
					if (j == 0)
						{
							i = 1;	/* force exit from loop */
						}

				}

			++ old_bucket_p;
		}

	FreeMemory (old_buckets_p);
}


/**
 * Does the HashBucket contain a valid key-value pair.
 *
 * @param bucket_p The HashBucket.
 * @return true if the bucket does contain a valid key-value pair, false if it is empty.
 */
bool IsValidHashBucket (const HashBucket * const bucket_p)
{
	return (bucket_p -> hb_value_p != NULL);
}


void PrintHashTable (const HashTable * const hash_table_p, OutputStream * const stream_p)
{
	uint32 i = 0;
	const uint32 capacity = hash_table_p -> ht_capacity;
	HashBucket *bucket_p = hash_table_p -> ht_buckets_p;
	void (*print_bucket_fn) (const HashBucket * const bucket_p, OutputStream * const stream_p) = hash_table_p -> ht_print_bucket_fn;
	PrintToOutputStream (stream_p, __FILE__, __LINE__, "size/capacity/load limit: %lu/%lu/%u/%lu\n", hash_table_p -> ht_size, capacity, hash_table_p -> ht_load, hash_table_p -> ht_load_limit);

	if (hash_table_p -> ht_size > 0)
		{
			while (i < capacity)
				{
					PrintToOutputStream (stream_p, __FILE__, __LINE__, "[%lu]: ", i);
					print_bucket_fn (bucket_p, stream_p);
					++ bucket_p;
					++ i;
				}		/* while (i < capacity) */

		}		/* if (hash_table_p -> ht_size > 0) */
}


HashTable *CopyHashTable (const HashTable * const src_table_p, const bool deep_copy_if_necessary)
{
	HashTable *dest_table_p = NULL;
	HashBucket *(*create_buckets_fn) (const uint32 num_buckets) = NULL;
	const HashBucket *src_bucket_p = src_table_p -> ht_buckets_p;

	if (src_bucket_p -> hb_owns_key == MF_SHALLOW_COPY)
		{
			create_buckets_fn = deep_copy_if_necessary ? CreateDeepCopyHashBuckets : CreateShadowUseHashBuckets;
		}
	else
		{
			create_buckets_fn = src_table_p -> ht_create_buckets_fn;
		}


	dest_table_p = AllocateHashTable (src_table_p -> ht_capacity, src_table_p -> ht_load, src_table_p -> ht_hash_fn, create_buckets_fn, NULL, src_table_p -> ht_fill_bucket_fn, src_table_p -> ht_compare_keys_fn, src_table_p -> ht_print_bucket_fn, src_table_p -> ht_save_bucket_fn);

	if (dest_table_p)
		{
			HashBucket *dest_bucket_p = dest_table_p -> ht_buckets_p;
			uint32 i = src_table_p -> ht_capacity;

			while (i > 0)
				{
					if (IsValidHashBucket (src_bucket_p))
						{
							if (!dest_table_p -> ht_fill_bucket_fn (dest_bucket_p, src_bucket_p -> hb_key_p, src_bucket_p -> hb_value_p))
								{
									FreeHashTable (dest_table_p);
									return NULL;
								}		/* if (!dest_table_p -> ht_fill_bucket_fn (dest_bucket_p, src_bucket_p -> hb_key_p, src_bucket_p -> hb_value_p)) */
						}		/* if (IsValidHashBucket (src_bucket_p)) */

					++ src_bucket_p;
					++ dest_bucket_p;
					-- i;

				}		/* while (i > 0) */

			return dest_table_p;
		}		/* if (dest_table_p) */

	return NULL;
}



bool SaveHashTable (const HashTable * const table_p, const char * const filename_s, int (*compare_fn) (const void *v0_p, const void *v1_p))
{
	bool success_flag = false;
	FILE *out_f = fopen (filename_s, "w");

	if (out_f)
		{
			if (compare_fn)
				{
					success_flag = SaveOrderedHashTable (table_p, out_f, compare_fn);
				}
			else
				{
					success_flag = SaveUnorderedHashTable (table_p, out_f);
				}

			fclose (out_f);
		}		/* if (out_f) */

	return success_flag;
}


static bool SaveUnorderedHashTable (const HashTable * const table_p, FILE *out_f)
{
	bool success_flag = false;
	bool (*save_bucket_fn) (const HashBucket * const bucket_p, FILE *out_f) = table_p -> ht_save_bucket_fn;

	if (save_bucket_fn)
		{
			HashBucket *bucket_p = table_p -> ht_buckets_p;
			uint32 i = table_p -> ht_capacity;

			success_flag = true;

			while ((i > 0) && success_flag)
				{
					if (IsValidHashBucket (bucket_p))
						{
							success_flag = save_bucket_fn (bucket_p, out_f);

							if (success_flag)
								{
									++ bucket_p;
									-- i;
								}		/* if (success_flag) */

						}		/* if (IsValidHashBucket (bucket_p)) */

				}		/* while ((i > 0) && success_flag) */

		}		/* if (table_p -> ht_save_bucket_fn) */

	return success_flag;
}


bool SaveOrderedHashTable (const HashTable * const table_p, FILE *out_f, int (*compare_fn) (const void *v0_p, const void *v1_p))
{
	bool success_flag = false;
	bool (*save_bucket_fn) (const HashBucket * const bucket_p, FILE *out_f) = table_p -> ht_save_bucket_fn;

	if (save_bucket_fn)
		{
			void **keys_pp = GetKeysIndexFromHashTable (table_p);

			if (keys_pp)
				{
					void **key_pp = keys_pp;
					uint32 i = table_p -> ht_size;

					qsort (keys_pp, i, sizeof (const void *), compare_fn);
					success_flag = true;

					while ((i > 0) && success_flag)
						{
							HashBucket * const bucket_p = GetMatchingBucket (table_p, *key_pp, false);

							if (bucket_p && IsValidHashBucket (bucket_p))
								{
									success_flag = save_bucket_fn (bucket_p, out_f);
								}		/* if (bucket_p && IsValidHashBucket (bucket_p)) */

							if (success_flag)
								{
									++ key_pp;
									-- i;
								}		/* if (success_flag) */

						}		/* while ((i > 0) && success_flag) */

					FreeKeysIndex (keys_pp);
				}		/* if (keys_pp) */

		}		/* if (save_bucket_fn) */

	return success_flag;
}



bool LoadHashTable (HashTable * const config_p, char *root_path_s, const char * const filename_s)
{
	FILE *in_f = fopen (filename_s, "rb");
	bool success_flag = (in_f != NULL);

	if (success_flag)
		{
			char *line_p = NULL;

			while (success_flag && GetLineFromFile (in_f, &line_p))
				{
					char *key_p;
					char *value_p;

					if (GetKeyValuePair (line_p, &key_p, &value_p, '#'))
						{
							/* Is it a separate config file to include? */
							if (strcmp (S_INCLUDE_DIRECTIVE_S, key_p) == 0)
								{
									if (IsPathAbsolute (value_p))
										{
											success_flag = LoadHashTable (config_p, root_path_s, value_p);
										}
									else
										{											
											char *current_dir_s = root_path_s ? root_path_s : GetCurrentWorkingDirectory ();
											success_flag = false;

											if (current_dir_s)
												{
													char *full_path_s = MakeFilename (current_dir_s, value_p);

													if (full_path_s)
														{
															success_flag = LoadHashTable (config_p, current_dir_s, full_path_s);
															FreeCopiedString (full_path_s);
														}

													if (current_dir_s && (current_dir_s != root_path_s))
														{
															FreeCopiedString (current_dir_s);
														}		/* if (current_dir_s && (current_dir_s != root_path_s)) */
													
												}		/* if (current_dir_s) */

										}		/* if (IsPathAbsolute (filename_s)) */

									FreeCopiedString (key_p);
									FreeCopiedString (value_p);
								}
							else
								{
									PutInHashTable (config_p, key_p, value_p);
								}
						}

					FreeLineBuffer (line_p);
					line_p = NULL;
				}

			fclose (in_f);
		}

	return success_flag;
}



