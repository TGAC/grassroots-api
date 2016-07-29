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
#include "user_details.h"

/**
 * An enumeration of the possible status values
 * of a stream being used by a Handler.
 */
typedef enum
{
	/** The stream is in a valid state. */
	HS_GOOD,

	/** The Handler has reached the end of the stream. */
	HS_FINISHED,

	/** The Handler has encountered an error using the stream. */
	HS_BAD,

	/** The Handler does not have an open stream. */
	HS_NONE
} HandlerStatus;


struct Handler;


/**
 * A Handler is a datatype for accessing data.
 *
 * It is an abstraction of the common data input/output operations, e.g., opening
 * and closing files, reading and writing data, etc. Any Grassroots component
 * can use any Handler to access data without having to worry about the internals.
 * So just by loading the appropriate Handler, a task can read data from a file,
 * and iRODS data object, a webpage, etc. simply by using the Handler API.
 */
typedef struct Handler
{
	/**
	 * The platform-specific plugin that the code for the Client is
	 * stored in.
	 */
	struct Plugin *ha_plugin_p;

	/**
	 * Initialise a Handler ready for use.
	 *
	 * This is for when the Handler may need some custom initialisation e.g. permissions
	 * to be used.
	 *
	 * @param handler_p This Handler.
	 * @param user_p Any custom credentials or configuration values. This can be <code>NULL</code>.
	 * @return <code>true</code> upon success, <code>false</code> upon failure.
	 * @memberof Handler
	 */
	bool (*ha_init_fn) (struct Handler *handler_p, const UserDetails *user_p);

	/**
	 * Determine if the Handler is designed for a given Resource.
	 *
	 * @param handler_p This Handler.
	 * @param resource_p The Resource to check the Handler with.
	 * @return <code>true</code> if the Handler is for the Resource, <code>false</code> false otherwise.
	 */
	bool (*ha_match_fn) (struct Handler *handler_p, const Resource * resource_p);

	/**
	 * Get the protocol of a Handler.
	 *
	 * @param handler_p This Handler.
	 * @return The protocol of the Handler.
	 * @see PROTOCOL_FILE
	 * @see PROTOCOL_IRODS
	 * @see PROTOCOL_HTTP
	 */
	const char *(*ha_get_protocol_fn) (struct Handler *handler_p);


	/**
	 * Get the name of a Handler.
	 *
	 * @param handler_p This Handler.
	 * @return The name of the Handler.
	 */
	const char *(*ha_get_name_fn) (struct Handler *handler_p);


	/**
	 * Get the description of a Handler.
	 *
	 * @param handler_p This Handle.
	 * @return The description of the Handler.
	 */
	const char *(*ha_get_description_fn) (struct Handler *handler_p);

	/**
	 * Open a Resource with the Handler.
	 *
	 * @param handler_p This Handler.
	 * @param resource_p The Resource to use.
	 * @param resource_mem How the Handler will treat the Resource
	 * when the Handler is freed.
	 * @param mode_s The mode to open with. This is the same as
	 * the mode used with the standard c library fopen.
	 * @return <code>true</code> upon success, <code>false</code> upon failure.
	 */
	bool (*ha_open_fn) (struct Handler *handler_p, Resource *resource_p, MEM_FLAG resource_mem, const char * const mode_s);


	/**
	 * Read data from a Handler into a buffer.
	 *
	 * The data will be read from the current position of the Handler
	 * within its stream.
	 *
	 * @param handler_p This Handler.
	 * @param buffer_p The buffer to save the data into.
	 * @param length The number of bytes to read.
	 * @return The number of bytes that were read.
	 */
	size_t (*ha_read_fn) (struct Handler *handler_p, void *buffer_p, const size_t length);


	/**
	 * Write data from a buffer into a Handler.
	 *
	 * The data will be written to the current position of the Handler
	 * within its stream.
	 *
	 * @param handler_p This Handler.
	 * @param buffer_p The buffer to read the data from.
	 * @param length The number of bytes to write.
	 * @return The number of bytes that were written.
	 */
	size_t (*ha_write_fn) (struct Handler *handler_p, const void *buffer_p, const size_t length);


	/**
	 * Move a Handler to a new position in its stream.
	 *
	 * This is equivalent to fseek in the standard c library.
	 *
	 * @param handler_p This Handler.
	 * @param offset The new position.
	 * @param whence The reference position to use. These are the constants defined in \<stdio.h\>
	 *  * SEEK_SET: Start of the stream.
	 *  * SEEK_CUR: Current position in the stream.
	 *  * SEEK_END: End of the stream.
	 * @return <code>true</code> upon success, <code>false</code> upon failure.
	 */
	bool (*ha_seek_fn) (struct Handler *handler_p, long offset, int whence);


	/**
	 * Close a Handler.
	 *
	 * @param handler_p This Handler.
	 * @return <code>true</code> upon success, <code>false</code> upon failure.
	 */
	bool (*ha_close_fn) (struct Handler *handler_p);



	/**
	 * Get the HandlerStatus of a Handler.
	 *
	 * @param handler_p This Handler.
	 * @return The HandlerStatus
	 */
	HandlerStatus (*ha_status_fn) (struct Handler *handler_p);


	/**
	 * Calculate the FileInformation for the current Resource in use by a Handler.
	 *
	 * @param handler_p This Handler.
	 * @param info_p Pointer to where the FileInformation will be stored upon success.
	 * @return <code>true</code> upon success, <code>false</code> upon failure.
	 */
	bool (*ha_file_info_fn) (struct Handler *handler_p, FileInformation *info_p);	


	/**
	 * Free a Handler.
	 *
	 * The Handler will be closed if needed and then freed.
	 *
	 * @param handler_p This Handler.
	 */
	void (*ha_free_handler_fn) (struct Handler *handler_p);
	
	/**
	 * The current Resource for this Handler.
	 */
	Resource *ha_resource_p;

	/**
	 * The MEM_FLAG for the Handler's Resource.
	 */
	MEM_FLAG ha_resource_mem;
} Handler;


/**
 * A datatype for storing a Handler on a LinkedList.
 *
 * @extends ListItem
 */
typedef struct HandlerNode
{
	/** The base list node. */
	ListItem hn_node;

	/** The Handler. */
	Handler *hn_handler_p;
} HandlerNode;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Initialise a Handler from a loaded Plugin.
 *
 * @param handler_p The Handler to initialise.
 * @param init_fn The callback function to use for the Handler's ha_init_fn function.
 * @see ha_init_fn
 * @param match_fn The callback function to use for the Handler's ha_match_fn function.
 * @see ha_match_fn
 * @param get_protocol_fn The callback function to use for the Handler's ha_get_protocol_fn function.
 * @see ha_get_protocol_fn
 * @param get_name_fn The callback function to use for the Handler's ha_get_name_fn function.
 * @see ha_get_name_fn
 * @param get_description_fn The callback function to use for the Handler's ha_get_description_fn function.
 * @see ha_get_description_fn
 * @param open_fn The callback function to use for the Handler's ha_open_fn function.
 * @see ha_open_fn
 * @param read_fn The callback function to use for the Handler's ha_read_fn function.
 * @see ha_read_fn
 * @param write_fn The callback function to use for the Handler's ha_write_fn function.
 * @see ha_write_fn
 * @param seek_fn The callback function to use for the Handler's ha_seek_fn function.
 * @see ha_seek_fn
 * @param close_fn The callback function to use for the Handler's ha_close_fn function.
 * @see ha_close_fn
 * @param status_fn The callback function to use for the Handler's ha_status_fn function.
 * @see ha_status_fn
 * @param file_info_fn The callback function to use for the Handler's ha_file_info_fn function.
 * @see ha_file_info_fn
 * @param free_handler_fn The callback function to use for the Handler's ha_free_handler_fn function.
 * @param ha_free_handler_fn
 */
GRASSROOTS_HANDLER_API void InitialiseHandler (Handler * const handler_p,
	bool (*init_fn) (struct Handler *handler_p, const UserDetails *user_p),
	bool (*match_fn) (struct Handler *handler_p, const Resource * resource_p),
	const char *(*get_protocol_fn) (struct Handler *handler_p),
	const char *(*get_name_fn) (struct Handler *handler_p),
	const char *(*get_description_fn) (struct Handler *handler_p),
	bool (*open_fn) (struct Handler *handler_p, Resource *resource_p, MEM_FLAG resource_mem, const char * const mode_s),
	size_t (*read_fn) (struct Handler *handler_p, void *buffer_p, const size_t length),
	size_t (*write_fn) (struct Handler *handler_p, const void *buffer_p, const size_t length),
	bool (*seek_fn) (struct Handler *handler_p, long offset, int whence),
	bool (*close_fn) (struct Handler *handler_p),
	HandlerStatus (*status_fn) (struct Handler *handler_p),
	bool (*file_info_fn) (struct Handler *handler_p, FileInformation *info_p),	
	void (*free_handler_fn) (struct Handler *handler_p));


/**
 * Initialise a Handler ready for use.
 *
 * This is for when the Handler may need some custom initialisation e.g. permissions
 * to be used.
 *
 * @param handler_p The Handler to initialise.
 * @param credentials_p Any custom credentials or configuration values. This can be <code>NULL</code>.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 * @see ha_init_fn
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API bool InitHandler (struct Handler *handler_p, json_t *credentials_p);


/**
 * Open a Resource with the Handler.
 *
 * @param handler_p The Handler to use
 * @param resource_p The Resource to use
 * @param resource_mem How the Handler will treat the Resource
 * when the Handler is freed.
 * @param mode_s The mode to open with. This is the same as
 * the mode used with the standard c library fopen.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 * @see ha_open_fn
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API bool OpenHandler (struct Handler *handler_p, Resource *resource_p, MEM_FLAG resource_mem, const char * const mode_s);


/**
 * Read data from a Handler into a buffer.
 *
 * The data will be read from the current position of the Handler
 * within its stream.
 *
 * @param handler_p The Handler to read from.
 * @param buffer_p The buffer to save the data into.
 * @param length The number of bytes to read.
 * @return The number of bytes that were read.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API size_t ReadFromHandler (struct Handler *handler_p, void *buffer_p, const size_t length);


/**
 * Write data from a buffer into a Handler.
 *
 * The data will be written to the current position of the Handler
 * within its stream.
 *
 * @param handler_p The Handler to write to.
 * @param buffer_p The buffer to read the data from.
 * @param length The number of bytes to write.
 * @return The number of bytes that were written.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API size_t WriteToHandler (struct Handler *handler_p, const void *buffer_p, const size_t length);


/**
 * Move a Handler to a new position in its stream.
 *
 * This is equivalent to fseek in the standard c library.
 *
 * @param handler_p The Handler to alter.
 * @param offset The new position.
 * @param whence The reference position to use. These are the constants defined in \<stdio.h\>
 *  * SEEK_SET: Start of the stream.
 *  * SEEK_CUR: Current position in the stream.
 *  * SEEK_END: End of the stream.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API bool SeekHandler (struct Handler *handler_p, size_t offset, int whence);


/**
 * Close a Handler.
 *
 * @param handler_p The Handler to close.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API bool CloseHandler (struct Handler *handler_p);


/**
 * Determine if the Handler is designed for a given Resource.
 *
 * @param handler_p The Handler to check.
 * @param resource_p The Resource to check the Handler with.
 * @return <code>true</code> if the Handler is for the Resource, <code>false</code> false otherwise.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API bool IsHandlerForResource (struct Handler *handler_p, const Resource * resource_p);


/**
 * Get the name of a Handler.
 *
 * @param handler_p The Handler to get the name of.
 * @return The name of the Handler.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API const char *GetHandlerName (struct Handler *handler_p);


/**
 * Get the description of a Handler.
 *
 * @param handler_p The Handler to get the description of.
 * @return The description of the Handler.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API const char *GetHandlerDescription (struct Handler *handler_p);



/**
 * Get the protocol of a Handler.
 *
 * @param handler_p The Handler to get the protocol of.
 * @return The protocol of the Handler.
 * @memberof Handler
 * @see PROTOCOL_FILE
 * @see PROTOCOL_IRODS
 * @see PROTOCOL_HTTP
 */
GRASSROOTS_HANDLER_API const char *GetHandlerProtocol (struct Handler *handler_p);


/**
 * Get the HandlerStatus of a Handler.
 *
 * @param handler_p The Handler to get the HandlerStatus of.
 * @return The HandlerStatus
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API HandlerStatus GetHandlerStatus (struct Handler *handler_p);


/**
 * Calculate the FileInformation for the current Resource in use by a Handler.
 *
 * @param handler_p The Handler to query.
 * @param info_p Pointer to where the FileInformation will be stored upon success.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API bool CalculateFileInformationFromHandler (struct Handler *handler_p, FileInformation *info_p);


/**
 * Free a Handler.
 *
 * The Handler will be closed if needed and then freed.
 *
 * @param handler_p The Handler to free.
 * @memberof Handler
 */
GRASSROOTS_HANDLER_API void FreeHandler (struct Handler *handler_p);


/**
 * Allocate a HandlerNode for the given Handler.
 *
 * @param handler_p The Handler to associate with the newly-created HandlerNode.
 * @return The new HandlerNode or <code>NULL</code> upon error.
 * @memberof HandlerNode
 */
GRASSROOTS_HANDLER_API HandlerNode *AllocateHandlerNode (struct Handler *handler_p);


/**
 * Free a HandlerNode.
 *
 * This frees a HandlerNode and its associated Handler.
 *
 * @param node_p
 * @memberof HandlerNode
 * @see FreeHandler
 */
GRASSROOTS_HANDLER_API void FreeHandlerNode (ListItem *node_p);


/**
 * Load a Handler from a Plugin.
 *
 * @param plugin_p The Plugin to load the Handler from.
 * @param tags_p Any configuration details for the Handler. This can be <code>NULL</code>.
 * @return The Handler or <code>NULL</code> if one could not be loaded successfully.
 */
GRASSROOTS_HANDLER_API Handler *GetHandlerFromPlugin (Plugin * const plugin_p, const json_t *tags_p);


/**
 * Free the Plugin associated with a Handler.
 *
 * @param plugin_p The Plugin to free.
 * @return <code>true</code> upon success, <code>false</code> upon failure.
 */
GRASSROOTS_HANDLER_API bool DeallocatePluginHandler (Plugin * const plugin_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GRASSROOTS_HANDLE_H */

