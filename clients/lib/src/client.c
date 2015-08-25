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
	json_t *(*display_results_fn) (ClientData *client_data_p, const json_t *response_p),
	int (*add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p),
	ClientData *data_p,
	Connection *connection_p)
{
	client_p -> cl_get_client_name_fn = get_client_name_fn;
	client_p -> cl_get_client_description_fn = get_client_description_fn;
	client_p -> cl_run_fn = run_fn;
	client_p -> cl_display_results_fn = display_results_fn;
	client_p -> cl_add_service_fn = add_service_fn;
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


int AddServiceToClient (Client *client_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p)
{
	return (client_p -> cl_add_service_fn (client_p -> cl_data_p, service_name_s, service_description_s, service_info_uri_s, params_p));
}


json_t *DisplayResultsInClient (Client *client_p, const json_t *response_p)
{
	#if CLIENT_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (response_p, "DisplayResultsInClient: ", STM_LEVEL_FINE);
	#endif


	return (client_p -> cl_display_results_fn (client_p -> cl_data_p, response_p));
}


void FreeClient (Client *client_p)
{
	FreeMemory (client_p);
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


LinkedList *LoadClients (const char * const clients_path_s, const char * const pattern_s)
{
	LinkedList *clients_p = NULL;

	return clients_p;
}


Client *LoadClient (const char * const clients_path_s, const char * const client_s, Connection *connection_p)
{
	Client *client_p = NULL;
	const char *plugin_s = MakePluginName (client_s);

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

				}		/* if (full_filename_s) */

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
					void (*fn_p) (Client *) = (void (*) (Client *)) symbol_p;

					fn_p (plugin_p -> pl_client_p);

					plugin_p -> pl_client_p = NULL;
					success_flag = true;
				}
		}

	return success_flag;
}
