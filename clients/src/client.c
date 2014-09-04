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


int main(int argc, char *argv[])
{
	int sock_fd;  
	struct addrinfo *server_p = NULL;
	const char *hostname_s = NULL;
	const char *port_s = NULL;
	
	if (argc == 2)
		{
			switch (argc)
				{
					case 2:
						port_s = argv [1];
							
					/* deliberate fall through */
					case 1:
						hostname_s = argv [0];
						break;
					
					default:
						break;
				}
			
			sock_fd = ConnectToServer (hostname_s, port_s, &server_p);
			if (sock_fd >= 0)
				{
					


					freeaddrinfo (server_p);
					close (sock_fd);
				}
			
		}
		
	return 0;
}




