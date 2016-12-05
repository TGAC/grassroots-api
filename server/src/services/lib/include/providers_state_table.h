/*
** Copyright 2014-2015 The Genome Analysis Centre
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
/*
 * providers_state_table.h
 *
 *  Created on: 6 May 2016
 *      Author: tyrrells
 */

#ifndef SRC_NETWORK_SRC_PROVIDERS_STATE_TABLE_H_
#define SRC_NETWORK_SRC_PROVIDERS_STATE_TABLE_H_

#include "grassroots_service_library.h"
#include "json_tools.h"
#include "json_util.h"

/**
 * This is a datatype that is used to keep track of which servers
 * and services have already been processed when merging services
 * together to present to a server or client.
 *
 * As requests are sent between connected Servers, a ProvidersStateTable
 * maintains a list of the Services already used on each Server. This is used
 * to, for instance, prevent deadlocks occurring where two Servers have connected
 * Services with each other.
 */
typedef struct ProvidersStateTable
{
	/** @private */
	json_t *pst_values_p;
} ProvidersStateTable;



#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate a ProvidersStateTable and populate it with any details within a
 * specified json request.
 *
 * @param req_p The json_t request to populate the ProvidersStateTable with. Any previous
 * ProvidersStateTable details will be stored by placing the output of
 * GetProvidersStateTableAsJSON with the SERVERS_S key.
 * @return The newly-allocated ProvidersStateTable or <code>NULL</code> upon error.
 * @see SERVERS_S
 * @see GetProvidersStateTableAsJSON
 * @see InitProvidersStateTableFromRequest
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API ProvidersStateTable *AllocateProvidersStateTable (const json_t *req_p);


/**
 * Deallocate a ProvidersStateTable
 *
 * @param providers_p The ProvidersStateTable to free.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API void FreeProvidersStateTable (ProvidersStateTable *providers_p);


/**
 * Create a new ProvidersStateTable and populate it with any details within a
 * specified json request and a list of Services for this Server.
 *
 * @param req_p The json_t request to populate the ProvidersStateTable with. Any previous
 * ProvidersStateTable details will be stored by placing the output of
 * GetProvidersStateTableAsJSON with the SERVERS_S key.
 * @param services_p A LinkedList of ServiceNodes that will get added to the ProvidersStateTable
 * for this Server.
 * @return The newly-allocated ProvidersStateTable or <code>NULL</code> upon error.
 * @see AllocateProvidersStateTable
 * @see AddServicesListToProvidersStateTable
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API ProvidersStateTable *GetInitialisedProvidersStateTable (const json_t *req_p, LinkedList *services_p);


/**
 * Create a new ProvidersStateTable and populate it with any details within a
 * specified json request and a single Server-Service pair.
 *
 * @param req_p The json_t request to populate the ProvidersStateTable with. Any previous
 * ProvidersStateTable details will be stored by placing the output of
 * GetProvidersStateTableAsJSON with the SERVERS_S key.
 * @param server_uri_s The URI of the Server to add to the ProvidersStateTable if it is not
 * already there.
 * @param service_s The name of the Service for the given Server that will be added to the ProvidersStateTable.
 * @return The newly-allocated ProvidersStateTable or <code>NULL</code> upon error.
 * @see AllocateProvidersStateTable
 * @see AddToProvidersStateTable
 * @memberof ProvidersStateTable
 */GRASSROOTS_SERVICE_API ProvidersStateTable *GetInitialisedProvidersStateTableForSingleService (const json_t *req_p, const char * const server_uri_s, const char * const service_s);


/**
 * Add a server and service to a ProvidersStateTable
 *
 * @param providers_p The ProvidersStateTable to amend.
 * @param server_uri_s The unique identifier for a server. The URI for the server is used.
 * @param service_s The name of the service to add.
 * @return <code>true</code> if the service was added to the ProvidersStateTable successfully, <code>false</code> otherwise.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API bool AddToProvidersStateTable (ProvidersStateTable *providers_p, const char *server_uri_s, const char *service_s);


/**
 * Check whether a given server and service are in a ProvidersStateTable
 *
 * @param providers_p The ProvidersStateTable to check.
 * @param server_uri_s The URI for the server.
 * @param service_s The name of the service.
 * @return <code>true</code> if the server-service pair is in the given ProvidersStateTable, <code>false</code> otherwise.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API bool IsServiceInProvidersStateTable (const ProvidersStateTable *providers_p, const char *server_uri_s, const char *service_s);


/**
 * Get the json representation of a ProvidersStateTable.
 *
 * @param providers_p The ProvidersStateTable to get the json representation of
 * @return The json representation or <code>NULL</code> upon error.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API json_t *GetProvidersStateTableAsJSON (const ProvidersStateTable *providers_p);


/**
 * Create a json request that to get the list of all available Services
 * making sure not to duplicate any Server-Service pairs stored within
 * a given ProvidersStateTable.
 *
 * @param providers_p The ProvidersStateTable detailing all of the Server-Service pairs already processed.
 * @param user_p An optional UserDetails for the current User if access restrictions to any Server-Service
 * combinations sent in response to the request generated by this method.
 * @param sv_p If you wish to create a JSON fragment for a different version of the Grassroots system,
 * then you can set this value to the version that you require. If this is <code>NULL</code>, then the
 * current version of the running Grassroots system will be used.
 * @return The json representation or <code>NULL</code> upon error.
 * @see AddProvidersStateTableToRequest
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API json_t *GetAvailableServicesRequestForAllProviders (const ProvidersStateTable * providers_p, UserDetails *user_p, const SchemaVersion * const sv_p);



/**
 * Add a list of Services for this Server to a ProvidersStateTable.
 *
 * @param providers_p The ProvidersStateTable to add the Service definitions to.
 * @param services_p A LinkedList of ServiceNodes that will get added to the ProvidersStateTable
 * for this Server.
 * @return <code>true</code> if the Services were added successfully or <code>false</code> upon error.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API bool AddServicesListToProvidersStateTable (ProvidersStateTable *providers_p, LinkedList *services_p);


/**
 * Add the details to a json request to get the list of all available Services on
 * a Server
 * making sure not to duplicate any Server-Service pairs stored within
 * a given ProvidersStateTable.
 *
 * @param providers_p The ProvidersStateTable detailing all of the Server-Service pairs already processed.
 * @param req_p The json request to add the details to.
 * @return <code>true</code> if the ProvidersStateTable details were added to the given request successfully,
 * <code>false</code> otherwise.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API bool AddProvidersStateTableToRequest (const ProvidersStateTable * providers_p, json_t *req_p);


/**
 * Add the details from a json request to
 * a given ProvidersStateTable.
 *
 * @param providers_p The ProvidersStateTable to initialise.
 * @param req_p The json request to extract the Server-Service details from and add to the given ProvidersStateTable.
 * @return <code>true</code> if the ProvidersStateTable was initialised successfully,
 * <code>false</code> otherwise.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API bool InitProvidersStateTableFromRequest (ProvidersStateTable * const providers_p, const json_t * const req_p);


/**
 * Clear all of the servers and services that are being
 * tracked within a ProvidersStateTable.
 * @param providers_p The ProvidersStateTable to clear.
 * @return <code>true</code> if the ProvidersStateTable was cleared successfully, <code>false</code> otherwise.
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API bool ClearProvidersStateTable (ProvidersStateTable *providers_p);


/**
 * Clear a given ProvidersStateTable and add the details from a json request and a given Server-Service pair
 * to it.
 *
 * @param providers_p The ProvidersStateTable to reinitialise.
 * @param req_p The json request to extract the Server-Service details from and add to the given ProvidersStateTable.
 * @param server_uri_s The unique identifier for a server. The URI for the server is used.
 * @param service_name_s The name of the service to add.
 * @return <code>true</code> if the ProvidersStateTable was reinitialised successfully,
 * <code>false</code> otherwise.
 * @see ClearProvidersStateTable
 * @see InitProvidersStateTableFromRequest
 * @see AddToProvidersStateTable
 * @memberof ProvidersStateTable
 */
GRASSROOTS_SERVICE_API bool ReinitProvidersStateTable (ProvidersStateTable *providers_p, const json_t * const req_p, const char *server_uri_s, const char *service_name_s);


#ifdef __cplusplus
}
#endif


#endif /* SRC_NETWORK_SRC_PROVIDERS_STATE_TABLE_H_ */
