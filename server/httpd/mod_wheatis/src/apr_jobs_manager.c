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


static const char s_mutex_filename_s [] = "logs/wheatis_lock";

/**************************/

static apr_status_t CleanUpAPRJobsManagerConfig (void *value_p);

static unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p);

static void DebugJobsManager (APRJobsManager *manager_p);

static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob *job_p);

static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key);

static ServiceJob *RemoveServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key);


/**************************/

APRJobsManager *InitAPRJobsManager (apr_pool_t *pool_p)
{
	APRJobsManager manager_p = (APRJobsManager *) AllocMemory (sizeof (APRJobsManager));

	if (manager_p)
		{
			bool success_flag = false;

			InitJobsManager (& (manager_p -> ajmc_base_manager), AddServiceJobToAPRJobsManager, GetServiceJobFromAprJobsManager, RemoveServiceJobFromAprJobsManager);

			apr_status_t status = apr_global_mutex_create (& (manager_p -> ajmc_mutex_p), s_mutex_filename_s, APR_THREAD_MUTEX_UNNESTED, pool_p);

			if (status == APR_SUCCESS)
				{
					manager_p -> ajmc_running_jobs_p = apr_hash_make_custom (pool_p, HashUUIDForAPR);

					if (manager_p -> ajmc_running_jobs_p)
						{
							manager_p -> ajmc_pool_p = pool_p;

							apr_pool_cleanup_register (pool_p, manager_p, CleanUpAPRJobsManagerConfig, apr_pool_cleanup_null);

							success_flag = true;
						}
					else
						{
							apr_global_mutex_destroy (manager_p -> ajmc_mutex_p);
							manager_p -> ajmc_mutex_p = NULL;
						}
				}

			if (!success_flag)
				{
					FreeMemory (manager_p);
					manager_p = NULL;
				}
		}


	return manager_p;
}


bool DestroyAPRJobsManager (APRJobsManager *jobs_manager_p)
{
	if (jobs_manager_p)
		{
			apr_hash_index_t *index_p =  apr_hash_first (jobs_manager_p -> ajmc_pool_p, jobs_manager_p -> ajmc_running_jobs_p);
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

			apr_hash_clear (jobs_manager_p -> ajmc_running_jobs_p);
			jobs_manager_p -> ajmc_running_jobs_p = NULL;


			apr_global_mutex_destroy (jobs_manager_p -> ajmc_mutex_p);
			jobs_manager_p -> ajmc_mutex_p = NULL;

			FreeMemory (jobs_manager_p);
		}

	return true;
}


static bool WheatISChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
	ModWheatISConfig *config_p = ap_get_module_config (server_p -> module_config, &wheatis_module);

  /* Now that we are in a child process, we have to reconnect
   * to the global mutex and the shared segment. We also
   * have to find out the base address of the segment, in case
   * it moved to a new address. */
	apr_status_t res = apr_global_mutex_child_init (& (config_p -> wisc_jobs_manager_p), s_mutex_filename_s, pool_p);

	if (res != APR_SUCCESS)
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to attach to wheatis global mutex file '%s', res %d", s_mutex_filename_s, res);
			return false;
    }

    /* We only need to attach to the segment if we didn't inherit
     * it from the parent process (ie. Windows) */
    if (!scfg->counters_shm) {
        rv = apr_shm_attach(&scfg->counters_shm, scfg->shmcounterfile, p);
        if (rv != APR_SUCCESS) {
            ap_log_error(APLOG_MARK, APLOG_CRIT, rv, s, "Failed to attach to "
                         "mod_shm_counter shared memory file '%s'",
                         scfg->shmcounterfile ?
                             /* Just in case the file was NULL. */
                             scfg->shmcounterfile : "NULL");
            return;
        }
    }

    scfg->counters = apr_shm_baseaddr_get(scfg->counters_shm);
}



static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob *job_p)
{
	bool success_flag = false;
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;

	apr_status_t status = apr_global_mutex_lock (manager_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			apr_hash_set (manager_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE, job_p);

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
				{
					char *uuid_s = GetUUIDAsString (job_key);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, "Added %s to jobs manager", uuid_s);
							FreeCopiedString (uuid_s);
						}
				}
			#endif

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p -> ajmc_running_jobs_p);
			#endif


			status = apr_global_mutex_unlock (manager_p -> ajmc_mutex_p);
			success_flag = true;

			if (status != APR_SUCCESS)
				{

				}		/* if (status != APR_SUCCESS) */

		}		/* if (status == APR_SUCCESS) */

	return success_flag;
}



static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;

	apr_status_t status = apr_global_mutex_lock (manager_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			job_p = (ServiceJob *) apr_hash_get (manager_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE);

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
				{
					char *uuid_s = GetUUIDAsString (job_key);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, "Getting %s from jobs manager gave %x", uuid_s, job_p);
							FreeCopiedString (uuid_s);
						}
				}
			#endif

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p -> ajmc_running_jobs_p);
			#endif

			status = apr_global_mutex_unlock (manager_p -> ajmc_mutex_p);

			if (status != APR_SUCCESS)
				{

				}		/* if (status != APR_SUCCESS) */

		}		/* if (status == APR_SUCCESS) */


	return job_p;
}


static ServiceJob *RemoveServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;

	apr_status_t status = apr_global_mutex_lock (manager_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			job_p = (ServiceJob *) apr_hash_get (manager_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE);

			if (job_p)
				{
					/* remove the entry from the hash table */
					apr_hash_set (manager_p -> ajmc_running_jobs_p, job_key, UUID_RAW_SIZE, NULL);
				}

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
				{
					char *uuid_s = GetUUIDAsString (job_key);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, "Removed %s from jobs manager", uuid_s);
							FreeCopiedString (uuid_s);
						}
				}
			#endif

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p -> ajmc_running_jobs_p);
			#endif

			status = apr_global_mutex_unlock (manager_p -> ajmc_mutex_p);

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


static void DebugJobsManager (APRJobsManager *manager_p)
{
	apr_hash_index_t *index_p;
	uint32 size = apr_hash_count (manager_p -> ajmc_running_jobs_p);

	PrintLog (STM_LEVEL_FINE, "Jobs manager %x size %lu", manager_p -> ajmc_running_jobs_p, size);

	for (index_p = apr_hash_first (manager_p -> ajmc_pool_p, manager_p -> ajmc_running_jobs_p); index_p; index_p = apr_hash_next (index_p))
		{
			const uuid_t *key_p = NULL;
			ServiceJob *job_p = NULL;
			apr_ssize_t key_length;

			apr_hash_this (index_p, (const void **) &key_p, &key_length, (void **) &job_p);

			if (key_p)
				{
					char *uuid_s = GetUUIDAsString (*key_p);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, "key %s", uuid_s);
							FreeCopiedString (uuid_s);
						}
				}

			if (job_p)
				{
					const char *service_name_s = NULL;

					if (job_p -> sj_service_p)
						{
							service_name_s = GetServiceName (job_p -> sj_service_p);
						}

					PrintLog (STM_LEVEL_FINE, "job name %s service %s", job_p -> sj_name_s, service_name_s);
				}
			else
				{
					PrintLog (STM_LEVEL_FINE, "No job\n");
				}
		}
}
