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
	const char *username_s = NULL;
	const char *password_s = NULL;
	
	if (argc < 3)
		{
			printf ("USAGE: client <username> <password>\n");
			return 0;
		}
		
		
	switch (argc)
		{
			case 5:
				port_s = argv [4];
				
			/* deliberate fall through */
			case 4:
				hostname_s = argv [3];
				
			/* deliberate fall through */
			default:
				username_s = argv [1];
				password_s = argv [2];
				break;
		}

	sock_fd = ConnectToServer (hostname_s, port_s, &server_p);
	if (sock_fd >= 0)
		{
			json_t *json_p = GetAvailableServices (username_s, password_s);

			if (json_p)
				{
					uint32 id = 1;
					
					if (SendJsonRequest (sock_fd, id, json_p) > 0)
						{
							char buffer_s [10240] = { 0 };
							
							if (AtomicReceive (sock_fd, id, buffer_s, 10239) > 0)
								{
									printf ("%s\n", buffer_s);
								}
							else
								{
									printf ("no buffer\n");
								}
							
							
						}
						
						
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




