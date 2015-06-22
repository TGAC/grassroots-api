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


static void *FindObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length, const bool remove_flag);


APRGlobalStorage *AllocateAPRGlobalStorage (apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s)
{
	APRGlobalStorage *store_p = (APRGlobalStorage *) AllocMemory (sizeof (APRGlobalStorage));

	if (store_p)
		{
			if (InitAPRGlobalStorage (store_p, pool_p, hash_fn, make_key_fn, server_p, mutex_filename_s, cache_id_s))
				{
					return store_p;
				}

			FreeAPRGlobalStorage (store_p);
		}

	return NULL;
}

bool InitAPRGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s)
{
	bool success_flag = false;
	apr_status_t status = apr_global_mutex_create (& (storage_p -> ags_mutex_p), mutex_filename_s, APR_THREAD_MUTEX_UNNESTED, pool_p);

	if (status == APR_SUCCESS)
		{
			storage_p -> ags_entries_p = apr_hash_make_custom (pool_p, hash_fn);

			if (storage_p -> ags_entries_p)
				{
					storage_p -> ags_pool_p = pool_p;
					storage_p -> ags_server_p = server_p;
					storage_p -> ags_make_key_fn = make_key_fn;

					storage_p -> ags_cache_id_s = cache_id_s;
					storage_p -> ags_mutex_lock_filename_s = mutex_filename_s;

					storage_p -> ags_socache_instance_p = NULL;
					storage_p -> ags_socache_provider_p = NULL;

					apr_pool_cleanup_register (pool_p, storage_p, (const void *) FreeAPRGlobalStorage, apr_pool_cleanup_null);

					success_flag = true;
				}
			else
				{
					apr_global_mutex_destroy (storage_p -> ags_mutex_p);
					storage_p -> ags_mutex_p = NULL;
				}
		}

	return success_flag;
}


void FreeAPRGlobalStorage (APRGlobalStorage *storage_p)
{
	DestroyAPRGlobalStorage (storage_p);
	FreeMemory (storage_p);
}


void DestroyAPRGlobalStorage (APRGlobalStorage *storage_p)
{
	if (storage_p)
		{
			apr_hash_index_t *index_p =	apr_hash_first (storage_p -> ags_pool_p, storage_p -> ags_entries_p);
			char *key_s = NULL;
			apr_ssize_t keylen = 0;

			while (index_p)
				{
//					ExternalServer *server_p = NULL;

//					apr_hash_this (index_p, (const void **) &key_s, &keylen, (void **) &server_p);
//					if (server_p)
//						{
//							FreeExternalServer (server_p);
//						}

					index_p = apr_hash_next (index_p);
				}

			apr_hash_clear (storage_p -> ags_entries_p);
			storage_p -> ags_pool_p = NULL;

			apr_global_mutex_destroy (storage_p -> ags_mutex_p);
			storage_p -> ags_mutex_p = NULL;


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
			*key_len_p = UUID_STRING_BUFFER_SIZE;
		}

	return res_p;
}




bool AddObjectToAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned char *value_p, unsigned int value_length)
{
	bool success_flag = false;
	unsigned int key_len = 0;
	unsigned char *key_p = storage_p -> ags_make_key_fn (raw_key_p, raw_key_length, &key_len);

	if (key_p)
		{
			apr_status_t status = apr_global_mutex_lock (storage_p -> ags_mutex_p);

			if (status == APR_SUCCESS)
				{
					apr_time_t end_of_time = ~0;

					/* store it */
					status = storage_p -> ags_socache_provider_p -> store (storage_p -> ags_socache_instance_p,
																			 storage_p -> ags_server_p,
					                              key_p,
					                              key_len,
					                              end_of_time,
					                              value_p,
					                              value_length,
					                              storage_p -> ags_pool_p);


					#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
					PrintLog (STM_LEVEL_FINE, "Added %s to global store", key_p);
					#endif
				}		/* if (status == APR_SUCCESS) */
			else
				{

				}


			status = apr_global_mutex_unlock (storage_p -> ags_mutex_p);
			success_flag = true;

			if (status != APR_SUCCESS)
				{

				} /* if (status != APR_SUCCESS) */

			FreeMemory (key_p);
		} /* if (key_p) */

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



static void *FindObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length, const bool remove_flag)
{
	void *result_p = NULL;
	unsigned int key_len = 0;
	unsigned char *key_p = storage_p -> ags_make_key_fn (raw_key_p, raw_key_length, &key_len);

	if (key_p)
		{
			apr_status_t status = apr_global_mutex_lock (storage_p -> ags_mutex_p);

			if (status == APR_SUCCESS)
				{

					/* get the value */
					status = storage_p -> ags_socache_provider_p -> retrieve (storage_p -> ags_socache_instance_p,
																			 storage_p -> ags_server_p,
					                              key_p,
					                              key_len,
					                              result_p,
					                              &value_length,
					                              storage_p -> ags_pool_p);

					if ((status == APR_SUCCESS) && (result_p != NULL) && (remove_flag == true))
						{
							status = storage_p -> ags_socache_provider_p -> remove (storage_p -> ags_socache_instance_p,
							                                                        storage_p -> ags_server_p,
							                                                        key_p,
							                                                        key_len,
							                                                        storage_p -> ags_pool_p);
						}

					#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
					PrintLog (STM_LEVEL_FINE, "Got %s from global store %X\n", key_p, result_p);
					#endif
				}		/* if (status == APR_SUCCESS) */
			else
				{

				}


			status = apr_global_mutex_unlock (storage_p -> ags_mutex_p);

			if (status != APR_SUCCESS)
				{

				} /* if (status != APR_SUCCESS) */

			FreeMemory (key_p);
		} /* if (key_p) */

	return result_p;
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
			PrintErrors (STM_LEVEL_SEVERE, "failed to register %s mutex", storage_p -> ags_cache_id_s);
		}

	return success_flag;
}


bool PostConfigureGlobalStorage  (APRGlobalStorage *storage_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s, struct ap_socache_hints *cache_hints_p)
{
	/* Set up the maximum expiry time as we never want it to expire */
	apr_interval_time_t expiry = 0;
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
					const char *err_msg_s = storage_p -> ags_socache_provider_p -> create (& (storage_p -> ags_socache_instance_p), NULL, config_pool_p, config_pool_p);

					if (err_msg_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, "failed to create mod_socache_shmcb socache instance: %s", err_msg_s);
							success_flag = false;
						}
				}

			if (success_flag)
				{
					res = ap_global_mutex_create (& (storage_p -> ags_mutex_p), NULL, storage_p -> ags_cache_id_s, NULL, server_p, config_pool_p, 0);

					if (res != APR_SUCCESS)
						{
							PrintErrors (STM_LEVEL_SEVERE, "failed to create %s mutex", storage_p -> ags_cache_id_s);
							success_flag = false;
						}

					res = storage_p -> ags_socache_provider_p -> init (storage_p -> ags_socache_instance_p, storage_p -> ags_cache_id_s, cache_hints_p, server_p, config_pool_p);
					if (res != APR_SUCCESS)
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to initialise %s cache", storage_p -> ags_cache_id_s);
							success_flag = false;
						}
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Please select a socache provider with AuthnCacheSOCache (no default found on this platform). Maybe you need to load mod_socache_shmcb or another socache module first");
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
			PrintErrors (STM_LEVEL_SEVERE, "Failed to attach wheatis child to global mutex file '%s', res %d", storage_p -> ags_mutex_lock_filename_s, res);
		}
	else
		{
			success_flag = true;
		}

	return success_flag;
}
