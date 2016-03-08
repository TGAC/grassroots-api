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
#ifndef HANDLE_UTILS_H
#define HANDLE_UTILS_H


#include "grassroots_handler_library.h"
#include "parameter.h"
#include "handler.h"




#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_HANDLER_API Handler *GetResourceHandler (const Resource *resource_p, const json_t *tags_p);


/**
 * This allocates the internal structures used by the Handler library.
 *
 * @return <code>true</code> upon success, <code>false</code>s on failure.
 */
GRASSROOTS_HANDLER_API bool InitHandlerUtil (void);


/**
 * This frees the internal structures used by the Handler library.
 *
 * @return true upon success, false on failure.
 */
GRASSROOTS_HANDLER_API bool DestroyHandlerUtil (void);



GRASSROOTS_HANDLER_API const char *GetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, time_t *time_p);

GRASSROOTS_HANDLER_API bool SetMappedFilename (const char *protocol_s, const char *user_id_s, const char *filename_s, const char *mapped_filename_s, const time_t last_mod_time);



#ifdef __cplusplus
}
#endif

#endif		/* #ifndef HANDLE_UTILS_H */
