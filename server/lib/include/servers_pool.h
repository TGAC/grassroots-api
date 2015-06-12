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


/**
 * Allocate an ExternalServer
 *
 * @param uri_s The URI for the ExternalServer
 * @param ct The ConnectionType of how to connect to the ExternalServer
 * @return A newly-allocated ExternalServer or <code>NULL</code> upon eror.
 * @see FreeExternalServer
 * @memberof ExternalServer
 */
ExternalServer *AllocateExternalServer (char *uri_s, ConnectionType ct);


/**
 * Free an ExternalServer.
 *
 * @param server_p The ExternalServer to free.
 * @memberof ExternalServer
 */
void FreeExternalServer (ExternalServer *server_p);


#endif /* SERVERS_POOL_H_ */
