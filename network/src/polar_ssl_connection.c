
#include "polar_ssl_connection.h"



PolarSSLConnection *InitPolarSSLConnection (const char *personalised_s);
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
					memset (& (connection_p -> psc_ssl), 0, sizeof (ssl_context);

					x509_crt_init (& (connection_p -> psc_cacert));
					entropy_init (& (connection_p -> psc_entropy));

					ret = ctr_drbg_init (& (connection_p -> ctr_drbg), entropy_func, & (connection_p -> psc_entropy), (const unsigned char *) personalised_s, strlen (personalised_s));

					if (ret == 0)
						{

						}

					x509_crt_init (& (connection_p -> psc_cacert));
					entropy_free (& (connection_p -> psc_entropy))

					FreeMemory (connection_p);
				}		/* if (connection_p)*/

		}		/* if (personalised_s) */

	return NULL;
}

