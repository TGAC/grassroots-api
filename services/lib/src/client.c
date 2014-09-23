#include "client.h"
#include "plugin.h"

#include "memory_allocations.h"
#include "linked_list.h"



void InitialiseClient (Client * const client_p,
	const char *(*get_client_name_fn) (void),
	const char *(*get_client_description_fn) (void),
	int (*run_fn) (ClientData *client_data_p, const char * const filename_s, ParameterSet *param_set_p),
	ClientData *data_p)
{
	client_p -> cl_get_client_name_fn = get_client_name_fn;
	client_p -> cl_get_client_description_fn = get_client_description_fn;
	client_p -> cl_run_fn = run_fn;
	client_p -> cl_data_p = data_p;
	
	if (client_p -> cl_data_p)
		{
			client_p -> cl_data_p -> cd_client_p = client_p;
		}
}


int RunClient (Client *client_p, const char * const filename_s, ParameterSet *param_set_p)
{
	return (client_p -> cl_run_fn (client_p -> cl_data_p, filename_s, param_set_p));
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


