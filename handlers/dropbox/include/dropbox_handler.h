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
#ifndef DROPBOX_HANDLER_H
#define DROPBOX_HANDLER_H

#include <jansson.h>
#include <stdlib.h>

#include "dropbox_handler_library.h"
#include "handler.h"
#include "dropbox.h"
#include "dropboxOAuth.h"

/* 
 * This handler uses thte library available from 
 * 
 * https://github.com/Dwii/Dropbox-C
 */
 
 
typedef struct DropboxHandler
{
	Handler dh_base_handler;

	drbClient *dh_client_p;
	
	HandlerStatus dh_status;
	
	FILE *dh_local_copy_f;
	
	char *dh_dropbox_filename_s;
	
	bool dh_updated_flag;
	
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
	#define DROPBOX_HANDLER_DECLARE DROPBOX_HANDLER_LOCAL
	#define DROPBOX_HANDLER_VAL(x)	= x
#else
	#define DROPBOX_HANDLER_DECLARE extern
	#define DROPBOX_HANDLER_VAL(x)
#endif

DROPBOX_HANDLER_DECLARE const char *DROPBOX_APP_KEY_S DROPBOX_HANDLER_VAL("65viide1m1ye7pd");
DROPBOX_HANDLER_DECLARE const char *DROPBOX_APP_SECRET_S DROPBOX_HANDLER_VAL("kwd6z8djlcd3g5k");

DROPBOX_HANDLER_DECLARE const char *DROPBOX_TOKEN_KEY_S DROPBOX_HANDLER_VAL("token_key");
DROPBOX_HANDLER_DECLARE const char *DROPBOX_TOKEN_SECRET_S DROPBOX_HANDLER_VAL("token_secret");


 
 
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

