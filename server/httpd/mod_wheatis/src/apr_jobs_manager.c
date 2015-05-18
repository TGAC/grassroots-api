/*
 * apr_jobs_manager.c
 *
 *  Created on: 18 May 2015
 *      Author: tyrrells
 */

#include "apr_hash.h"

#include "jobs_manager.h"

#include "service_job.h"
#include "mod_wheatis_config.h"
#include "string_utils.h"


static apr_hash_t *s_running_jobs_p = NULL;
static apr_pool_t *s_mem_pool_p = NULL;

static WheatISConfig *s_config_p;


static unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p);

static void *APRAllocMemory (size_t l);



static void *APRAllocMemory (size_t l)
{
	return apr_palloc (s_mem_pool_p, l);
}


static unsigned int HashUUIDForAPR (const char *key_s, apr_ssize_t *len_p)
{
	unsigned int res = 0;
	char *uuid_s = GetUUIDAsString ((const uint8 *) key_s);

	if (uuid_s)
		{
			res = apr_hashfunc_default (uuid_s, APR_HASH_KEY_STRING);

			FreeCopiedString (uuid_s);
		}

	return res;
}



/**
 * Create a HashTable where both the keys are strings and the values are services
 *
 * @param initital_capacity The initial number of HashBuckets for the HashTable.
 * @param load_percentage The percentage value for how full the HashTable should
 * be allowed to become before it is extended.
 * @return The HashTable or NULL is there was an error.
 */
bool InitJobsManager (WheatISConfig *config_p, apr_pool_t *pool_p)
{
	s_config_p = config_p;
	s_running_jobs_p = apr_hash_make_custom (pool_p, HashUUIDForAPR);

	return ((s_running_jobs_p != NULL) ? true : false);
}


bool DestroyJobsManager (apr_pool_t *pool_p)
{
	if (s_running_jobs_p)
		{
			apr_hash_index_t *index_p =  apr_hash_first (pool_p, s_running_jobs_p);
			char *key_s = NULL;
			apr_ssize_t keylen = 0;

			while (index_p)
				{
					ServiceJob *job_p = NULL;

					apr_hash_this (index_p, (const void **) &key_s, &keylen,(void**) &job_p);
					if (job_p)
						{
							//FreeServiceJob (job_p);
						}

					index_p = apr_hash_next(index_p);
				}

			apr_hash_clear(s_running_jobs_p);
			s_running_jobs_p = NULL;
		}

	return true;
}


bool AddServiceJobToJobsManager (uuid_t job_key, ServiceJob *job_p)
{
	bool success_flag = false;

	const char *uuid_s = GetUUIDAsStringCustom (job_key, APRAllocMemory);

	if (uuid_s)
		{
			apr_status_t status = apr_thread_mutex_lock (s_config_p -> wisc_mutex_p);

			if (status == APR_SUCCESS)
				{
					apr_hash_set (s_running_jobs_p, uuid_s, APR_HASH_KEY_STRING, job_p);

					status = apr_thread_mutex_unlock (s_config_p -> wisc_mutex_p);
					success_flag = true;

					if (status != APR_SUCCESS)
						{

						}		/* if (status != APR_SUCCESS) */

				}		/* if (status == APR_SUCCESS) */

		}		/* if (uuid_s) */

	return success_flag;
}


ServiceJob *GetServiceJobFromJobsManager (const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	char *uuid_s = GetUUIDAsString (job_key);

	if (uuid_s)
		{
			apr_status_t status = apr_thread_mutex_lock (s_config_p -> wisc_mutex_p);

			if (status == APR_SUCCESS)
				{
					job_p = (ServiceJob *) apr_hash_get (s_running_jobs_p, uuid_s, APR_HASH_KEY_STRING);

					status = apr_thread_mutex_unlock (s_config_p -> wisc_mutex_p);

					if (status != APR_SUCCESS)
						{

						}		/* if (status != APR_SUCCESS) */

				}		/* if (status == APR_SUCCESS) */

			FreeCopiedString (uuid_s);
		}		/* if (uuid_s) */

	return job_p;
}


ServiceJob *RemoveServiceJobFromJobsManager (const uuid_t job_key)
{
	ServiceJob *job_p = NULL;
	char *uuid_s = GetUUIDAsString (job_key);

	if (uuid_s)
		{
			apr_status_t status = apr_thread_mutex_lock (s_config_p -> wisc_mutex_p);

			if (status == APR_SUCCESS)
				{
					job_p = (ServiceJob *) apr_hash_get (s_running_jobs_p, uuid_s, APR_HASH_KEY_STRING);

					if (job_p)
						{
							apr_hash_set (s_running_jobs_p, uuid_s, APR_HASH_KEY_STRING, NULL);
						}

					status = apr_thread_mutex_unlock (s_config_p -> wisc_mutex_p);

					if (status != APR_SUCCESS)
						{

						}		/* if (status != APR_SUCCESS) */

				}		/* if (status == APR_SUCCESS) */

			FreeCopiedString (uuid_s);
		}		/* if (uuid_s) */

	return job_p;
}


void ServiceJobFinished (uuid_t job_key)
{
	ServiceJob *job_p = RemoveServiceJobFromJobsManager (job_key);

	if (job_p)
		{
		}
}


