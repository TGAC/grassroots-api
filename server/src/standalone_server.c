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

#include "thpool.h"
 
#include "string_linked_list.h"
#include "string_utils.h"


#include "server.h"


/**********************/
/***** PROTOTYPES *****/ 
/**********************/

static int BindToPort (const char *port_s, struct addrinfo **loaded_address_pp);

static void *HandleConnection (void *socket_desc_p);

static void InterruptHandler (int sig);

static void RunServer (int server_socket_fd, int num_threads);

static char *ProcessMessage (const char * const request_s);


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
	int max_queue_size;
	int num_threads = 4;
	int i = 0;
	
	switch (argc)
		{
			case 2:
				i = atoi (argv [1]);
				if (i > 0)
					{
						num_threads = i;
						i = 0;
					}
				
				/* deliberate fall through */
				case 1:
					port_s = argv [0];
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

	if (socket_fd >= 0)
		{
			RunServer (socket_fd, num_threads);
							
			close (socket_fd);
		}		/* if (socket_fd >= 0) */

	return 0;
}
 
 
static void RunServer (int server_socket_fd, int num_threads)
{
	thpool_t *thread_pool_p = thpool_init (num_threads);
	
	if (thread_pool_p)
		{
			while (g_running_flag)
				{
					struct sockaddr remote;
					socklen_t t = sizeof (remote);
					int client_socket_fd;
					
					if ((client_socket_fd = accept (server_socket_fd, (struct sockaddr *) &remote, &t)) != -1) 
						{
							void *(*worker_fn) (void *) = HandleConnection;
							
							thpool_add_work (thread_pool_p, worker_fn, (void *) &client_socket_fd);
						}
				}
				
			thpool_destroy (thread_pool_p);
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
			struct addrinfo *res_p = *loaded_address_pp;
			
			// make a socket:
			sock_fd = socket (res_p -> ai_family, res_p -> ai_socktype, res_p -> ai_protocol);
			
			if (sock_fd >= 0)
				{
					if (bind (sock_fd, res_p -> ai_addr, res_p -> ai_addrlen) == 0)
						{
							/* success */
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
			else
				{
					
				}
		}
	else
		{
			
		}

	return sock_fd;
}

 
/*
 * This will handle connection for each client
 * */
static void *HandleConnection (void *socket_desc_p)
{
	#define BUFFER_SIZE (1024)
	
	//Get the socket descriptor
	int socket_fd = * (int *) socket_desc_p;
	ssize_t read_size;
	char client_buffer_s [BUFFER_SIZE];
	LinkedList *buffer_p = AllocateStringLinkedList ();
	
	if (buffer_p)
		{
			bool success_flag = true;
			
			/* Get the message from the client */
			while (success_flag && ((read_size = recv (socket_fd, client_buffer_s, BUFFER_SIZE - 1, 0)) > 0))
				{
					//end of string marker
					* (client_buffer_s + read_size) = '\0';
					
					success_flag = AddStringToStringLinkedList (buffer_p, client_buffer_s, MF_DEEP_COPY);
				}
			
			if (success_flag)
				{
					char *message_s = GetStringLinkedListAsString (buffer_p);
					
					if (message_s)
						{
							char *response_s = ProcessMessage (message_s);
							
							if (response_s)
								{
									
									
								}		/* if (response_s) */
							
							FreeCopiedString (message_s);
						}		/* if (message_s) */
					
				}		/* if (success_flag) */
			
			FreeLinkedList (buffer_p);
		}

	return NULL;
} 
