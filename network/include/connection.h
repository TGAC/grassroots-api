/*
 * connection.h
 *
 *  Created on: 16 Mar 2015
 *      Author: billy
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netdb.h>

#include "typedefs.h"
#include "byte_buffer.h"
#include "curl_tools.h"
#include "network_library.h"
#include "jansson.h"

typedef enum
{
	CT_RAW,
	CT_WEB,
	CT_NUM_TYPES
} ConnectionType;

typedef struct Connection
{
	uint32 co_id;
	ByteBuffer *co_data_buffer_p;
	ConnectionType co_type;
} Connection;


typedef struct RawConnection
{
	Connection rc_base;
	int rc_sock_fd;
	bool rc_server_connection_flag;

	union
		{
			struct addrinfo *rc_server_p;
			struct sockaddr *rc_client_p;
		}
	rc_data;

} RawConnection;


typedef struct WebConnection
{
	Connection wc_base;
	CurlTool *wc_curl_p;
	char *wc_uri_s;
} WebConnection;


#ifdef __cplusplus
extern "C"
{
#endif


WHEATIS_NETWORK_API Connection *AllocateRawServerConnection (const char * const hostname_s, const char * const port_s);


WHEATIS_NETWORK_API Connection *AllocateWebServerConnection (const char * const full_uri_s);


WHEATIS_NETWORK_API Connection *AllocaterRawClientConnection (int server_socket_fd);


WHEATIS_NETWORK_API const char *GetConnectionData (Connection *connection_p);


WHEATIS_NETWORK_API void FreeConnection (Connection *connection_p);


WHEATIS_NETWORK_API const char *MakeRemoteJsonCallViaConnection (Connection *connection_p, json_t *req_p);


#ifdef __cplusplus
}
#endif


#endif /* CONNECTION_H_ */
