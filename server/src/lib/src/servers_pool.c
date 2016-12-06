/*
** Copyright 2014-2016 The Earlham Institute
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
#include <string.h>

#include "servers_pool.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "streams.h"
#include "json_util.h"
#include "json_tools.h"
#include "key_value_pair.h"


#ifdef _DEBUG
	#define SERVERS_POOL_DEBUG	(STM_LEVEL_INFO)
#else
	#define SERVERS_POOL_DEBUG	(STM_LEVEL_NONE)
#endif



static ServersManager *s_servers_manager_p = NULL;


static bool AddPairedServiceFromJSON (ExternalServer *server_p, json_t *paired_service_json_p);


ServersManager *GetServersManager (void)
{
	return s_servers_manager_p;
}


void InitServersManager (ServersManager *manager_p,
                      bool (*add_server_fn) (ServersManager *manager_p, ExternalServer *server_p, ExternalServerSerialiser serialise_fn),
											ExternalServer *(*get_server_fn)  (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn),
											ExternalServer *(*remove_server_fn) (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn),
											LinkedList *(*get_all_servers_fn) (struct ServersManager *manager_p, ExternalServerDeserialiser deserialise_fn),
											bool (*free_servers_manager_fn) (struct ServersManager *manager_p))
{
	uuid_generate (manager_p -> sm_server_id);
	ConvertUUIDToString (manager_p -> sm_server_id, manager_p -> sm_server_id_s);

	manager_p -> sm_add_server_fn = add_server_fn;
	manager_p -> sm_get_server_fn = get_server_fn;
	manager_p -> sm_remove_server_fn = remove_server_fn;
	manager_p -> sm_get_all_servers_fn = get_all_servers_fn;
	manager_p -> sm_free_servers_manager_fn = free_servers_manager_fn;

	s_servers_manager_p = manager_p;
}


const char *GetLocalServerIdAsString (void)
{
	ServersManager *manager_p = GetServersManager ();
	return manager_p -> sm_server_id_s;
}


uuid_t *GetLocalServerId (void)
{
	ServersManager *manager_p = GetServersManager ();
	uuid_t *id_p = & (manager_p -> sm_server_id);

	return id_p;
}


bool AddExternalServerToServersManager (ServersManager *manager_p, ExternalServer *server_p, ExternalServerSerialiser serialise_fn)
{
	return manager_p -> sm_add_server_fn (manager_p, server_p, serialise_fn);
}


ExternalServer *GetExternalServerFromServersManager (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn)
{
	return manager_p -> sm_get_server_fn (manager_p, server_uri_s, deserialise_fn);
}


ExternalServer *RemoveExternalServerFromServersManager (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn)
{
	return manager_p -> sm_remove_server_fn (manager_p, server_uri_s, deserialise_fn);
}


LinkedList *GetAllExternalServersFromServersManager (ServersManager *manager_p, ExternalServerDeserialiser deserialise_fn)
{
	return manager_p -> sm_get_all_servers_fn (manager_p, deserialise_fn);
}


bool FreeServersManager (ServersManager *manager_p)
{
	return manager_p -> sm_free_servers_manager_fn (manager_p);
}


json_t *PairServices (ExternalServer *external_server_p, LinkedList *services_p)
{
	return NULL;
}


json_t *AddExternalServerOperationsToJSON (ServersManager *manager_p, LinkedList *internal_services_p, Operation op)
{
	/* build the request that we will send to each external server */
	json_error_t error;
	json_t *op_p = json_pack ("{s:{s:i}}", SERVER_OPERATIONS_S, OPERATION_ID_S, op);
	json_t *ops_array_p = NULL;

	if (op_p)
		{
			LinkedList *servers_p = GetAllExternalServersFromServersManager (manager_p, DeserialiseExternalServerFromJSON);

			if (servers_p && (servers_p -> ll_size > 0))
				{
					ExternalServerNode *node_p = (ExternalServerNode *) servers_p -> ll_head_p;

					while (node_p)
						{
							ExternalServer *external_server_p = node_p -> esn_server_p;
							const char *response_s = MakeRemoteJsonCallViaConnection (external_server_p -> es_connection_p, op_p);

							if (response_s)
								{
									json_t *server_response_p = json_loads (response_s, 0, &error);

									/*
									 * If the external server has paired services, try and pair them
									 */

									if (server_response_p)
										{
											json_t *default_external_provider_p = json_object_get (server_response_p, SERVER_PROVIDER_S);

											#if SERVERS_POOL_DEBUG >= STM_LEVEL_FINE
											PrintJSONToLog (ops_array_p, "local server json:\n", STM_LEVEL_FINE, __FILE__, __LINE__);
											PrintJSONToLog (default_external_provider_p, "default_external_provider_p:\n", STM_LEVEL_FINE, __FILE__, __LINE__);
											#endif



											/*
											 * The elements to get are dependent on the api call
											 */
											switch (op)
												{
													case OP_LIST_ALL_SERVICES:
													case OP_LIST_INTERESTED_SERVICES:
													case OP_GET_NAMED_SERVICES:
														{
															json_t *src_services_p = json_object_get (server_response_p, SERVICES_NAME_S);

															if (src_services_p)
																{
																	json_t *src_ops_p = json_object_get (src_services_p, SERVER_OPERATIONS_S);

																	if (src_ops_p)
																		{
																			#if SERVERS_POOL_DEBUG >= STM_LEVEL_FINE
																			PrintJSONToLog (src_ops_p, "src_ops:\n", STM_LEVEL_FINE, __FILE__, __LINE__);
																			#endif

																			if (json_is_array (src_ops_p))
																				{
																					json_t *dest_p = json_object ();

																					if (dest_p)
																						{
																							/*
																							 * If the op doesn't have an explicit provider, add
																							 * the default one
																							 */
																							bool success_flag = true;
																							json_t *provider_p = json_object_get (server_response_p, SERVER_PROVIDER_S);

																							if (!provider_p)
																								{
																									provider_p = default_external_provider_p;
																								}

																							if (provider_p)
																								{
																									if (json_object_set (dest_p, SERVER_PROVIDER_S, provider_p) != 0)
																										{
																											success_flag = false;
																										}
																								}

																							if (success_flag)
																								{
																									success_flag = false;

																									if (json_object_set (dest_p, SERVER_OPERATIONS_S, src_ops_p) == 0)
																										{
																											if (json_array_append_new (ops_array_p, dest_p) == 0)
																												{
																													success_flag = true;
																												}
																										}
																								}		/* if (success_flag) */

																							if (!success_flag)
																								{
																									WipeJSON (dest_p);
																								}


																						}		/* if (dest_p) */

																				}		/* if (json_is_array (src_ops_p)) */

																		}		/* if (src_ops_p) */

																}		/* if (src_services_p) */
														}

														break;

													case OP_RUN_KEYWORD_SERVICES:
													case OP_GET_SERVICE_RESULTS:
													//	element_name_s = SERVICE_RESULTS_S;
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

											WipeJSON (server_response_p);

										}		/* if (server_response_p) */
									else
										{

										}

								}		/* if (response_s) */
							else
								{

								}

							node_p = (ExternalServerNode *) node_p -> esn_node.ln_next_p;
						}		/* while (node_p) */

					FreeLinkedList (servers_p);
				}		/* if (servers_p) */

			WipeJSON (op_p);
		}		/* if (op_p) */
	else
		{

		}


	#if SERVERS_POOL_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (ops_array_p, "final ops p:\n", STM_LEVEL_FINE, __FILE__, __LINE__);
	#endif

	return ops_array_p;
}



json_t *GetExternalServerAsJSON (ExternalServer *server_p)
{
	json_error_t err;
	char uuid_s [UUID_STRING_BUFFER_SIZE];
	json_t *server_json_p = NULL;

	/* Start by adding the name, uri and uuid */
	ConvertUUIDToString (server_p -> es_id, uuid_s);
	server_json_p = json_pack_ex (&err, 0, "{s:s,s:s,s:s}", SERVER_NAME_S, server_p -> es_name_s, SERVER_URI_S, server_p -> es_uri_s, SERVER_UUID_S, uuid_s);

	if (server_json_p)
		{
			bool success_flag = true;

			/* Add all paired services */
			if (server_p -> es_paired_services_p -> ll_size > 0)
				{
					json_t *pairs_p = json_array ();

					if (pairs_p)
						{
							KeyValuePairNode *node_p = (KeyValuePairNode *) (server_p -> es_paired_services_p -> ll_head_p);

							while (node_p && success_flag)
								{
									const KeyValuePair *pair_p = node_p -> kvpn_pair_p;
									json_t *pair_json_p = json_pack_ex (&err, 0, "{s:s,s:s}", SERVER_LOCAL_PAIRED_SERVCE_S, pair_p -> kvp_key_s, SERVER_REMOTE_PAIRED_SERVCE_S, pair_p -> kvp_value_s);

									if (pair_json_p)
										{
											if (json_array_append_new (pairs_p, pair_json_p) == 0)
												{
													node_p = (KeyValuePairNode *) (node_p -> kvpn_node.ln_next_p);
												}
											else
												{
													success_flag = false;
												}
										}
									else
										{
											success_flag = false;
										}

								}		/* while (node_p && success_flag) */

							if (success_flag)
								{
									if (json_object_set_new (server_json_p, SERVER_PAIRED_SERVICES_S, pairs_p) != 0)
										{
											success_flag = false;
											json_decref (pairs_p);
										}
								}
							else
								{
									json_decref (pairs_p);
								}

						}		/* if (pairs_p) */

				}		/* if (server_p -> es_paired_services_p -> ll_size > 0) */

			if (success_flag)
				{
					return server_json_p;
				}		/* if (success_flag) */

			json_decref (server_json_p);
		}		/* if (server_json_p) */

	return NULL;
}


ExternalServer *CreateExternalServerFromJSON (const json_t *json_p)
{
	ExternalServer *server_p = NULL;
	const char *name_s = GetJSONString (json_p, SERVER_NAME_S);

	if (name_s)
		{
			const char *uri_s = GetJSONString (json_p, SERVER_URI_S);

			if (uri_s)
				{
					const char *uuid_s = GetJSONString (json_p, SERVER_UUID_S);
					ConnectionType ct = CT_WEB;
					const char *type_s = GetJSONString (json_p, SERVER_CONNECTION_TYPE_S);

					if (type_s)
						{
							if (strcmp (type_s, CONNECTION_RAW_S) == 0)
								{
									ct = CT_RAW;
								}
						}		/* if (type_s) */


					server_p = AllocateExternalServer (name_s, uri_s, uuid_s, ct);

					if (server_p)
						{
							json_t *paired_services_json_p = json_object_get (json_p, SERVER_PAIRED_SERVICES_S);

							if (paired_services_json_p)
								{
									if (json_is_array (paired_services_json_p))
										{
											size_t i;
											json_t *paired_service_json_p;

											json_array_foreach (paired_services_json_p, i, paired_service_json_p)
												{
													if (!AddPairedServiceFromJSON (server_p, paired_service_json_p))
														{
															char *dump_s = json_dumps (paired_service_json_p, JSON_INDENT (2));

															if (dump_s)
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add paired service from \"%s\"", dump_s);
																	free (dump_s);
																}
															else
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add paired service from json");
																}
														}
												}

										}
									else if (json_is_object (paired_services_json_p))
										{
											if (!AddPairedServiceFromJSON (server_p, paired_services_json_p))
												{
													char *dump_s = json_dumps (paired_services_json_p, JSON_INDENT (2));

													if (dump_s)
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add paired service from \"%s\"", dump_s);
															free (dump_s);
														}
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add paired service from json");
														}
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "%s is of type %d", SERVER_PAIRED_SERVICES_S, json_typeof (paired_services_json_p));
										}

								}		/* if (paired_services_json_p) */

						}		/* if (server_p) */
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate external server %s on %s", name_s, uri_s);
						}

				}		/* if (uri_s) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to find uri for external server on %s", name_s);
				}

		}		/* if (name_s) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to find name for external server");
		}

	return server_p;
}


bool AddExternalServerFromJSON (const json_t *json_p)
{
	bool success_flag = false;
	ServersManager *manager_p = GetServersManager ();
	ExternalServer *server_p = CreateExternalServerFromJSON (json_p);

	if (server_p)
		{
			if (AddExternalServerToServersManager (manager_p, server_p, NULL))
				{
					char *uuid_s = GetUUIDAsString (server_p -> es_id);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "Added external server %s on %s to manager with id %s", server_p -> es_name_s, server_p -> es_uri_s, uuid_s);
							FreeUUIDString (uuid_s);
						}
					else
						{
							PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "Added external server %s on %s to manager with id %s", server_p -> es_name_s, server_p -> es_uri_s);
						}

					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add external server %s on %s to manager", server_p -> es_name_s, server_p -> es_uri_s);
				}

			FreeExternalServer (server_p);
		}		/* if (server_p) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate external server from json");
		}

	return success_flag;
}


ExternalServer *AllocateExternalServer (const char *name_s, const char *uri_s, const char *uuid_s, ConnectionType ct)
{
	Connection *connection_p = NULL;

	if (ct == CT_WEB)
		{
			connection_p = AllocateWebServerConnection (uri_s);
		}

	if (connection_p)
		{
			char *copied_name_s = CopyToNewString (name_s, 0, false);

			if (copied_name_s)
				{
					char *copied_uri_s = CopyToNewString (uri_s, 0, false);

					if (copied_uri_s)
						{
							LinkedList *paired_services_p = AllocateLinkedList (FreeKeyValuePairNode);

							if (paired_services_p)
								{
									ExternalServer *server_p = (ExternalServer *) AllocMemory (sizeof (ExternalServer));

									if (server_p)
										{
											bool success_flag = true;

											if (uuid_s)
												{
													if (uuid_parse (uuid_s, server_p -> es_id) != 0)
														{
															success_flag = false;
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create id from \"%\"", uuid_s);
														}
												}
											else
												{
													uuid_generate (server_p -> es_id);
												}

											if (success_flag)
												{
													server_p -> es_paired_services_p = paired_services_p;
													server_p -> es_connection_p = connection_p;
													server_p -> es_name_s = copied_name_s;
													server_p -> es_uri_s = copied_uri_s;

													return server_p;
												}		/* if (success_flag) */

											FreeMemory (server_p);
										}		/* if (server_p) */

									FreeLinkedList (paired_services_p);
								}

							FreeCopiedString (copied_uri_s);
						}		/* if (copied_uri_s) */

					FreeCopiedString (copied_name_s);
				}		/* if (copied_name_s) */

			FreeConnection (connection_p);
		}		/* if (connection_p) */

	return NULL;
}


void FreeExternalServer (ExternalServer *server_p)
{
	FreeCopiedString (server_p -> es_uri_s);
	FreeCopiedString (server_p -> es_name_s);
	FreeConnection (server_p -> es_connection_p);
	FreeLinkedList (server_p -> es_paired_services_p);

	FreeMemory (server_p);
}


ExternalServerNode *AllocateExternalServerNode (ExternalServer *server_p, MEM_FLAG mf)
{
	ExternalServerNode *node_p = (ExternalServerNode *) AllocMemory (sizeof (ExternalServerNode));

	if (node_p)
		{
			switch (mf)
				{
					case MF_SHADOW_USE:
					case MF_SHALLOW_COPY:
						node_p -> esn_server_p = server_p;
						break;

					case MF_DEEP_COPY:
						node_p -> esn_server_p = CopyExternalServer (server_p);
						break;

					default:
						break;
				}

			if (node_p -> esn_server_p)
				{
					node_p -> esn_server_mem = mf;
					node_p -> esn_node.ln_prev_p = NULL;
					node_p -> esn_node.ln_next_p = NULL;
				}
			else
				{
					FreeMemory (node_p);
					node_p = NULL;
				}
		}

	return node_p;
}


void FreeExternalServerNode (ListItem *node_p)
{
	ExternalServerNode *server_node_p = (ExternalServerNode *) node_p;

	switch (server_node_p -> esn_server_mem)
		{
			case MF_DEEP_COPY:
			case MF_SHALLOW_COPY:
				FreeExternalServer (server_node_p -> esn_server_p);
				break;

			default:
				break;
		}

	FreeMemory (server_node_p);
}





json_t *MakeRemoteJSONCallToExternalServer (ExternalServer *server_p, json_t *request_p)
{
	json_t *response_p = NULL;
	const char *result_s = MakeRemoteJsonCallViaConnection (server_p -> es_connection_p, request_p);

	if (result_s)
		{
			json_error_t error;

			response_p = json_loads (result_s, 0, &error);

			if (!response_p)
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to make call to external server %s, error at %d, %d %s\n", server_p -> es_uri_s, error.line, error.column, error.source);
				}
		}		/* if (result_s) */

	return response_p;
}


ExternalServer *CopyExternalServer (const ExternalServer * const src_p)
{
	return NULL;
}


unsigned char *SerialiseExternalServerToJSON (ExternalServer * const external_server_p, unsigned int *value_length_p)
{
	char *serialised_data_p = NULL;
	json_t *server_json_p = GetExternalServerAsJSON (external_server_p);

	if (server_json_p)
		{
			serialised_data_p = json_dumps (server_json_p, JSON_INDENT (2));
			*value_length_p = strlen (serialised_data_p) + 1;
		}		/* if (job_json_p) */

	return ((unsigned char *) serialised_data_p);
}


ExternalServer *DeserialiseExternalServerFromJSON (const unsigned char *raw_json_data_s)
{
	ExternalServer *external_server_p = NULL;
	json_error_t err;
	json_t *server_json_p = json_loads ((char *) raw_json_data_s, 0, &err);

	if (server_json_p)
		{
			external_server_p = CreateExternalServerFromJSON (server_json_p);

			if (!external_server_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create external server from \"%s\"", raw_json_data_s);
				}

			json_decref (server_json_p);
		}		/* if (server_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load external server json from \"%s\"", raw_json_data_s);
		}

	return external_server_p;

}


const char *GetRemotePairedServiceName (const ExternalServer * const src_p, const char * const local_service_name_s)
{
	KeyValuePairNode *node_p = (KeyValuePairNode *) (src_p -> es_paired_services_p -> ll_head_p);

	while (node_p)
		{
			if (strcmp (node_p -> kvpn_pair_p -> kvp_key_s, local_service_name_s) == 0)
				{
					return (node_p -> kvpn_pair_p -> kvp_value_s);
				}

			node_p = (KeyValuePairNode *) (node_p -> kvpn_node.ln_next_p);
		}

	return NULL;
}



static bool AddPairedServiceFromJSON (ExternalServer *server_p, json_t *paired_service_json_p)
{
	const char *local_s = GetJSONString (paired_service_json_p, SERVER_LOCAL_PAIRED_SERVCE_S);

	if (local_s)
		{
			const char *remote_s = GetJSONString (paired_service_json_p, SERVER_REMOTE_PAIRED_SERVCE_S);

			if (remote_s)
				{
					KeyValuePairNode *node_p = AllocateKeyValuePairNode (local_s, remote_s);

					if (node_p)
						{
							LinkedListAddTail (server_p -> es_paired_services_p, (ListItem * const) node_p);
							return true;
						}
				}

		}		/* if (local_s) */

	return false;
}
