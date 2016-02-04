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

static const char s_mutex_filename_s [] = "logs/grassroots_servers_manager_lock";

/**************************/


static bool AddExternalServerToAprServersManager (ServersManager *servers_manager_p, ExternalServer *server_p, unsigned char *(*serialise_fn) (ExternalServer *server_p, uint32 *length_p));


static ExternalServer *GetExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key, ExternalServer *(*deserialise_fn) (unsigned char *data_p));


static ExternalServer *RemoveExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key, ExternalServer *(*deserialise_fn) (unsigned char *data_p));


static ExternalServer *QueryExternalServerFromAprServersManager (ServersManager *jobs_manager_p, const uuid_t key, ExternalServer *(*deserialise_fn) (unsigned char *data_p), void *(*storage_callback_fn) (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length));


static LinkedList *GetAllExternalServersFromAprServersManager (ServersManager *servers_manager_p, ExternalServer *(*deserialise_fn) (unsigned char *data_p));


static apr_status_t AddExternalServerFromSOCache (ap_socache_instance_t *instance_p, server_rec *server_p, void *user_data_p, const unsigned char *id_s, unsigned int id_length, const unsigned char *data_p, unsigned int data_length, apr_pool_t *pool_p);


static void FreeAPRExternalServer (unsigned char *key_p, void *value_p);


static bool DestroyAPRServersManager (ServersManager *manager_p);



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
					                    GetAllExternalServersFromAprServersManager,
					                    DestroyAPRServersManager);

					apr_pool_cleanup_register (pool_p, manager_p, CleanUpAPRServersManager, apr_pool_cleanup_null);

					return manager_p;
				}

			FreeMemory (manager_p);
		}

	return NULL;
}


static bool DestroyAPRServersManager (ServersManager *manager_p)
{
	APRServersManager *servers_manager_p = (APRServersManager *) manager_p;

	if (servers_manager_p)
		{
			//FreeAPRGlobalStorage (manager_p -> asm_store_p);
			FreeMemory (servers_manager_p);
		}

	return true;
}



bool APRServersManagerPreConfigure (APRServersManager *manager_p, apr_pool_t *config_pool_p)
{
	manager_p -> asm_store_p -> ags_cache_id_s = APR_SERVERS_MANAGER_CACHE_ID_S;

	return PreConfigureGlobalStorage (manager_p -> asm_store_p, config_pool_p);
}


bool PostConfigAPRServersManager (APRServersManager *manager_p, apr_pool_t *server_pool_p, server_rec *server_p, const char *provider_name_s)
{
	bool success_flag = false;

	/* Set up the maximum expiry time as we never want it to expire */
	apr_interval_time_t expiry = APR_INT64_MAX;
	struct ap_socache_hints cache_hints = { UUID_STRING_BUFFER_SIZE, sizeof (ExternalServer), expiry };

	success_flag = PostConfigureGlobalStorage(manager_p -> asm_store_p, server_pool_p, server_p, provider_name_s, &cache_hints);

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



static bool AddExternalServerToAprServersManager (ServersManager *servers_manager_p, ExternalServer *server_p, unsigned char *(*serialise_fn) (ExternalServer *server_p, uint32 *length_p))
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;
	bool success_flag = false;
	unsigned char *value_p = NULL;
	unsigned int value_length = 0;
	void (*free_value_fn) (void *data_p)  = NULL;
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	ConvertUUIDToString (server_p -> es_id, uuid_s);

	if (serialise_fn)
		{
			value_p = serialise_fn (server_p, &value_length);
			free_value_fn = free;
		}
	else
		{
			/* We store the c-style string for the ExternalServer's json */
			json_t *server_json_p = GetExternalServerAsJSON (server_p);

			if (server_json_p)
				{
					char *server_s = json_dumps (server_json_p, JSON_INDENT (2));

					if (server_s)
						{
							/*
							 * include the terminating \0 to make sure
							 * the value as a valid c-style string
							 */
							value_length = strlen (server_s) + 1;
							value_p = (unsigned char *) server_s;
							free_value_fn = free;
						}		/* if (job_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json_dumps failed for \"%s\"", uuid_s);
						}

				}		/* if (job_json_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetServiceJobAsJSON failed for \"%s\"", uuid_s);
				}
		}

	if (value_p)
		{
			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
				{
					PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Adding \"%s\"=\"%s\"", uuid_s, value_p);
				}
			#endif

			success_flag = AddObjectToAPRGlobalStorage (manager_p -> asm_store_p, server_p -> es_id, UUID_RAW_SIZE, value_p, value_length);

			#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINER
				{
					PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Added \"%s\"=\"%s\", success=%d", uuid_s, value_p, success_flag);
				}
			#endif

			if (free_value_fn)
				{
					free_value_fn (value_p);
				}

		}		/* if (value_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't serialise ServiceJob");
		}

	return success_flag;
}



static ExternalServer *GetExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key, ExternalServer *(*deserialise_fn) (unsigned char *data_p))
{
	return QueryExternalServerFromAprServersManager (manager_p, key, deserialise_fn, GetObjectFromAPRGlobalStorage);
}


static ExternalServer *RemoveExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key, ExternalServer *(*deserialise_fn) (unsigned char *data_p))
{
	return QueryExternalServerFromAprServersManager (manager_p, key, deserialise_fn, RemoveObjectFromAPRGlobalStorage);
}


static ExternalServer *QueryExternalServerFromAprServersManager (ServersManager *jobs_manager_p, const uuid_t key, ExternalServer *(*deserialise_fn) (unsigned char *data_p), void *(*storage_callback_fn) (APRGlobalStorage *storage_p, const void *raw_key_p, unsigned int raw_key_length))
{
	APRJobsManager *manager_p = (APRJobsManager *) jobs_manager_p;
	ExternalServer *server_p = NULL;
	unsigned char *value_p = NULL;
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	ConvertUUIDToString (key, uuid_s);

	#if APR_SERVERS_MANAGER_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Looking for %s", uuid_s);
	#endif


	value_p = storage_callback_fn (manager_p -> ajm_store_p, key, UUID_RAW_SIZE);

	if (value_p)
		{
			if (deserialise_fn)
				{
					server_p = deserialise_fn (value_p);

					if (!server_p)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "deserialise_fn failed for \"%s\"", uuid_s);
						}
				}
			else
				{
					json_error_t err;
					json_t *server_json_p = NULL;

					#if APR_SERVERS_MANAGER_DEBUG >= STM_LEVEL_FINER
					PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "For job %s, got: \"%s\"", uuid_s, value_p);
					#endif

					server_json_p = json_loads ((char *) value_p, 0, &err);

					if (server_json_p)
						{
							server_p = CreateExternalServerFromJSON (server_json_p);

							if (!server_p)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "CreateExternalServerFromJSON failed for \"%s\"", uuid_s);
								}

						}		/* if (server_json_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to json for uuid \"%s\", err \"%s\" at line %d, column %d", value_p, uuid_s, err.text, err.line, err.column);
						}
				}

			FreeMemory (value_p);
		}		/* if (value_p) */
	else
		{

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get stored value for \"%s\"", uuid_s);
		}

	return server_p;
}



static LinkedList *GetAllExternalServersFromAprServersManager (ServersManager *servers_manager_p, ExternalServer *(*deserialise_fn) (unsigned char *data_p))
{
	LinkedList *servers_p = AllocateLinkedList (FreeExternalServerNode);

	if (servers_p)
		{
			APRServersManager *manager_p = (APRServersManager *) servers_manager_p;
			ap_socache_iterator_t *iterator_p = AddExternalServerFromSOCache;

			IterateOverAPRGlobalStorage (manager_p -> asm_store_p, iterator_p, servers_p);

			if (servers_p -> ll_size == 0)
				{
					FreeLinkedList (servers_p);
					servers_p = NULL;
				}
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
	ServersManager *manager_p = (ServersManager *) value_p;

	return (DestroyAPRServersManager (manager_p) ? APR_SUCCESS : APR_EGENERAL);
}



static apr_status_t AddExternalServerFromSOCache (ap_socache_instance_t *instance_p, server_rec *server_p, void *user_data_p, const unsigned char *id_s, unsigned int id_length, const unsigned char *data_p, unsigned int data_length, apr_pool_t *pool_p)
{
	apr_status_t status = APR_SUCCESS;
	LinkedList *servers_p = (LinkedList *) user_data_p;
	ExternalServer *external_server_p = DeserialiseExternalServerFromJSON (data_p);

	if (external_server_p)
		{
			ExternalServerNode *node_p = AllocateExternalServerNode (external_server_p, MF_SHADOW_USE);

			if (node_p)
				{
					LinkedListAddTail (servers_p, (ListItem *) node_p);
				}
			else
				{
					status = APR_ENOMEM;
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create external server from \"%s\"", (char *) data_p);
		}

	return status;
}
