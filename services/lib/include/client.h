#ifndef WHEATIS_CLIENT_H
#define WHEATIS_CLIENT_H

#include "wheatis_service_library.h"

#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"
#include "user_details.h"

#include "jansson.h"


/**
 * A datatype detailing the addon services
 * that the wheatis offers. These are the
 * equivalent to iRods microservices.
 */

/* forward declarations */
struct Plugin;
struct Client;


typedef struct ClientData
{
	/** The Client that owns this data. */
	struct Client *cd_client_p;

	struct Connection *cc_connection_p;
} ClientData;


/**
 * A datatype which defines an available client, its capabilities and
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


typedef struct
{
	ListItem cn_node;
	Client *cn_client_p;
} ClientNode;


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_SERVICE_API void InitialiseClient (Client * const client_p,
	const char *(*get_client_name_fn) (ClientData *client_data_p),
	const char *(*get_client_description_fn) (ClientData *client_data_p),
	json_t *(*run_fn) (ClientData *client_data_p),
	json_t *(*display_results_fn) (ClientData *client_data_p, const json_t *response_p),
	int (*add_service_fn) (ClientData *client_data_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s,ParameterSet *params_p),
	ClientData *data_p);

WHEATIS_SERVICE_API json_t *RunClient (Client *client_p);


WHEATIS_SERVICE_API json_t *DisplayResultsInClient (Client *client_p, const json_t *response_p);


WHEATIS_SERVICE_API	int AddServiceToClient (Client *client_p, const char * const service_name_s, const char * const service_description_s, const char * const service_info_uri_s,ParameterSet *params_p);


/**
 * Free a Service and its associated Parameters and ServiceData.

 *
 * @param service_p The Service to free.
 */
WHEATIS_SERVICE_API void FreeClient (Client *client_p);


WHEATIS_SERVICE_API ClientNode *AllocateClientNode (Client *client_p);


WHEATIS_SERVICE_API void FreeClientNode (ListItem *node_p);


WHEATIS_SERVICE_API LinkedList *LoadClients (const char * const clients_path_s, const char * const pattern_s);


WHEATIS_SERVICE_API Client *LoadClient (const char * const clients_path_s, const char * const client_s);


WHEATIS_SERVICE_API Client *GetClientFromPlugin (struct Plugin * const plugin_p);


WHEATIS_SERVICE_API bool DeallocatePluginClient (struct Plugin * const plugin_p);


#ifdef __cplusplus
}
#endif




#endif		/* #ifndef WHEATIS_CLIENT_H */
