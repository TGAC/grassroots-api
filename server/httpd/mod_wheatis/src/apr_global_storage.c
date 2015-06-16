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

APRGlobalStorage *AllocateAPRGlobalStorage (void)
{
	APRGlobalStorage *store_p = (APRGlobalStorage *) AllocMemory (sizeof (APRGlobalStorage));

	if (store_p)
		{
			if (InitAPRGlobalStorage (store_p))
				{
					return store_p;
				}

			FreeAPRGlobalStorage (store_p);
		}

	return NULL;
}


bool InitAPRGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *pool_p, apr_hashfunc_t hash_fn, server_rec *server_p, const char *mutex_filename_s)
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

					apr_pool_cleanup_register (pool_p, storage_p, FreeAPRGlobalStorage, apr_pool_cleanup_null);

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
					ExternalServer *server_p = NULL;

					apr_hash_this (index_p, (const void **) &key_s, &keylen, (void **) &server_p);
					if (server_p)
						{
							FreeExternalServer (server_p);
						}

					index_p = apr_hash_next (index_p);
				}

			apr_hash_clear (storage_p -> ags_pool_p);
			storage_p -> ags_pool_p = NULL;

			apr_global_mutex_destroy (storage_p -> ags_mutex_p);
			storage_p -> ags_mutex_p = NULL;


			if (storage_p -> ags_socache_instance_p)
				{
					storage_p -> ags_socache_provider_p -> destroy (storage_p -> ags_socache_instance_p, storage_p -> ags_server_p);
				}
		}
}

