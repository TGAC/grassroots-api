/*
** Copyright 2014-2015 The Genome Analysis Centre
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

#include "polar_ssl_connection.h"
#include "memory_allocations.h"
#include "typedefs.h"


PolarSSLConnection *AllocatePolarSSLConnection (const char *personalised_s)
{
	if (personalised_s)
		{
			PolarSSLConnection *connection_p = (PolarSSLConnection *) AllocMemory (sizeof (PolarSSLConnection));

			if (connection_p)
				{
					int ret = -1;
					/*
					 * 0. Initialize the RNG and the session data
					 */
					memset (& (connection_p -> psc_ssl), 0, sizeof (ssl_context));

					x509_crt_init (& (connection_p -> psc_cacert));
					entropy_init (& (connection_p -> psc_entropy));

					ret = ctr_drbg_init (& (connection_p -> psc_ctr_drbg), entropy_func, & (connection_p -> psc_entropy), (const unsigned char *) personalised_s, strlen (personalised_s));

					if (ret == 0)
						{
							connection_p -> psc_server_fd = -1;
							
							return connection_p;
						}

					x509_crt_free (& (connection_p -> psc_cacert));
					entropy_free (& (connection_p -> psc_entropy));

					FreeMemory (connection_p);
				}		/* if (connection_p)*/

		}		/* if (personalised_s) */

	return NULL;
}


void FreePolarSSLConnection (PolarSSLConnection *connection_p)
{
	if (connection_p -> psc_server_fd != -1)
		{
			net_close (connection_p -> psc_server_fd);
		}

	ssl_free (& (connection_p -> psc_ssl));
	ctr_drbg_free (& (connection_p -> psc_ctr_drbg));
	x509_crt_free (& (connection_p -> psc_cacert));
	entropy_free (& (connection_p -> psc_entropy));

	FreeMemory (connection_p);
}


int ConnectPolarSSL (PolarSSLConnection *connection_p, const char *hostname_s, const int port)
{
	/* Start the connection */
	int ret = net_connect (& (connection_p -> psc_server_fd), hostname_s, port);
	
	if (ret == 0)
		{
			ssl_context * const ssl_p = & (connection_p -> psc_ssl);


			/* Set up the SSL/TLS structure */
			ret = ssl_init (ssl_p);
			
			if (ret == 0)
				{
					bool loop_flag = true;
					bool success_flag = true;
					
					ssl_set_endpoint (ssl_p, SSL_IS_CLIENT);

					/* Verify all certificates */
					ssl_set_authmode (ssl_p, SSL_VERIFY_REQUIRED);

					ssl_set_ca_chain (ssl_p, & (connection_p -> psc_cacert), NULL, "PolarSSL Server 1" );

					ssl_set_rng (ssl_p, ctr_drbg_random, & (connection_p -> psc_ctr_drbg));
					//ssl_set_dbg (ssl_p, my_debug, stdout);
					ssl_set_bio (ssl_p, net_recv, & (connection_p -> psc_server_fd), net_send, & (connection_p -> psc_server_fd));					
					
					/* Only accept strong ciphers */
					ssl_set_ciphersuites (ssl_p, ssl_list_ciphersuites ());


					/* Perform the SSL/TLS handshake */				
					while (loop_flag && success_flag)
						{
							ret = ssl_handshake (ssl_p);
							
							switch (ret)
								{
									case POLARSSL_ERR_NET_WANT_READ:
									case POLARSSL_ERR_NET_WANT_WRITE:
										/* carry on */
										break;
									
									case 0:
										loop_flag = false;
										break;
										
									default: 
										success_flag = false;
										break;
								}
						}
						
					if (success_flag)
						{
							/* Verify the server certificate */
							ret = ssl_get_verify_result (ssl_p);
							
							if (ret != 0)
								{
									        if( ( ret & BADCERT_EXPIRED ) != 0 )
            printf( "  ! server certificate has expired\n" );

        if( ( ret & BADCERT_REVOKED ) != 0 )
            printf( "  ! server certificate has been revoked\n" );

        if( ( ret & BADCERT_CN_MISMATCH ) != 0 )
            printf( "  ! CN mismatch (expected CN=%s)\n", "PolarSSL Server 1" );

        if( ( ret & BADCERT_NOT_TRUSTED ) != 0 )
            printf( "  ! self-signed or not signed by a trusted CA\n" );

								}
						}
				}
				
		}
	return ret;
}


int SendPolarSSL (PolarSSLConnection *connection_p, const unsigned char *buffer_p, const size_t num_to_send)
{
	int num_sent = 0;
	int i;	
	bool loop_flag = true;
	
	while (loop_flag)
		{
			i = ssl_write (& (connection_p -> psc_ssl), buffer_p, num_to_send);
			
			if (i > 0)
				{
					num_sent += i;
					buffer_p += i;
					
					loop_flag = (num_sent < num_to_send);
				}
			else if ((i != POLARSSL_ERR_NET_WANT_READ) && (i != POLARSSL_ERR_NET_WANT_WRITE))
				{
					loop_flag = false;
					num_sent = -num_sent;
				}
		}
		
	return num_sent;
}


int RecievePolarSSL (PolarSSLConnection *connection_p, void *buffer_p, const uint32 num_to_receive)
{
	int num_received = 0;
	int i;	
	bool loop_flag = true;
	
	while (loop_flag)
		{
			i = ssl_read (& (connection_p -> psc_ssl), buffer_p, num_to_receive);
			
			switch (i)
				{
					case POLARSSL_ERR_NET_WANT_READ:
					case POLARSSL_ERR_NET_WANT_WRITE:
						/* loop and try again */
						break;
						
					case POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY:
						num_received = -num_received;
						loop_flag = false;
						break;
						
					case 0:
						/* EOF */
						loop_flag = false;
						break;
						
					default:
						if (i > 0)
							{
								num_received += i;
								buffer_p += i;
								
								loop_flag = (num_received < num_to_receive);
							}
						else 
							{
								loop_flag = false;
								num_received = -num_received;
							}
						break;
				}
		}
		
	return num_received;	
}


