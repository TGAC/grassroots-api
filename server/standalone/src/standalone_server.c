/*
	Reworked on an example taken from https://gist.github.com/silv3rm00n/5821760

	A simplistic server allowing us to test the wheatis architecture. 
*/
 
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    
#include <signal.h>

#include <sys/socket.h>
#include <netdb.h>

#include <arpa/inet.h> //inet_addr

#include <unistd.h>    //write

#include <pthread.h>
 
#include "string_linked_list.h"
#include "string_utils.h"


#include "server.h"
#include "request_tools.h"


/**********************/
/***** PROTOTYPES *****/ 
/**********************/

static int BindToPort (const char *port_s, struct addrinfo **loaded_address_pp);

static void *HandleConnection (void *socket_desc_p);

static void InterruptHandler (int sig);

static void RunServer (int server_socket_fd);


/**********************/
/******* GLOBALS ******/ 
/**********************/

static int volatile g_running_flag = 1;


/**********************/
/***** FUNCTIONS ******/ 
/**********************/
 
int main (int argc, char *argv [])
{
	struct addrinfo *address_p = NULL;
	const char *port_s = DEFAULT_SERVER_PORT;
	char *root_dir_s = NULL;
	int backlog = 4;
	int i = 0;
	
	switch (argc)
		{
			case 4:
				SetServerRootDirectory (argv [3]);
							
			/* deliberate fall through */
			case 3:
				i = atoi (argv [2]);
				if (i > 0)
					{
						backlog = i;
						i = 0;
					}
				
				/* deliberate fall through */
				case 2:
					port_s = argv [1];
					break;
				
				default:
					break;
		}
	
	
	/* 
	 * Since the server is to run continuously we need to install
	 * an interrupt handler to take care of a user request to 
	 * stop the server.
	 */
	signal (SIGINT, InterruptHandler);
	
	int socket_fd = BindToPort (port_s, &address_p);

	freeaddrinfo (address_p); // all done with this structure

	if (socket_fd >= 0)
		{
			if (listen (socket_fd, backlog) != -1)
				{
					if (InitHandlerUtil ())
						{
							RunServer (socket_fd);					
							
							if (!DestroyHandlerUtil ())
								{
									fprintf (stderr, "Failed to destory handler util");
								}
						}					
				}
							
			close (socket_fd);
		}		/* if (socket_fd >= 0) */


	
	FreeServerResources ();

	return 0;
}
 
 
static void RunServer (int server_socket_fd)
{
	while (g_running_flag)
		{
			struct sockaddr remote;
			socklen_t t = sizeof (remote);
			int client_socket_fd;
			
			if (g_running_flag && (client_socket_fd = accept (server_socket_fd, (struct sockaddr *) &remote, &t)) != -1) 
				{
					pthread_t worker_thread;
					
					if (pthread_create (&worker_thread, NULL, HandleConnection, (void *) &client_socket_fd) != 0)
						{
							perror ("Could not create thread");
							return;
						}
				}
		}
}
	
 
static void InterruptHandler (int sig)
{
	g_running_flag = 0;
	signal (sig, SIG_IGN);
} 


static int BindToPort (const char *port_s, struct addrinfo **loaded_address_pp)
{
	struct addrinfo hints;
	int sock_fd = -1;
	int i;
	
	// first, load up address structs with getaddrinfo():
	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	i = getaddrinfo (NULL, port_s, &hints, loaded_address_pp);
	if (i == 0)
		{
			/* success */
			struct addrinfo *addr_p = *loaded_address_pp;
			int loop_flag = 1;
			
			/* loop through all the results and connect to the first we can */
			while (loop_flag)
				{
					sock_fd = socket (addr_p -> ai_family, addr_p -> ai_socktype, addr_p -> ai_protocol);

					if (sock_fd != -1)
						{
							int yes = 1;
							
							if (setsockopt (sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) != -1)
								{
									if (bind (sock_fd, addr_p -> ai_addr, addr_p -> ai_addrlen) == 0)
										{
											/* success */
											loop_flag = 0;
										}
									else
										{
											if (close (sock_fd) != 0)
												{
													/* error closing socket */
													
												}
												
											sock_fd = -1;
										}
									
								}
					
						}
						
					if (loop_flag)	
						{
							addr_p = addr_p -> ai_next;
							loop_flag = (addr_p != NULL);
						}
				}		/* while (addr_p) */
		}
		
	return sock_fd;
}

 
/*
 * This will handle connection for each client
 * */
static void *HandleConnection (void *socket_desc_p)
{
	//Get the socket descriptor
	int socket_fd = * (int *) socket_desc_p;
	ssize_t read_size;

	ByteBuffer *buffer_p = AllocateByteBuffer (1024);
	
	if (buffer_p)
		{
			bool success_flag = true;
			int id = 1;
			bool connected_flag = true;
			
			/* Get the message from the client */
			while (connected_flag && g_running_flag)
				{
					read_size = AtomicReceive (socket_fd, id, buffer_p);
				
					if (read_size > 0)
						{
							if (MakeByteBufferDataValidString (buffer_p))
								{
									char *request_s = buffer_p -> bb_data_p;

									if (strcmp (request_s, "QUIT") == 0)
										{
											connected_flag = false;
										}
									else
										{
											json_t *response_p = ProcessServerRawMessage (request_s, socket_fd);
											char *response_s = NULL;									
											
											if (response_p)
												{
													response_s = json_dumps (response_p, 0);
												}
											else
												{
													json_t *error_p = json_pack ("{s:s, s:s}", "error", "unable to process", "client_request", buffer_p -> bb_data_p);
													
													response_s = json_dumps (error_p, 0);
									
													json_decref (error_p);							
												}
											
											if (response_s)
												{
													int result = AtomicSendString (socket_fd, id, response_s);
													
													free (response_s);
												}
										}
									
								}		/* if (MakeByteBufferDataValidString (buffer_p)) */
						}
					else
						{
							connected_flag = false;
						}
				}
				
			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */

	return NULL;
} 
