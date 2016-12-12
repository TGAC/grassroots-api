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
#ifndef HANDLE_UTILS_H
#define HANDLE_UTILS_H


#include "grassroots_handler_library.h"
#include "parameter.h"
#include "handler.h"




#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get the appropriate Handler for a given Resource.
 *
 * @param resource_p The Resource to get the Handler for.
 * @param user_p An optional UserDetails for any user authentication if needed.
 * @return The matching Handler or <code>NULL</code> if none could be found.
 */
GRASSROOTS_HANDLER_API Handler *GetResourceHandler (const Resource *resource_p, const UserDetails *user_p);


/**
 * This allocates the internal structures used by the Handler library.
 *
 * @return <code>true</code> upon success, <code>false</code> on failure.
 */
GRASSROOTS_HANDLER_API bool InitHandlerUtil (void);


/**
 * This frees the internal structures used by the Handler library.
 *
 * @return <code>true</code> upon success, <code>false</code> on failure.
 */
GRASSROOTS_HANDLER_API bool DestroyHandlerUtil (void);


/**
 * Create a local file for caching changes to a remote file allowing changes
 * to be committed once editing has finished,
 *
 * @param protocol_s The protocol of the remote file.
 * @param user_id_s The id of the cuurent user. This can be <code>NULL</code>.
 * @param filename_s The remote filename.
 * @param time_p The time when the local cached file was created.
 * @return The local filename or <code>NULL</code> upon error.
 */
GRASSROOTS_HANDLER_API const char *GetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, time_t *time_p);


GRASSROOTS_HANDLER_API bool SetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, const char *mapped_filename_s, const time_t last_mod_time);



#ifdef __cplusplus
}
#endif

#endif		/* #ifndef HANDLE_UTILS_H */
