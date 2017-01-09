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
#include "grassroots_service_manager_library.h"
#include "linked_list.h"
#include "operation.h"
#include "memory_allocations.h"
#include "string_utils.h"

/**
 * @brief A datatype to allow access to an external Grassroots Server
 *
 * This stores the information needed to consolidate the Services
 * available on other Grassroots Servers
 * @ingroup server_group
 */
typedef struct ExternalServer
{
	/** The user-friendly name of the server */
	char *es_name_s;

	/** The URI for the Server */
	char *es_uri_s;

	/** The Connection to the ExternalServer */
	Connection *es_connection_p;

	/**
	 * @brief A unique id to identify the ExternalServer.
	 *
	 * This is only unique for this Server. It is for tracking requests
	 * for given Services.
	 */
	uuid_t es_id;


	/**
	 * A List of KeyValuePairNodes where the key is
	 * the name of the Service on the local Server
	 * and the value is the name of the Service on
	 * this ExternalServer
	 */
	LinkedList *es_paired_services_p;


} ExternalServer;


/**
 * A datatype for storing ExternalServers
 * in a LinkedList.
 *
 * @extends ListItem
 * @ingroup server_group
 */
typedef struct ExternalServerNode
{
	/** The base node. */
	ListItem esn_node;

	/** The ExternalServer */
	ExternalServer *esn_server_p;

	/**
	 * The MEM_FLAG indicating the ownership of the ExternalServer
	 * from this ExternalServerNode and whether the ExternalServer
	 * will be freed when this ExternalServerNode is.
	 */
	MEM_FLAG esn_server_mem;
} ExternalServerNode;


/* forward declaration */
struct ServersManager;


/**
 * A function to take a raw binary data representation of a previously stored
 * ExternalServer and recreate it.
 *
 * @param data_p The raw binary data representation of the ExternalServer.
 * @return The newly created ExternalServer or <code>NULL</code> upon error.
 * @ingroup server_group
 */
typedef ExternalServer *(*ExternalServerDeserialiser) (const unsigned char *data_p);



/**
 * A function to generate a raw binary data representation of a
 * ExternalServer allowing it to be recreated at a later time.
 *
 * @param server_p The ExternalServer to generate the representation for.
 * @param length_p Upon success, the length of the generated binary data
 * stream will be stored here.
 * @return The raw binary data or <code>NULL</code> upon error.
 * @ingroup server_group
 */
typedef unsigned char *(*ExternalServerSerialiser) (ExternalServer *server_p, uint32 *length_p);


/**
 * @brief A datatype for monitoring ExternalServers.
 *
 * A ServersManager is used to store details of the ExternalServers that are currently
 * running on a Server. ExternalServers can be added, removed and searched for.
 * @ingroup server_group
 */
typedef struct ServersManager
{
	/** The UUID for this Server */
	uuid_t sm_server_id;

	/**
	 * The UUID of this ExternalServer as a string.
	 *
	 * @see sm_server_id
	 */
	char sm_server_id_s [UUID_STRING_BUFFER_SIZE];


	/**
	 * @brief Add an ExternalServer to the ServersManager.
	 *
	 * @param manager_p The ServersManager to add the ExternalServer to.
	 * @param server_p The ExternalServer to add.
	 * @param serialise_fn The function used to serialise the ExternalServer into the unsigned char array
	 * which is the value stored in the ServersManager. If this is <code>NULL</code> then
	 * SerialiseExternalServerToJSON is used by default.
	 * @return <code>true</code> if the ExternalServer was added successfully,
	 * <code>false</code> otherwise.
	 * @memberof ServersManager
	 * @see SerialiseExternalServerToJSON
	 */
	bool (*sm_add_server_fn) (struct ServersManager *manager_p, ExternalServer *server_p, ExternalServerSerialiser serialise_fn);

	/**
	 * @brief Find an ExternalServer.
	 *
	 * Get a previously-added ExternalServer.
	 *
	 * @param manager_p The ServersManager to add the ExternalServer to.
	 * @param server_uri_s The uri for the ExternalServer to search for.
	 * @param deserialise_fn The function used to deserialise the data stored in this ServersManager to create ExternalServer.
	 * If this is <code>NULL</code> then DeserialiseExternalServerFromJSON is used by default.
	 * @return A pointer to the matching ExternalServer or <code>NULL</code>
	 * if it could not be found.
	 * @memberof ServersManager
	 * @see GetExternalServerFromServersManager
	 */
	ExternalServer *(*sm_get_server_fn)  (struct ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn);

	/**
 	 * @brief Remove an ExternalServer.
	 *
	 * Remove a previously-added ExternalServer.
	 *
	 * @param manager_p The ServersManager to remove the ExternalServer from.
	 * @param server_uri_s The uri for the ExternalServer to search for.
	 * @param deserialise_fn The function used to deserialise the data stored in this ServersManager to create ExternalServer.
	 * If this is <code>NULL</code> then DeserialiseExternalServerFromJSON is used by default.
	 * @return A pointer to the matching ExternalServer which will have been
	 * removed from the ServersManager or <code>NULL</code>
	 * if it could not be found.
	 * @see RemoveExternalServerFromServersManager
	 */
	ExternalServer *(*sm_remove_server_fn) (struct ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn);



	/**
 	 * @brief Get all ExternalServers.
	 *
	 * Get a LinkedList of ExternalServerNodes for all active ExternalServers.
	 *
	 * @param manager_p The ServersManager to remove the ExternalServer from.
	 * @param deserialise_fn The function used to deserialise the data stored in this ServersManager to create ExternalServer.
	 * If this is <code>NULL</code> then DeserialiseExternalServerFromJSON is used by default.
	 * @return A LinkedList of ExternalServerNodes of all of the ExternalServers or <code>NULL</code> if there are not any.
	 * @see GetAllExternalServersFromServersManager
	 */
	LinkedList *(*sm_get_all_servers_fn) (struct ServersManager *manager_p, ExternalServerDeserialiser deserialise_fn);


	/**
	 * Free a given ServersManager.
	 *
	 * @param manager_p This ServersManager.
	 * @return <code>true</code> if the ServersManager was freed successfully, <code>false</code> othewise.
	 */
	bool (*sm_free_servers_manager_fn) (struct ServersManager *manager_p);


	/**
	 * Get a LinkedList of ExternalServerNodes that have a given Service.
	 *
	 * @param manager_p This ServersManager.
	 * @param local_service_name_s The name of the Service to match.
	 * @return A LinkedList of ExternalServerNodes of all of the ExternalServers or <code>NULL</code> if there are not any.
	 */
	LinkedList *(*sm_get_all_matching_servers_fn) (struct ServersManager *manager_p, const char * const local_service_name_s);


} ServersManager;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the current Server-wide ServersManager.
 *
 * @return The ServersManager.
 * @ingroup server_group
 */
GRASSROOTS_SERVICE_MANAGER_API ServersManager *GetServersManager (void);


/**
 * Get the current Server uuid.
 *
 * @return The pointer to the uuid.
 * @ingroup server_group
 */
GRASSROOTS_SERVICE_MANAGER_API uuid_t *GetLocalServerId (void);


/**
 * Get the current Server uuid as a c-style string.
 *
 * @return The c-style string of the uuid.
 * @ingroup server_group
 */
GRASSROOTS_SERVICE_MANAGER_API const char *GetLocalServerIdAsString (void);


/**
 * @brief Initialise a ServersManager.
 *
 * Set up the callback functions for a given ServersManager.
 *
 * @param manager_p The ServersManager to set up.
 * @param add_server_fn The callback function to set for sm_add_server_fn for the given ServersManager.
 * @param get_server_fn The callback function to set for sm_get_server_fn for the given ServersManager.
 * @param remove_server_fn The callback function to set for sm_remove_server_fn for the given ServersManager.
 * @param get_all_servers_fn The callback function to set for sm_get_all_servers_fn for the given ServersManager.
 * @param free_servers_manager_fn The callback function to set for sm_free_servers_manager_fn for the given ServersManager.
 * @memberof ServersManager
 */
GRASSROOTS_SERVICE_MANAGER_API void InitServersManager (ServersManager *manager_p,
                      bool (*add_server_fn) (ServersManager *manager_p, ExternalServer *server_p, ExternalServerSerialiser serialise_fn),
											ExternalServer *(*get_server_fn)  (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn),
											ExternalServer *(*remove_server_fn) (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn),
											LinkedList *(*get_all_servers_fn) (struct ServersManager *manager_p, ExternalServerDeserialiser deserialise_fn),
											bool (*free_servers_manager_fn) (struct ServersManager *manager_p));


/**
 * @brief Add an ExternalServer to the ServersManager.
 *
 * @param manager_p The ServersManager to add the ExternalServer to.
 * @param server_p The ExternalServer to add.
 * @param serialise_fn The function used to serialise the ExternalServer into the unsigned char array
 * which is the value stored in the ServersManager. If this is <code>NULL</code> then
 * SerialiseExternalServerToJSON is used by default.
 * @return <code>true</code> if the ExternalServer was added successfully,
 * <code>false</code> otherwise.
 * @memberof ServersManager
 * @see SerialiseExternalServerToJSON
 * @memberof ServersManager
 * @see sm_add_server_fn
 */
GRASSROOTS_SERVICE_MANAGER_API bool AddExternalServerToServersManager (ServersManager *manager_p, ExternalServer *server_p, ExternalServerSerialiser serialise_fn);


/**
 * @brief Find a ExternalServer.
 *
 * Get a previously-added ExternalServer. This is simply a convenience
 * wrapper around sm_get_server_fn for the given ServersManager.
 *
 * @param manager_p The ServersManager to search on.
 * @param server_uri_s The uri for the ExternalServer to search for.
 * @param deserialise_fn The function used to deserialise the data stored in this ServersManager to create ExternalServer.
 * If this is <code>NULL</code> then DeserialiseExternalServerFromJSON is used by default.
 * @return A pointer to the matching ExternalServers or <code>NULL</code>
 * if it could not be found.
 * @memberof ServersManager
 * @see sm_get_server_fn
 */
GRASSROOTS_SERVICE_MANAGER_API ExternalServer *GetExternalServerFromServersManager (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn);


/**
 * @brief Remove a ExternalServer.
 *
 * Remove a previously-added ExternalServer from the ServersManager.
 * This is simply a convenience wrapper around sm_remove_server_fn
 * for the given ServersManager.
 *
 * @param manager_p The ServersManager to search on.
 * @param server_uri_s The uri for the ExternalServer to search for.
 * @param deserialise_fn The function used to deserialise the data stored in this ServersManager to create ExternalServer.
 * If this is <code>NULL</code> then DeserialiseExternalServerFromJSON is used by default.
 * @return A pointer to the matching ExternalServers or <code>NULL</code>
 * if it could not be found.
 * @memberof ServersManager
 * @see sm_remove_server_fn
 */
GRASSROOTS_SERVICE_MANAGER_API ExternalServer *RemoveExternalServerFromServersManager (ServersManager *manager_p, const char * const server_uri_s, ExternalServerDeserialiser deserialise_fn);


/**
 * @brief Get all ExternalServers.
 *
 * Get a LinkedList of ExternalServerNodes for all active ExternalServers.
 *
 * @param manager_p The ServersManager to remove the ExternalServer from.
 * @param deserialise_fn The function used to deserialise the data stored in this ServersManager to create ExternalServer.
 * If this is <code>NULL</code> then DeserialiseExternalServerFromJSON is used by default.
 * @return A LinkedList of const ExternalServerNodes or <code>NULL</code>
 * if there are not any ExternalServers.
 * @memberof ServersManager
 * @see sm_get_all_servers_fn
 */
GRASSROOTS_SERVICE_MANAGER_API LinkedList *GetAllExternalServersFromServersManager (ServersManager *manager_p, ExternalServerDeserialiser deserialise_fn);



/**
 * @brief Free a ServersManager
 *
 * Free a ServersManager and all of its associated ExternalServers.
 *
 * @param manager_p The ServersManager to free.
 * @return <code>true</code> if the ServersManager and its associated ExternalServers were freed
 * successfully, <code>false</code>otherwise.
 * @memberof ServersManager
 * @see sm_free_servers_manager_fn
 */
GRASSROOTS_SERVICE_MANAGER_API bool FreeServersManager (ServersManager *manager_p);


/**
 * Get the JSON fragment which will form part of a response to other servers and
 * clients. representing the capabilities available on for all
 * available ExternalServers for a given Operation.
 *
 * @param manager_p The ServersManager containing the details of all of the ExternalServers.
 * @param op The Operation to get the available facilities for.
 * @return The JSON fragment containing all of the details or <code>NULL</code> upon error.
 * @memberof ServersManager
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *AddExternalServerOperationsToJSON (ServersManager *manager_p, Operation op);


/**
 * Allocate an ExternalServer
 *
 * @param name_s The user-friendly name for the server
 * @param uri_s The URI to call for the ExternalServer's Services.
 * @param uuid_s The string representation of the UUID to addign to the ExternalServer.
 * @param ct The ConnectionType of how to connect to the ExternalServer
 * @return A newly-allocated ExternalServer or <code>NULL</code> upon error.
 * @see FreeExternalServer
 * @memberof ExternalServer
 */
GRASSROOTS_SERVICE_MANAGER_API ExternalServer *AllocateExternalServer (const char *name_s, const char *uri_s, const char *uuid_s, ConnectionType ct);


/**
 * Free an ExternalServer.
 *
 * @param server_p The ExternalServer to free.
 * @memberof ExternalServer
 */
GRASSROOTS_SERVICE_MANAGER_API void FreeExternalServer (ExternalServer *server_p);


/**
 * Allocate an ExternalServerNode
 *
 * @param server_p The ExternalServer that this ExternalServerNode will be based upon
 * @param mf How the ExternalServer that this node will contain will be constructed
 * @return A newly-allocated ExternalServerNode or <code>NULL</code> upon error.
 * @see ExternalServerNode
 * @memberof ExternalServerNode
 */
GRASSROOTS_SERVICE_MANAGER_API ExternalServerNode *AllocateExternalServerNode (ExternalServer *server_p, MEM_FLAG mf);


/**
 * Free an ExternalServerNode.
 *
 * @param node_p The ExternalServerNode to free.
 * @memberof ExternalServerNode
 */
GRASSROOTS_SERVICE_MANAGER_API void FreeExternalServerNode (ListItem *node_p);


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
GRASSROOTS_SERVICE_MANAGER_API json_t *MakeRemoteJSONCallToExternalServer (ExternalServer *server_p, json_t *request_p);


/**
 * @brief Add an ExternalServer from a JSON-based definition.
 *
 * This will read in the SERVERS_S json object within the global
 * configuration file and build the ExternalServer definitions
 * from it.
 *
 * @param json_p The global configuration json_t object.
 * @return bool <code>true</code> if the ExternalServer was generated
 * successfully, <code>false</code> otherwise.
 * @memberof ExternalServer
 * @see SERVERS_S
 * @see ServersManager
 */
GRASSROOTS_SERVICE_MANAGER_API bool AddExternalServerFromJSON (const json_t *json_p);


/**
 * Get the name of the remote paired service corresponding to a given local
 * service name.
 *
 * @param src_p The ExternalServer which has the remote Service.
 * @param local_service_name_s The name of the local Service.
 * @return The name of the remote paired service or <code>NULL</code>
 * upon error.
 */
GRASSROOTS_SERVICE_MANAGER_API const char *GetRemotePairedServiceName (const ExternalServer * const src_p, const char * const local_service_name_s);



/**
 * @brief Create a ExternalServer from a json_t object.
 *
 * This will create a ExternalServer from a json_t object.
 *
 * @param json_p The json object representing a ExternalServer.
 * @return <code>true</code> if the v was created successfully,
 * <code>false</code> otherwise.
 * @memberof ExternalServer
 */
GRASSROOTS_SERVICE_MANAGER_API ExternalServer *CreateExternalServerFromJSON (const json_t *json_p);


/**
 * Get a raw unsigned char array that allows the ExternalServer to be stored in the ServersManager.
 *
 * The array is the result of GetExternalServerAsJSON converted to a string
 *
 * @param external_server_p The ExternalServer to serialise.
 * @param value_length_p Upon success, the length in bytes of the resultant array will be stored here.
 * @return The serialised representation of the ExternalServer or <code>NULL</code> upon error.
 * @memberof ExternalServer
 * @see DeserialiseExternalServerToJSON
 */
GRASSROOTS_SERVICE_MANAGER_API unsigned char *SerialiseExternalServerToJSON (ExternalServer * const external_server_p, unsigned int *value_length_p);


/**
 * Create an ExternalServer from a raw data memory block.
 *
 * @param raw_json_data_s The raw data from a previous call to SerialiseExternalServerToJSON.
 * @return The newly-created ExternalServer or <code>NULL</code> upon error.
 * @memberof ExternalServer
 * @see SerialiseExternalServerToJSON
 */
GRASSROOTS_SERVICE_MANAGER_API ExternalServer *DeserialiseExternalServerFromJSON (const unsigned char *raw_json_data_s);


/**
 * Get the JSON fragment that describes the given ExternalServer.
 *
 * @param server_p The ExternalServer to get the JSON fragment for.
 * @return The JSON fragment or <code>NULL</code> upon error.
 * @memberof ExternalServer
 */
GRASSROOTS_SERVICE_MANAGER_API json_t *GetExternalServerAsJSON (ExternalServer *server_p);


#ifdef __cplusplus
}
#endif


#endif /* SERVERS_POOL_H_ */
