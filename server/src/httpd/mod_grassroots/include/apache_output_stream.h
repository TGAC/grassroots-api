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

/**
 * @file
 * @brief
 */
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
 * @brief An OutputStream for use by a Grassroots system running on Apache httpd.
 *
 * When running as a module on an Apache httpd, this OutputStream will redirect
 * all server messages into the standard logs.
 *
 * @extends OutputStream
 * @ingroup httpd_server
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
 * @memberof ApacheOutputStream
 */
OutputStream *AllocateApacheOutputStream (server_rec *server_p);


/**
 * Free an ApacheOutputStream.
 *
 * @param stream_p The ApacheOutputStream to free.
 * @memberof ApacheOutputStream
 */
void DeallocateApacheOutputStream (OutputStream *stream_p);

#ifdef __cplusplus
}
#endif


#endif /* APACHE_OUTPUT_STREAM_H_ */
