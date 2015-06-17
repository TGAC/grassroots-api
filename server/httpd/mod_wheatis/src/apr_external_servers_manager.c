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


static bool PostConfigAPRJobsManager (APRServersManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s);


/**************************/

APRServersManager *InitAPRServersManager (server_rec *server_p, apr_pool_t *pool_p, const char *provider_name_s)
{
	APRServersManager *manager_p = (APRServersManager *) AllocMemory (sizeof (APRServersManager));

	if (manager_p)
		{
			APRGlobalStorage *storage_p = AllocateAPRGlobalStorage ();

			if (storage_p)
				{
					manager_p -> asm_store_p = storage_p;

					return manager_p;
				}

			FreeMemory (manager_p);
		}

	return NULL;
}


bool DestroyAPRServersManager (APRServersManager *manager_p)
{
	if (manager_p)
		{
			FreeAPRGlobalStorage (manager_p -> asm_store_p);
			FreeMemory (manager_p);
		}

	return true;
}



bool APRJobsManagerPreConfigure (APRServersManager *manager_p, apr_pool_t *config_pool_p)
{
	return PreConfigureGlobalStorage (manager_p -> asm_store_p, config_pool_p);
}


static bool PostConfigAPRServersManager (APRServersManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s)
{
	/* Set up the maximum expiry time as we never want it to expire */
	apr_interval_time_t expiry = 0;
	struct ap_socache_hints cache_hints = { UUID_STRING_BUFFER_SIZE, sizeof (ExternalServer), expiry };

	return PostConfigureGlobalStorage(manager_p -> asm_store_p, config_pool_p, server_p, provider_name_s, &cache_hints);
}



bool APRJobsManagerChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
	ModWheatISConfig *config_p = ap_get_module_config (server_p -> module_config, GetWheatISModule ());
	bool success_flag = InitAPRGlobalStorageForChild (config_p -> wisc_servers_manager_p -> asm_store_p, pool_p);

	return success_flag;
}


static bool AddServiceJobToAPRServersManager (ServersManager *servers_manager_p, uuid_t job_key, ServiceJob *job_p)
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;

	return AddObjectToAPRGlobalStorage (manager_p -> asm_store_p, job_key, UUID_RAW_SIZE, job_p, sizeof (ServiceJob));
}


static ServiceJob *GetServiceJobFromAprServersManager (ServersManager *servers_manager_p, const uuid_t job_key)
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;

	return ((ServiceJob *) GetObjectFromAPRGlobalStorage (manager_p -> asm_store_p, job_key, UUID_RAW_SIZE, sizeof (ServiceJob)));
}


static ServiceJob *RemoveServiceJobFromAprServersManager (ServersManager *servers_manager_p, const uuid_t job_key)
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;

	return ((ServiceJob *) RemoveObjectFromAPRGlobalStorage (manager_p -> asm_store_p, job_key, UUID_RAW_SIZE, sizeof (ServiceJob)));
}



apr_status_t CleanUpAPRJobsManager (void *value_p)
{
	APRServersManager *manager_p = NULL;

	return (DestroyAPRJobsManager (manager_p) ? APR_SUCCESS : APR_EGENERAL);
}

