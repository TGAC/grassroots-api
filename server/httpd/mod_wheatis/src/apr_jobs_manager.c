/*
 * apr_jobs_manager.c
 *
 *  Created on: 18 May 2015
 *      Author: tyrrells
 */

#include <limits.h>

#include "apr_hash.h"

#include "jobs_manager.h"
#include "apr_jobs_manager.h"

#include "service_job.h"
#include "mod_wheatis_config.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "util_mutex.h"

#ifdef _DEBUG
#define APR_JOBS_MANAGER_DEBUG	(STM_LEVEL_FINEST)
#else
#define APR_JOBS_MANAGER_DEBUG	(STM_LEVEL_NONE)
#endif

/**************************/

static const char s_mutex_filename_s [] = "logs/wheatis_lock";
static const char s_cache_id_s [] = "wheatis-socache";

/**************************/

static apr_status_t CleanUpAPRJobsManagerConfig (void *value_p);

static unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p);

static void DebugJobsManager (APRJobsManager *manager_p);

static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p,
                                           uuid_t job_key, ServiceJob *job_p);

static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *jobs_manager_p,
                                                    const uuid_t job_key);

static ServiceJob *RemoveServiceJobFromAprJobsManager (
		JobsManager *jobs_manager_p, const uuid_t job_key);

/**************************/

APRJobsManager *InitAPRJobsManager (server_rec *server_p, apr_pool_t *pool_p)
{
	APRJobsManager *manager_p =
			(APRJobsManager *) AllocMemory(sizeof(APRJobsManager));


	if (manager_p)
		{
			bool success_flag = false;

			InitJobsManager (&(manager_p -> ajmc_base_manager), AddServiceJobToAPRJobsManager, GetServiceJobFromAprJobsManager, RemoveServiceJobFromAprJobsManager);

			apr_status_t status = apr_global_mutex_create (&(manager_p -> ajmc_mutex_p), s_mutex_filename_s, APR_THREAD_MUTEX_UNNESTED, pool_p);

			if (status == APR_SUCCESS)
				{
					manager_p -> ajmc_running_jobs_p = apr_hash_make_custom (pool_p, HashUUIDForAPR);

					if (manager_p -> ajmc_running_jobs_p)
						{
							manager_p -> ajmc_pool_p = pool_p;

							manager_p -> ajmc_server_p = server_p;

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
					FreeMemory(manager_p);
					manager_p = NULL;
				}
		}

	return manager_p;
}

bool DestroyAPRJobsManager (APRJobsManager *jobs_manager_p)
{
	if (jobs_manager_p)
		{
			apr_hash_index_t *index_p =
					apr_hash_first (jobs_manager_p -> ajmc_pool_p, jobs_manager_p -> ajmc_running_jobs_p);
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

			FreeMemory(jobs_manager_p);
		}

	return true;
}

static int APRJobsManagerPreConfigure (apr_pool_t *config_pool_p,
                                       apr_pool_t *log_pool_p,
                                       apr_pool_t *temp_pool_p)
{
	apr_status_t res = ap_mutex_register (config_pool_p, s_cache_id_s, NULL, APR_LOCK_DEFAULT, 0);

	if (res == APR_SUCCESS)
		{
			s_provider_p = ap_lookup_provider (AP_SOCACHE_PROVIDER_GROUP, AP_SOCACHE_DEFAULT_PROVIDER, AP_SOCACHE_PROVIDER_VERSION);

		}
	else
		{
			ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, plog, APLOGNO(01673)
			              "failed to register %s mutex", s_cache_id_s);
			return 500; /* An HTTP status would be a misnomer! */
		}
	configured = 0;
	return OK;
}


static void ap_authn_cache_store (request_rec *r, const char *module,
                                  const char *user, const char *realm,
                                  const char* data)
{
	apr_status_t rv;
	ModWheatISConfig *config_p = ap_get_module_config (server_p -> module_config, &wheatis_module);
	const char *key;
	apr_time_t expiry;
	int i;
	int use_cache = 0;

	/* first check whether we're cacheing for this module */
	dcfg = ap_get_module_config(r -> per_dir_config, &authn_socache_module);
	if (!configured || !dcfg -> providers)
		{
			return;
		}
	for (i = 0; i < dcfg -> providers -> nelts; ++i)
		{
			if (!strcmp (module, APR_ARRAY_IDX(dcfg -> providers, i, const char*)))
				{
					use_cache = 1;
					break;
				}
		}
	if (!use_cache)
		{
			return;
		}

	/* OK, we're on.  Grab mutex to do our business */
	rv = apr_global_mutex_trylock (authn_cache_mutex);
	if (APR_STATUS_IS_EBUSY(rv))
		{
			/* don't wait around; just abandon it */
			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, rv, r, APLOGNO(01679)
			              "authn credentials for %s not cached (mutex busy)", user);
			return;
		}
	else if (rv != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(01680)
			              "Failed to cache authn credentials for %s in %s",
			              module, dcfg -> context);
			return;
		}

	/* We have the mutex, so go ahead */
	/* first build our key and determine expiry time */
	key = construct_key (r, dcfg -> context, user, realm);
	expiry = apr_time_now () + dcfg -> timeout;

	/* store it */
	rv = socache_provider -> store (socache_instance, r -> server, (unsigned char*) key, strlen (key), expiry, (unsigned char*) data, strlen (data), r -> pool);
	if (rv == APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01681)
			              "Cached authn credentials for %s in %s",
			              user, dcfg -> context);
		}
	else
		{
			ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(01682)
			              "Failed to cache authn credentials for %s in %s",
			              module, dcfg -> context);
		}

	/* We're done with the mutex */
	rv = apr_global_mutex_unlock (authn_cache_mutex);
	if (rv != APR_SUCCESS)
		{
			ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(01683) "Failed to release mutex!");
		}
	return;
}

static int authn_cache_post_config (apr_pool_t *pconf, apr_pool_t *plog,
                                    apr_pool_t *ptmp, server_rec *s)
{
	apr_status_t rv;
	apr_interval_time_t expiry = ~0;
	static struct ap_socache_hints job_cache_hints = { UUID_STRING_BUFFER_SIZE, sizeof (ServiceJob *), expiry };
	const char *errmsg;

	if (!configured)
		{
			return OK; /* don't waste the overhead of creating mutex & cache */
		}
	if (socache_provider == NULL)
		{
			ap_log_perror(APLOG_MARK, APLOG_CRIT, 0, plog, APLOGNO(01674)
			              "Please select a socache provider with AuthnCacheSOCache "
			              "(no default found on this platform). Maybe you need to "
			              "load mod_socache_shmcb or another socache module first");
			return 500; /* An HTTP status would be a misnomer! */
		}

	/* We have socache_provider, but do not have socache_instance. This should
	 * happen only when using "default" socache_provider, so create default
	 * socache_instance in this case. */
	if (socache_instance == NULL)
		{
			errmsg = socache_provider -> create (&socache_instance, NULL, ptmp, pconf);
			if (errmsg)
				{
					ap_log_perror(APLOG_MARK, APLOG_CRIT, 0, plog, APLOGNO(02612)
					              "failed to create mod_socache_shmcb socache "
					              "instance: %s", errmsg);
					return 500;
				}
		}

	rv =
			ap_global_mutex_create (&authn_cache_mutex, NULL, authn_cache_id, NULL, s, pconf, 0);
	if (rv != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, plog, APLOGNO(01675)
			              "failed to create %s mutex", authn_cache_id);
			return 500; /* An HTTP status would be a misnomer! */
		}
	apr_pool_cleanup_register (pconf, NULL, remove_lock, apr_pool_cleanup_null);

	rv =
			socache_provider -> init (socache_instance, authn_cache_id, &authn_cache_hints, s, pconf);
	if (rv != APR_SUCCESS)
		{
			ap_log_perror(APLOG_MARK, APLOG_CRIT, rv, plog, APLOGNO(01677)
			              "failed to initialise %s cache", authn_cache_id);
			return 500; /* An HTTP status would be a misnomer! */
		}
	apr_pool_cleanup_register (pconf, (void*) s, destroy_cache, apr_pool_cleanup_null);
	return OK;
}

static bool WheatISChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
	ModWheatISConfig *config_p = ap_get_module_config(server_p -> module_config, &wheatis_module);

	/* Now that we are in a child process, we have to reconnect
	 * to the global mutex and the shared segment. We also
	 * have to find out the base address of the segment, in case
	 * it moved to a new address. */
	apr_status_t res =
			apr_global_mutex_child_init (&(config_p -> wisc_jobs_manager_p), s_mutex_filename_s, pool_p);

	if (res != APR_SUCCESS)
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to attach to wheatis global mutex file '%s', res %d", s_mutex_filename_s, res);
			return false;
		}

	/* We only need to attach to the segment if we didn't inherit
	 * it from the parent process (ie. Windows) */
	if (!scfg -> counters_shm)
		{
			rv = apr_shm_attach (&scfg -> counters_shm, scfg -> shmcounterfile, p);
			if (rv != APR_SUCCESS)
				{
					ap_log_error (APLOG_MARK, APLOG_CRIT, rv, s, "Failed to attach to "
					              "mod_shm_counter shared memory file '%s'",
					              scfg -> shmcounterfile ?
					              		/* Just in case the file was NULL. */
					              		scfg -> shmcounterfile :
					              		"NULL");
					return;
				}
		}

	scfg -> counters = apr_shm_baseaddr_get (scfg -> counters_shm);
}

static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p,
                                           uuid_t job_key, ServiceJob *job_p)
{
	bool success_flag = false;
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;
	char *uuid_s = GetUUIDAsString (job_key);

	if (uuid_s)
		{
			apr_status_t status = apr_global_mutex_lock (manager_p -> ajmc_mutex_p);

			if (status == APR_SUCCESS)
				{
					apr_time_t end_of_time = ~0;

					/* store it */
					status = manager_p -> ajmc_socache_instance_p -> store (manager_p -> ajmc_socache_instance_p, manager_p -> ajmc_server_p, (unsigned char *) uuid_s, UUID_STRING_BUFFER_SIZE, end_of_time, (unsigned char *) job_p, sizeof (job_p), manager_p -> ajmc_pool_p);

					#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINE
					PrintLog (STM_LEVEL_FINE, "Added %s to jobs manager", uuid_s);
					#endif
				}		/* if (status == APR_SUCCESS) */
			else
				{

				}

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p -> ajmc_running_jobs_p);
			#endif

			status = apr_global_mutex_unlock (manager_p -> ajmc_mutex_p);
			success_flag = true;

			if (status != APR_SUCCESS)
				{

				} /* if (status != APR_SUCCESS) */

			FreeCopiedString (uuid_s);
		} /* if (uuid_s) */

	return success_flag;
}

static ServiceJob *GetServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;

	apr_status_t status = apr_global_mutex_lock (manager_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			char *uuid_s = GetUUIDAsString (job_key);

			if (uuid_s)
				{
					PrintLog (STM_LEVEL_FINE, "Getting %s from jobs manager gave %x", uuid_s, job_p);
					status = manager_p -> ajmc_socache_provider_p -> retrieve (manager_p -> ajmc_socache_instance_p, manager_p -> ajmc_server_p, (unsigned char *) uuid_s, UUID_STRING_BUFFER_SIZE, job_p, sizeof (job_p), manager_p -> ajmc_pool_p);

					FreeCopiedString (uuid_s);
				}

#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p -> ajmc_running_jobs_p);
#endif

			status = apr_global_mutex_unlock (manager_p -> ajmc_mutex_p);

			if (status != APR_SUCCESS)
				{

				} /* if (status != APR_SUCCESS) */

		} /* if (status == APR_SUCCESS) */

	return job_p;
}

static ServiceJob *RemoveServiceJobFromAprJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;

	apr_status_t status = apr_global_mutex_lock (manager_p -> ajmc_mutex_p);

	if (status == APR_SUCCESS)
		{
			char *uuid_s = GetUUIDAsString (job_key);

			if (uuid_s)
				{
					status = manager_p -> ajmc_socache_provider_p -> retrieve (manager_p -> ajmc_socache_instance_p, manager_p -> ajmc_server_p, (unsigned char *) uuid_s, UUID_STRING_BUFFER_SIZE, job_p, sizeof (job_p), manager_p -> ajmc_pool_p);
					PrintLog (STM_LEVEL_FINE, "Removing %s from jobs manager gave %x", uuid_s, job_p);

					status = manager_p -> ajmc_socache_provider_p -> remove (manager_p -> ajmc_socache_instance_p, manager_p -> ajmc_server_p, (unsigned char *) uuid_s, UUID_STRING_BUFFER_SIZE, manager_p -> ajmc_pool_p);

					FreeCopiedString (uuid_s);
				}

#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p -> ajmc_running_jobs_p);
#endif

			status = apr_global_mutex_unlock (manager_p -> ajmc_mutex_p);

			if (status != APR_SUCCESS)
				{

				} /* if (status != APR_SUCCESS) */

		} /* if (status == APR_SUCCESS) */

	return job_p;
}

void APRServiceJobFinished (JobManager *manager_p, uuid_t job_key)
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

	for (index_p =
			apr_hash_first (manager_p -> ajmc_pool_p, manager_p -> ajmc_running_jobs_p);
			index_p; index_p = apr_hash_next (index_p))
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
