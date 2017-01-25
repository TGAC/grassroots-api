/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <string.h>
#include <unistd.h>

#include "connection.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "json_tools.h"
#include "request_tools.h"
#include "streams.h"


#ifdef _DEBUG
	#define CONNECTION_DEBUG	(STM_LEVEL_FINE)
#else
	#define CONNECTION_DEBUG	(STM_LEVEL_NONE)
#endif


static int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);


static bool InitConnection (Connection *connection_p, ConnectionType type);


static void ReleaseConnection (Connection *connection_p);


static void FreeWebConnection (WebConnection *connection_p);


static void FreeRawConnection (RawConnection *connection_p);


/******************************/
/***** METHOD DEFINITIONS *****/
/******************************/


Connection *AllocaterRawClientConnection (int server_socket_fd)
{
	RawConnection *connection_p = (RawConnection *) AllocMemory (sizeof (RawConnection));

	if (connection_p)
		{
			if (InitConnection (& (connection_p -> rc_base), CT_RAW))
				{
					connection_p -> rc_data_buffer_p = AllocateByteBuffer (1024);

					if (connection_p -> rc_data_buffer_p)
						{
							struct sockaddr *remote_p = (struct sockaddr *) AllocMemory (sizeof (struct sockaddr));

							if (remote_p)
								{
									socklen_t t = sizeof (struct sockaddr);
									int client_socket_fd = accept (server_socket_fd, remote_p, &t);

									if (client_socket_fd != -1)
										{
											connection_p -> rc_sock_fd = client_socket_fd;
											connection_p -> rc_data.rc_client_p = remote_p;
											connection_p -> rc_server_connection_flag = false;

											return (& (connection_p -> rc_base));
										}

									FreeMemory (remote_p);
								}		/* if (remote_p) */

							FreeByteBuffer (connection_p -> rc_data_buffer_p);
						}		/* if (connection_p -> rc_data_buffer_p) */

					ReleaseConnection (& (connection_p -> rc_base));
				}		/* if (InitConnection (& (connection_p -> rc_base))) */

			FreeMemory (connection_p);
		}		/* if (connection_p) */

	return NULL;
}


static bool InitConnection (Connection *connection_p, ConnectionType type)
{
	bool success_flag = true;

	connection_p -> co_id = 1;
	connection_p -> co_type = type;

	return success_flag;
}


static void ReleaseConnection (Connection * UNUSED_PARAM (connection_p))
{

}


Connection *AllocateRawServerConnection (const char * const hostname_s, const char * const port_s)
{
	RawConnection *connection_p = (RawConnection *) AllocMemory (sizeof (RawConnection));

	if (connection_p)
		{
			if (InitConnection (& (connection_p -> rc_base), CT_RAW))
				{
					connection_p -> rc_data_buffer_p = AllocateByteBuffer (1024);

					if (connection_p -> rc_data_buffer_p)
						{
							struct addrinfo *server_p = NULL;
							int fd = ConnectToServer (hostname_s, port_s, &server_p);

							if (fd >= 0)
								{
									connection_p -> rc_sock_fd = fd;
									connection_p -> rc_data.rc_server_p = server_p;
									connection_p -> rc_server_connection_flag = true;

									return (& (connection_p -> rc_base));
								}		/* if (fd >= 0) */

							FreeByteBuffer (connection_p -> rc_data_buffer_p);
						}

					ReleaseConnection (& (connection_p -> rc_base));
				}		/* if (InitConnection (& (connection_p -> rc_base))) */

			FreeMemory (connection_p);
		}		/* if (connection_p) */


	return NULL;
}


Connection *AllocateWebServerConnection (const char * const full_uri_s)
{
	WebConnection *connection_p = (WebConnection *) AllocMemory (sizeof (WebConnection));

	if (connection_p)
		{
			CurlTool *curl_p = AllocateCurlTool (CM_MEMORY);

			if (curl_p)
				{
					char *uri_s = CopyToNewString (full_uri_s, 0, false);

					if (uri_s)
						{
							connection_p -> wc_uri_s = uri_s;

							if (SetUriForCurlTool (curl_p, uri_s))
								{
									if (SetCurlToolForJSONPost (curl_p))
										{
											if (InitConnection (& (connection_p -> wc_base), CT_WEB))
												{
													connection_p -> wc_curl_p = curl_p;

													return (& (connection_p -> wc_base));
												}		/* if (InitConnection (& (connection_p -> wc_base))) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "InitConnection failed for %s", uri_s);
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "SetCurlToolForJSONPost failed for %s", uri_s);
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to SetUriForCurlTool to %s", uri_s);
								}

							FreeCopiedString (connection_p -> wc_uri_s);
						}		/* if (uri_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate copy %s", full_uri_s);
						}

					FreeCurlTool (curl_p);
				}		/* if (curl_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate CurlTool to %s", full_uri_s);
				}

			FreeMemory (connection_p);
		}		/* if (connection_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate WebConnection to %s", full_uri_s);
		}

	return NULL;
}


void FreeConnection (Connection *connection_p)
{

	switch (connection_p -> co_type)
		{
			case CT_RAW:
				FreeRawConnection ((RawConnection *) connection_p);
				break;

			case CT_WEB:
				FreeWebConnection ((WebConnection *) connection_p);
				break;

			default:
				break;
		}

	ReleaseConnection (connection_p);

	FreeMemory (connection_p);
}



const char *MakeRemoteJsonCallViaConnection (Connection *connection_p, json_t *req_p)
{
	bool success_flag = false;

	#if CONNECTION_DEBUG >= STM_LEVEL_FINE
	char *req_s = json_dumps (req_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "MakeRemoteJsonCallViaConnection req:\n%s\n", req_s);
	#endif

	if (connection_p -> co_type == CT_RAW)
		{
			RawConnection *raw_connection_p = (RawConnection *) connection_p;

			if (SendJsonRequestViaRawConnection (raw_connection_p, req_p) > 0)
				{
					if (AtomicReceiveViaRawConnection (raw_connection_p) > 0)
						{
							success_flag = true;
						}
				}
		}
	else if (connection_p -> co_type == CT_WEB)
		{
			WebConnection *web_connection_p = (WebConnection *) connection_p;

			if (MakeRemoteJSONCallFromCurlTool (web_connection_p -> wc_curl_p, req_p))
				{
					success_flag = true;
				}		/* if (MakeRemoteJSONCallFromCurlTool (web_connection_p -> wc_curl_p, req_p)) */
		}

	#if CONNECTION_DEBUG >= STM_LEVEL_FINE
	free (req_s);
	#endif

	return (success_flag ? GetConnectionData (connection_p) : NULL);
}



int SendJsonRequestViaRawConnection (RawConnection *connection_p, const json_t *json_p)
{
	int res = -1;
	char *req_s = json_dumps (json_p, 0);

	if (req_s)
		{
			res = AtomicSendStringViaRawConnection (req_s, connection_p);
			free (req_s);
		}

	return res;
}



static void FreeWebConnection (WebConnection *connection_p)
{
	FreeCurlTool (connection_p -> wc_curl_p);
	FreeCopiedString (connection_p -> wc_uri_s);
}


static void FreeRawConnection (RawConnection *connection_p)
{
	if (connection_p -> rc_server_connection_flag)
		{
			if (connection_p -> rc_data.rc_server_p)
				{
					freeaddrinfo (connection_p -> rc_data.rc_server_p);
				}
		}
	else
		{
			if (connection_p -> rc_data.rc_client_p)
				{
					FreeMemory (connection_p -> rc_data.rc_client_p);
				}
		}

	FreeByteBuffer (connection_p -> rc_data_buffer_p);


	close (connection_p -> rc_sock_fd);
}


const char *GetConnectionData (Connection *connection_p)
{
	const char *data_s = NULL;

	if (connection_p -> co_type == CT_RAW)
		{
			RawConnection *raw_connection_p = (RawConnection *) connection_p;

			data_s = GetByteBufferData (raw_connection_p -> rc_data_buffer_p);
		}
	else if (connection_p -> co_type == CT_WEB)
		{
			WebConnection *web_connection_p = (WebConnection *) connection_p;

			data_s = GetCurlToolData (web_connection_p -> wc_curl_p);
		}

	return data_s;
}


static int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp)
{
	struct addrinfo hints;
	int i;
	int sock_fd = -1;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	i = getaddrinfo (hostname_s, port_s, &hints, server_pp);

	if (i == 0)
		{
			struct addrinfo *addr_p = *server_pp;
			int loop_flag = 1;

			/* loop through all the results and connect to the first we can */
			while (loop_flag)
				{
					sock_fd = socket (addr_p -> ai_family, addr_p -> ai_socktype, addr_p -> ai_protocol);

					if (sock_fd != -1)
						{
							i = connect (sock_fd, addr_p -> ai_addr, addr_p -> ai_addrlen);

							if (i != -1)
								{

									loop_flag = 0;
								}
							else
								{
									close (sock_fd);
									sock_fd = -1;
								}
						}

					if (loop_flag)
						{
							addr_p = addr_p -> ai_next;
							loop_flag = (addr_p != NULL);
						}
				}		/* while (addr_p) */

		}		/* if (i == 0) */

	return sock_fd;
}

