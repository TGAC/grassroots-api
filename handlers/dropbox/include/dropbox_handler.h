#ifndef DROPBOX_HANDLER_H
#define DROPBOX_HANDLER_H

#include <jansson.h>

#include "dropbox_handler_library.h"

/* 
 * This handler uses thte library available from 
 * 
 * https://github.com/Dwii/Dropbox-C
 */
 
 
typedef struct DropboxHandler
{
	Handler dh_base_handler;
} DropboxHandler;

/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to 
 * 
 * 		extern const char *SERVICE_NAME_S;
 * 
 * however if ALLOCATE_JSON_TAGS is defined then it will 
 * become
 * 
 * 		const char *SERVICE_NAME_S = "path";
 * 
 * ALLOCATE_JSON_TAGS must be defined only once prior to 
 * including this header file. Currently this happens in
 * json_util.c.
 */
#ifdef ALLOCATE_DROPBOX_TAGS
	#define PREFIX DROPBOX_HANDLER_LOCAL
	#define VAL(x)	= x
#else
	#define PREFIX extern
	#define VAL(x)	
#endif

PREFIX const char *DROPBOX_APP_KEY_S VAL("65viide1m1ye7pd");
PREFIX const char *DROPBOX_APP_SECRET_S VAL("kwd6z8djlcd3g5k");


 
 
#ifdef __cplusplus
extern "C" 
{
#endif

DROPBOX_HANDLER_API Handler *GetHandler (const json_t *tags_p);

DROPBOX_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif




#endif	/* #ifndef DROPBOX_HANDLER_H */

