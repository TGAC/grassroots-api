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
 * apr_jobs_manager.c
 *
 *  Created on: 18 May 2015
 *      Author: tyrrells
 */

#include <limits.h>

#include "apr_hash.h"

#include "jobs_manager.h"

#define ALLOCATE_APR_JOBS_MANAGER_TAGS (1)
#include "apr_jobs_manager.h"

#include "service_job.h"
#include "mod_grassroots_config.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "util_mutex.h"

#ifdef _DEBUG
#define APR_JOBS_MANAGER_DEBUG	(STM_LEVEL_FINEST)
#else
#define APR_JOBS_MANAGER_DEBUG	(STM_LEVEL_NONE)
#endif

/**************************/

static const char s_mutex_filename_s [] = "logs/grassroots_jobs_manager_lock";

/**************************/



static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob *job_p);

static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key);

static ServiceJob *RemoveServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key);


static void FreeAPRServerJob (unsigned char *key_p, void *value_p);


/**************************/


APRJobsManager *InitAPRJobsManager (server_rec *server_p, apr_pool_t *pool_p, const char *provider_name_s)
{
	APRJobsManager *manager_p = (APRJobsManager *) AllocMemory (sizeof (APRJobsManager));

	if (manager_p)
		{
			unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p) = MakeKeyFromUUID;

			APRGlobalStorage *storage_p = AllocateAPRGlobalStorage (pool_p,
																															HashUUIDForAPR,
																															make_key_fn,
																															FreeAPRServerJob,
																															server_p,
																															s_mutex_filename_s,
																															APR_JOBS_MANAGER_CACHE_ID_S,
																															provider_name_s);


			if (storage_p)
				{
					manager_p -> ajm_store_p = storage_p;

					InitJobsManager (& (manager_p -> ajm_base_manager), AddServiceJobToAPRJobsManager, GetServiceJobFromAprJobsManager, RemoveServiceJobFromAprJobsManager);

					apr_pool_cleanup_register (pool_p, manager_p, CleanUpAPRJobsManager, apr_pool_cleanup_null);

					return manager_p;
				}

			FreeMemory (manager_p);
		}


	return NULL;
}


bool DestroyAPRJobsManager (APRJobsManager *jobs_manager_p)
{
	if (jobs_manager_p)
		{
			FreeMemory (jobs_manager_p);
		}

	return true;
}


bool APRJobsManagerPreConfigure (APRJobsManager *manager_p, apr_pool_t *config_pool_p)
{
	manager_p -> ajm_store_p -> ags_cache_id_s = APR_JOBS_MANAGER_CACHE_ID_S;

	return PreConfigureGlobalStorage (manager_p -> ajm_store_p, config_pool_p);
}



bool PostConfigAPRJobsManager (APRJobsManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s)
{
	/* Set up the maximum expiry time as we never want it to expire */
	apr_interval_time_t expiry = 0;
	struct ap_socache_hints job_cache_hints = { UUID_STRING_BUFFER_SIZE, sizeof (ServiceJob *), expiry };

	return PostConfigureGlobalStorage(manager_p -> ajm_store_p, config_pool_p, server_p, provider_name_s, &job_cache_hints);
}



bool APRJobsManagerChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
	ModGrassrootsConfig *config_p = ap_get_module_config (server_p -> module_config, GetGrassrootsModule ());
	bool success_flag = InitAPRGlobalStorageForChild (config_p -> wisc_jobs_manager_p -> ajm_store_p, pool_p);

	return success_flag;
}


static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob *job_p)
{
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;
	unsigned int object_size = sizeof (ServiceJob);
	bool success_flag = AddObjectToAPRGlobalStorage (manager_p -> ajm_store_p, job_key, UUID_RAW_SIZE, (unsigned char *) job_p, object_size);

	return success_flag;
}


static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key)
{
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;

	return ((ServiceJob *) GetObjectFromAPRGlobalStorage (manager_p -> ajm_store_p, job_key, UUID_RAW_SIZE, sizeof (ServiceJob)));
}


static ServiceJob *RemoveServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key)
{
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;

	return ((ServiceJob *) RemoveObjectFromAPRGlobalStorage (manager_p -> ajm_store_p, job_key, UUID_RAW_SIZE, sizeof (ServiceJob)));
}


static void FreeAPRServerJob (unsigned char *key_p, void *value_p)
{
	if (key_p)
		{
			FreeMemory (key_p);
		}

	if (value_p)
		{
			 ServiceJob *job_p = (ServiceJob *) value_p;
			 ClearServiceJob (job_p);
		}
}



void APRServiceJobFinished (JobsManager *jobs_manager_p, uuid_t job_key)
{
	ServiceJob *job_p = RemoveServiceJobFromJobsManager (jobs_manager_p, job_key);

	if (job_p)
		{
		}
}



apr_status_t CleanUpAPRJobsManager (void *value_p)
{
	APRJobsManager *manager_p = (APRJobsManager *) value_p;

	return (DestroyAPRJobsManager (manager_p) ? APR_SUCCESS : APR_EGENERAL);
}



