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
