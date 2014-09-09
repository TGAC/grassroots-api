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


/*****************************/
/***** STATIC PROTOTYPES *****/
/*****************************/

static int SendData (int socket_fd, const char *buffer_p, int32 num_to_send);

static int ReceiveData (int socket_fd, char *buffer_p, int32 num_to_receive);



/******************************/
/***** METHOD DEFINITIONS *****/
/******************************/


int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp)
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


int AtomicSendString (int socket_fd,  const char *buffer_p)
{
	return SendData (socket_fd, buffer_p, strlen (buffer_p));
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
int AtomicSend (int socket_fd, uint32 id, const char *buffer_p, int32 num_to_send)
{
	int num_sent = 0;
	
	/* Send the length of the message */
	char *header_s = ConvertIntegerToString (num_to_send);	
	if (header_s)
		{
			char *id_s = ConvertIntegerToString (id);
			
			if (id_s)
				{
					num_sent = AtomicSendString (socket_fd, header_s);
					
					if (num_sent > 0)
						{
							num_sent = AtomicSendString (socket_fd, id_s);
							
							if (num_sent > 0)
								{
									/* Send the message */
									num_sent = SendData (socket_fd, buffer_p, num_to_send);
								}
						}
					
					FreeCopiedString (id_s);
				}
				
			FreeCopiedString (header_s);
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
static int SendData (int socket_fd, const char *buffer_p, int32 num_to_send)
{
	int num_sent = 0;
	int i;	
	bool loop_flag = true;
	
	while (loop_flag)
		{
			i = send (socket_fd, buffer_p, num_to_send, 0);
			
			if (i != -1)
				{
					loop_flag = false;
					num_sent = -num_sent;
				}
			else
				{
					num_sent += i;
					num_to_send -= i;
					
					loop_flag = (num_sent < num_to_send);
				}
		}
		
	return num_sent;
}





int AtomicReceiveString (int socket_fd, char *buffer_p)
{
	return ReceiveData (socket_fd, buffer_p, strlen (buffer_p));
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
int AtomicReceive (int socket_fd, uint32 id, char *buffer_p, int32 num_to_receive)
{
	int num_received = 0;
	
	/* Send the length of the message */
	char *header_s = ConvertIntegerToString (num_to_receive);	
	if (header_s)
		{
			char *id_s = ConvertIntegerToString (id);
			
			if (id_s)
				{
					num_received = AtomicReceiveString (socket_fd, header_s);
					
					if (num_received > 0)
						{
							num_received = AtomicReceiveString (socket_fd, id_s);
							
							if (num_received > 0)
								{
									/* Receive the message */
									num_received = ReceiveData (socket_fd, buffer_p, num_to_receive);
								}
						}
					
					FreeCopiedString (id_s);
				}
				
			FreeCopiedString (header_s);
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
static int ReceiveData (int socket_fd, char *buffer_p, int32 num_to_receive)
{
	int num_received = 0;
	int i;	
	bool loop_flag = true;
	
	while (loop_flag)
		{
			i = recv (socket_fd, buffer_p, num_to_receive, 0);
			
			if (i != -1)
				{
					loop_flag = false;
					num_received = -num_received;
				}
			else
				{
					num_received += i;
					num_to_receive -= i;
					
					loop_flag = (num_received < num_to_receive);
				}
		}
		
	return num_received;
}



