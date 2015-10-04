#ifndef REQUEST_TOOLS_H
#define REQUEST_TOOLS_H

#include <netdb.h>

#include "jansson.h"

#include "network_library.h"
#include "typedefs.h"

#include "byte_buffer.h"
#include "connection.h"




#ifdef __cplusplus
extern "C" 
{
#endif



/**
 * Receive data over a RawConnection
 * 
 * @param connection_p The RawConnection to use to receive the data with.
 * @return A positive integer for the number of bytes received upon success. When negative,
 * it indicates that there was an error with this value being -(num bytes received) that were
 * sent successfully before the error occurred. If this is zero, it means that there was 
 * an error sending the initial message containing the length header.
 * @memberof RawConnection
 */
GRASSROOTS_NETWORK_API int AtomicReceiveViaRawConnection (RawConnection *connection_p);



/**
 * Send a string over a network connection.
 *
 * @param data_s The string to send.
 * @param connection_p The RawConnection to use to send the data with.
 * @return A positive integer for the number of bytes sent upon success. When negative,
 * it indicates that there was an error with this value being -(num bytes sent) that were
 * sent successfully before the error occurred. If this is zero, it means that there was
 * an error sending the initial message containing the length header.
 * @memberof RawConnection
 */
GRASSROOTS_NETWORK_API int AtomicSendStringViaRawConnection (const char *data_s, RawConnection *connection_p);


/**
 * Send a string over a network connection.
 * 
 * @param buffer_p The buffer to send the message from.
 * @param num_to_send The length of the buffer to send.
 * @param connection_p The RawConnection to use to send the data with.
 * @return A positive integer for the number of bytes sent upon success. When negative,
 * it indicates that there was an error with this value being -(num bytes sent) that were
 * sent successfully before the error occurred. If this is zero, it means that there was 
 * an error sending the initial message containing the length header.
 * @memberof RawConnection
 */
GRASSROOTS_NETWORK_API int AtomicSendViaRawConnection (const char *buffer_p, uint32 num_to_send, RawConnection *connection_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef REQUEST_TOOLS_H */
