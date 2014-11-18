#ifndef POLAR_SSL_CONNECTION_H
#define POLAR_SSL_CONNECTION_H

#ifndef POLARSSL_CONFIG_FILE
	#include "polarssl/config.h"
#else
	#include POLARSSL_CONFIG_FILE
#endif

#include "polarssl/net.h"
#include "polarssl/debug.h"
#include "polarssl/ssl.h"
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"
#include "polarssl/error.h"
#include "polarssl/certs.h"


typedef struct PolarSSLConnection
{
	entropy_context psc_entropy;
	ctr_drbg_context psc_ctr_drbg;
	ssl_context psc_ssl;
	x509_crt psc_cacert;
} PolarSSLConnection;


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_NETWORK_API PolarSSLConnection *InitPolarSSLConnection (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);



#ifdef __cplusplus
}
#endif



#endif		/* #ifndef POLAR_SSL_CONNECTION_H */

