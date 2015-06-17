/*
 * apr_external_servers__manager.c
 *
 *  Created on: 16 May 2015
 *      Author: tyrrells
 */

#include <limits.h>

#include "apr_hash.h"

#include "servers_pool.h"
#include "apr_jobs_manager.h"

#include "mod_wheatis_config.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "util_mutex.h"

#ifdef _DEBUG
#define APR_SERVERS_MANAGER_DEBUG	(STM_LEVEL_FINEST)
#else
#define APR_SERVERS_MANAGER_DEBUG	(STM_LEVEL_NONE)
#endif

/**************************/

static const char s_mutex_filename_s [] = "logs/wheatis_lock";
static const char s_cache_id_s [] = "wheatis-socache";

/**************************/


static bool AddExternalServerToAprServersManager (ServersManager *manager_p, ExternalServer *server_p);


static ExternalServer *GetExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key);


static ExternalServer *RemoveExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key);


static bool PostConfigAPRJobsManager (APRJobsManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s);


/**************************/

APRJobsManager *InitAPRJobsManager (server_rec *server_p, apr_pool_t *pool_p, const char *provider_name_s)
{
	APRJobsManager *manager_p = (APRJobsManager *) AllocMemory (sizeof (APRJobsManager));

	if (manager_p)
		{
			bool success_flag = false;

			InitJobsManager (&(manager_p -> ajm_base_manager), AddServiceJobToAPRJobsManager, GetServiceJobFromAprJobsManager, RemoveServiceJobFromAprJobsManager);

			apr_status_t status = apr_global_mutex_create (& (manager_p -> ajm_mutex_p), s_mutex_filename_s, APR_THREAD_MUTEX_UNNESTED, pool_p);

			if (status == APR_SUCCESS)
				{
					PostConfigAPRJobsManager (manager_p, pool_p, server_p, provider_name_s);

					manager_p -> ajm_running_jobs_p = apr_hash_make_custom (pool_p, HashUUIDForAPR);

					if (manager_p -> ajm_running_jobs_p)
						{
							manager_p -> ajm_pool_p = pool_p;

							manager_p -> ajm_server_p = server_p;

							apr_pool_cleanup_register (pool_p, manager_p, CleanUpAPRJobsManager, apr_pool_cleanup_null);

							success_flag = true;
						}
					else
						{
							apr_global_mutex_destroy (manager_p -> ajm_mutex_p);
							manager_p -> ajm_mutex_p = NULL;
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
			apr_hash_index_t *index_p =	apr_hash_first (jobs_manager_p -> ajm_pool_p, jobs_manager_p -> ajm_running_jobs_p);
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

			apr_hash_clear (jobs_manager_p -> ajm_running_jobs_p);
			jobs_manager_p -> ajm_running_jobs_p = NULL;

			apr_global_mutex_destroy (jobs_manager_p -> ajm_mutex_p);
			jobs_manager_p -> ajm_mutex_p = NULL;


			if (jobs_manager_p -> ajm_socache_instance_p)
				{
					jobs_manager_p -> ajm_socache_provider_p -> destroy (jobs_manager_p -> ajm_socache_instance_p, jobs_manager_p -> ajm_server_p);
				}

			FreeMemory (jobs_manager_p);
		}

	return true;
}


bool APRJobsManagerPreConfigure (APRJobsManager *manager_p, apr_pool_t *config_pool_p)
{
	bool success_flag = false;
	apr_status_t status = ap_mutex_register (config_pool_p, s_cache_id_s, NULL, APR_LOCK_DEFAULT, 0);

	if (status == APR_SUCCESS)
		{
			manager_p -> ajm_socache_provider_p = ap_lookup_provider (AP_SOCACHE_PROVIDER_GROUP, AP_SOCACHE_DEFAULT_PROVIDER, AP_SOCACHE_PROVIDER_VERSION);
			success_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "failed to register %s mutex", s_cache_id_s);
		}

	return success_flag;
}


static bool PostConfigAPRJobsManager (APRJobsManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s)
{
	/* Set up the maximum expiry time as we never want it to expire */
	apr_interval_time_t expiry = 0;
	struct ap_socache_hints job_cache_hints = { UUID_STRING_BUFFER_SIZE, sizeof (ServiceJob *), expiry };
	apr_status_t res;
	bool success_flag = true;

	/*
	 * Have we got a valid set of config directives?
	 */
	if (manager_p -> ajm_socache_provider_p)
		{
			/* We have socache_provider, but do not have socache_instance. This should
			 * happen only when using "default" socache_provider, so create default
			 * socache_instance in this case. */
			if (!manager_p -> ajm_socache_instance_p)
				{
					const char *err_msg_s = manager_p -> ajm_socache_provider_p -> create (& (manager_p -> ajm_socache_instance_p), NULL, config_pool_p, config_pool_p);

					if (err_msg_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, "failed to create mod_socache_shmcb socache instance: %s", err_msg_s);
							success_flag = false;
						}
				}

			if (success_flag)
				{
					res = ap_global_mutex_create (& (manager_p -> ajm_mutex_p), NULL, s_cache_id_s, NULL, server_p, config_pool_p, 0);

					if (res != APR_SUCCESS)
						{
							PrintErrors (STM_LEVEL_SEVERE, "failed to create %s mutex", s_cache_id_s);
							success_flag = false;
						}

					res = manager_p -> ajm_socache_provider_p -> init (manager_p -> ajm_socache_instance_p, s_cache_id_s, &job_cache_hints, server_p, config_pool_p);
					if (res != APR_SUCCESS)
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to initialise %s cache", s_cache_id_s);
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



bool APRJobsManagerChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
	bool success_flag = false;
	ModWheatISConfig *config_p = ap_get_module_config (server_p -> module_config, GetWheatISModule ());

	/* Now that we are in a child process, we have to reconnect
	 * to the global mutex and the shared segment. We also
	 * have to find out the base address of the segment, in case
	 * it moved to a new address. */
	apr_status_t res = apr_global_mutex_child_init (& (config_p -> wisc_jobs_manager_p -> ajm_mutex_p), s_mutex_filename_s, pool_p);

	if (res != APR_SUCCESS)
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to attach wheatis child to global mutex file '%s', res %d", s_mutex_filename_s, res);
		}
	else
		{
			success_flag = true;
		}

	return success_flag;
}


static bool AddServiceJobToAPRJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob *job_p)
{
	bool success_flag = false;
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;
	char *uuid_s = GetUUIDAsString (job_key);

	if (uuid_s)
		{
			apr_status_t status = apr_global_mutex_lock (manager_p -> ajm_mutex_p);

			if (status == APR_SUCCESS)
				{
					apr_time_t end_of_time = ~0;

					/* store it */
					status = manager_p -> ajm_socache_provider_p -> store (manager_p -> ajm_socache_instance_p,
					                              manager_p -> ajm_server_p,
					                              (unsigned char *) uuid_s,
					                              UUID_STRING_BUFFER_SIZE,
					                              end_of_time,
					                              (unsigned char *) job_p,
					                              sizeof (job_p),
					                              manager_p -> ajm_pool_p);


					#if APR_SERVERS_MANAGER_DEBUG >= STM_LEVEL_FINE
					PrintLog (STM_LEVEL_FINE, "Added %s to jobs manager", uuid_s);
					#endif
				}		/* if (status == APR_SUCCESS) */
			else
				{

				}

			#if APR_SERVERS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p);
			#endif

			status = apr_global_mutex_unlock (manager_p -> ajm_mutex_p);
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

	apr_status_t status = apr_global_mutex_lock (manager_p -> ajm_mutex_p);

	if (status == APR_SUCCESS)
		{
			char *uuid_s = GetUUIDAsString (job_key);

			if (uuid_s)
				{
					unsigned int len = sizeof (ServiceJob *);

					status = manager_p -> ajm_socache_provider_p -> retrieve (manager_p -> ajm_socache_instance_p, manager_p -> ajm_server_p, (unsigned char *) uuid_s, UUID_STRING_BUFFER_SIZE, (unsigned char *) job_p, &len, manager_p -> ajm_pool_p);

					PrintLog (STM_LEVEL_FINE, "Getting %s from jobs manager gave %x", uuid_s, job_p);

					FreeCopiedString (uuid_s);
				}

#if APR_SERVERS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p);
#endif

			status = apr_global_mutex_unlock (manager_p -> ajm_mutex_p);

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

	apr_status_t status = apr_global_mutex_lock (manager_p -> ajm_mutex_p);

	if (status == APR_SUCCESS)
		{
			char *uuid_s = GetUUIDAsString (job_key);

			if (uuid_s)
				{
					unsigned int len = sizeof (job_p);
					status = manager_p -> ajm_socache_provider_p -> retrieve (manager_p -> ajm_socache_instance_p, manager_p -> ajm_server_p, (unsigned char *) uuid_s, UUID_STRING_BUFFER_SIZE, (unsigned char *) job_p, &len, manager_p -> ajm_pool_p);
					PrintLog (STM_LEVEL_FINE, "Removing %s from jobs manager gave %x", uuid_s, job_p);

					status = manager_p -> ajm_socache_provider_p -> remove (manager_p -> ajm_socache_instance_p, manager_p -> ajm_server_p, (unsigned char *) uuid_s, UUID_STRING_BUFFER_SIZE, manager_p -> ajm_pool_p);

					FreeCopiedString (uuid_s);
				}

#if APR_SERVERS_MANAGER_DEBUG >= STM_LEVEL_FINEST
			DebugJobsManager (manager_p);
#endif

			status = apr_global_mutex_unlock (manager_p -> ajm_mutex_p);

			if (status != APR_SUCCESS)
				{

				} /* if (status != APR_SUCCESS) */

		} /* if (status == APR_SUCCESS) */

	return job_p;
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
	APRJobsManager *manager_p = NULL;

	return (DestroyAPRJobsManager (manager_p) ? APR_SUCCESS : APR_EGENERAL);
}


static void DebugJobsManager (APRJobsManager *manager_p)
{
	apr_hash_index_t *index_p;
	uint32 size = apr_hash_count (manager_p -> ajm_running_jobs_p);

	PrintLog (STM_LEVEL_FINE, "Jobs manager %x size %lu", manager_p -> ajm_running_jobs_p, size);

	for (index_p =
			apr_hash_first (manager_p -> ajm_pool_p, manager_p -> ajm_running_jobs_p);
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
