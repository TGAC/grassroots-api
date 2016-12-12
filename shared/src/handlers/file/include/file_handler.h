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
#ifndef GRASSROOTS_FILE_HANDLER_H
#define GRASSROOTS_FILE_HANDLER_H

#include <stdio.h>

#include "grassroots_file_handler_library.h"
#include "handler.h"


/**
 * A datatype for a Handler that can access locally
 * mounted files.
 *
 * @extends Handler
 *
 */
typedef struct FileHandler
{
	/** The base Handler. */
	Handler fh_base_handler;

	/** The current file. */
	FILE *fh_handler_f;
} FileHandler;


#ifdef __cplusplus
extern "C" 
{
#endif


/**
 * Get a FileHandler.
 *
 * @param user_p The UserDetails used to determine file access permissions. This can be <code>NULL</code>.
 * @return  The newly-allocated FileHandler or <code>NULL</code> upon error.
 * @memberof FileHandler
 */
FILE_HANDLER_API Handler *GetHandler (const UserDetails *user_p);


/**
 * Free a FileHandler.
 *
 * @param handler_p The FileHandler to free.
 * @memberof FileHandler.
 */
FILE_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_FILE_HANDLER_H */
