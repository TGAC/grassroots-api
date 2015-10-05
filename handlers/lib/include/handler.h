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
#ifndef GRASSROOTS_HANDLE_H
#define GRASSROOTS_HANDLE_H

#include <stddef.h>
#include <time.h>

#include "typedefs.h"
#include "grassroots_handler_library.h"
#include "plugin.h"
#include "data_resource.h"
#include "parameter.h"
#include "jansson.h"
#include "filesystem_utils.h"


typedef enum
{
	HS_GOOD,
	HS_FINISHED,
	HS_BAD,
	HS_NONE
} HandlerStatus;


struct Handler;


typedef struct Handler
{
	/**
	 * The platform-specific plugin that the code for the Client is
	 * stored in.
	 */
	struct Plugin *ha_plugin_p;

	bool (*ha_init_fn) (struct Handler *handler_p, json_t *credentials_p);

	bool (*ha_match_fn) (struct Handler *handler_p, const Resource * resource_p);

	const char *(*ha_get_protocol_fn) (struct Handler *handler_p);
	const char *(*ha_get_name_fn) (struct Handler *handler_p);
	const char *(*ha_get_description_fn) (struct Handler *handler_p);


	bool (*ha_open_fn) (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);
	size_t (*ha_read_fn) (struct Handler *handler_p, void *buffer_p, const size_t length);
	size_t (*ha_write_fn) (struct Handler *handler_p, const void *buffer_p, const size_t length);
	bool (*ha_seek_fn) (struct Handler *handler_p, long offset, int whence);
	bool (*ha_close_fn) (struct Handler *handler_p);
	HandlerStatus (*ha_status_fn) (struct Handler *handler_p);
	bool (*ha_file_info_fn) (struct Handler *handler_p, FileInformation *info_p);	
	void (*ha_free_handler_fn) (struct Handler *handler_p);
	
	char *ha_filename_s;
} Handler;


typedef struct HandlerNode
{
	ListItem hn_node;
	Handler *hn_handler_p;
} HandlerNode;


#ifdef __cplusplus
extern "C"
{
#endif

GRASSROOTS_HANDLER_API void InitialiseHandler (Handler * const handler_p,
	bool (*init_fn) (struct Handler *handler_p, json_t *credentials_p),
	bool (*match_fn) (struct Handler *handler_p, const Resource * resource_p),
	const char *(*get_protocol_fn) (struct Handler *handler_p),
	const char *(*get_name_fn) (struct Handler *handler_p),
	const char *(*get_description_fn) (struct Handler *handler_p),
	bool (*open_fn) (struct Handler *handler_p, const char * const filename_s, const char * const mode_s),
	size_t (*read_fn) (struct Handler *handler_p, void *buffer_p, const size_t length),
	size_t (*write_fn) (struct Handler *handler_p, const void *buffer_p, const size_t length),
	bool (*seek_fn) (struct Handler *handler_p, long offset, int whence),
	bool (*close_fn) (struct Handler *handler_p),
	HandlerStatus (*status_fn) (struct Handler *handler_p),
	bool (*file_info_fn) (struct Handler *handler_p, FileInformation *info_p),	
	void (*free_handler_fn) (struct Handler *handler_p));


GRASSROOTS_HANDLER_API bool InitHandler (struct Handler *handler_p, json_t *credentials_p);

GRASSROOTS_HANDLER_API bool OpenHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);

GRASSROOTS_HANDLER_API size_t ReadFromHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

GRASSROOTS_HANDLER_API size_t WriteToHandler (struct Handler *handler_p, const void *buffer_p, const size_t length);

GRASSROOTS_HANDLER_API size_t SeekHandler (struct Handler *handler_p, size_t offset, int whence);

GRASSROOTS_HANDLER_API bool CloseHandler (struct Handler *handler_p);

GRASSROOTS_HANDLER_API bool IsHandlerForResource (struct Handler *handler_p, const Resource * resource_p);

GRASSROOTS_HANDLER_API const char *GetHandlerName (struct Handler *handler_p);

GRASSROOTS_HANDLER_API const char *GetHandlerDescription (struct Handler *handler_p);

GRASSROOTS_HANDLER_API const char *GetHandlerProtocol (struct Handler *handler_p);

GRASSROOTS_HANDLER_API HandlerStatus GetHandlerStatus (struct Handler *handler_p);

GRASSROOTS_HANDLER_API bool CalculateFileInformationFromHandler (struct Handler *handler_p, FileInformation *info_p);

GRASSROOTS_HANDLER_API void FreeHandler (struct Handler *handler_p);

GRASSROOTS_HANDLER_API HandlerNode *AllocateHandlerNode (struct Handler *handler_p);

GRASSROOTS_HANDLER_API void FreeHandlerNode (ListItem *node_p);

GRASSROOTS_HANDLER_API Handler *GetHandlerFromPlugin (Plugin * const plugin_p, const json_t *tags_p);

GRASSROOTS_HANDLER_API bool DeallocatePluginHandler (Plugin * const plugin_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_HANDLE_H */
