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
#ifndef DROPBOX_HANDLER_H
#define DROPBOX_HANDLER_H

#include <jansson.h>
#include <stdlib.h>

#include "dropbox_handler_library.h"
#include "handler.h"
#include "dropbox.h"
#include "dropboxOAuth.h"

 
 /**
 * A datatype for a Handler that can access data
 * stored within a Dropbox account.
 *
 * This handler uses the library available from
 *
 * https://github.com/Dwii/Dropbox-C
 *
 * @extends Handler
 */
typedef struct DropboxHandler
{
	/**
	 * The base Handler.
	 */
	Handler dh_base_handler;

	/**
	 * @private
	 * The client to the Dropbox server.
	 */
	drbClient *dh_client_p;
	

	/**
	 * @private
	 * The status of this Handler.
	 */
	HandlerStatus dh_status;
	
	/**
	 * @private
	 * If the file has been downloaded to the local machine
	 * to allow editing, this is the FILE it has been
	 * downloaded to.
	 */
	FILE *dh_local_copy_f;
	
	/**
	 * @private
	 * The full path to the file on the Dropbox server.
	 */
	char *dh_dropbox_filename_s;
	
	/**
	 * @private
	 * Has the file been editing and thus require uploading
	 * back to Dropbox to synchronise.
	 */
	bool dh_updated_flag;
	
} DropboxHandler;

/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to 
 * 
 * 		extern const char *SERVICE_NAME_S;
 * 
 * however if ALLOCATE_DROPBOX_TAGS is defined then it will
 * become
 * 
 * 		const char *SERVICE_NAME_S = "path";
 * 
 * ALLOCATE_DROPBOX_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * dropbox_handler.c.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_DROPBOX_TAGS
	#define DROPBOX_HANDLER_DECLARE DROPBOX_HANDLER_LOCAL
	#define DROPBOX_HANDLER_VAL(x)	= x
#else
	#define DROPBOX_HANDLER_DECLARE extern
	#define DROPBOX_HANDLER_VAL(x)
#endif

#endif		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */

/**
 * The token key value from the dropbox servers allowing Grassroots to access
 * the files and directories for the given account.
 */
DROPBOX_HANDLER_DECLARE const char *DROPBOX_TOKEN_KEY_S DROPBOX_HANDLER_VAL("token_key");


/**
 * The token secret value from the dropbox servers allowing Grassroots to access
 * the files and directories for the given account.
 */
DROPBOX_HANDLER_DECLARE const char *DROPBOX_TOKEN_SECRET_S DROPBOX_HANDLER_VAL("token_secret");


 
 
#ifdef __cplusplus
extern "C" 
{
#endif

/**
 * Get a DropboxHandler.
 *
 * @param user_p The UserDetails used to determine file access permissions. This can be <code>NULL</code>.
 * @return The newly-allocated DropboxHandler or <code>NULL</code> upon error.
 * @memberof DropboxHandler
 * @ingroup dropbox_handler_group
 */
DROPBOX_HANDLER_API Handler *GetHandler (const UserDetails *user_p);


/**
 * Free a DropboxHandler.
 *
 * @param handler_p The DropboxHandler to free.
 * @memberof DropboxHandler
 * @ingroup dropbox_handler_group
 */
DROPBOX_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif




#endif	/* #ifndef DROPBOX_HANDLER_H */

