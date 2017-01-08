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

/**
 * @file
 * @brief
 */
/**
 * @addtogroup Client
 */
#ifndef GRASSROOTS_CLIENT_H
#define GRASSROOTS_CLIENT_H

#include "grassroots_client_library.h"

#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "user_details.h"
#include "request_tools.h"
#include "schema_version.h"


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

	/** The SchemaVerision to use. */
	SchemaVersion *cd_schema_p;

} ClientData;


/**
 * @brief A datatype which defines an available client, its capabilities and
 * its parameters.
 *
 * Client defines the interface that an instance needs to fulfil to access a
 * Server. This is achieved using a variety of callback functions.
 */
typedef struct Client
{
	/**
	 * The platform-specific Plugin that the code for the Client is
	 * stored in.
	 */
	struct Plugin *cl_plugin_p;

	/**
	 * Add a Service to be displayed in a Client.
	 *
	 * @param client_data_p The Client-specific data.
	 * @param service_name_s The name of the Service to add.
	 * @param service_description_s An optional description of the Service.
	 * This can be <code>NULL</code>.
	 * @param service_info_uri_s An optional uri for the Service.
	 * This can be <code>NULL</code>.
	 * @param provider_p The JSON fragment with the details of the provider of the Service.
	 * @param params_p The ParameterSet for the given Service.
	 * @return 0 upon success, non-zero on error.
	 */
	int (*cl_add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const json_t *provider_p, ParameterSet *params_p);

	/**
	 * @brief Launch the Client.
	 *
	 * Run the Client.
	 * @param client_data_p
	 * @return Any resultant data after the Client has finished running.
	 */
	json_t *(*cl_run_fn) (ClientData *client_data_p);

 	/**
 	 * @brief Function to get the user-friendly name of the Client.
 	 *
 	 * @param client_data_p The ClientData belonging to this Client.
 	 * @return The name of the Client to display to the user.
 	 */
	const char *(*cl_get_client_name_fn) (ClientData *client_data_p);


 	/**
 	 * @brief Function to get the user-friendly description of the Client.
 	 *
 	 * @param client_data_p The ClientData belonging to this Client.
 	 * @return The name of the Client to display to the user.
 	 */
	const char *(*cl_get_client_description_fn) (ClientData *client_data_p);


	/**
	 * @brief Display the results of running 1 or more Services in the Client.
	 *
	 * The Client will process the response from the Server and show the results
	 * to the user.
	 * @param client_data_p The base data for this Client.
	 * @param response_p The response from the Server detailing the results from the
	 * Services that have run on the Server.
	 * @return Any resultant data after the Client has finished displaying the results.
	 */
	json_t *(*cl_display_results_fn) (ClientData *client_data_p, json_t *response_p);


	/**
	 * Callback function to free this Client.
	 *
	 * @param client_p This Client.
	 * @return <code>true</code> if the Client was freed successfully,
	 * <code>false</code> otherwise.
	 */
	bool (*cl_free_client_fn) (struct Client *client_p);


	/**
	 * @brief Any custom data that the Client needs to store.
	 *
	 * A pointer to a base ClientData that can be cast to an appropriate subclass by the
	 * Client in use.
	 */
	ClientData *cl_data_p;

} Client;


/**
 * @brief A datatype for storing a Client on a LinkedList.
 *
 * @extends ListItem
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
 * @brief Initialise a Client.
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
 * @param free_client_fn The function to free the Client.
 * @see cl_free_client_fn
 * @param data_p The ClientData for this Client.
 * @param connection_p The Connection that this Client will use.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API void InitialiseClient (Client * const client_p,
	const char *(*get_client_name_fn) (ClientData *client_data_p),
	const char *(*get_client_description_fn) (ClientData *client_data_p),
	json_t *(*run_fn) (ClientData *client_data_p),
	json_t *(*display_results_fn) (ClientData *client_data_p, json_t *response_p),
	int (*add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const json_t *provider_p, ParameterSet *params_p),
	bool (*free_client_fn) (Client *client_p),
	ClientData *data_p,
	Connection *connection_p);

/**
 * Run the Client.
 *
 * This calls the cl_run_fn for the Client.
 *
 * @param client_p The Client to run.
 * @return Any resultant data after the Client has finished displaying the results.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API json_t *RunClient (Client *client_p);


/**
 * Display the results of 1 or more Services in the Client.
 *
 * This calls the cl_display_results_fn for the Client.
 *
 * @param client_p The Client to display the results.
 * @param response_p The JSON response from a Server that ran the Services.
 * @return Any resultant data after the Client has finished displaying the results.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API json_t *DisplayResultsInClient (Client *client_p, json_t *response_p);


/**
 * Add a Service to be displayed in a Client.
 *
 * @param client_p The Client to display the Service in.
 * @param service_name_s The name of the Service.
 * @param service_description_s The description of the Service.
 * @param service_info_uri_s An optional URI for the Service. This can be <code>NULL</code>.
 * @param provider_p The JSON fragment with the details of the provider of the Service.
 * @param params_p The ParameterSet for the Service.
 * @return 0 upon success, non-zero on error.
 * @memberof Client
 * @see cl_add_service_fn
 */
GRASSROOTS_CLIENT_API	int AddServiceToClient (Client *client_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s, const json_t *provider_p, ParameterSet *params_p);


/**
 * Free a Client and its associated data.
 *
 * @param client_p The Client to free.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API void FreeClient (Client *client_p);


/**
 * Allocate a ClientNode and set it to contain the given Client.
 *
 * @param client_p The Client that the ClientNode will refer to.
 * @return A newly-allocated ClientNode or <code>NULL</code> upon error.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API ClientNode *AllocateClientNode (Client *client_p);


/**
 * Free a ClientNode and its associated Client.
 *
 * @param node_p The ClientNode to free. It will call FreeClient on the node's data.
 * @memberof Client
 * @see FreeClient
 */
GRASSROOTS_CLIENT_API void FreeClientNode (ListItem *node_p);


/**
 * Load all Clients from a given directory.
 *
 * @param clients_path_s The directory where the Client modules are stored.
 * @param pattern_s The platform-dependent pattern for a Client module.
 * @return A newly-allocated LinkedList of ClientNodes or <code>NULL</code>
 * if none could be loaded.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API LinkedList *LoadClients (const char * const clients_path_s, const char * const pattern_s);


/**
 * Load a Client.
 *
 * @param clients_path_s The directory where the Client modules are stored.
 * @param client_s The platform-independent name of the Client. For example
 * "my_client" would automatically expand to libmy_client.so on Unix, my_client.dll
 * on Windows, <i>etc.</i>
 * @param connection_p The connection to a Grassroots Server.
 * @return The successfully loaded client upon success or <code>NULL</code> upon error.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API Client *LoadClient (const char * const clients_path_s, const char * const client_s, Connection *connection_p);


/**
 * Get a Client from a loaded Plugin.
 *
 * @param plugin_p The Plugin which contains the Client.
 * @param connection_p The connection to a Grassroots Server.
 * @return The successfully loaded client upon success or <code>NULL</code> upon error.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API Client *GetClientFromPlugin (struct Plugin * const plugin_p, Connection *connection_p);


/**
 * Free a Client loaded from a given Plugin.
 *
 * @param plugin_p The Plugin from which the Client was loaded.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API bool DeallocatePluginClient (struct Plugin * const plugin_p);


/**
 * Set the SchemaVersion that a Client will use.
 *
 * @param client_p The Client that will use the given SchemaVersion.
 * @param sv_p The SchemeVersion that the Client will use. The Client
 * will free this when FreeClient is called.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API void SetClientSchema (Client *client_p, SchemaVersion *sv_p);


/**
 * Get all available Services in a Client from its connected Server.
 *
 * @param client_p The Client to get all of the Services for.
 * @param user_p The current user in case of restricted user access or any of the Services.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API void GetAllServicesInClient (Client *client_p, UserDetails *user_p);


/**
 * Get all interested Services for given Resource in a Client.
 *
 * @param client_p The Client to get all of the Services for.
 * @param protocol_s The protocol of the Resource.
 * @param query_s The name of the Resource.
 * @param user_p The current user in case of restricted user access or any of the Services.
 * This can be <code>NULL</code>.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API void GetInterestedServicesInClient (Client *client_p, const char * const protocol_s, const char * const query_s, UserDetails *user_p);


/**
 * Load a JSON fragment containing a set of Services into a Client.
 *
 * @param response_p The JSON fragment containing a set of Services to show.
 * @param client_p The Client to display the Services with.
 * @param user_p The current user in case of restricted user access or any of the Services.
 * This can be <code>NULL</code>.
 * @param connection_p The connection to a Grassroots Server.
 * @memberof Client
 * @return The JSON fragment detailing any messages from the Client after it has been run.
 */
GRASSROOTS_CLIENT_API json_t *ShowServices (json_t *response_p, Client *client_p, UserDetails *user_p, Connection *connection_p);


/**
 * Get a named Service in a Client from its connected Server.
 *
 * @param client_p The Client to get all of the Services for.
 * @param service_s The name of the Service to get.
 * @param user_p The current user in case of restricted user access or any of the Services.
 * @memberof Client
 */
GRASSROOTS_CLIENT_API void GetNamedServicesInClient (Client *client_p, const char * const service_s, UserDetails *user_p);


/**
 * Add a single Service to a Client from its JSON definition.
 *
 * @param client_p The Client to add the Service to.
 * @param service_json_p The JSON fragment for the Service.
 * @param provider_p A JSON fragment detailing the Proivider for this Service.
 * @return 0 upon success, non-zero on error.
 * @see AddServiceToClient
 */
GRASSROOTS_CLIENT_LOCAL int AddServiceDetailsToClient (Client *client_p, json_t *service_json_p, const json_t *provider_p);


#ifdef __cplusplus
}
#endif




#endif		/* #ifndef GRASSROOTS_CLIENT_H */
