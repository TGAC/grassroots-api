#include "plugin.h"
#include "service.h"

#include "string_utils.h"


bool InitBasePlugin (Plugin * const plugin_p, const char * const path_s)
{
	bool success_flag = false;

	plugin_p -> pl_path_s = NULL;
	plugin_p -> pl_path_mem = MF_ALREADY_FREED;
	plugin_p -> pl_service_p = NULL;
	plugin_p -> pl_client_p = NULL;
	plugin_p -> pl_handler_p = NULL;
	plugin_p -> pl_status = PS_UNSET;

	if (path_s)
		{
			plugin_p -> pl_path_s = CopyToNewString (path_s, 0, true);

			if (plugin_p -> pl_path_s)
				{
					plugin_p -> pl_path_mem = MF_DEEP_COPY;
				}
			else
				{
					success_flag = false;
				}
		}

	return success_flag;
}






void ClearPluginPath (Plugin * const plugin_p)
{
	if (plugin_p -> pl_path_s)
		{
			switch (plugin_p -> pl_path_mem)
				{
					case MF_DEEP_COPY:
					case MF_SHALLOW_COPY:
						FreeMemory (plugin_p -> pl_path_s);
						break;

					case MF_SHADOW_USE:
					default:
						break;
				}

			plugin_p -> pl_path_s = NULL;
			plugin_p -> pl_path_mem = MF_ALREADY_FREED;
		}
}


void ClearBasePlugin (Plugin * const plugin_p)
{
	/* this must be before resetting plugin_p -> pl_type */
	DeallocatePluginService (plugin_p);

	ClearPluginPath (plugin_p);
}


void FreePluginListNode (ListItem * const node_p)
{
	PluginListNode *plugin_node_p = (PluginListNode *) node_p;
	Plugin *plugin_p = plugin_node_p -> pln_plugin_p;

	plugin_node_p -> pln_plugin_p = NULL;
	ClosePlugin (plugin_p);
	FreeMemory (plugin_node_p);
}


PluginListNode *AllocatePluginListNode (Plugin * const plugin_p)
{
	PluginListNode *node_p = (PluginListNode *) AllocMemory (sizeof (PluginListNode));

	if (node_p)
		{
			node_p -> pln_plugin_p = (Plugin *) plugin_p;
		}

	return node_p;
}
