/**
 * @addtogroup Client
 */
#ifndef WHEATIS_CLIENT_H
#define WHEATIS_CLIENT_H

#include "wheatis_client_library.h"

#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "user_details.h"
#include "request_tools.h"

#include "jansson.h"



/* forward declarations */
struct Plugin;
struct Client;


/**
 * @brief The base structure for any Client-specific data.
 */
typedef struct ClientData
{
	/** The Client that owns this data. */
	struct Client *cd_client_p;

	/** The Connection for this client. */
	struct Connection *cd_connection_p;
} ClientData;


/**
 * @brief A datatype which defines an available client, its capabilities and
 * its parameters.
 */
typedef struct Client
{
	/**
	 * The platform-specific plugin that the code for the Client is
	 * stored in.
	 */
	struct Plugin *cl_plugin_p;

	int (*cl_add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p);

	json_t *(*cl_run_fn) (ClientData *client_data_p);

 	/**
 	 * Function to get the user-friendly name of the Client.
 	 */
	const char *(*cl_get_client_name_fn) (ClientData *client_data_p);

	/**
	 * Function to get the user-friendly description of the Client.
	 */
	const char *(*cl_get_client_description_fn) (ClientData *client_data_p);


	/**
	 * Function to get the user-friendly description of the Client.
	 */
	json_t *(*cl_display_results_fn) (ClientData *client_data_p, const json_t *response_p);

	/**
	 * Any custom data that the Client needs to store.
	 */
	ClientData *cl_data_p;

} Client;


/**
 * @brief A datatype for storing a Client on a LinkedList.
 */
typedef struct
{
	/** The ListItem */
	ListItem cn_node;

	/** A pointer to the Client. */
	Client *cn_client_p;
} ClientNode;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialise a Client.
 *
 * @param client_p The Client to iniitalise.
 * @param get_client_name_fn The function to get the name of the Client.
 * @see cl_get_client_name_fn
 * @param get_client_description_fn The function to get the description of the Client.
 * @see cl_get_client_description_fn
 * @param run_fn The function to run the Client.
 * @see cl_run_fn
 * @param display_results_fn The function to use to display the server results in the Client.
 * @see cl_display_results_fn
 * @param add_service_fn The function to get the name of the client.
 * @see cl_add_service_fn
 * @param data_p The ClientData for this Client.
 * @param connection_p The Connection that this Client will use.
 * @memberof Client
 */
WHEATIS_CLIENT_API void InitialiseClient (Client * const client_p,
	const char *(*get_client_name_fn) (ClientData *client_data_p),
	const char *(*get_client_description_fn) (ClientData *client_data_p),
	json_t *(*run_fn) (ClientData *client_data_p),
	json_t *(*display_results_fn) (ClientData *client_data_p, const json_t *response_p),
	int (*add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s,ParameterSet *params_p),
	ClientData *data_p,
	Connection *connection_p);

/**
 * Run the Client.
 *
 * This calls the cl_run_fn for the Client.
 *
 * @param client_p The Client to run.
 * @memberof Client
 */
WHEATIS_CLIENT_API json_t *RunClient (Client *client_p);


/**
 * Display the results of 1 or more Services in the Client.
 *
 * This calls the cl_display_results_fn for the Client.
 *
 * @param client_p The Client to display the results.
 * @param response_p The JSON response from a Server that ran the Services.
 * @memberof Client
 */
WHEATIS_CLIENT_API json_t *DisplayResultsInClient (Client *client_p, const json_t *response_p);


/**
 * Add a Service to be displayed in a Client.
 *
 * @param client_p The Client to display the Service in.
 * @param service_name_s The name of the Service.
 * @param service_description_s The description of the Service.
 * @param service_info_uri_s An optional URI for the Service. This can be <code>NULL</code>.
 * @param params_p The ParameterSet for the Servioce.
 * @return 0 upon success, non-zero on error.
 * @memberof Client
 *
 */
WHEATIS_CLIENT_API	int AddServiceToClient (Client *client_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, ParameterSet *params_p);


/**
 * Free a Client and its associated data.
 *
 * @param client_p The Client to free.
 * @memberof Client
 */
WHEATIS_CLIENT_API void FreeClient (Client *client_p);


/**
 * Allocate a ClientNode and set it to contain the given Client.
 *
 * @param client_p The Client that the ClientNode will refer to.
 * @return A newly-allocated ClientNode or <code>NULL</code> upon error.
 * @memberof Client
 */
WHEATIS_CLIENT_API ClientNode *AllocateClientNode (Client *client_p);


/**
 * Free a ClientNode and its associated Client.
 *
 * @param node_p The ClientNode to free. It will call FreeClient on the node's data.
 * @memberof Client
 * @see FreeClient
 */
WHEATIS_CLIENT_API void FreeClientNode (ListItem *node_p);


/**
 * Load all Clients from a given directory.
 *
 * @param clients_path_s The directory where the Client modules are stored.
 * @param pattern_s The platform-dependent pattern for a Client module.
 * @return A newly-allocated LinkedList of ClientNodes or <code>NULL</code>
 * if none could be loaded.
 * @memberof Client
 */
WHEATIS_CLIENT_API LinkedList *LoadClients (const char * const clients_path_s, const char * const pattern_s);


/**
 * Load a Client.
 *
 * @param clients_path_s The directory where the Client modules are stored.
 * @param client_s The platform-independent name of the Client. For example
 * "my_client" would automatically expand to libmy_client.so on Unix, my_client.dll
 * on Windows, <i>etc.</i>
 * @param connection_p The connection to a WheatIS Server.
 * @return The successfully loaded client upon success or <code>NULL</code> upon error.
 * @memberof Client
 */
WHEATIS_CLIENT_API Client *LoadClient (const char * const clients_path_s, const char * const client_s, Connection *connection_p);


/**
 * Get a Client from a loaded Plugin.
 *
 * @param plugin_p The Plugin which contains the Client.
 * @param connection_p The connection to a WheatIS Server.
 * @return The successfully loaded client upon success or <code>NULL</code> upon error.
 * @memberof Client
 */
WHEATIS_CLIENT_API Client *GetClientFromPlugin (struct Plugin * const plugin_p, Connection *connection_p);


/**
 * Free a Client loaded from a given Plugin.
 *
 * @param plugin_p The Plugin from which the Client was loaded.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * @memberof Client
 */
WHEATIS_CLIENT_API bool DeallocatePluginClient (struct Plugin * const plugin_p);


#ifdef __cplusplus
}
#endif




#endif		/* #ifndef WHEATIS_CLIENT_H */
