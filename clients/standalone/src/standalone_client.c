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
#include "connection.h"
#include "string_utils.h"


#ifdef _DEBUG
	#define STANDALONE_CLIENT_DEBUG	(STM_LEVEL_FINE)
#else
	#define STANDALONE_CLIENT_DEBUG	(STM_LEVEL_NONE)
#endif


/*********************************/
/******* STATIC PROTOTYPES *******/
/*********************************/


static bool ShowResults (json_t *response_p, Client *client_p);


static json_t *ShowServices (json_t *response_p, Client *client_p, const char *username_s, const char *password_s, Connection *connection_p);


static char *GetFullServerURI (const char *hostname_s, const char *port_s, const char *uri_s);


static int AddServiceDetailsToClient (Client *client_p, json_t *service_json_p, const json_t *provider_p);


/*************************************/
/******* FUNCTION DEFINITIONS  *******/
/*************************************/

int main (int argc, char *argv [])
{
	const char *hostname_s = "localhost";
	const char *port_s = DEFAULT_SERVER_PORT;
	const char *username_s = NULL;
	const char *password_s = NULL;
	const char *from_s = NULL;
	const char *to_s = NULL;
	const char *query_s = NULL;
	const char *uri_s = NULL;
	const char *client_s = "wheatis-qt-client";
	const char *protocol_s = NULL;
	bool web_server_flag = false;
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
									
									case 'U':
										if (++ i < argc)
											{
												uri_s = argv [i];
											}
										else
											{
												error_arg = * (argv [i] - 1);
											}
										break;

									case 'W':
										web_server_flag = true;
										break;

									default:
										break;									
								}
						}
						
					++ i;
				}		/* while (i < argc) */

				if (web_server_flag)
					{
						char *full_uri_s = GetFullServerURI (hostname_s, port_s, uri_s);

						if (full_uri_s)
							{
								connection_p = AllocateWebServerConnection (full_uri_s);
								FreeCopiedString (full_uri_s);
							}
					}
				else
					{
						connection_p = AllocateRawServerConnection (hostname_s, port_s);
					}

				if (connection_p)
					{
						Client *client_p = LoadClient ("clients", client_s, connection_p);

						if (client_p)
							{
								json_t *req_p = NULL;
								json_t *response_p = NULL;

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

										case OP_GET_NAMED_SERVICES:
											{
												req_p = GetNamedServicesRequest (username_s, password_s, query_s);

												if (req_p)
													{
														response_p = MakeRemoteJsonCall (req_p, connection_p);

														if (response_p)
															{
																ShowServices (response_p, client_p, username_s, password_s, connection_p);
															}		/* if (response_p) */

													}		/* if (req_p) */
											}
											break;

										case OP_CHECK_SERVICE_STATUS:
											{
												req_p = GetCheckServicesRequest (username_s, password_s, query_s);

												if (req_p)
													{
														response_p = MakeRemoteJsonCall (req_p, connection_p);

														if (response_p)
															{
																char *dump_s = json_dumps (response_p, JSON_INDENT (2));

																if (dump_s)
																	{
																		PrintLog (STM_LEVEL_INFO, "%s", dump_s);
																		free (dump_s);
																	}

																//ShowServices (response_p, client_p, username_s, password_s, connection_p);
															}		/* if (response_p) */

													}		/* if (req_p) */
											}
											break;

										default:
											break;
									}		/* switch (api_id) */

								WipeJSON (req_p);
								WipeJSON (response_p);

							}		/* if (client_p) */

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


static char *GetFullServerURI (const char *hostname_s, const char *port_s, const char *uri_s)
{
	char *full_uri_s = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			if (!hostname_s)
				{
					hostname_s = "localhost";
				}

			if (AppendToByteBuffer (buffer_p, hostname_s, strlen (hostname_s)))
				{
					bool success_flag = true;

					if (port_s)
						{
							success_flag = AppendStringsToByteBuffer (buffer_p, ":", port_s, NULL);
						}

					if (success_flag)
						{
							if (uri_s)
								{
									success_flag = AppendStringsToByteBuffer (buffer_p, "/", uri_s, NULL);
								}

							if (success_flag)
								{
									const char *data_s = GetByteBufferData (buffer_p);

									if (data_s)
										{
											full_uri_s = CopyToNewString (data_s, 0, false);
										}
								}
						}

				}		/* if (AppendToByteBuffer (buffer_p, hostname_s, strlen (hostname_s))) */

			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */

	return full_uri_s;
}
	


static json_t *ShowServices (json_t *response_p, Client *client_p, const char *username_s, const char *password_s, Connection *connection_p)
{
	json_t *services_json_p = NULL;

	#if STANDALONE_CLIENT_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (response_p, "res:\n", STANDALONE_CLIENT_DEBUG);
	#endif
	
	if (json_is_array (response_p))
		{
			json_t *client_results_p = NULL;
			const size_t num_services = json_array_size (response_p);
			size_t i = 0;
			
			for (i = 0; i < num_services; ++ i)
				{
					json_t *service_json_p = json_array_get (response_p, i);
					json_t *ops_p = json_object_get (service_json_p, SERVER_OPERATIONS_S);
					json_t *provider_p = json_object_get (service_json_p, SERVER_PROVIDER_S);


					#if STANDALONE_CLIENT_DEBUG >= STM_LEVEL_FINER
					PrintJSONToLog (service_json_p, "next service:\n", STANDALONE_CLIENT_DEBUG);
					#endif

					if (ops_p)
						{
							if (json_is_array (ops_p))
								{
									size_t i;
									json_t *op_p;

									json_array_foreach (ops_p, i, op_p)
										{
											AddServiceDetailsToClient (client_p, op_p, provider_p);
										}
								}
							else
								{
									AddServiceDetailsToClient (client_p, ops_p, provider_p);
								}
						}

				}		/* for (i = 0; i < num_services; ++ i) */

			/* Get the results of the user's configuration */
			client_results_p = RunClient (client_p);
		}		/* if (json_is_array (response_p)) */

	return services_json_p;
}


static int AddServiceDetailsToClient (Client *client_p, json_t *service_json_p, const json_t *provider_p)
{
	int res = -1;
	const char *service_name_s = GetJSONString (service_json_p, OPERATION_ID_S);

	#if STANDALONE_CLIENT_DEBUG >= STM_LEVEL_FINER
	PrintJSONToLog (service_json_p, "client received service:\n", STANDALONE_CLIENT_DEBUG);
	#endif

	if (service_name_s)
		{
			const char *service_description_s = GetJSONString (service_json_p, SERVICES_DESCRIPTION_S);

			if (service_description_s)
				{
					ParameterSet *params_p = CreateParameterSetFromJSON (service_json_p);

					if (params_p)
						{
							const char *service_info_uri_s = GetJSONString (service_json_p, OPERATION_INFORMATION_URI_S);

							res = AddServiceToClient (client_p, service_name_s, service_description_s, service_info_uri_s, provider_p, params_p);
						}		/* if (params_p) */

				}		/* if (service_description_s) */

		}		/* if (service_name_s) */

	return res;
}


