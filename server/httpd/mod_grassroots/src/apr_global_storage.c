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
 * apr_global_storage.c
 *
 *  Created on: 16 Jun 2015
 *      Author: tyrrells
 */

#include "memory_allocations.h"
#include "apr_thread_mutex.h"
#include "typedefs.h"
#include "apr_global_storage.h"
#include "streams.h"
#include "string_utils.h"
#include "util_mutex.h"



#ifdef _DEBUG
#define APR_GLOBAL_STORAGE_DEBUG	(STM_LEVEL_FINEST)
#else
#define APR_GLOBAL_STORAGE_DEBUG	(STM_LEVEL_NONE)
#endif

static void *FindObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length, const bool remove_flag);

static char *GetKeyAsValidString (char *raw_key_p, unsigned int key_length, bool *alloc_key_flag);

static apr_status_t IterateOverSOCache (ap_socache_instance_t *instance,
    server_rec *s,
    void *userctx,
    const unsigned char *id,
    unsigned int idlen,
    const unsigned char *data,
    unsigned int datalen,
    apr_pool_t *pool);




APRGlobalStorage *AllocateAPRGlobalStorage (apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), void (*free_key_and_value_fn) (unsigned char *key_p, void *value_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s, const char *provider_name_s)
{
	APRGlobalStorage *store_p = (APRGlobalStorage *) AllocMemory (sizeof (APRGlobalStorage));

	if (store_p)
		{
			memset (store_p, 0, sizeof (APRGlobalStorage));

			if (InitAPRGlobalStorage (store_p, pool_p, hash_fn, make_key_fn, free_key_and_value_fn, server_p, mutex_filename_s, cache_id_s, provider_name_s))
				{
					return store_p;
				}

			FreeAPRGlobalStorage (store_p);
		}

	return NULL;
}


bool InitAPRGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), void (*free_key_and_value_fn) (unsigned char *key_p, void *value_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s, const char *provider_name_s)
{
	ap_socache_provider_t *provider_p = ap_lookup_provider (AP_SOCACHE_PROVIDER_GROUP, provider_name_s, AP_SOCACHE_PROVIDER_VERSION);

	if (provider_p)
		{
			apr_status_t status = apr_global_mutex_create (& (storage_p -> ags_mutex_p), mutex_filename_s, APR_THREAD_MUTEX_UNNESTED, pool_p);

			if (status == APR_SUCCESS)
				{
					storage_p -> ags_entries_p = apr_hash_make_custom (pool_p, hash_fn);

					if (storage_p -> ags_entries_p)
						{
							storage_p -> ags_pool_p = pool_p;
							storage_p -> ags_server_p = server_p;
							storage_p -> ags_make_key_fn = make_key_fn;
							storage_p -> ags_free_key_and_value_fn = free_key_and_value_fn;

							storage_p -> ags_largest_entry_size = 0;
							storage_p -> ags_cache_id_s = cache_id_s;
							storage_p -> ags_mutex_lock_filename_s = mutex_filename_s;

							storage_p -> ags_socache_instance_p = NULL;
							storage_p -> ags_socache_provider_p = provider_p;

							apr_pool_cleanup_register (pool_p, storage_p, (const void *) FreeAPRGlobalStorage, apr_pool_cleanup_null);

							return true;
						}

					apr_global_mutex_destroy (storage_p -> ags_mutex_p);
					storage_p -> ags_mutex_p = NULL;
				}		/* if (status == APR_SUCCESS) */


		}		/* if (provider_p) */

	return false;
}


apr_status_t FreeAPRGlobalStorage (void *data_p)
{
	APRGlobalStorage *storage_p = (APRGlobalStorage *) data_p;
	DestroyAPRGlobalStorage (storage_p);
	FreeMemory (storage_p);

	return APR_SUCCESS;
}


void DestroyAPRGlobalStorage (APRGlobalStorage *storage_p)
{
	if (storage_p)
		{
			if (storage_p -> ags_entries_p)
				{
					unsigned char *key_s = NULL;
					apr_ssize_t keylen = 0;
					void *value_p = NULL;
					apr_hash_index_t *index_p =	apr_hash_first (storage_p -> ags_pool_p, storage_p -> ags_entries_p);

					while (index_p)
						{
							apr_hash_this (index_p, (const void **) &key_s, &keylen, (void **) &value_p);

							storage_p -> ags_free_key_and_value_fn (key_s, value_p);

							index_p = apr_hash_next (index_p);
						}

					apr_hash_clear (storage_p -> ags_entries_p);
				}

			storage_p -> ags_pool_p = NULL;

			if (storage_p -> ags_mutex_p)
				{
					apr_global_mutex_destroy (storage_p -> ags_mutex_p);
					storage_p -> ags_mutex_p = NULL;
				}

			if (storage_p -> ags_socache_instance_p)
				{
					storage_p -> ags_socache_provider_p -> destroy (storage_p -> ags_socache_instance_p, storage_p -> ags_server_p);
				}
		}
}


unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p)
{
	unsigned int res = 0;
	char *uuid_s = GetUUIDAsString ((const uint8 *) key_s);

	if (uuid_s)
		{
			apr_ssize_t len = APR_HASH_KEY_STRING;
			res = apr_hashfunc_default (uuid_s, &len);

			#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "uuid \"%s\" res %u len %u", uuid_s, res, len);
			#endif

			FreeCopiedString (uuid_s);
		}

	return res;
}


unsigned char *MakeKeyFromUUID (const void *data_p, uint32 raw_key_length, uint32 *key_len_p)
{
	unsigned char *res_p = NULL;
	char *uuid_s = GetUUIDAsString ((const uint8 *) data_p);

	if (uuid_s)
		{
			res_p = (unsigned char *) uuid_s;
			*key_len_p = UUID_STRING_BUFFER_SIZE - 1;
		}

	#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "uuid \"%s\" len %u res \"%s\"", uuid_s, *key_len_p, res_p);
	#endif


	return res_p;
}


void PrintAPRGlobalStorage (APRGlobalStorage *storage_p)
{
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Begin iterating storage");

	storage_p -> ags_socache_provider_p -> iterate (storage_p -> ags_socache_instance_p,
                                                  storage_p -> ags_server_p,
                                                  NULL,
                                                  IterateOverSOCache,
                                                  storage_p -> ags_pool_p);

	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "End iterating storage");
}


static apr_status_t IterateOverSOCache (ap_socache_instance_t *instance,
    server_rec *s,
    void *userctx,
    const unsigned char *id,
    unsigned int idlen,
    const unsigned char *data,
    unsigned int datalen,
    apr_pool_t *pool)
{
	apr_status_t ret = OK;

	char *copied_id_s = CopyToNewString ((const char *) id, idlen, false);

	if (copied_id_s)
		{
			char *copied_data_s = CopyToNewString ((const char *) data, datalen, false);

			if (copied_data_s)
				{
					PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "id \"%s\" idlen %u data \"%s\" datalen %u  %X", copied_id_s, idlen, copied_data_s, datalen, data);

					FreeCopiedString (copied_data_s);
				}

			FreeCopiedString (copied_id_s);
		}

	return ret;
}


bool AddObjectToAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned char *value_p, unsigned int value_length)
{
	bool success_flag = false;
	unsigned int key_len = 0;
	unsigned char *key_p = storage_p -> ags_make_key_fn (raw_key_p, raw_key_length, &key_len);

	if (key_p)
		{
			bool alloc_key_flag = false;
			char *key_s = GetKeyAsValidString ((char *) key_p, key_len, &alloc_key_flag);
			apr_status_t status = apr_global_mutex_lock (storage_p -> ags_mutex_p);

			if (status == APR_SUCCESS)
				{
					apr_time_t end_of_time = APR_INT64_MAX;


					/* store it */
					status = storage_p -> ags_socache_provider_p -> store (storage_p -> ags_socache_instance_p,
																			 storage_p -> ags_server_p,
					                              key_p,
					                              key_len,
					                              end_of_time,
					                              value_p,
					                              value_length,
					                              storage_p -> ags_pool_p);

					if (status == APR_SUCCESS)
						{
							success_flag = true;

							if (storage_p -> ags_largest_entry_size < value_length)
								{
									storage_p -> ags_largest_entry_size = value_length;
								}

							#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINE
							PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Added \"%s\" length %u, value %.16X length %u to global store", key_s, key_len, value_p, value_length);
							#endif

						}
					else
						{
							PrintErrors (STM_LEVEL_FINE, __FILE__, __LINE__, "Failed to add \"%s\" length %u, value %.16X length %u to global store", key_s, key_len, value_p, value_length);
						}

					status = apr_global_mutex_unlock (storage_p -> ags_mutex_p);

					if (status != APR_SUCCESS)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to unlock mutex, status %s after adding %s", status, key_s);
						} /* if (status != APR_SUCCESS) */


				}		/* if (status == APR_SUCCESS) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to lock mutex, status %s to add %s", status, key_s);
				}

			if (alloc_key_flag)
				{
					FreeCopiedString (key_s);
				}

			FreeMemory (key_p);
		} /* if (key_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to make key");
		}

	#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINEST
	PrintAPRGlobalStorage (storage_p);
	#endif

	return success_flag;
}



void *GetObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length)
{
	return FindObjectFromAPRGlobalStorage (storage_p, raw_key_p, raw_key_length, value_length, false);
}


void *RemoveObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length)
{
	return FindObjectFromAPRGlobalStorage (storage_p, raw_key_p, raw_key_length, value_length, true);
}



static char *GetKeyAsValidString (char *raw_key_p, unsigned int key_length, bool *alloc_key_flag_p)
{
	char *key_s = NULL;

	if (* (raw_key_p + key_length) == '\0')
		{
			key_s = (char *) raw_key_p;
		}
	else
		{
			key_s = CopyToNewString ((const char * const) raw_key_p, key_length, false);

			if (key_s)
				{
					*alloc_key_flag_p = true;
				}
			else
				{
					key_s = "DUMMY KEY";
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to make key");
				}
		}

	#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Added \"%s\" from raw key of length %u", key_s, key_length);
	#endif

	return key_s;
}


static void *FindObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length, const bool remove_flag)
{
	void *result_p = NULL;
	unsigned int key_len = 0;
	unsigned char *key_p = storage_p -> ags_make_key_fn (raw_key_p, raw_key_length, &key_len);

	if (key_p)
		{
			apr_status_t status;
			bool alloc_key_flag = false;
			char *key_s = GetKeyAsValidString ((char *) key_p, key_len, &alloc_key_flag);

			#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINEST,  __FILE__, __LINE__,"Made key: %s", key_s);
			#endif


			status = apr_global_mutex_lock (storage_p -> ags_mutex_p);

			if (status == APR_SUCCESS)
				{
					unsigned char *temp_p = NULL;

					#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINEST
					PrintLog (STM_LEVEL_FINEST,  __FILE__, __LINE__,"Locked mutex");
					#endif

					/* We don't know how big the value might be so allocate the largest value that we've seen so far */
					temp_p = (unsigned char *) AllocMemoryArray (storage_p -> ags_largest_entry_size, sizeof (unsigned char));

					if (temp_p)
						{
							value_length = storage_p -> ags_largest_entry_size;

							/* get the value */
							status = storage_p -> ags_socache_provider_p -> retrieve (storage_p -> ags_socache_instance_p,
																					 storage_p -> ags_server_p,
							                              key_p,
							                              key_len,
							                              temp_p,
							                              &value_length,
							                              storage_p -> ags_pool_p);


							#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINEST
							PrintLog (STM_LEVEL_FINEST,  __FILE__, __LINE__,"status %d key %s length %u result_p %0.16X remove_flag %d", status, key_s, key_len, temp_p, remove_flag);
							#endif

							if (status == APR_SUCCESS)
								{
									result_p = temp_p;

									if (remove_flag == true)
										{
											status = storage_p -> ags_socache_provider_p -> remove (storage_p -> ags_socache_instance_p,
											                                                        storage_p -> ags_server_p,
											                                                        key_p,
											                                                        key_len,
											                                                        storage_p -> ags_pool_p);


											#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINEST
											PrintLog (STM_LEVEL_FINEST,  __FILE__, __LINE__,"status after removal %d", status);
											#endif
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE,  __FILE__, __LINE__,"status %d key %s result_p %0.16X remove_flag %d", status, key_s, temp_p, remove_flag);
									FreeMemory (temp_p);
								}

						}		/* if (temp_p) */

				}		/* if (status == APR_SUCCESS) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to lock mutex for %s, status %s", key_s, status);
				}


			status = apr_global_mutex_unlock (storage_p -> ags_mutex_p);

			if (status != APR_SUCCESS)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to unlock mutex for %s, status %s after finding %s", key_s, status);
				} /* if (status != APR_SUCCESS) */

			FreeMemory (key_p);

			if (alloc_key_flag)
				{
					FreeCopiedString (key_s);
				}
		} /* if (key_p) */

	#if APR_GLOBAL_STORAGE_DEBUG >= STM_LEVEL_FINEST
	PrintAPRGlobalStorage (storage_p);
	#endif


	return result_p;
}


bool IterateOverAPRGlobalStorage (APRGlobalStorage *storage_p, ap_socache_iterator_t *iterator_p, void *data_p)
{
	bool did_all_elements_flag = true;
	apr_status_t status = apr_global_mutex_lock (storage_p -> ags_mutex_p);

	if (status == APR_SUCCESS)
		{
			status = storage_p -> ags_socache_provider_p -> iterate (storage_p -> ags_socache_instance_p, storage_p -> ags_server_p, data_p, iterator_p, storage_p -> ags_pool_p);

			if (status != APR_SUCCESS)
				{
					did_all_elements_flag = false;
				}

			status = apr_global_mutex_unlock (storage_p -> ags_mutex_p);
		}		/* if (status == APR_SUCCESS) */


	return did_all_elements_flag;
}



bool PreConfigureGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *config_pool_p)
{
	bool success_flag = false;
	apr_status_t status = ap_mutex_register (config_pool_p, storage_p -> ags_cache_id_s, NULL, APR_LOCK_DEFAULT, 0);

	if (status == APR_SUCCESS)
		{
			storage_p -> ags_socache_provider_p = ap_lookup_provider (AP_SOCACHE_PROVIDER_GROUP, AP_SOCACHE_DEFAULT_PROVIDER, AP_SOCACHE_PROVIDER_VERSION);
			success_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "failed to register %s mutex", storage_p -> ags_cache_id_s);
		}

	return success_flag;
}


bool PostConfigureGlobalStorage  (APRGlobalStorage *storage_p, apr_pool_t *server_pool_p, server_rec *server_p, const char *provider_name_s, struct ap_socache_hints *cache_hints_p)
{
	apr_status_t res;
	bool success_flag = true;

	/*
	 * Have we got a valid set of config directives?
	 */
	if (storage_p -> ags_socache_provider_p)
		{
			/* We have socache_provider, but do not have socache_instance. This should
			 * happen only when using "default" socache_provider, so create default
			 * socache_instance in this case. */
			if (! (storage_p -> ags_socache_instance_p))
				{
					const char *err_msg_s = storage_p -> ags_socache_provider_p -> create (& (storage_p -> ags_socache_instance_p), NULL, server_pool_p, server_pool_p);

					if (err_msg_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "failed to create mod_socache_shmcb socache instance: %s", err_msg_s);
							success_flag = false;
						}
				}

			if (success_flag)
				{
					res = ap_global_mutex_create (& (storage_p -> ags_mutex_p), NULL, storage_p -> ags_cache_id_s, NULL, server_p, server_pool_p, 0);

					if (res == APR_SUCCESS)
						{
							res = storage_p -> ags_socache_provider_p -> init (storage_p -> ags_socache_instance_p, storage_p -> ags_cache_id_s, cache_hints_p, server_p, server_pool_p);

							if (res != APR_SUCCESS)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to initialise %s cache", storage_p -> ags_cache_id_s);
									success_flag = false;
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "failed to create %s mutex", storage_p -> ags_cache_id_s);
							success_flag = false;
						}
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Please select a socache provider with AuthnCacheSOCache (no default found on this platform). Maybe you need to load mod_socache_shmcb or another socache module first");
			success_flag = false;
		}


	return success_flag;
}


bool InitAPRGlobalStorageForChild (APRGlobalStorage *storage_p, apr_pool_t *pool_p)
{
	bool success_flag = false;

	/* Now that we are in a child process, we have to reconnect
	 * to the global mutex and the shared segment. We also
	 * have to find out the base address of the segment, in case
	 * it moved to a new address. */
	apr_status_t res = apr_global_mutex_child_init (& (storage_p -> ags_mutex_p), storage_p -> ags_mutex_lock_filename_s, pool_p);

	if (res != APR_SUCCESS)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to attach grassroots child to global mutex file '%s', res %d", storage_p -> ags_mutex_lock_filename_s, res);
		}
	else
		{
			success_flag = true;
		}

	return success_flag;
}
