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

					ReleaseConnection (& (connection_p -> rc_base));
				}		/* if (InitConnection (& (connection_p -> rc_base))) */

			FreeMemory (connection_p);
		}		/* if (connection_p) */

	return NULL;
}


static bool InitConnection (Connection *connection_p, ConnectionType type)
{
	bool success_flag = false;

	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			connection_p -> co_data_buffer_p = buffer_p;
			connection_p -> co_id = 1;
			connection_p -> co_type = type;

			success_flag = true;
		}		/* if (buffer_p) */

	return success_flag;
}


static void ReleaseConnection (Connection *connection_p)
{
	FreeByteBuffer (connection_p -> co_data_buffer_p);
}


Connection *AllocateRawServerConnection (const char * const hostname_s, const char * const port_s)
{
	RawConnection *connection_p = (RawConnection *) AllocMemory (sizeof (RawConnection));

	if (connection_p)
		{
			if (InitConnection (& (connection_p -> rc_base), CT_RAW))
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
			CurlTool *curl_p = AllocateCurlTool ();

			if (curl_p)
				{
					bool success_flag = false;
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

										}
								}

							FreeCopiedString (connection_p -> wc_uri_s);
						}		/* if (uri_s) */

					FreeCurlTool (curl_p);
				}		/* if (curl_p) */



			FreeMemory (connection_p);
		}		/* if (connection_p) */

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
	char *req_s = json_dumps (req_p, JSON_INDENT (2) | JSON_PRESERVE_ORDER);

	#if CONNECTION_DEBUG >= STM_LEVEL_FINE
	PrintLog (STM_LEVEL_FINE, "MakeRemoteJsonCallViaConnection req:\n%s\n", req_s);
	#endif

	if (connection_p -> co_type == CT_RAW)
		{
			RawConnection *raw_connection_p = (RawConnection *) connection_p;

			if (SendJsonRawRequest (req_p, raw_connection_p) > 0)
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

	free (req_s);

	return (success_flag ? GetConnectionData (connection_p) : NULL);
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

	close (connection_p -> rc_sock_fd);
}


const char *GetConnectionData (Connection *connection_p)
{
	return GetByteBufferData (connection_p -> co_data_buffer_p);
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

