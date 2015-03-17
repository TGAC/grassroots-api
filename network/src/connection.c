#include <string.h>
#include <unistd.h>

#include "connection.h"
#include "memory_allocations.h"
#include "string_utils.h"


static int ConnectToServer (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);


static bool InitConnection (Connection *connection_p);


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
			if (InitConnection (& (connection_p -> rc_base)))
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


static bool InitConnection (Connection *connection_p)
{
	bool success_flag = false;

	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			connection_p -> co_data_buffer_p = buffer_p;
			connection_p -> co_id = 1;

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
			if (InitConnection (& (connection_p -> rc_base)))
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


Connection *AllocateWebServerConnection (const char * const hostname_s, const char * const port_s)
{
	WebConnection *connection_p = (WebConnection *) AllocMemory (sizeof (WebConnection));

	if (connection_p)
		{
			CurlTool *curl_p = AllocateCurlTool ();

			if (curl_p)
				{
					bool success_flag = false;
					ByteBuffer *buffer_p = AllocateByteBuffer (1024);

					if (buffer_p)
						{
							if (AppendStringsToByteBuffer (buffer_p, hostname_s, ":", port_s, NULL))
								{
									const char *uri_s = GetByteBufferData (buffer_p);

									if (uri_s)
										{
											char *copy_s = CopyToNewString (uri_s, 0, false);

											if (copy_s)
												{
													connection_p -> wc_uri_s = copy_s;

													if (SetUriForCurlTool (curl_p, uri_s))
														{
															if (SetCurlToolForJSONPost (curl_p))
																{
																	success_flag = true;
																}
														}

													if (!success_flag)
														{
															FreeCopiedString (connection_p -> wc_uri_s);
														}
												}		/* if (copy_s) */
										}
								}

							FreeByteBuffer (buffer_p);
						}		/* if (buffer_p) */

					if (success_flag)
						{
							if (InitConnection (& (connection_p -> wc_base)))
								{
									return (& (connection_p -> wc_base));
								}		/* if (InitConnection (& (connection_p -> wc_base))) */

						}		/* if (success_flag) */

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

