/**@file plugin.h
*/ 

#ifndef PLUGIN_H
#define PLUGIN_H

#include "typedefs.h"
#include "irods_library.h"
#include "linked_list.h"
#include "memory_allocations.h"

#ifdef __cplusplus
	extern "C" {
#endif


/*********** FORWARD DECLARATION ****************/
struct Service;


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


typedef struct Plugin
{
	/* path of the plugin */
	char *pl_path_s;
	MEM_FLAG pl_path_mem;

	PluginStatus pl_status;

	struct Service *pl_service_p;
} Plugin;



/**
 * A ListNode for LinkedLists that also stores
 * a plugin value.
 */
typedef struct PluginListNode
{
	/** The ListNode. */
	ListNode pln_node;

	/** The plugin. */
	Plugin *pln_plugin_p;

	/** How the memory for the Plugin is stored and freed. */
	MEM_FLAG pln_plugin_flag;
} PluginListNode;


/***********************************/
/**********  API METHODS  **********/
/***********************************/


IRODS_LIB_API struct Service *GetServiceFromPlugin (Plugin * const plugin_p);


/**
 * Get the string with the platform-specific plugin parts
 * e.g. *.dll for windows, lib*.so for unix. for use in the
 * configuration hash table.
 *
 * @param plugin_p The Plugin to get the config string for.
 * @result A read-only string containing the config
 * string or NULL upon error.
*/
IRODS_LIB_API const char *GetPluginConfigName (const Plugin * const plugin_p);


/***********************************/
/*********  LOCAL METHODS  *********/
/***********************************/

IRODS_LIB_LOCAL char *MakePluginName (const char * const name);


IRODS_LIB_LOCAL bool InitBasePlugin (Plugin * const plugin_p, const char * const path_p);
IRODS_LIB_LOCAL void ClearBasePlugin (Plugin * const plugin_p);

/**
 * The following functions are platform-specific
 */

IRODS_LIB_LOCAL Plugin *AllocatePlugin (const char * const path_s);
IRODS_LIB_LOCAL void FreePlugin (Plugin * const plugin_data_p);


IRODS_LIB_LOCAL bool OpenPlugin (Plugin * const plugin_p);
IRODS_LIB_LOCAL void ClosePlugin (Plugin * const plugin_p);


IRODS_LIB_LOCAL void ClearPluginPath (Plugin * const plugin_p);


IRODS_LIB_LOCAL bool DeallocatePluginService (Plugin * const plugin_p);



/**
 * Create a new PluginListNode.
 *
 * @param plugin_p The Plugin to store in the newly-created PluginListNode.
 * @return The new PluginListNode or NULL upon error.
 */
IRODS_LIB_LOCAL PluginListNode *AllocatePluginListNode (Plugin * const plugin_p);


/**
 * Free a PluginListNode.
 *
 * @param node_p The ListNode to free.
 */
IRODS_LIB_LOCAL void FreePluginListNode (ListNode * const node_p);



IRODS_LIB_LOCAL bool IsPluginOpen (const Plugin * const plugin_p);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef PLUGIN_H */
