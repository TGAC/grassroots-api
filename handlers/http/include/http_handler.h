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
#ifndef GRASSROOTS_HTTP_HANDLER_H
#define GRASSROOTS_HTTP_HANDLER_H

#include <stdio.h>

#include "grassroots_http_handler_library.h"
#include "handler.h"

/**
 * A Handler allowing access to http(s) resources.
 */
typedef struct HttpHandler
{
	/**
	 * The base Handler.
	 */
	Handler hh_base_handler;
	
} FileHandler;


#ifdef __cplusplus
extern "C" 
{
#endif


HTTP_HANDLER_API Handler *GetHandler (const json_t *tags_p);

HTTP_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_HTTP_HANDLER_H */
