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


static int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);




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


static int SendJsonRequest (int socket_fd, json_t *json_p)
{
	int res = -1;
	char *req_s = json_dumps (json_p, 0);
	
	if (req_s)
		{
			res = SendQuery (socket_fd, req_s);
			free (req_s);
		}
		
	return res;
}


static int SendRequest (int socket_fd, const char * const req_s)
{
	int res = -1;

		
	return res;
}



/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetLoginJson (const char * const username_s, const char *password_s)
{
	json_t *json_p = json_pack ("s{s{ssss}}", "irods", "credentials", "user", username_s, "password", password_s);
	
	return json_p;
}

/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetModifiedFiles (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s)
{
	json_t *json_p = json_pack ("s{s{ssss}s{ssss}}", "irods", "credentials", "user", username_s, "password", password_s, "interval", "from", from_s, "to", to_s);
	
	return json_p;
}


/*
 * Obviously for a real system we'd be using encryption, tokens and the like
 */
json_t *GetAvailableServices (const char * const username_s, const char * const password_s, const char * const from_s, const char * const to_s)
{
	json_t *json_p = json_pack ("s", "services");
	return json_p;
}




static int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp)
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

