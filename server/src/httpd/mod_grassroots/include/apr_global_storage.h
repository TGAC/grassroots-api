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
 * apr_global_storage.h
 *
 *  Created on: 16 Jun 2015
 *      Author: billy
 */

#ifndef APR_GLOBAL_STORAGE_H_
#define APR_GLOBAL_STORAGE_H_

#include "apr_hash.h"
#include "httpd.h"
#include "apr_global_mutex.h"
#include "ap_provider.h"
#include "ap_socache.h"

/**
 * @brief A datatype to manage all of the ServiceJobs on a Grassroots system running on Apache httpd.
 *
 * This datatype uses the Apache httpd shared object module functionality to store data that will be
 * needed for longer than a single request.
 *
 * @see APRJobsManager
 * @see APRServersManager
 */
typedef struct APRGlobalStorage
{
	/** @privatesection */
	apr_hash_t *ags_entries_p;

	/** Our cross-thread/cross-process mutex */
	apr_global_mutex_t *ags_mutex_p;

	/** The pool to use for any temporary memory allocations */
	apr_pool_t *ags_pool_p;

	/**
	 * The filename for the mutex lock which is used to
	 * control and coordinate access to this APRGlobalStorage.
	 */
	const char *ags_mutex_lock_filename_s;

	/**
	 * A user-friendly identifier to denote this APRGlobalStorage.
	 */
	const char *ags_cache_id_s;

	/**
	 * This points to the size of the largest entry that
	 * has been added to the storage. Since we are in a
	 * multi-process environment, this will be in shared
	 * memory
	 */
	int ags_largest_entry_memory_id;

	/** The httpd server. */
	server_rec *ags_server_p;

	/**
	 * The provider used for the shared object cache.
	 */
	ap_socache_provider_t *ags_socache_provider_p;

	/**
	 * The shared object cache instance.
	 */
	ap_socache_instance_t *ags_socache_instance_p;


	/**
	 * This function is used to take a pointer and
	 * construct a key that will be used to store a
	 * value in the shared object cache.
	 *
	 * @param raw_key_p The memory to construct the key from.
	 * @param raw_key_length The size in bytes of the raw key.
	 * @param key_len_p Upon successful construction of the key,
	 * its length will be stored in the memory pointed to by this.
	 * @return The constructed key or <code>NULL</code> upon error.
	 */
	unsigned char *(*ags_make_key_fn) (const void *raw_key_p, uint32 raw_key_length, uint32 *key_len_p);

	/**
	 * Callback function to free the memory used by a key-value pair
	 * within the shared object cache.
	 *
	 * @param key_p The key.
	 * @param value_p The value.
	 */
	void (*ags_free_key_and_value_fn) (unsigned char *key_p, void *value_p);

} APRGlobalStorage;



#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Initialise an APRGlobalStorage.
 *
 * @param storage_p The APRGlobalStorage to initialise.
 * @param pool_p The memory pool used for any allocations.
 * @param hash_fn The callback function used to determine the slot in the underlying shared object cache.
 * to put or get any items.
 * @param make_key_fn The callback function used to create the key that will be used by the hash function.
 * @param free_key_and_value_fn The callback function used to free an entry in the underlying shared object cache.
 * @param server_p The server_rec that will own the APRGlobalStorage.
 * @param mutex_filename_s The filename used to store the mutex variable governing access to the APRGlobalStorage.
 * @param cache_id_s The id used to identify this APRGlobalStorage.
 * @param provider_name_s The name of the shared object cache provider to use.
 * @return <code>true</code> if the initialisation was successful or <code>false</code> if there was a problem.
 * @memberof APRGlobalStorage
 */
bool InitAPRGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), void (*free_key_and_value_fn) (unsigned char *key_p, void *value_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s, const char *provider_name_s);


/**
 * Allocate an APRGlobalStorage.
 *
 * @param pool_p The memory pool used for any allocations.
 * @param hash_fn The callback function used to determine the slot in the underlying shared object cache.
 * to put or get any items.
 * @param make_key_fn The callback function used to create the key that will be used by the hash function.
 * @param free_key_and_value_fn The callback function used to free an entry in the underlying shared object cache.
 * @param server_p The server_rec that will own the APRGlobalStorage.
 * @param mutex_filename_s The filename used to store the mutex variable governing access to the APRGlobalStorage.
 * @param cache_id_s The id used to identify this APRGlobalStorage.
 * @param provider_name_s The name of the shared object cache provider to use.
 * @return The newly-allocated APRGlobalStorage or <code>NULL</code> upon error.
 * @see InitAPRGlobalStorage.
 * @see FreeAPRGlobalStorage.
 * @memberof APRGlobalStorage
 */
APRGlobalStorage *AllocateAPRGlobalStorage (apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), void (*free_key_and_value_fn) (unsigned char *key_p, void *value_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s, const char *provider_name_s);

/**
 * Free an APRGlobalStorage.
 *
 * This will simply call DestroyAPRGlobalStorage and exists
 * to match the function signature required for those that
 * can be triggered at APR memory pool cleanup.
 *
 * @param data_p The APRGlobalStorage that will get freed.
 * @return APR_OK upon success, any other code signifies an error.
 * @see DestroyAPRGlobalStorage.
 * @memberof APRGlobalStorage
 */
apr_status_t FreeAPRGlobalStorage (void *data_p);


/**
 * Free an APRGlobalStorage.
 *
 * @param storage_p The APRGlobalStorage to free.
 * @memberof APRGlobalStorage
 */
void DestroyAPRGlobalStorage (APRGlobalStorage *storage_p);


/**
 * Calculate a hash code for a given string.
 *
 * @param uuid_s The raw UUID data to calculate the hashed value for.
 * @param len_p The length of the data.
 * @return The hash value.
 */
unsigned int HashUUIDForAPR (const char *uuid_s, apr_ssize_t *len_p);


/**
 * Get a string representation of a raw UUID value.
 *
 * @param data_p Pointer to the raw UUID value.
 * @param raw_key_length The length of the raw value given by data_p.
 * @param key_len_p POinter to where the length of the returned string
 * will be stored.
 * @return The string representation of the UUID value or <code>NULL</code>
 * upon error.
 */
unsigned char *MakeKeyFromUUID (const void *data_p, uint32 raw_key_length, uint32 *key_len_p);


/**
 * Add an object to an APRGlobalStorage.
 *
 * @param storage_p The APRGlobalStorage to search in.
 * @param raw_key_p The raw key for the object that is being searched for.
 * @param raw_key_length The size in bytes of the raw key.
 * @param value_p The value to copy to store in the APRGlobalStorage. It wll be mapped to
 * the given key.
 * @param value_length The size in bytes of the value.
 * @return The matched value or <code>NULL</code> if the key is not in the APRGlobalStorage.
 * @memberof APRGlobalStorage
 */
bool AddObjectToAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned char *value_p, unsigned int value_length);


/**
 * Get an object from an APRGlobalStorage.
 *
 * @param storage_p The APRGlobalStorage to search in.
 * @param raw_key_p The raw key for the object that is being searched for.
 * @param raw_key_length The size in bytes of the raw key.
 * @return The matched value or <code>NULL</code> if the key is not in the APRGlobalStorage.
 * @memberof APRGlobalStorage
 */
void *GetObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length);


/**
 * Remove an object from an APRGlobalStorage.
 *
 * @param storage_p The APRGlobalStorage to search in.
 * @param raw_key_p The raw key for the object that is being searched for.
 * @param raw_key_length The size in bytes of the raw key.
 * @return The matched value or <code>NULL</code> if the key is not in the APRGlobalStorage. If found,
 * the value will be removed from the APRGlobalStorage.
 * @memberof APRGlobalStorage
 */
void *RemoveObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length);


/**
 * Iterate over the data stored within an APRGlobalStorage.
 *
 * @param storage_p The APRGlobalStorage to iterate over.
 * @param iterator_p The iterator to use.
 * @param data_p An opitional custom data pointer that the iterator might want to use. This can be <code>NULL</code>.
 * @return <code>true</code> if the iteration was successful or <code>false</code> if there was a problem.
 * @memberof APRGlobalStorage
 */
bool IterateOverAPRGlobalStorage (APRGlobalStorage *storage_p, ap_socache_iterator_t *iterator_p, void *data_p);


/**
 * Initialise an APRGlobalStorage for usage in an Apache child process.
 *
 * @param storage_p The APRGlobalStorage to initialise.
 * @param pool_p A memory pool to use if needed.
 * @return <code>true</code> if the initialisation was successful or <code>false</code> if there was a problem.
 * @memberof APRGlobalStorage
 */
bool InitAPRGlobalStorageForChild (APRGlobalStorage *storage_p, apr_pool_t *pool_p);


/**
 * Set up an APRGlobalStorage prior to the configuration details being used.
 *
 * @param storage_p The APRGlobalStorage to initialise.
 * @param config_pool_p A memory pool to use if needed.
 * @return <code>true</code> if the initialisation was successful or <code>false</code> if there was a problem.
 * @memberof APRGlobalStorage
 */
bool PreConfigureGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *config_pool_p);


/**
 * Configure an APRGlobalStorage in the Apache parent process before any child processes
 * are launched.
 *
 * @param storage_p The APRGlobalStorage to configure.
 * @param config_pool_p The memory pool available to use.
 * @param server_p The Apache server structure.
 * @param provider_name_s The name of sharded object cache provider to use.
 * @param cache_hints_p A set of hints which may be used when initialising the shared object cache. Providers may ignore some or all of these hints.
 * @return <code>true</code> if successful or <code>false</code> if there was a problem.
 * @memberof APRGlobalStorage
 */
bool PostConfigureGlobalStorage  (APRGlobalStorage *storage_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s, struct ap_socache_hints *cache_hints_p);


/**
 * Print the contents of an APRGlobalStorage object to the log OUtputStream.
 *
 * @param storage_p The APRGlobalStorage to print.
 * @memberof APRGlobalStorage
 */
void PrintAPRGlobalStorage (APRGlobalStorage *storage_p);

#ifdef __cplusplus
}
#endif


#endif /* APR_GLOBAL_STORAGE_H_ */
