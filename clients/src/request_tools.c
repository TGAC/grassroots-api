#include <string.h>

#include <netdb.h>

#include "request_tools.h"


int SendRequest (int socket_fd, const char * const req_s)
{
	int res = -1;

		
	return res;
}




int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp)
{
	struct addrinfo hints;
	int i;
	
	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	i = getaddrinfo (hostname_s, port_s, &hints, server_pp);
	
	if (i == 0)
		{
			struct addrinfo *addr_p = *server_pp;
			int sock_fd = -1;
			
			/* loop through all the results and connect to the first we can */
			while (addr_p)
				{
					sock_fd = socket (addr_p -> ai_family, addr_p -> ai_socktype, addr_p -> ai_protocol);

					if (sock_fd != -1)
						{
							addr_p = NULL;
						}
					else
						{
							addr_p = addr_p -> ai_next;
						}
				}		/* while (addr_p) */
				
			/* If we have a valid socket, try to connect to it */
			if (sock_fd != -1)
				{
					if (connect (sock_fd, addr_p -> ai_addr, addr_p -> ai_addrlen) != 0)
						{
							i = -1;
						}
				}
				
		}		/* if (i == 0) */
	
	return i;
}
