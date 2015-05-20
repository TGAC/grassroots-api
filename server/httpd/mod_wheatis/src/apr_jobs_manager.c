/*
 * apr_jobs_manager.c
 *
 *  Created on: 18 May 2015
 *      Author: tyrrells
 */

#include "apr_hash.h"

#include "jobs_manager.h"
#include "apr_jobs_manager.h"

#include "service_job.h"
#include "mod_wheatis_config.h"
#include "string_utils.h"

#ifdef _DEBUG
	#define APR_JOBS_MANAGER_DEBUG	(STM_LEVEL_FINEST)
#else
	#define APR_JOBS_MANAGER_DEBUG	(STM_LEVEL_NONE)
#endif

/**************************/

typedef struct APRJobsManagerConfig
{
	apr_hash_t *ajmc_running_jobs_p;
	apr_thread_mutex_t *ajmc_mutex_p;
	apr_pool_t *ajmc_pool_p;
} APRJobsManagerConfig;


static APRJobsManagerConfig *s_config_p = NULL;

/**************************/

static apr_status_t CleanUpAPRJobsManagerConfig (void *value_p);

static unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p);

static void *APRAllocMemory (size_t l);

static void DebugJobsManager (apr_hash_t *table_p);

static char *GetUUIDAsStringCustom (const uuid_t id, void *(*alloc_fn) (size_t size));

/**************************/

bool InitAPRJobsManager (apr_pool_t *server_pool_p)
{
	if (!s_config_p)
		{
			s_config_p = (APRJobsManagerConfig *) AllocMemory (sizeof (APRJobsManagerConfig));

			if (s_config_p)
				{
					bool success_flag = false;

					apr_status_t status = apr_thread_mutex_create (& (s_config_p -> ajmc_mutex_p), APR_THREAD_MUTEX_UNNESTED, server_pool_p);

					if (status == APR_SUCCESS)
						{
							s_config_p -> ajmc_running_jobs_p = apr_hash_make_custom (server_pool_p, HashUUIDForAPR);

							if (s_config_p -> ajmc_running_jobs_p)
								{
									s_config_p -> ajmc_pool_p = server_pool_p;

									apr_pool_cleanup_register (server_pool_p, s_config_p, CleanUpAPRJobsManagerConfig, apr_pool_cleanup_null);

									success_flag = true;
								}
							else
								{
									apr_thread_mutex_destroy (s_config_p -> ajmc_mutex_p);
								}
						}

					if (!success_flag)
						{
							FreeMemory (s_config_p);
							s_config_p = NULL;
						}
				}
		}

	return ((s_config_p != NULL) ? true : false);
}


bool DestroyAPRJobsManager (void)
{
	if (s_config_p)
		{
			apr_hash_index_t *index_p =  apr_hash_first (s_config_p -> ajmc_pool_p, s_config_p -> ajmc_running_jobs_p);
			char *key_s = NULL;
			apr_ssize_t keylen = 0;

			while (index_p)
				{
					ServiceJob *job_p = NULL;

					apr_hash_this (index_p, (const void **) &key_s, &keylen, (void **) &job_p);
					if (job_p)
						{
							//FreeServiceJob (job_p);
						}

					index_p = apr_hash_next (index_p);
				}

			apr_hash_clear (s_config_p -> ajmc_running_jobs_p);
			s_config_p -> ajmc_running_jobs_p = NULL;
		}

	return true;
}


bool AddServiceJobToJobsManager (uuid_t job_key, ServiceJob *job_p)
{
	bool success_flag = false;

	apr_status_t status = apr_thread_mutex_lock (s_config_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			apr_hash_set (s_config_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE, job_p);

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
				{
					char *uuid_s = GetUUIDAsString (job_key);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, "Added %s to jobs manager\n", uuid_s);
							FreeCopiedString (uuid_s);
						}
				}
			#endif

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (s_config_p -> ajmc_running_jobs_p);
			#endif


			status = apr_thread_mutex_unlock (s_config_p -> ajmc_mutex_p);
			success_flag = true;

			if (status != APR_SUCCESS)
				{

				}		/* if (status != APR_SUCCESS) */

		}		/* if (status == APR_SUCCESS) */

	return success_flag;
}



ServiceJob *GetServiceJobFromJobsManager (const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	apr_status_t status = apr_thread_mutex_lock (s_config_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			job_p = (ServiceJob *) apr_hash_get (s_config_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE);

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
				{
					char *uuid_s = GetUUIDAsString (job_key);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, "Getting %s from jobs manager gave %x\n", uuid_s, job_p);
							FreeCopiedString (uuid_s);
						}
				}
			#endif

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (s_config_p -> ajmc_running_jobs_p);
			#endif

			status = apr_thread_mutex_unlock (s_config_p -> ajmc_mutex_p);

			if (status != APR_SUCCESS)
				{

				}		/* if (status != APR_SUCCESS) */

		}		/* if (status == APR_SUCCESS) */


	return job_p;
}


ServiceJob *RemoveServiceJobFromJobsManager (const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	apr_status_t status = apr_thread_mutex_lock (s_config_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			job_p = (ServiceJob *) apr_hash_get (s_config_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE);

			if (job_p)
				{
					/* remove the entry from the hash table */
					apr_hash_set (s_config_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE, NULL);
				}

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
				{
					char *uuid_s = GetUUIDAsString (job_key);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, "removed %s to jobs manager\n", uuid_s);
							FreeCopiedString (uuid_s);
						}
				}
			#endif

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (s_config_p -> ajmc_running_jobs_p);
			#endif

			status = apr_thread_mutex_unlock (s_config_p -> ajmc_mutex_p);

			if (status != APR_SUCCESS)
				{

				}		/* if (status != APR_SUCCESS) */

		}		/* if (status == APR_SUCCESS) */


	return job_p;
}


void ServiceJobFinished (uuid_t job_key)
{
	ServiceJob *job_p = RemoveServiceJobFromJobsManager (job_key);

	if (job_p)
		{
		}
}


static char *GetUUIDAsStringCustom (const uuid_t id, void *(*alloc_fn) (size_t size))
{
	char *uuid_s = (char *) alloc_fn (UUID_STRING_BUFFER_SIZE * sizeof (char));

	if (uuid_s)
		{
			ConvertUUIDToString (id, uuid_s);
		}

	return uuid_s;
}


static void *APRAllocMemory (size_t l)
{
	void *mem_p = NULL;

	if (s_config_p -> ajmc_pool_p)
		{
			return apr_palloc (s_config_p -> ajmc_pool_p, l);
		}

	return mem_p;
}


static unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p)
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


static apr_status_t CleanUpAPRJobsManagerConfig (void *value_p)
{
	return (DestroyAPRJobsManager () ? APR_SUCCESS : APR_EGENERAL);
}


static void DebugJobsManager (apr_hash_t *table_p)
{
	apr_hash_index_t *index_p;
	uint32 size = apr_hash_count (table_p);

	PrintLog (STM_LEVEL_FINE, "Jobs manager size %lu\n", size);

	for (index_p = apr_hash_first (s_config_p -> ajmc_pool_p, table_p); index_p; index_p = apr_hash_next (index_p))
		{
			const char *key_s = (const char *) apr_hash_this_key (index_p);
			PrintLog (STM_LEVEL_FINE, "key %s\n", key_s);
		}
}
