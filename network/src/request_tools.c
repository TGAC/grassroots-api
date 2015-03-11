#include <string.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "request_tools.h"
#include "string_utils.h"
#include "math_utils.h"
#include "memory_allocations.h"

/*****************************/
/***** STATIC PROTOTYPES *****/
/*****************************/

static int SendData (int socket_fd, const void *buffer_p, const size_t num_to_send);

static int ReceiveData (int socket_fd, void *buffer_p, const uint32 num_to_receive);

static int ReceiveDataIntoByteBuffer (int socket_fd, ByteBuffer *buffer_p, const uint32 num_to_receive, bool append_flag);

static int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);


/******************************/
/***** METHOD DEFINITIONS *****/
/******************************/


static int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp)
{
	struct addrinfo hints;
	int i;
	int sock_fd = -1;
	
	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	i = getaddrinfo (hostname_s, port_s, &hints, server_pp);
	
	if (i == 0)
		{
			struct addrinfo *addr_p = *server_pp;
			int loop_flag = 1;
			
			/* loop through all the results and connect to the first we can */
			while (loop_flag)
				{
					sock_fd = socket (addr_p -> ai_family, addr_p -> ai_socktype, addr_p -> ai_protocol);

					if (sock_fd != -1)
						{
							i = connect (sock_fd, addr_p -> ai_addr, addr_p -> ai_addrlen);
					
							if (i != -1)
								{
									
									loop_flag = 0;
								}
							else
								{
									close (sock_fd);
									sock_fd = -1;
								}							
						}
						
					if (loop_flag)	
						{
							addr_p = addr_p -> ai_next;
							loop_flag = (addr_p != NULL);
						}
				}		/* while (addr_p) */
				
		}		/* if (i == 0) */
	
	return sock_fd;
}


Connection *AllocateConnection (const char * const hostname_s, const char * const port_s)
{
	Connection *connection_p = (Connection *) AllocMemory (sizeof (Connection));

	if (connection_p)
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);

			if (buffer_p)
				{
					struct addrinfo *server_p = NULL;
					int fd = ConnectToServer (hostname_s, port_s, &server_p);

					if (fd >= 0)
						{
							connection_p -> co_data_buffer_p = buffer_p;
							connection_p -> co_sock_fd = fd;
							connection_p -> co_server_p = server_p;
							connection_p -> co_id = 0;

							return connection_p;
						}		/* if (fd >= 0) */

					FreeByteBuffer (buffer_p);
				}		/* if (buffer_p) */

			FreeMemory (connection_p);
		}		/* if (connection_p) */


	return NULL;
}



void FreeConnection (Connection *connection_p)
{
	FreeByteBuffer (connection_p -> co_data_buffer_p);
	freeaddrinfo (connection_p -> co_server_p);
	close (connection_p -> co_sock_fd);

	FreeMemory (connection_p);
}


const char *GetConnectionData (Connection *connection_p)
{
	return GetByteBufferData (connection_p -> co_data_buffer_p);
}


int AtomicSendString (const char *data_s, Connection *connection_p)
{
	return AtomicSend (data_s, strlen (data_s), connection_p);
}


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
int AtomicSend (const char *buffer_p, const uint32 num_to_send, Connection *connection_p)
{
	int num_sent = 0;
	const size_t header_size = sizeof (uint32);
	char header_s [header_size];	
	
	/* Get the length of the message */
	uint32 i = htonl (num_to_send);
	
	/* 
	 * Send the header size. Note that header_s
	 * isn't a valid c string as it is not null-
	 * terminated.
	 */
	memcpy (header_s, &i, header_size);	
	num_sent = SendData (connection_p -> co_sock_fd, (const void *) header_s, header_size);

	if (num_sent == header_size)
		{
			
			/* 
			 * Send the id. Note that header_s
			 * isn't a valid c string as it is not null-
			 * terminated.
			 */
			i = htonl (connection_p -> co_id);
			memcpy (header_s, &i, header_size);	
			num_sent = SendData (connection_p -> co_sock_fd, (const void *) header_s, header_size);

			if (num_sent == header_size)
				{
					/* Send the message */
					num_sent = SendData (connection_p -> co_sock_fd, buffer_p, num_to_send);
				}
		}


	return num_sent;
}


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
static int SendData (int socket_fd, const void *buffer_p, const size_t num_to_send)
{
	int num_sent = 0;
	int i;	
	bool loop_flag = true;
	
	while (loop_flag)
		{
			i = send (socket_fd, buffer_p, num_to_send, 0);
			
			if (i != -1)
				{
					num_sent += i;
					buffer_p += i;
					
					loop_flag = (num_sent < num_to_send);
				}
			else
				{
					loop_flag = false;
					num_sent = -num_sent;
				}
		}
		
	return num_sent;
}




/*
int AtomicReceiveString (int socket_fd, uint32 id, char *buffer_p)
{
	return AtomicReceive (socket_fd, id, buffer_p, strlen (buffer_p));
}
*/

/**
 * Make sure that we keep sending until the complete message has been
 * transferred.
 * 
 * @param socket_fd The socket to send to.
 * @param buffer_p The buffer to send the message from.
 * @param num_to_send The length of the buffer to send.
 * @return A positive integer for the number of bytes received upon success. When negative,
 * it indicates that there was an error with this value being -(num bytes sent) that were
 * sent successfully before the error occurred. If this is zero, it means that there was 
 * an error sending the initial message containing the length header.
 */
int AtomicReceive (Connection *connection_p)
{
	int num_received = 0;
	const int header_size = sizeof (uint32);	
	char header_s [header_size];	
	
	/* Get the length of the message */
	num_received = ReceiveData (connection_p -> co_sock_fd, header_s, header_size);

	if (num_received == header_size)
		{
			uint32 *val_p = (uint32 *) header_s;
			uint32 message_size = ntohl (*val_p);

			/* Get the id of the message */
			num_received = ReceiveData (connection_p -> co_sock_fd, header_s, header_size);

			if (num_received == header_size)
				{
					val_p = (uint32 *) header_s;
					uint32 id_val = ntohl (*val_p);
										
					num_received = ReceiveDataIntoByteBuffer (connection_p -> co_sock_fd, connection_p -> co_data_buffer_p, message_size, false);
				}
		}

	return num_received;
}


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
static int ReceiveData (int socket_fd, void *buffer_p, const uint32 num_to_receive)
{
	int num_received = 0;
	int i;	
	bool loop_flag = true;
	
	while (loop_flag)
		{
			i = recv (socket_fd, buffer_p, num_to_receive, 0);
			
			if (i != -1)
				{
					num_received += i;
					buffer_p += i;
					
					loop_flag = (num_received < num_to_receive);
				}
			else
				{
					loop_flag = false;
					num_received = -num_received;
				}
		}
		
	return num_received;
}



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
static int ReceiveDataIntoByteBuffer (int socket_fd, ByteBuffer *buffer_p, const uint32 num_to_receive, bool append_flag)
{
	int num_received = 0;
	int i;	
	bool loop_flag = true;


	if (append_flag)
		{
			size_t space = GetRemainingSpaceInByteBuffer (buffer_p);
			
			if (num_to_receive >= space)
				{
					if (!ExtendByteBuffer (buffer_p, num_to_receive - space + 1))
						{
							return 0;
						}
				}		
		}
	else
		{
			ResetByteBuffer (buffer_p);

			/* Receive the message */
			if (num_to_receive >= buffer_p -> bb_size)
				{
					if (!ResizeByteBuffer (buffer_p, num_to_receive + 1))
						{
							return 0;
						}
				}									
		}
	
	while (loop_flag)
		{
			char buffer [1024];
			i = recv (socket_fd, buffer, 1024, 0);
			
			if (i != -1)
				{
					num_received += i;
					
					if (AppendToByteBuffer (buffer_p, buffer, i))
						{					
							loop_flag = (num_received < num_to_receive);
						}
					else
						{
							loop_flag = false;
							num_received = -num_received;							
						}
				}
			else
				{
					loop_flag = false;
					num_received = -num_received;
				}
		}
		
	return num_received;
}
