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
	const char *from_s = NULL;
	const char *to_s = NULL;
	const char *filename_s = NULL;
	int api_id = -1;
	int i;
	
	if (argc < 3)
		{
			printf ("USAGE: client <username> <password>\n");
			return 0;
		}
	else
		{
			i = 1;
			
			while (i < argc)
				{
					char error_arg = 0;
					
					if (*argv [i] == '-')
						{
							switch (* (argv [i] + 1))
								{
									case 'u':
										if (++ i < argc)
											{
												username_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;
										
									case 's':
										if (++ i < argc)
											{
												port_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;

									case 'h':
										if (++ i < argc)
											{
												hostname_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;
										
									case 'p':
										if (++ i < argc)
											{
												password_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;

									case 'a':
										if (++ i < argc)
											{
												api_id = atoi (argv [i]);
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;

									case 'f':
										if (++ i < argc)
											{
												from_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;
										
									case 't':
										if (++ i < argc)
											{
												to_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;										
									
									case 'q':
										if (++ i < argc)
											{
												filename_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;										
									
									default:
										break;									
								}
						}
						
					++ i;
				}		/* while (i < argc) */
				

		sock_fd = ConnectToServer (hostname_s, port_s, &server_p);
		if (sock_fd >= 0)
			{
				json_t *json_p = NULL;
				
				switch (api_id)
					{
						case OP_LIST_ALL_SERVICES:
							json_p = GetAvailableServicesRequest (username_s, password_s);
							break;
							
							
						case OP_IRODS_MODIFIED_DATA:
							json_p = GetModifiedFilesRequest (username_s, password_s, from_s, to_s);
							break;
							
						case OP_LIST_INTERESTED_SERVICES:
							{
								if (filename_s)
									{					
										json_error_t error;										
										json_t *irods_file_p = json_pack_ex (&error, 0, "{s: {s:s}}", KEY_IRODS, KEY_FILENAME, filename_s);
										
										if (irods_file_p)
											{
												json_p = GetInterestedServicesRequest (username_s, password_s, irods_file_p);																
											}
									}
							}		
							break;
							
						default:
							break;
					}

				if (json_p)
					{
						uint32 id = 1;
						char *json_s = json_dumps (json_p, 0);
					
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
							
							
						free (json_s);
							
						json_decref (json_p);
					}

				freeaddrinfo (server_p);
				close (sock_fd);
			}
		else
			{
				printf ("failed to connect to server with code %d\n", sock_fd);
			}
			
		}
		
	


					
	return 0;
}





