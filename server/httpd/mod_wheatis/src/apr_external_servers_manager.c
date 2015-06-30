/*
 * apr_external_servers__manager.c
 *
 *  Created on: 16 May 2015
 *      Author: tyrrells
 */

#include <limits.h>

#include "apr_hash.h"

#include "servers_pool.h"

#include "mod_wheatis_config.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "util_mutex.h"
#include "apr_servers_manager.h"
#include "json_tools.h"


#ifdef _DEBUG
#define APR_SERVERS_MANAGER_DEBUG	(STM_LEVEL_FINEST)
#else
#define APR_SERVERS_MANAGER_DEBUG	(STM_LEVEL_NONE)
#endif

/**************************/

static const char s_mutex_filename_s [] = "logs/wheatis_servers_manager_lock";
static const char s_cache_id_s [] = "wheatis_servers_manager_cache";

/**************************/


static bool AddExternalServerToAprServersManager (ServersManager *manager_p, ExternalServer *server_p);


static ExternalServer *GetExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key);


static ExternalServer *RemoveExternalServerFromAprServersManager (ServersManager *manager_p, const uuid_t key);



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
			                                                        server_p,
			                                                        s_mutex_filename_s,
			                                                        s_cache_id_s);
			if (storage_p)
				{
					manager_p -> asm_store_p = storage_p;

					InitServersManager (& (manager_p -> asm_base_manager), AddExternalServerToAprServersManager, GetExternalServerFromAprServersManager, RemoveExternalServerFromAprServersManager);

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



bool APRServersManagerPreConfigure (APRServersManager *manager_p, apr_pool_t *config_pool_p)
{
	return PreConfigureGlobalStorage (manager_p -> asm_store_p, config_pool_p);
}


bool PostConfigAPRServersManager (APRServersManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s)
{
	/* Set up the maximum expiry time as we never want it to expire */
	apr_interval_time_t expiry = 0;
	struct ap_socache_hints cache_hints = { UUID_STRING_BUFFER_SIZE, sizeof (ExternalServer), expiry };

	return PostConfigureGlobalStorage(manager_p -> asm_store_p, config_pool_p, server_p, provider_name_s, &cache_hints);
}



bool APRServersManagerChildInit (apr_pool_t *pool_p, server_rec *server_p)
{
	ModWheatISConfig *config_p = ap_get_module_config (server_p -> module_config, GetWheatISModule ());
	bool success_flag = InitAPRGlobalStorageForChild (config_p -> wisc_servers_manager_p -> asm_store_p, pool_p);

	return success_flag;
}


static bool AddExternalServerToAprServersManager (ServersManager *servers_manager_p, ExternalServer *server_p)
{
	APRServersManager *manager_p = (APRServersManager *) servers_manager_p;

	return AddObjectToAPRGlobalStorage (manager_p -> asm_store_p, server_p -> es_id, UUID_RAW_SIZE, (unsigned char *) server_p, sizeof (ExternalServer));
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


apr_status_t CleanUpAPRServersManager (void *value_p)
{
	APRServersManager *manager_p = (APRServersManager *) value_p;

	return (DestroyAPRServersManager (manager_p) ? APR_SUCCESS : APR_EGENERAL);
}


json_t *AddExternalServerOperationsToJSON (ServersManager *manager_p, json_t *res_p, Operation op)
{
	/* build the request that we will send to each external server */
	json_error_t error;
	json_t *result_p = NULL;
	json_t *op_p = json_pack ("{s:{s:i}}", SERVER_OPERATIONS_S, OPERATION_ID_S, op);

	if (op_p)
		{
			ExternalServer *server_p;

			const char *response_s = MakeRemoteJsonCallViaConnection (server_p -> es_connection_p, op_p);

			if (response_s)
				{
					json_t *server_response_p = json_loads (response_s, 0, &error);

					if (server_response_p)
						{
							const char *element_name_s = NULL;

							/*
							 * The elements to get are dependent on the api call
							 */
							switch (op)
								{
									case OP_LIST_ALL_SERVICES:
									case OP_LIST_INTERESTED_SERVICES:
									case OP_GET_NAMED_SERVICES:
										element_name_s = SERVICES_NAME_S;
										break;

									case OP_RUN_KEYWORD_SERVICES:
									case OP_GET_SERVICE_RESULTS:
										element_name_s = SERVICE_RESULTS_S;
										break;

									case OP_IRODS_MODIFIED_DATA:
										break;

									case OP_CHECK_SERVICE_STATUS:
										break;

									case OP_CLEAN_UP_JOBS:
										break;

									default:
										break;
								}

							if (element_name_s)
								{
									json_t *dest_p = json_object_get (res_p, element_name_s);

									if (!dest_p)
										{
											dest_p = json_array ();

											if (dest_p)
												{
													if (json_object_set_new (res_p, element_name_s, dest_p) != 0)
														{
															WipeJSON (dest_p);
															dest_p = NULL;
														}
												}
										}

									if (dest_p && json_is_array (dest_p))
										{
											json_t *src_p = json_object_get (server_response_p, element_name_s);

											if (src_p)
												{
													/* copy the values from the response into res_p */

													if (json_is_array (src_p))
														{
															size_t index = 0;
															json_t *value_p;

															while ((value_p = json_array_get (src_p, index)) != NULL)
																{
																	if (json_array_append_new (dest_p, value_p) != 0)
																		{
																			++ index;
																		}
																}
														}
													else
														{
															if (json_array_append_new (dest_p, src_p) != 0)
																{

																}
														}


												}		/* if (value_p) */
										}

								}		/* if (element_name_s) */

						}		/* if (server_response_p) */
					else
						{

						}

				}		/* if (response_s) */
			else
				{

				}

			WipeJSON (op_p);
		}		/* if (op_p) */
	else
		{

		}

	return res_p;
}


