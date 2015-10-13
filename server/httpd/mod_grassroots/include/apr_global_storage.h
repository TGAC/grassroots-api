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
 *
 * @see JobsManager
 */

typedef struct APRGlobalStorage
{
	/** @privatesection */
	apr_hash_t *ags_entries_p;

	/** Our cross-thread/cross-process mutex */
	apr_global_mutex_t *ags_mutex_p;

	/** The pool to use for any temporary memory allocations */
	apr_pool_t *ags_pool_p;

	const char *ags_mutex_lock_filename_s;

	const char *ags_cache_id_s;

	server_rec *ags_server_p;

	ap_socache_provider_t *ags_socache_provider_p;
	ap_socache_instance_t *ags_socache_instance_p;

	unsigned char *(*ags_make_key_fn) (const void *raw_key_p, uint32 raw_key_length, uint32 *key_len_p);

	void (*ags_free_key_and_value_fn) (unsigned char *key_p, void *value_p);

} APRGlobalStorage;



#ifdef __cplusplus
extern "C"
{
#endif

bool InitAPRGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), void (*free_key_and_value_fn) (unsigned char *key_p, void *value_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s, const char *provider_name_s);

APRGlobalStorage *AllocateAPRGlobalStorage (apr_pool_t *pool_p, apr_hashfunc_t hash_fn, unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p), void (*free_key_and_value_fn) (unsigned char *key_p, void *value_p), server_rec *server_p, const char *mutex_filename_s, const char *cache_id_s, const char *provider_name_s);

void FreeAPRGlobalStorage (APRGlobalStorage *storage_p);

void DestroyAPRGlobalStorage (APRGlobalStorage *storage_p);

unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p);

unsigned char *MakeKeyFromUUID (const void *data_p, uint32 raw_key_length, uint32 *key_len_p);

bool AddObjectToAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned char *value_p, unsigned int value_length);

void *GetObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length);

void *RemoveObjectFromAPRGlobalStorage (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length, unsigned int value_length);


bool IterateOverAPRGlobalStorage (APRGlobalStorage *storage_p, ap_socache_iterator_t *iterator_p, void *data_p);

bool InitAPRGlobalStorageForChild (APRGlobalStorage *storage_p, apr_pool_t *pool_p);

bool PreConfigureGlobalStorage (APRGlobalStorage *storage_p, apr_pool_t *config_pool_p);

bool PostConfigureGlobalStorage  (APRGlobalStorage *storage_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s, struct ap_socache_hints *cache_hints_p);

void PrintAPRGlobalStorage (APRGlobalStorage *storage_p);

#ifdef __cplusplus
}
#endif


#endif /* APR_GLOBAL_STORAGE_H_ */