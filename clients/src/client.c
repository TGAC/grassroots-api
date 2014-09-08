/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>


#include "json_tools.h"
#include "request_tools.h"
#include "server.h"

int main(int argc, char *argv[])
{
	int sock_fd;  
	struct addrinfo *server_p = NULL;
	const char *hostname_s = "localhost";
	const char *port_s = DEFAULT_SERVER_PORT;
	
	
	switch (argc)
		{
			case 3:
				port_s = argv [1];
				
			/* deliberate fall through */
			case 2:
				hostname_s = argv [0];
				break;
				
			default:
				break;
		}

	sock_fd = ConnectToServer (hostname_s, port_s, &server_p);
	if (sock_fd >= 0)
		{
			json_t *json_p = GetLoginJson ("foo", "bar");

			if (json_p)
				{
					SendJsonRequest (sock_fd, json_p);
					json_decref (json_p);
				}

			freeaddrinfo (server_p);
			close (sock_fd);
		}
	else
		{
			printf ("failed to connect to server with code %d\n", sock_fd);
		}
					
	return 0;
}




