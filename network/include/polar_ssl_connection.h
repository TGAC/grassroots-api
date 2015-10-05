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
#ifndef POLAR_SSL_CONNECTION_H
#define POLAR_SSL_CONNECTION_H


#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "network_library.h"

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
	int psc_server_fd;
} PolarSSLConnection;


#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_NETWORK_API PolarSSLConnection *InitPolarSSLConnection (const char *hostname_s, const char *port_s, struct addrinfo **server_pp);



#ifdef __cplusplus
}
#endif



#endif		/* #ifndef POLAR_SSL_CONNECTION_H */

