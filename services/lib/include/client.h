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


	int (*cl_run_fn) (ClientData *client_data_p, const char * const filename_s, ParameterSet *param_set_p);

 	/**
 	 * Function to get the user-friendly name of the Client.
 	 */
	const char *(*cl_get_client_name_fn) (void);

	/**
	 * Function to get the user-friendly description of the Client.
	 */
	const char *(*cl_get_client_description_fn) (void);

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
	const char *(*get_service_name_fn) (void),
	const char *(*get_service_description_fn) (void),
	int (*run_fn) (ClientData *client_data_p, const char * const filename_s, ParameterSet *param_set_p),
	ClientData *data_p);

WHEATIS_SERVICE_API int RunClient (Client *client_p, const char * const filename_s, ParameterSet *param_set_p);

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




#ifdef __cplusplus
}
#endif




#endif		/* #ifndef WHEATIS_CLIENT_H */
