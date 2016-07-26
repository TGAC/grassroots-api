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

/**
 * The APRJobsManager stores key value pairs. The keys are the uuids for
 * the ServiceJobs that are converted to strings. The values are the ServiceJob
 * pointers.
 */

/**************************/

static const char s_mutex_filename_s [] = "logs/grassroots_jobs_manager_lock";

/**************************/


static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob  *job_p);


static void FreeAPRServerJob (unsigned char *key_p, void *value_p);

static ServiceJob *QueryServiceJobFromAprJobsManager (JobsManager *jobs_manager_p,
																									const uuid_t job_key,
																									bool get_job_flag,
																									void *(*storage_callback_fn) (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length));


static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *manager_p, const uuid_t job_key);


static ServiceJob *RemoveServiceJobFromAprJobsManager (JobsManager *manager_p, const uuid_t job_key, bool get_job_flag);



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
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate APRGlobalStorage");
				}

			FreeMemory (manager_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for APRJobsManager");
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


static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob  *job_p)
{
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;
	bool success_flag = false;
	unsigned char *value_p = NULL;
	unsigned int value_length = 0;
	char uuid_s [UUID_STRING_BUFFER_SIZE];
	Service *service_p = job_p -> sj_service_p;

	json_t *job_json_p = NULL;

	ConvertUUIDToString (job_key, uuid_s);

	if (DoesServiceHaveCustomServiceJobSerialisation (service_p))
		{
			job_json_p = CreateSerialisedJSONForServiceJobFromService (service_p, job_p);

			if (!job_json_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create custom serialised job for %s from %s", uuid_s, GetServiceName (service_p));
				}
		}
	else
		{
			/* We store the c-style string for the ServiceJob's json */
			job_json_p = GetServiceJobAsJSON (job_p);

			if (!job_json_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create serialised job for %s from %s", uuid_s, GetServiceName (service_p));
				}
		}


	if (job_json_p)
		{
			char *job_s = json_dumps (job_json_p, JSON_INDENT (2));

			if (job_s)
				{
					/*
					 * include the terminating \0 to make sure
					 * the value as a valid c-style string
					 */
					value_length = strlen (job_s) + 1;
					value_p = (unsigned char *) job_s;

					#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
						{
							PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Adding \"%s\"=\"%s\"", uuid_s, value_p);
						}
					#endif

					success_flag = AddObjectToAPRGlobalStorage (manager_p -> ajm_store_p, job_key, UUID_RAW_SIZE, value_p, value_length);

					#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINER
						{
							PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Added \"%s\"=\"%s\", success=%d", uuid_s, value_p, success_flag);
						}
					#endif


					free (job_s);
				}		/* if (job_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json_dumps failed for \"%s\"", uuid_s);
				}

		}		/* if (job_json_p) */


	return success_flag;
}


static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *manager_p, const uuid_t job_key)
{
	return QueryServiceJobFromAprJobsManager (manager_p, job_key, true, GetObjectFromAPRGlobalStorage);
}



static ServiceJob *RemoveServiceJobFromAprJobsManager (JobsManager *manager_p, const uuid_t job_key, bool get_job_flag)
{
	return QueryServiceJobFromAprJobsManager (manager_p, job_key, get_job_flag, RemoveObjectFromAPRGlobalStorage);
}


static ServiceJob *QueryServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key, bool get_job_flag, void *(*storage_callback_fn) (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length))
{
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;
	ServiceJob *job_p = NULL;
	unsigned char *value_p = NULL;

	char uuid_s [UUID_STRING_BUFFER_SIZE];
	ConvertUUIDToString (job_key, uuid_s);

	#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Looking for %s", uuid_s);
	#endif

	value_p = storage_callback_fn (manager_p -> ajm_store_p, job_key, UUID_RAW_SIZE);

	if (value_p)
		{
			json_error_t err;
			json_t *job_json_p = NULL;

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "For job %s, got: \"%s\"", uuid_s, value_p);
			#endif

			job_json_p = json_loads ((char *) value_p, 0, &err);

			if (job_json_p)
				{
					job_p = CreateServiceJobFromJSON (job_json_p);

					if (job_p)
						{

						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "CreateServiceJobFromJSON failed for \"%s\"", uuid_s);
						}

				}		/* if (job_json_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to json for uuid \"%s\", err \"%s\" at line %d, column %d", value_p, uuid_s, err.text, err.line, err.column);
				}

			FreeMemory (value_p);
		}		/* if (value_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get stored value for \"%s\"", uuid_s);
		}

	return job_p;
}



static void FreeAPRServerJob (unsigned char *key_p, void *value_p)
{
	if (key_p)
		{
			FreeMemory (key_p);
		}

	if (value_p)
		{
			char *value_s = value_p;

			free (value_s);
		}
}



void APRServiceJobFinished (JobsManager *jobs_manager_p, uuid_t job_key)
{
	ServiceJob *job_p = RemoveServiceJobFromJobsManager (jobs_manager_p, job_key, true);

	if (job_p)
		{
		}
}



apr_status_t CleanUpAPRJobsManager (void *value_p)
{
	APRJobsManager *manager_p = (APRJobsManager *) value_p;

	return (DestroyAPRJobsManager (manager_p) ? APR_SUCCESS : APR_EGENERAL);
}



