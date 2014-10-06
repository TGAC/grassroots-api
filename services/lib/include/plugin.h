/**@file plugin.h
*/ 

#ifndef PLUGIN_H
#define PLUGIN_H

#include "typedefs.h"
#include "wheatis_service_library.h"
#include "linked_list.h"
#include "memory_allocations.h"

#ifdef __cplusplus
	extern "C" {
#endif


/*********** FORWARD DECLARATION ****************/
struct Service;
struct Client;
struct Handler;


typedef enum PluginStatus
{
	/** No attempt to load a plugin of this type yet. */
	PS_UNSET,

	/** Plugin of this type has been loaded successfully */
	PS_LOADED,

	/** Plugin of this type failed to previously load */
	PS_UNAVAILABLE,

	/** Plugin is in the process of closing */
	PS_CLOSING

} PluginStatus;


typedef enum PluginNature
{
	PN_UNKNOWN = -1,
	
	PN_SERVICE,
	
	PN_CLIENT,
	
	PN_HANDLER,

	PN_NUM_TYPES
} PluginNature;


typedef struct Plugin
{
	/* path of the plugin */
	char *pl_path_s;
	
	MEM_FLAG pl_path_mem;

	PluginStatus pl_status;
	
	PluginNature pl_type;

	struct Service *pl_service_p;
	
	struct Client *pl_client_p;
	
	struct Handler *pl_handler_p;
} Plugin;



/**
 * A ListNode for LinkedLists that also stores
 * a plugin value.
 */
typedef struct PluginListNode
{
	/** The ListNode. */
	ListItem pln_node;

	/** The plugin. */
	Plugin *pln_plugin_p;

	/** How the memory for the Plugin is stored and freed. */
	MEM_FLAG pln_plugin_flag;
} PluginListNode;


/***********************************/
/**********  API METHODS  **********/
/***********************************/





/**
 * Get the string with the platform-specific plugin parts
 * e.g. *.dll for windows, lib*.so for unix. for use in the
 * configuration hash table.
 *
 * @param plugin_p The Plugin to get the config string for.
 * @result A read-only string containing the config
 * string or NULL upon error.
*/
WHEATIS_SERVICE_API const char *GetPluginConfigName (const Plugin * const plugin_p);


/***********************************/
/*********  LOCAL METHODS  *********/
/***********************************/

WHEATIS_UTIL_LOCAL char *MakePluginName (const char * const name);


WHEATIS_UTIL_LOCAL bool InitBasePlugin (Plugin * const plugin_p, const char * const path_p);
WHEATIS_UTIL_LOCAL void ClearBasePlugin (Plugin * const plugin_p);

/**
 * The following functions are platform-specific
 */

WHEATIS_UTIL_LOCAL Plugin *AllocatePlugin (const char * const path_s);
WHEATIS_UTIL_LOCAL void FreePlugin (Plugin * const plugin_data_p);


WHEATIS_UTIL_LOCAL bool OpenPlugin (Plugin * const plugin_p);
WHEATIS_UTIL_LOCAL void ClosePlugin (Plugin * const plugin_p);


WHEATIS_UTIL_LOCAL void ClearPluginPath (Plugin * const plugin_p);


WHEATIS_UTIL_LOCAL bool DeallocatePluginService (Plugin * const plugin_p);



/**
 * Create a new PluginListNode.
 *
 * @param plugin_p The Plugin to store in the newly-created PluginListNode.
 * @return The new PluginListNode or NULL upon error.
 */
WHEATIS_UTIL_LOCAL PluginListNode *AllocatePluginListNode (Plugin * const plugin_p);


/**
 * Free a PluginListNode.
 *
 * @param node_p The ListNode to free.
 */
WHEATIS_UTIL_LOCAL void FreePluginListNode (ListItem * const node_p);



WHEATIS_UTIL_LOCAL bool IsPluginOpen (const Plugin * const plugin_p);


WHEATIS_SERVICE_API struct Service *GetServiceFromPlugin (Plugin * const plugin_p);


WHEATIS_SERVICE_API struct Client *GetClientFromPlugin (Plugin * const plugin_p);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef PLUGIN_H */
