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

static int ReceiveData (int socket_fd, void *buffer_p, const size_t num_to_receive);

static int ReceiveDataIntoByteBuffer (int socket_fd, ByteBuffer *buffer_p, const size_t num_to_receive, bool append_flag);


/******************************/
/***** METHOD DEFINITIONS *****/
/******************************/



int AtomicSendStringViaRawConnection (const char *data_s, RawConnection *connection_p)
{
	return AtomicSendViaRawConnection (data_s, strlen (data_s), connection_p);
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
int AtomicSendViaRawConnection (const char *buffer_p, const uint32 num_to_send, RawConnection *connection_p)
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
	num_sent = SendData (connection_p -> rc_sock_fd, (const void *) header_s, header_size);

	if (num_sent >= 0)
		{
			if ((size_t) num_sent == header_size)
				{
					/*
					 * Send the id. Note that header_s
					 * isn't a valid c string as it is not null-
					 * terminated.
					 */
					i = htonl (connection_p -> rc_base.co_id);
					memcpy (header_s, &i, header_size);
					num_sent = SendData (connection_p -> rc_sock_fd, (const void *) header_s, header_size);

					if (num_sent >= 0)
						{
							if ((size_t) num_sent == header_size)
								{
									/* Send the message */
									num_sent = SendData (connection_p -> rc_sock_fd, buffer_p, num_to_send);
								}
						}
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
	size_t num_sent = 0;
	int res = 0;
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
					res = -num_sent;
				}
		}
		
	return res;
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
int AtomicReceiveViaRawConnection (RawConnection *connection_p)
{
	int num_received = 0;
	const int header_size = sizeof (uint32);	
	char header_s [header_size];	
	
	/* if the buffer isn't empty, clear it */
	if (GetByteBufferSize (connection_p -> rc_data_buffer_p) > 0)
		{
			ResetByteBuffer (connection_p -> rc_data_buffer_p);
		}


	/* Get the length of the message */
	num_received = ReceiveData (connection_p -> rc_sock_fd, header_s, header_size);

	if (num_received == header_size)
		{
			uint32 *val_p = (uint32 *) header_s;
			uint32 message_size = ntohl (*val_p);

			/* Get the id of the message */
			num_received = ReceiveData (connection_p -> rc_sock_fd, header_s, header_size);

			if (num_received == header_size)
				{
					val_p = (uint32 *) header_s;
					uint32 id_val = ntohl (*val_p);
										
					num_received = ReceiveDataIntoByteBuffer (connection_p -> rc_sock_fd, connection_p -> rc_data_buffer_p, message_size, false);
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
static int ReceiveData (int socket_fd, void *buffer_p, const size_t num_to_receive)
{
	size_t num_received = 0;
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
static int ReceiveDataIntoByteBuffer (int socket_fd, ByteBuffer *buffer_p, const size_t num_to_receive, bool append_flag)
{
	size_t num_received = 0;
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
