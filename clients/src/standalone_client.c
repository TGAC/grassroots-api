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
#include "parameter_set.h"
#include "client.h"
#include "server.h"

/*********************************/
/******* STATIC PROTOTYPES *******/
/*********************************/

static json_t *SendRequest (const int sock_fd, json_t *req_p, const uint32 id);


/*************************************/
/******* FUNCTION DEFINITIONS  *******/
/*************************************/

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
	const char *client_s = "qt";
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
				json_t *req_p = NULL;
				json_t *response_p = NULL;
				uint32 id = 1;
				
				switch (api_id)
					{
						case OP_LIST_ALL_SERVICES:
							req_p = GetAvailableServicesRequest (username_s, password_s);
							response_p = SendRequest (sock_fd, req_p, id);						
							break;
							
							
						case OP_IRODS_MODIFIED_DATA:
							req_p = GetModifiedFilesRequest (username_s, password_s, from_s, to_s);
							response_p = SendRequest (sock_fd, req_p, id);						
							break;
							
						case OP_LIST_INTERESTED_SERVICES:
							{
								if (filename_s)
									{					
										json_error_t error;										
										json_t *irods_file_p = json_pack_ex (&error, 0, "{s: {s:s}}", KEY_IRODS, KEY_FILENAME, filename_s);
										
										if (irods_file_p)
											{
												req_p = GetInterestedServicesRequest (username_s, password_s, irods_file_p);																
											}
											
										if (req_p)
											{
												response_p = SendRequest (sock_fd, req_p, id);						
												
												if (response_p)
													{
														ParameterSet *params_p = CreateParameterSetFromJSON (response_p);
														
														if (params_p)
															{
																Client *client_p = LoadClient ("clients", client_s);
																
																if (client_p)
																	{
																		if (RunClient (client_p, filename_s, params_p))
																			{
																				/* run the service */
																				json_t *param_json_p = json_object_get (response_p, "params");
																				
																				if (param_json_p)
																					{
																						
																					}
																				else
																					{
																						
																					}
																				
																			}
																	}
															}
													}
											}
									}
							}		
							break;
							
						default:
							break;
					}

				if (req_p)
					{
						
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


/*
static void RunServicesOnFile ()
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
*/

static json_t *SendRequest (const int sock_fd, json_t *req_p, const uint32 id)
{
	char *req_s = json_dumps (req_p, 0);
	json_t *response_p = NULL;

	if (SendJsonRequest (sock_fd, id, req_p) > 0)
		{
			char buffer_s [10240] = { 0 };
			
			if (AtomicReceive (sock_fd, id, buffer_s, 10239) > 0)
				{						
					json_error_t err;
														
					printf ("%s\n", buffer_s);

					response_p = json_loads (buffer_s, 0, &err);

					if (!response_p)
						{
							printf ("error decoding response: \"%s\"\n\"%s\"\n%d %d %d\n", err.text, err.source, err.line, err.column, err.position);
						}										
				}
			else
				{
					printf ("no buffer\n");
				}
								
		}
	
	free (req_s);							
	json_decref (req_p);

	return response_p;
}
	





