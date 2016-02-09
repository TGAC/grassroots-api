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
	Reworked on an example taken from https://gist.github.com/silv3rm00n/5821760

	A simplistic server allowing us to test the grassroots architecture.
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
#include "service_config.h"
#include "system_util.h"
#include "connection.h"
#include "parameter_group.h"

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
					if (InitInformationSystem ())
						{
							RunServer (socket_fd);					
							
							if (!DestroyInformationSystem ())
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
			Connection *connection_p = NULL;
			
			if (g_running_flag && (connection_p = AllocaterRawClientConnection (server_socket_fd)))
				{
					pthread_t worker_thread;

					if (pthread_create (&worker_thread, NULL, HandleConnection, (void *) connection_p) != 0)
						{
							perror ("Could not create thread");
							return;
						}

					//FreeConnection (connection_p);
				}
		}
}
	
 
static void InterruptHandler (int sig)
{
	printf ("InterruptHandler %d\n", sig);
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
static void *HandleConnection (void *conn_p)
{
	//Get the socket descriptor
	RawConnection *connection_p = (RawConnection *) conn_p;
	ssize_t read_size;
	bool success_flag = true;
	bool connected_flag = true;
	
	/* Get the message from the client */
	while (connected_flag && g_running_flag)
		{
			read_size = AtomicReceiveViaRawConnection (connection_p);

			if (read_size > 0)
				{
					const char *request_s = GetConnectionData (& (connection_p -> rc_base));

					if (strcmp (request_s, "QUIT") == 0)
						{
							connected_flag = false;
						}
					else
						{
							json_t *response_p = ProcessServerRawMessage (request_s, connection_p -> rc_sock_fd);
							char *response_s = NULL;

							if (response_p)
								{
									response_s = json_dumps (response_p, 0);
								}
							else
								{
									json_t *error_p = json_pack ("{s:s, s:s}", "error", "unable to process", "client_request", request_s);
									
									response_s = json_dumps (error_p, 0);

									json_decref (error_p);
								}
							
							if (response_s)
								{
									int result = AtomicSendStringViaRawConnection (response_s, connection_p);
									
									free (response_s);
								}
						}
				}
			else
				{
					connected_flag = false;
				}
		}

	return NULL;
} 
