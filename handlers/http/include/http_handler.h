#ifndef GRASSROOTS_HTTP_HANDLER_H
#define GRASSROOTS_HTTP_HANDLER_H

#include <stdio.h>

#include "grassroots_http_handler_library.h"
#include "handler.h"


typedef struct HttpHandler
{
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
