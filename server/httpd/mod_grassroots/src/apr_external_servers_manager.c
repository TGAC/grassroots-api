/*
 * apr_external_servers__manager.c
 *
 *  Created on: 16 May 2015
 *      Author: tyrrells
 */

#include <limits.h>

#include "apr_hash.h"

#include "servers_pool.h"

#include "mod_grassroots_config.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "util_mutex.h"

#define ALLOCATE_APR_SERVERS_MANAGER_TAGS (1)
#include "apr_servers_manager.h"
#include "json_tools.h"
#include "grassroots_config.h"

#ifdef _DEBUG
#define APR_SERVERS_MANAGER_DEBUG	(STM_LEVEL_FINEST)
#else
#define APR_SERVERS_MANAGER_DEBUG	(STM_LEVEL_NONE)
#endif

/**************************/

static const char s_mutex_filename_s [] = "logs/wheatis_servers_manager_lock";

/**************************/


static bool AddExternalServerToAprServersManager (ServersManager *manager_p, ExternalServer *server_p);


static ExternalServer *GetExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key);


static ExternalServer *RemoveExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key);


static LinkedList *GetAllExternalServersFromAprServersManager (ServersManager *manager_p);


static apr_status_t AddExternalServerFromSOCache (ap_socache_instance_t *instance_p, server_rec *server_p, void *user_data_p, const unsigned char *id_s, unsigned int id_length, const unsigned char *data_p, unsigned int data_length, apr_pool_t *pool_p);


static void FreeAPRExternalServer (unsigned char *key_p, void *value_p);


/**************************/

APRServersManager *InitAPRServersManager (server_rec *server_p, apr_pool_t *pool_p, const char *provider_name_s)
{
	APRServersManager *manager_p = (APRServersManager *) AllocMemory (sizeof (APRServersManager));

	if (manager_p)
		{
			unsigned char *(*make_key_fn) (const void *data_p, uint32 raw_key_length, uint32 *key_len_p) = MakeKeyFromUUID;
			APRGlobalStorage *storage_p = AllocateAPRGlobalStorage (pool_p,
			                                                        HashUUIDForAPR,
			                                                        make_key_fn,
			                                                        FreeAPRExternalServer,
			                                                        server_p,
			                                                        s_mutex_filename_s,
			                                                        APR_SERVERS_MANAGER_CACHE_ID_S,
			                                                        provider_name_s);
			if (storage_p)
				{
					manager_p -> asm_store_p = storage_p;

					InitServersManager (& (manager_p -> asm_base_manager),
					                    AddExternalServerToAprServersManager,
					                    GetExternalServerFromAprServersManager,
					                    RemoveExternalServerFromAprServersManager,
					                    GetAllExternalServersFromAprServersManager);

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
			FreeMemory (manager_p);
		}

	return true;
}



bool APRServersManagerPreConfigure (APRServersManager *manager_p, apr_pool_t *config_pool_p)
{
	manager_p -> asm_store_p -> ags_cache_id_s = APR_SERVERS_MANAGER_CACHE_ID_S;

	return PreConfigureGlobalStorage (manager_p -> asm_store_p, config_pool_p);
}


bool PostConfigAPRServersManager (APRServersManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s)
{
	bool success_flag = false;

	/* Set up the maximum expiry time as we never want it to expire */
	apr_interval_time_t expiry = APR_INT64_MAX;
	struct ap_socache_hints cache_hints = { UUID_STRING_BUFFER_SIZE, sizeof (ExternalServer), expiry };

	success_flag = PostConfigureGlobalStorage(manager_p -> asm_store_p, config_pool_p, server_p, provider_name_s, &cache_hints);

//	if (success_flag)
//		{
//			ConnectToExternalServers ();
//		}

	return success_flag;
}



bool APRServersManagerChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
	ModGrassrootsConfig *config_p = ap_get_module_config (server_p -> module_config, GetGrassrootsModule ());
	bool success_flag = InitAPRGlobalStorageForChild (config_p -> wisc_servers_manager_p -> asm_store_p, pool_p);

	return success_flag;
}


static bool AddExternalServerToAprServersManager (ServersManager *servers_manager_p, ExternalServer *server_p)
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;
	unsigned int object_size = sizeof (ExternalServer);
	bool success_flag = AddObjectToAPRGlobalStorage (manager_p -> asm_store_p, server_p -> es_id, UUID_RAW_SIZE, (unsigned char *) server_p, object_size);

	return success_flag;
}


static ExternalServer *GetExternalServerFromAprServersManager (ServersManager *servers_manager_p, const uuid_t key)
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;

	return ((ExternalServer *) GetObjectFromAPRGlobalStorage (manager_p -> asm_store_p, key, UUID_RAW_SIZE, sizeof (ExternalServer)));
}


static ExternalServer *RemoveExternalServerFromAprServersManager (ServersManager *servers_manager_p, const uuid_t key)
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;

	return ((ExternalServer *) RemoveObjectFromAPRGlobalStorage (manager_p -> asm_store_p, key, UUID_RAW_SIZE, sizeof (ExternalServer)));
}


static LinkedList *GetAllExternalServersFromAprServersManager (ServersManager *servers_manager_p)
{
	LinkedList *servers_p = AllocateLinkedList (FreeExternalServerNode);

	if (servers_p)
		{
			APRServersManager *manager_p = (APRServersManager *) servers_manager_p;
			ap_socache_iterator_t *iterator_p = AddExternalServerFromSOCache;

			IterateOverAPRGlobalStorage (manager_p -> asm_store_p, iterator_p, servers_p);

		}		/* if (servers_p) */

	return servers_p;
}



static void FreeAPRExternalServer (unsigned char *key_p, void *value_p)
{
	if (key_p)
		{
			FreeMemory (key_p);
		}

	if (value_p)
		{
			ExternalServer *server_p = (ExternalServer *) value_p;
			FreeExternalServer (server_p);
		}
}



apr_status_t CleanUpAPRServersManager (void *value_p)
{
	APRServersManager *manager_p = (APRServersManager *) value_p;

	return (DestroyAPRServersManager (manager_p) ? APR_SUCCESS : APR_EGENERAL);
}



static apr_status_t AddExternalServerFromSOCache (ap_socache_instance_t *instance_p, server_rec *server_p, void *user_data_p, const unsigned char *id_s, unsigned int id_length, const unsigned char *data_p, unsigned int data_length, apr_pool_t *pool_p)
{
	apr_status_t status = APR_SUCCESS;
	LinkedList *servers_p = (LinkedList *) user_data_p;
	ExternalServer *external_server_p = (ExternalServer *) data_p;
	ExternalServerNode *node_p = AllocateExternalServerNode (external_server_p, MF_SHADOW_USE);

	if (node_p)
		{
			LinkedListAddTail (servers_p, (ListItem *) node_p);
		}
	else
		{
			status = APR_ENOMEM;
		}

	return status;
}
