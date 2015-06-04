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


typedef struct ApacheOutputStream
{
	OutputStream aos_stream;
	server_rec *aos_server_p;
} ApacheOutputStream;


#ifdef __cplusplus
	extern "C" {
#endif

OutputStream *AllocateApacheOutputStream (server_rec *server_p);

void DeallocateApacheOutputStream (OutputStream *stream_p);

#ifdef __cplusplus
}
#endif


#endif /* APACHE_OUTPUT_STREAM_H_ */
