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
#include "json_util.h"
#include "request_tools.h"
#include "parameter_set.h"
#include "client.h"
#include "server.h"
#include "byte_buffer.h"


/*********************************/
/******* STATIC PROTOTYPES *******/
/*********************************/


static bool ShowResults (json_t *response_p, Client *client_p);


static json_t *ShowServices (json_t *response_p, Client *client_p, const char *username_s, const char *password_s, Connection *connection_p);


static json_t *GetUserParameters (json_t *client_results_p, const char * const username_s, const char * const password_s, Connection *connection_p);


/*************************************/
/******* FUNCTION DEFINITIONS  *******/
/*************************************/

int main(int argc, char *argv[])
{
	const char *hostname_s = "localhost";
	const char *port_s = DEFAULT_SERVER_PORT;
	const char *username_s = NULL;
	const char *password_s = NULL;
	const char *from_s = NULL;
	const char *to_s = NULL;
	const char *query_s = NULL;
	const char *client_s = "wheatis-qt-client";
	const char *protocol_s = NULL;
	int api_id = -1;
	int i;
	Connection *connection_p = NULL;
	
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
												query_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;										

									case 'P':
										if (++ i < argc)
											{
												protocol_s = argv [i];
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
				
		connection_p = AllocateConnection (hostname_s, port_s);
		if (connection_p)
			{
				json_t *req_p = NULL;
				json_t *response_p = NULL;
				uint32 id = 1;

				ByteBuffer *buffer_p = AllocateByteBuffer (1024);
				
				if (buffer_p)
					{				
						Client *client_p = LoadClient ("clients", client_s);

						if (client_p)
							{
								switch (api_id)
									{
										case OP_LIST_ALL_SERVICES:
											req_p = GetAvailableServicesRequest (username_s, password_s);

											if (req_p)
												{
													if (!AddCredentialsToJson (req_p, username_s, password_s))
														{
															printf ("Failed to add credentials\n");
														}

													response_p = MakeRemoteJsonCall (req_p, connection_p);

													if (response_p)
														{
															json_t *run_services_response_p = ShowServices (response_p, client_p, username_s, password_s, connection_p);

															if (run_services_response_p)
																{
																	ShowResults (run_services_response_p, client_p);
																}		/* if (run_services_response_p) */

														}		/* if (response_p) */

												}		/* if (req_p) */
											break;


										case OP_IRODS_MODIFIED_DATA:
											req_p = GetModifiedFilesRequest (username_s, password_s, from_s, to_s);
											response_p = MakeRemoteJsonCall (req_p, connection_p);
											break;

										case OP_LIST_INTERESTED_SERVICES:
											{
												if (protocol_s && query_s)
													{
														req_p = GetInterestedServicesRequest (username_s, password_s, protocol_s, query_s);

														if (req_p)
															{
																response_p = MakeRemoteJsonCall (req_p, connection_p);

																if (response_p)
																	{
																		ShowServices (response_p, client_p, username_s, password_s, connection_p);
																	}		/* if (response_p) */

															}		/* if (req_p) */

													}
											}
											break;

										case OP_RUN_KEYWORD_SERVICES:
											{
												if (query_s)
													{
														req_p = GetKeywordServicesRequest (username_s, password_s, query_s);

														if (req_p)
															{
																response_p = MakeRemoteJsonCall (req_p, connection_p);

																if (response_p)
																	{
																		ShowResults (response_p, client_p);
																	}		/* if (response_p) */

															}		/* if (req_p) */

													}		/* if (query_s) */
											}
											break;

										default:
											break;
									}
							}

						FreeByteBuffer (buffer_p);
					}		/* if (buffer_p) */				

				FreeConnection (connection_p);
			}
		else
			{
				printf ("failed to connect to server %s:%s\n", hostname_s, port_s);
			}
			
		}
	

					
	return 0;
}


static bool ShowResults (json_t *response_p, Client *client_p)
{
	bool success_flag = false;

	DisplayResultsInClient (client_p, response_p);

	return success_flag;
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

	


static json_t *ShowServices (json_t *response_p, Client *client_p, const char *username_s, const char *password_s, Connection *connection_p)
{
	json_t *services_json_p = NULL;

	#ifdef _DEBUG
	char *response_s = json_dumps (response_p, JSON_INDENT (2));
	printf ("res:\n%s\n", response_s);
	#endif
	
	if (json_is_array (response_p))
		{
			json_t *client_results_p = NULL;
			const size_t num_services = json_array_size (response_p);
			size_t i = 0;
			
			for (i = 0; i < num_services; ++ i)
				{
					json_t *service_json_p = json_array_get (response_p, i);
					const char *service_name_s = GetJSONString (service_json_p, SERVICES_NAME_S);

					#ifdef _DEBUG
					char *service_s = json_dumps (service_json_p, JSON_INDENT (2));
					printf ("client received service %ld:\n%s\n\n", i, service_s);
					#endif


					if (service_name_s)
						{
							const char *service_description_s = GetJSONString (service_json_p, SERVICES_DESCRIPTION_S);

							if (service_description_s)
								{
									json_t *ops_p = json_object_get (service_json_p, SERVER_OPERATIONS_S);

									if (ops_p)
										{
											ParameterSet *params_p = CreateParameterSetFromJSON (ops_p);
											
											if (params_p)
												{
													const char *service_info_uri_s = GetJSONString (ops_p, OPERATION_INFORMATION_URI_S);

													int res = AddServiceToClient (client_p, service_name_s, service_description_s, service_info_uri_s, params_p);
												}		/* if (params_p) */
										}
								}		/* if (service_description_s)	*/

						}		/* if (service_name_s) */

					#ifdef _DEBUG
					free (service_s);
					#endif

				}		/* for (i = 0; i < num_services; ++ i) */

			/* Get the results of the user's configuration */
			client_results_p = RunClient (client_p);

			services_json_p = GetUserParameters (client_results_p, username_s, password_s, connection_p);

		}		/* if (json_is_array (response_p)) */


	#ifdef _DEBUG
	free (response_s);
	#endif

	return services_json_p;
}




static json_t *GetUserParameters (json_t *client_results_p, const char * const username_s, const char * const password_s, Connection *connection_p)
{
	json_t *services_json_p = NULL;

	if (client_results_p)
		{
			char *client_results_s = json_dumps (client_results_p, JSON_INDENT (2));
			json_t *new_req_p = json_object ();

			if (new_req_p)
				{
					if (!AddCredentialsToJson (new_req_p, username_s, password_s))
						{
							printf ("failed to add credentials to request\n");
						}

					if (json_object_set_new (new_req_p, SERVICES_NAME_S, client_results_p) == 0)
						{
							char *new_req_s  = json_dumps (new_req_p, JSON_INDENT (2));

							printf ("client sending:\n%s\n", new_req_s);

							services_json_p = MakeRemoteJsonCall (new_req_p, connection_p);

							if (services_json_p)
								{
									char *response_s = json_dumps (services_json_p, JSON_INDENT (2));

									if (response_s)
										{
											printf ("%s\n", response_s);
											free (response_s);
										}
								}
							else
								{
									printf ("no response\n");
								}

							if (new_req_s)
								{
									free (new_req_s);
								}
							
						}		/* if (json_object_set_new (new_req_p, SERVICES_S, client_results_p) */

					json_decref (new_req_p);

				}		/* if (new_req_p) */

			if (client_results_s)
				{
					printf ("%s\n", client_results_s);
					free (client_results_s);
				}

		}
	else
		{
			printf ("no results from client\n");
		}
	
	return services_json_p;
}


