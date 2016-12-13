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
#ifndef GRASSROOTS_HTTP_HANDLER_H
#define GRASSROOTS_HTTP_HANDLER_H

#include <stdio.h>

#include "grassroots_http_handler_library.h"
#include "handler.h"
#include "curl_tool.h"


/**
 * A Handler allowing access to http(s) resources.
 *
 * @extends Handler
 *
 */
typedef struct HttpHandler
{
	/**
	 * The base Handler.
	 */
	Handler hh_base_handler;
	
	/**
	 * The CurlTool that this HttpHandler will use to connect to a URI.
	 */
	CurlTool *hh_curl_p;

	/**
	 * The FILE where the data will be written to.
	 */
	FILE *hh_local_f;

} HttpHandler;


#ifdef __cplusplus
extern "C" 
{
#endif


/**
 * Get a newly-created HttpHandler.
 *
 * @param user_p The UserDetails for the user requesting the HttpHandler.
 * @return The newly-created HttpHandler or <code>NULL</code> upon error.
 * @memberof HttpHandler
 * @ingroup http_handler
 */
HTTP_HANDLER_API Handler *GetHandler (const UserDetails *user_p);


/**
 * Free a HttpHandler.
 *
 * @param handler_p The HttpHandler to free.
 * @memberof HttpHandler
 * @ingroup http_handler
 */
HTTP_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_HTTP_HANDLER_H */
