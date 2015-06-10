/*
 * apache_output_stream.h
 *
 *  Created on: 18 May 2015
 *      Author: billy
 */

#ifndef APACHE_OUTPUT_STREAM_H_
#define APACHE_OUTPUT_STREAM_H_

#include "httpd.h"

#include "streams.h"

/**
 * @brief An OutputStream for use by a WheatIS system running on Apache httpd.
 *
 * When running as a module on an Apache httpd, this OutputStream will redirect
 * all server messages into the standard logs.
 *
 * @see OutputStream
 */
typedef struct ApacheOutputStream
{
	/** The base OutputStream */
	OutputStream aos_stream;

	/** The server that we are running on */
	server_rec *aos_server_p;
} ApacheOutputStream;


#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Allocate an ApacheOutputStream to use on the given server.
 *
 * @param server_p The httpd server to use the OutputStream on.
 * @return The newly-allocated ApacheOutputStream or <code>NULL</code>
 * upon error.
 */
OutputStream *AllocateApacheOutputStream (server_rec *server_p);


/**
 * Free an ApacheOutputStream.
 *
 * @param stream_p The ApacheOutputStream to free.
 */
void DeallocateApacheOutputStream (OutputStream *stream_p);

#ifdef __cplusplus
}
#endif


#endif /* APACHE_OUTPUT_STREAM_H_ */
