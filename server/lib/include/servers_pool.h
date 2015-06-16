/*
 * servers_pool.h
 *
 *  Created on: 12 Jun 2015
 *      Author: tyrrells
 */

#ifndef SERVERS_POOL_H_
#define SERVERS_POOL_H_

#include "uuid/uuid.h"

#include "connection.h"
#include "wheatis_service_manager_library.h"


/**
 * @brief A datatype to allow access to an external WheatIS Server
 */
typedef struct ExternalServer
{
	/** The URI for the Server */
	char *es_uri_s;

	/** The Connection to the ExternalServer */
	Connection *es_connnection_p;

	/**
	 * @brief A unique id to identify the ExternalServer.
	 *
	 * This is only unique for this Server. It is for tracking requests
	 * for given Services.
	 */
	uuid_t es_id;
} ExternalServer;


/* forward declaration */
struct ServersManager;

/**
 * @brief A datatype for monitoring ExternalServers.
 *
 * A ServersManager is used to store details of the ExternalServers that are currently
 * running on a Server. ExternalServers can be added, removed and searched for.
 */
typedef struct ServersManager
{
	/**
	 * @brief Add an ExternalServer to the ServersManager.
	 *
	 * @param manager_p The ServersManager to add the ExternalServer to.
	 * @param server_p The ExternalServer to add.
	 * @return <code>true</code> if the ExternalServer was added successfully,
	 * <code>false</code> otherwise.
	 * @memberof ServersManager
	 * @see AddExternalServerToServersManager
	 */
	bool (*sm_add_server_fn) (struct ServersManager *manager_p, ExternalServer *server_p);

	/**
	 * @brief Find an ExternalServer.
	 *
	 * Get a previously-added ExternalServer.
	 *
	 * @param manager_p The ServersManager to add the ExternalServer to.
	 * @param key The uuid_t for the ExternalServer to search for.
	 * @return A pointer to the matching ExternalServer or <code>NULL</code>
	 * if it could not be found.
	 * @memberof ServersManager
	 * @see GetExternalServerFromServersManager
	 */
	ExternalServer *(*sm_get_server_fn)  (struct ServersManager *manager_p, const uuid_t key);

	/**
 	 * @brief Remove an ExternalServer.
	 *
	 * Remove a previously-added ExternalServer.
	 *
	 * @param manager_p The ServersManager to remove the ExternalServer from.
	 * @param key The uuid_t for the ExternalServer to search for.
	 * @return A pointer to the matching ExternalServer which will have been
	 * removed from the ServersManager or <code>NULL</code>
	 * if it could not be found.
	 * @memberof ServersManager
	 * @see RemoveExternalServerFromServersManager
	 */
	ExternalServer *(*sm_remove_server_fn) (struct ServersManager *manager_p, const uuid_t key);

} ServersManager;


/**
 * Get the current Server-wide ServersManager.
 *
 * @return The ServersManager.
 */
WHEATIS_SERVICE_MANAGER_API ServersManager *GetServersManager (void);


/**
 * @brief Initialise a ServersManager.
 *
 * Set up the callback functions for a given ServersManager.
 *
 * @param manager_p The ServersManager to set up.
 * @param add_server_fn The callback function to set for sm_add_server_fn for the given ServersManager.
 * @param get_server_fn The callback function to set for sm_get_server_fn for the given ServersManager.
 * @param remove_server_fn The callback function to set for sm_remove_server_fn for the given ServersManager.
 */
WHEATIS_SERVICE_MANAGER_API void InitServersManager (ServersManager *manager_p,
                      bool (*add_server_fn) (ServersManager *manager_p, ExternalServer *server_p),
											ExternalServer *(*get_server_fn)  (ServersManager *manager_p, const uuid_t key),
											ExternalServer *(*remove_server_fn) (ServersManager *manager_p, const uuid_t key));


/**
 * @brief Add a ExternalServer to the ServersManager.
 *
 * This is simply a convenience wrapper around sm_add_server_fn
 * for the given ServersManager.
 *
 * @param manager_p The ServersManager to add the ExternalServer to.
 * @param server_p The ExternalServer to add.
 * @return <code>true</code> if the ExternalServer was added successfully,
 * <code>false</code> otherwise.
 * @memberof ServersManager
 * @see sm_add_server_fn
 */
WHEATIS_SERVICE_MANAGER_API bool AddExternalServerToServersManager (ServersManager *manager_p, ExternalServer *server_p);


/**
 * @brief Find a ExternalServer.
 *
 * Get a previously-added ExternalServer. This is simply a convenience
 * wrapper around sm_get_server_fn for the given ServersManager.
 *
 * @param manager_p The ServersManager to search on.
 * @param key The uuid_t for the ExternalServer to search for.
 * @return A pointer to the matching ExternalServers or <code>NULL</code>
 * if it could not be found.
 * @memberof ServersManager
 * @see sm_get_server_fn
 */
WHEATIS_SERVICE_MANAGER_API ExternalServer *GetExternalServerFromServersManager (ServersManager *manager_p, const uuid_t key);


/**
 * @brief Remove a ExternalServer.
 *
 * Remove a previously-added ExternalServer from the ServersManager.
 * This is simply a convenience wrapper around sm_remove_server_fn
 * for the given ServersManager.
 *
 * @param manager_p The ServersManager to search on.
 * @param key The uuid_t for the ExternalServer to search for.
 * @return A pointer to the matching ExternalServers or <code>NULL</code>
 * if it could not be found.
 * @memberof ServersManager
 * @see sm_remove_server_fn
 */
WHEATIS_SERVICE_MANAGER_API ExternalServer *RemoveExternalServerFromServersManager (ServersManager *manager_p, const uuid_t key);


/**
 * Allocate an ExternalServer
 *
 * @param uri_s The URI for the ExternalServer
 * @param ct The ConnectionType of how to connect to the ExternalServer
 * @return A newly-allocated ExternalServer or <code>NULL</code> upon error.
 * @see FreeExternalServer
 * @memberof ExternalServer
 */
WHEATIS_SERVICE_MANAGER_API ExternalServer *AllocateExternalServer (char *uri_s, ConnectionType ct);


/**
 * Free an ExternalServer.
 *
 * @param server_p The ExternalServer to free.
 * @memberof ExternalServer
 */
WHEATIS_SERVICE_MANAGER_API void FreeExternalServer (ExternalServer *server_p);


/**
 * @brief Call a remote server.
 *
 * This will proxy a call to an ExternalServer and will return
 * the response to the user. All of this happens transparently
 * from the user's point of view, the requested service appears
 * to be running on this Server.
 *
 * @param server_p The ExternalServer to proxy the request to.
 * @param request_p The request to proxy.
 * @return The response from the given ExternalServer.
 * @memberof ExternalServer
 */
WHEATIS_SERVICE_MANAGER_API json_t *MakeRemoteJSONCallToExternalServer (ExternalServer *server_p, json_t *request_p);


#endif /* SERVERS_POOL_H_ */