#include "client.h"
#include "plugin.h"

#include "memory_allocations.h"
#include "linked_list.h"
#include "string_utils.h"


void InitialiseClient (Client * const client_p,
	const char *(*get_client_name_fn) (void),
	const char *(*get_client_description_fn) (void),
	json_t *(*run_fn) (ClientData *client_data_p),
	int (*add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p),
	ClientData *data_p)
{
	client_p -> cl_get_client_name_fn = get_client_name_fn;
	client_p -> cl_get_client_description_fn = get_client_description_fn;
	client_p -> cl_run_fn = run_fn;
	client_p -> cl_add_service_fn = add_service_fn;
	client_p -> cl_data_p = data_p;

	if (client_p -> cl_data_p)
		{
			client_p -> cl_data_p -> cd_client_p = client_p;
		}
}


json_t *RunClient (Client *client_p)
{
	return (client_p -> cl_run_fn (client_p -> cl_data_p));
}


int AddServiceToClient (Client *client_p, const char * const service_name_s, const char * const service_description_s, ParameterSet *params_p)
{
	return (client_p -> cl_add_service_fn (client_p -> cl_data_p, service_name_s, service_description_s, params_p));	
}



void FreeClient (Client *client_p)
{
	FreeMemory (client_p);	
}


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


Client *LoadClient (const char * const clients_path_s, const char * const client_s)
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
							client_p = GetClientFromPlugin (plugin_p);

							if (!client_p)
								{
									ClosePlugin (plugin_p);
								}
						}
					
				}		/* if (full_filename_s) */				

		}		/* if (plugin_s) */

	return client_p;		
}



