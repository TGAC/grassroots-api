#include <sys/types.h>   // Types used in sys/socket.h and netinet/in.h
#include <netinet/in.h>  // Internet domain address structures and functions
#include <sys/socket.h>  // Structures and functions used for socket API
#include <netdb.h>       // Used for domain/DNS hostname lookup
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>



int main (int argc, char *argv [])
{
	int port;
	int socket_handle;

	/* create socket */
	if ((socket_handle = ConnectToSocket ()) >= 0)
		{
			if (BindToPort (socket_handle, port))
				{
					
				}
			
			close (socket_handle);
		}
}
          
