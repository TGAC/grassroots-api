/*
 * connection.h
 *
 *  Created on: 16 Mar 2015
 *      Author: billy
 *
 * @brief A datatype to encapsulate Server-Client connections.
 * @addtogroup Network
 * @{
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netdb.h>

#include "typedefs.h"
#include "byte_buffer.h"
#include "curl_tools.h"
#include "network_library.h"
#include "jansson.h"

/**
 * An enumeration listing the different types of Connections
 */
typedef enum
{
	/** A raw socket-based connection */
	CT_RAW,

	/** A connection using http(s) */
	CT_WEB,

	/** The number of possible ConnectionTypes */
	CT_NUM_TYPES
} ConnectionType;


/**
 * @brief The base class for building a Server-Client connection.
 */
typedef struct Connection
{
	/** The internal id for this Connection */
	uint32 co_id;

	/** The ConnectionType for this Connection */
	ConnectionType co_type;
} Connection;


/**
 * @brief A Connection that uses raw socket-based commmunication.
 */
typedef struct RawConnection
{
	/** The base Connection */
	Connection rc_base;

	/** The buffer where all of the received data is stored */
	ByteBuffer *rc_data_buffer_p;

	/** The socket the RawConnection is using. */
	int rc_sock_fd;

	/**
	 * <code>true</code> if the Connection is to a Server, <code>
	 * false</code> if it is to a Client.
	 */
	bool rc_server_connection_flag;

	/**
	 * Dependent upon rc_server_connection_flag this stores the underlying
	 * connection data.
	 */
	union
		{
			struct addrinfo *rc_server_p;
			struct sockaddr *rc_client_p;
		}
	rc_data;

} RawConnection;


/**
 * @brief A Connection that uses https(s) commmunication.
 */
typedef struct WebConnection
{
	/** The base Connection. */
	Connection wc_base;

	/** The CurlTool that encapsulates the web-based calls. */
	CurlTool *wc_curl_p;

	/** The URI that the WebConnection is to. */
	char *wc_uri_s;
} WebConnection;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate a RawConnection to a Server.
 *
 * @param hostname_s The hostname of the Server to connect to.
 * @param port_s The Server's port to connect to.
 * @return A Connection to the given Server or <code>NULL</code> upon error.
 * @memberof Connection
 */
GRASSROOTS_NETWORK_API Connection *AllocateRawServerConnection (const char * const hostname_s, const char * const port_s);


/**
 * Allocate a WebConnection to a Server.
 *
 * @param full_uri_s The URI to connect to.
 * @return A Connection to the given Server or <code>NULL</code> upon error.
 * @memberof Connection
 */
GRASSROOTS_NETWORK_API Connection *AllocateWebServerConnection (const char * const full_uri_s);


/**
 * Allocate a RawConnection to a Client.
 *
 * @param server_socket_fd The Client's socket_fd on the Server.
 * @return A Connection to the given Client or <code>NULL</code> upon error.
 * @memberof Connection
 */
GRASSROOTS_NETWORK_API Connection *AllocaterRawClientConnection (int server_socket_fd);


/**
 * Get the data stored in a Connection's buffer.
 *
 * @param connection_p The Connection to get the data from.
 * @return The data stored in a Connection's buffer.
 * @memberof Connection
 */
GRASSROOTS_NETWORK_API const char *GetConnectionData (Connection *connection_p);


/**
 * Free a Connection.
 *
 * @param connection_p The Connection to free.
 * @memberof Connection
 */
GRASSROOTS_NETWORK_API void FreeConnection (Connection *connection_p);


/**
 * Send JSON-based data over a Connection.
 *
 * @param connection_p The Connection to use.
 * @param req_p The data to send.
 * @return The response or <code>NULL</code> upon error.
 * @memberof Connection
 */
GRASSROOTS_NETWORK_API const char *MakeRemoteJsonCallViaConnection (Connection *connection_p, json_t *req_p);


GRASSROOTS_NETWORK_LOCAL int SendJsonRequestViaRawConnection (RawConnection *connection_p, const json_t *json_p);

/** @} */


#ifdef __cplusplus
}
#endif


#endif /* CONNECTION_H_ */
