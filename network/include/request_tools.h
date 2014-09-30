#ifndef REQUEST_TOOLS_H
#define REQUEST_TOOLS_H

#include <netdb.h>

#include "network_library.h"
#include "typedefs.h"

#include "byte_buffer.h"

#ifdef __cplusplus
extern "C" 
{
#endif


WHEATIS_NETWORK_API int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);


//WHEATIS_NETWORK_API int AtomicReceiveString (int socket_fd, uint32 id, char *buffer_p);


/**
 * Make sure that we keep receiving until the complete message has been
 * transferred.
 * 
 * @param socket_fd The socket to receive the data from.
 * @param buffer_p The buffer to store the received message to.
 * @return A positive integer for the number of bytes received upon success. When negative,
 * it indicates that there was an error with this value being -(num bytes received) that were
 * sent successfully before the error occurred. If this is zero, it means that there was 
 * an error sending the initial message containing the length header.
 */
WHEATIS_NETWORK_API int AtomicReceive (int socket_fd, uint32 id, ByteBuffer *buffer_p);


WHEATIS_NETWORK_API int AtomicSendString (int socket_fd, uint32 id, const char *buffer_p);


/**
 * Make sure that we keep sending until the complete message has been
 * transferred.
 * 
 * @param socket_fd The socket to send to.
 * @param buffer_p The buffer to send the message from.
 * @param num_to_send The length of the buffer to send.
 * @return A positive integer for the number of bytes sent upon success. When negative,
 * it indicates that there was an error with this value being -(num bytes sent) that were
 * sent successfully before the error occurred. If this is zero, it means that there was 
 * an error sending the initial message containing the length header.
 */
WHEATIS_NETWORK_API int AtomicSend (int socket_fd, uint32 id, const char *buffer_p, uint32 num_to_send);



#ifdef __cplusplus
}
#endif

#endif		/* #ifndef REQUEST_TOOLS_H */
