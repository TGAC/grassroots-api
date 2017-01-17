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
/*
 * client.c
 *
 *  Created on: 3 Jun 2015
 *      Author: tyrrells
 */


#include "client.h"
#include "plugin.h"

#include "memory_allocations.h"
#include "linked_list.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "json_util.h"
#include "json_tools.h"


#ifdef _DEBUG
	#define CLIENT_DEBUG (STM_LEVEL_FINE)
#else
	#define CLIENT_DEBUG (STM_LEVEL_NONE)
#endif


void InitialiseClient (Client * const client_p,
	const char *(*get_client_name_fn) (ClientData *client_data_p),
	const char *(*get_client_description_fn) (ClientData *client_data_p),
	json_t *(*run_fn) (ClientData *client_data_p),
	json_t *(*display_results_fn) (ClientData *client_data_p, json_t *response_p),
	int (*add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const json_t *provider_p, ParameterSet *params_p),
	bool (*free_client_fn) (Client *client_p),
	ClientData *data_p,
	Connection *connection_p)
{
	client_p -> cl_get_client_name_fn = get_client_name_fn;
	client_p -> cl_get_client_description_fn = get_client_description_fn;
	client_p -> cl_run_fn = run_fn;
	client_p -> cl_display_results_fn = display_results_fn;
	client_p -> cl_add_service_fn = add_service_fn;
	client_p -> cl_free_client_fn = free_client_fn;

	client_p -> cl_data_p = data_p;

	if (client_p -> cl_data_p)
		{
			client_p -> cl_data_p -> cd_client_p = client_p;
			client_p -> cl_data_p -> cd_connection_p = connection_p;
			client_p -> cl_data_p -> cd_schema_p = NULL;
		}
}


json_t *RunClient (Client *client_p)
{
	return (client_p -> cl_run_fn (client_p -> cl_data_p));
}


int AddServiceToClient (Client *client_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const json_t *provider_p, ParameterSet *params_p)
{
	return (client_p -> cl_add_service_fn (client_p -> cl_data_p, service_name_s, service_description_s, service_info_uri_s, provider_p, params_p));
}


json_t *DisplayResultsInClient (Client *client_p, json_t *response_p)
{
	#if CLIENT_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (STM_LEVEL_FINE, __FILE__, __LINE__, response_p, "DisplayResultsInClient: ");
	#endif


	return (client_p -> cl_display_results_fn (client_p -> cl_data_p, response_p));
}


void FreeClient (Client *client_p)
{
	Plugin *plugin_p = client_p -> cl_plugin_p;


	if (client_p -> cl_data_p -> cd_schema_p)
		{
			FreeSchemaVersion (client_p -> cl_data_p -> cd_schema_p);
			client_p -> cl_data_p -> cd_schema_p = NULL;
		}

	bool res = client_p -> cl_free_client_fn (client_p);

	plugin_p -> pl_client_p = NULL;
	FreePlugin (plugin_p);
}



/*

typedef struct ClientConnection
{
	int cc_sock_fd;
	uint32 cc_id;
	ByteBuffer *cc_data_buffer_p;
	struct addrinfo *cc_server_p;
} ClientConnection;


 */


ClientNode *AllocateClientNode (Client *client_p)
{
	ClientNode *node_p = (ClientNode *) sizeof (ClientNode);

	if (node_p)
		{
			node_p -> cn_node.ln_prev_p = NULL;
			node_p -> cn_node.ln_next_p = NULL;
			node_p -> cn_client_p = client_p;
		}

	return node_p;
}


void FreeClientNode (ListItem *node_p)
{
	ClientNode *client_node_p = (ClientNode *) node_p;

	FreeMemory (client_node_p);
}


LinkedList *LoadClients (const char * const UNUSED_PARAM (clients_path_s), const char * const UNUSED_PARAM (pattern_s))
{
	LinkedList *clients_p = NULL;

	return clients_p;
}


Client *LoadClient (const char * const clients_path_s, const char * const client_s, Connection *connection_p)
{
	Client *client_p = NULL;
	char *plugin_s = MakePluginName (client_s);

	if (plugin_s)
		{
			char *full_filename_s = MakeFilename (clients_path_s, plugin_s);

			if (full_filename_s)
				{
					Plugin *plugin_p = AllocatePlugin (full_filename_s);

					if (OpenPlugin (plugin_p))
						{
							client_p = GetClientFromPlugin (plugin_p, connection_p);

							if (!client_p)
								{
									ClosePlugin (plugin_p);
								}
						}

					FreeCopiedString (full_filename_s);
				}		/* if (full_filename_s) */

			FreeCopiedString (plugin_s);
		}		/* if (plugin_s) */

	return client_p;
}



//
//	Get Symbol
//
Client *GetClientFromPlugin (Plugin * const plugin_p, Connection *connection_p)
{
	if (!plugin_p -> pl_client_p)
		{
			void *symbol_p = GetSymbolFromPlugin (plugin_p, "GetClient");

			if (symbol_p)
				{
					Client *(*fn_p) (Connection *connection_p) = (Client *(*) (Connection *connection_p)) symbol_p;

					plugin_p -> pl_client_p = fn_p (connection_p);

					if (plugin_p -> pl_client_p)
						{
							plugin_p -> pl_client_p -> cl_plugin_p = plugin_p;
							plugin_p -> pl_type = PN_CLIENT;
						}
				}
		}

	return plugin_p -> pl_client_p;
}

bool DeallocatePluginClient (Plugin * const plugin_p)
{
	bool success_flag = (plugin_p -> pl_client_p == NULL);

	if (!success_flag)
		{
			void *symbol_p = GetSymbolFromPlugin (plugin_p, "ReleaseClient");

			if (symbol_p)
				{
					bool (*fn_p) (Client *) = (bool (*) (Client *)) symbol_p;
					bool res = fn_p (plugin_p -> pl_client_p);

					plugin_p -> pl_client_p = NULL;
					success_flag = true;
				}
		}

	return success_flag;
}



void SetClientSchema (Client *client_p, SchemaVersion *sv_p)
{
	if (client_p -> cl_data_p -> cd_schema_p)
		{
			FreeSchemaVersion (client_p -> cl_data_p -> cd_schema_p);
		}

	client_p -> cl_data_p -> cd_schema_p = sv_p;
}


void GetAllServicesInClient (Client *client_p, UserDetails *user_p)
{
	json_t *req_p = GetAvailableServicesRequest (user_p, client_p -> cl_data_p -> cd_schema_p);

	if (req_p)
		{
			json_t *response_p = NULL;

			if (!AddCredentialsToJson (req_p, user_p))
				{
					printf ("Failed to add credentials\n");
				}

			response_p = MakeRemoteJsonCall (req_p, client_p -> cl_data_p -> cd_connection_p);

			if (response_p)
				{
					json_t *run_services_response_p = NULL;

					SchemaVersion *server_schema_p = GetSchemaVersionFromJSON (response_p);

					if (server_schema_p)
						{
							SetClientSchema (client_p, server_schema_p);
						}

					run_services_response_p = ShowServices (response_p, client_p, user_p, client_p -> cl_data_p -> cd_connection_p);

					if (run_services_response_p)
						{
							json_decref (run_services_response_p);
						}

					json_decref (response_p);
				}		/* if (response_p) */

			json_decref (req_p);
		}		/* if (req_p) */

}


void GetInterestedServicesInClient (Client *client_p, const char * const protocol_s, const char * const query_s, UserDetails *user_p)
{
	if (protocol_s && query_s)
		{
			json_t *req_p = GetInterestedServicesRequest (user_p, protocol_s, query_s, client_p -> cl_data_p -> cd_schema_p);

			if (req_p)
				{
					json_t *response_p = MakeRemoteJsonCall (req_p, client_p -> cl_data_p -> cd_connection_p);

					if (response_p)
						{
							json_t *service_response_p = ShowServices (response_p, client_p, user_p, client_p -> cl_data_p -> cd_connection_p);

							if (service_response_p)
								{

									json_decref (service_response_p);
								}

							json_decref (response_p);
						}		/* if (response_p) */

					json_decref (req_p);
				}		/* if (req_p) */

		}

}



//void CallClient (Client *client_p, const uint32 api_id, UserDetails *user_p)
//{
//	Connection *connection_p = client_p -> cl_data_p -> cd_connection_p;
//
//	switch (api_id)
//		{
//			case OP_LIST_ALL_SERVICES:
//				{
//
//				}		/* case OP_LIST_ALL_SERVICES */
//				break;
//
//
//			case OP_IRODS_MODIFIED_DATA:
//				{
//					json_t *req_p = GetModifiedFilesRequest (user_p, from_s, to_s, client_p -> cl_data_p -> cd_schema_p);
//
//					if (req_p)
//						{
//							json_t *response_p = MakeRemoteJsonCall (req_p, connection_p);
//
//							if (response_p)
//								{
//
//									json_decref (response_p);
//								}
//
//							json_decref (req_p);
//						}		/* if (req_p) */
//				}
//				break;
//
//			case OP_LIST_INTERESTED_SERVICES:
//				{
//					if (protocol_s && query_s)
//						{
//							json_t *req_p = GetInterestedServicesRequest (user_p, protocol_s, query_s, client_p -> cl_data_p -> cd_schema_p);
//
//							if (req_p)
//								{
//									json_t *response_p = MakeRemoteJsonCall (req_p, connection_p);
//
//									if (response_p)
//										{
//											json_t *service_response_p = ShowServices (response_p, client_p, user_p, connection_p);
//
//											if (service_response_p)
//												{
//
//													json_decref (service_response_p);
//												}
//
//											json_decref (response_p);
//										}		/* if (response_p) */
//
//									json_decref (req_p);
//								}		/* if (req_p) */
//
//						}
//				}
//				break;
//
//			case OP_RUN_KEYWORD_SERVICES:
//				{
//					if (query_s)
//						{
//							json_t *req_p = GetKeywordServicesRequest (user_p, query_s, client_p -> cl_data_p -> cd_schema_p);
//
//							if (req_p)
//								{
//									json_t *response_p = MakeRemoteJsonCall (req_p, connection_p);
//
//									if (response_p)
//										{
//											if (DisplayResultsInClient (client_p, response_p))
//												{
//
//												}
//
//											json_decref (response_p);
//										}		/* if (response_p) */
//									else
//										{
//											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "MakeRemoteJsonCall failed");
//										}
//
//									json_decref (req_p);
//								}		/* if (req_p) */
//							else
//								{
//									PrintErrors (STM_LEVEL_SEVERE,__FILE__, __LINE__, "GetKeywordServicesRequest failed for %s", query_s);
//								}
//
//						}		/* if (query_s) */
//				}
//				break;
//
//			case OP_GET_NAMED_SERVICES:
//				{
//					json_t *req_p = GetNamedServicesRequest (user_p, query_s, client_p -> cl_data_p -> cd_schema_p);
//
//					if (req_p)
//						{
//							json_t *response_p = MakeRemoteJsonCall (req_p, connection_p);
//
//							if (response_p)
//								{
//									json_t *shown_services_p = ShowServices (response_p, client_p, user_p, connection_p);
//
//									if (shown_services_p)
//										{
//											json_decref (shown_services_p);
//										}		/* if (shown_services_p) */
//
//									json_decref (response_p);
//								}		/* if (response_p) */
//
//							json_decref (req_p);
//						}		/* if (req_p) */
//				}
//				break;
//
//			case OP_CHECK_SERVICE_STATUS:
//				{
//					json_t *req_p = GetCheckServicesRequest (user_p, query_s, client_p -> cl_data_p -> cd_schema_p);
//
//					if (req_p)
//						{
//							json_t *response_p = MakeRemoteJsonCall (req_p, connection_p);
//
//							if (response_p)
//								{
//									char *dump_s = json_dumps (response_p, JSON_INDENT (2));
//
//									if (dump_s)
//										{
//											PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "%s", dump_s);
//											free (dump_s);
//										}
//
//									json_decref (response_p);
//								}		/* if (response_p) */
//
//							json_decref (req_p);
//						}		/* if (req_p) */
//				}
//				break;
//
//			default:
//				break;
//		}		/* switch (api_id) */
//
//}


json_t *ShowServices (json_t *response_p, Client *client_p, UserDetails * UNUSED_PARAM (user_p), Connection * UNUSED_PARAM (connection_p))
{
	json_t *client_results_p = NULL;
	json_t *service_defs_p = json_object_get (response_p, SERVICES_NAME_S);

	#if STANDALONE_CLIENT_DEBUG >= STM_LEVEL_FINER
		{
			const char *args_s = "res:";
			PrintJSONToLog (STM_LEVEL_FINER, __FILE__, __LINE__, response_p, args_s);
		}
	#endif

	if (service_defs_p)
		{
			if (json_is_array (service_defs_p))
				{
					const size_t num_services = json_array_size (service_defs_p);
					size_t i = 0;

					for (i = 0; i < num_services; ++ i)
						{
							json_t *service_json_p = json_array_get (service_defs_p, i);
							json_t *ops_p = json_object_get (service_json_p, SERVER_OPERATIONS_S);
							json_t *provider_p = json_object_get (service_json_p, SERVER_PROVIDER_S);


							#if STANDALONE_CLIENT_DEBUG >= STM_LEVEL_FINER
							PrintJSONToLog (STANDALONE_CLIENT_DEBUG, __FILE__, __LINE__, service_json_p, "next service:\n");
							#endif

							if (ops_p)
								{
									if (json_is_array (ops_p))
										{
											size_t j;
											json_t *op_p;

											json_array_foreach (ops_p, j, op_p)
												{
													AddServiceDetailsToClient (client_p, op_p, provider_p);
												}
										}
									else
										{
											AddServiceDetailsToClient (client_p, ops_p, provider_p);
										}
								}

						}		/* for (i = 0; i < num_services; ++ i) */

					/* Get the results of the user's configuration */
					client_results_p = RunClient (client_p);
				}		/* if (json_is_array (service_defs_p)) */

		}		/* if (service_defs_p) */

	return client_results_p;
}


void GetNamedServicesInClient (Client *client_p, const char * const service_s, UserDetails *user_p)
{
	if (service_s)
		{
			json_t *req_p = GetNamedServicesRequest (user_p, service_s, client_p -> cl_data_p -> cd_schema_p);

			if (req_p)
				{
					json_t *response_p = MakeRemoteJsonCall (req_p, client_p -> cl_data_p -> cd_connection_p);

					if (response_p)
						{
							json_t *service_response_p = ShowServices (response_p, client_p, user_p, client_p -> cl_data_p -> cd_connection_p);

							if (service_response_p)
								{
									json_decref (service_response_p);
								}

							json_decref (response_p);
						}		/* if (response_p) */

					json_decref (req_p);
				}		/* if (req_p) */

		}

}


int AddServiceDetailsToClient (Client *client_p, json_t *service_json_p, const json_t *provider_p)
{
	int res = -1;
	const char *op_name_s = GetJSONString (service_json_p, OPERATION_ID_S);

#if CLIENT_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (STANDALONE_CLIENT_DEBUG, __FILE__, __LINE__, service_json_p, "client received service:\n");
#endif

	if (op_name_s)
		{
			const char *service_description_s = GetJSONString (service_json_p, SERVICES_DESCRIPTION_S);

			if (service_description_s)
				{
					ParameterSet *params_p = CreateParameterSetFromJSON (service_json_p, false);

					if (params_p)
						{
							const char *service_info_uri_s = GetJSONString (service_json_p, OPERATION_INFORMATION_URI_S);

							res = AddServiceToClient (client_p, op_name_s, service_description_s, service_info_uri_s, provider_p, params_p);
						}		/* if (params_p) */

				}		/* if (service_description_s) */

		}		/* if (service_name_s) */

	return res;
}


