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
#include "handler.h"

#include "string_utils.h"


/*********************/

static bool SetResourceForHandler (struct Handler *handler_p, Resource *resource_p, MEM_FLAG resource_mem);

static void RemoveResourceFromHandler (struct Handler *handler_p);

/*********************/

void InitialiseHandler (Handler * const handler_p,
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
	void (*free_handler_fn) (struct Handler *handler_p))
{
	handler_p -> ha_init_fn = init_fn;
	handler_p -> ha_match_fn = match_fn;
	handler_p -> ha_get_protocol_fn = get_protocol_fn;
	handler_p -> ha_get_name_fn = get_name_fn;
	handler_p -> ha_get_description_fn = get_description_fn;
	handler_p -> ha_open_fn = open_fn;
	handler_p -> ha_read_fn = read_fn;
	handler_p -> ha_write_fn = write_fn;
	handler_p -> ha_seek_fn = seek_fn;
	handler_p -> ha_close_fn = close_fn;
	handler_p -> ha_status_fn = status_fn;
	handler_p -> ha_file_info_fn = file_info_fn;
	handler_p -> ha_free_handler_fn = free_handler_fn;

	handler_p -> ha_resource_p = NULL;
	handler_p -> ha_resource_mem  = MF_ALREADY_FREED;
	handler_p -> ha_plugin_p = NULL;
}



bool InitHandler (struct Handler *handler_p, const UserDetails *user_p)
{
	return (handler_p -> ha_init_fn (handler_p, user_p));
}


bool OpenHandler (struct Handler *handler_p, Resource *resource_p, MEM_FLAG resource_mem, const char * const mode_s)
{
	bool success_flag = false;
	
	if (handler_p -> ha_open_fn (handler_p, resource_p, resource_mem, mode_s))
		{
			if (SetResourceForHandler (handler_p, resource_p, resource_mem))
				{
					success_flag = true;
				}
		}

	return success_flag;
}


size_t ReadFromHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	return (handler_p -> ha_read_fn (handler_p, buffer_p, length));
}


size_t WriteToHandler (struct Handler *handler_p, const void *buffer_p, const size_t length)
{
	return (handler_p -> ha_write_fn (handler_p, buffer_p, length));
}


bool CloseHandler (struct Handler *handler_p)
{
	bool success_flag = (handler_p -> ha_close_fn (handler_p));
	
	RemoveResourceFromHandler (handler_p);
	
	return success_flag;
}


bool SeekHandler (struct Handler *handler_p, size_t offset, int whence)
{
	return (handler_p -> ha_seek_fn (handler_p, offset, whence));
}


HandlerStatus GetHandlerStatus (struct Handler *handler_p)
{
	return (handler_p -> ha_status_fn (handler_p));
}


void FreeHandler (struct Handler *handler_p)
{
	handler_p -> ha_free_handler_fn (handler_p);
	RemoveResourceFromHandler (handler_p);
}


const char *GetHandlerName (struct Handler *handler_p)
{
	return (handler_p -> ha_get_name_fn (handler_p));
}


const char *GetHandlerDescription (struct Handler *handler_p)
{
	return (handler_p -> ha_get_description_fn (handler_p));
}


const char *GetHandlerProtocol (struct Handler *handler_p)
{
	return (handler_p -> ha_get_protocol_fn (handler_p));
}


bool IsHandlerForResource (struct Handler *handler_p, const Resource * resource_p)
{
	return (handler_p -> ha_match_fn (handler_p, resource_p));
}


bool CalculateFileInformationFromHandler (struct Handler *handler_p, FileInformation *info_p)
{
	return (handler_p -> ha_file_info_fn (handler_p, info_p));
}



HandlerNode *AllocateHandlerNode (struct Handler *handler_p)
{
	HandlerNode *node_p = (HandlerNode *) AllocMemory (sizeof (HandlerNode));

	if (node_p)
		{
			node_p -> hn_node.ln_prev_p = NULL;
			node_p -> hn_node.ln_next_p = NULL;
			node_p -> hn_handler_p = handler_p;
		}

	return node_p;
}


void FreeHandlerNode (ListItem *node_p)
{
	HandlerNode *handler_node_p = (HandlerNode *) node_p;

	if (handler_node_p -> hn_handler_p)
		{
			FreeHandler (handler_node_p -> hn_handler_p);
		}
		
	FreeMemory (handler_node_p);
}


//
//
//	Get Symbol
//
Handler *GetHandlerFromPlugin (Plugin * const plugin_p, const UserDetails *user_p)
{
	if (!plugin_p -> pl_handler_p)
		{
			void *symbol_p = GetSymbolFromPlugin (plugin_p, "GetHandler");

			if (symbol_p)
				{
					Handler *(*fn_p) (const UserDetails *) = (Handler *(*) (const UserDetails *)) symbol_p;

					plugin_p -> pl_handler_p = fn_p (user_p);

					if (plugin_p -> pl_handler_p)
						{
							plugin_p -> pl_handler_p -> ha_plugin_p = plugin_p;
							plugin_p -> pl_type = PN_HANDLER;
						}
				}
		}

	return plugin_p -> pl_handler_p;
}

bool DeallocatePluginHandler (Plugin * const plugin_p)
{
	bool success_flag = (plugin_p -> pl_handler_p == NULL);

	if (!success_flag)
		{
			void *symbol_p = GetSymbolFromPlugin (plugin_p, "ReleaseHandler");

			if (symbol_p)
				{
					void (*fn_p) (Handler *) = (void (*) (Handler *)) symbol_p;

					fn_p (plugin_p -> pl_handler_p);

					plugin_p -> pl_handler_p = NULL;
					success_flag = true;
				}
		}

	return success_flag;
}


/*********************************/


static bool SetResourceForHandler (struct Handler * UNUSED_PARAM (handler_p), Resource * UNUSED_PARAM (resource_p), MEM_FLAG UNUSED_PARAM (resource_mem))
{
	bool success_flag = false;

	return success_flag;
}


static void RemoveResourceFromHandler (struct Handler *handler_p)
{
	if (handler_p -> ha_resource_p)
		{
			switch (handler_p -> ha_resource_mem)
				{
					case MF_SHALLOW_COPY:
					case MF_DEEP_COPY:
						FreeResource (handler_p -> ha_resource_p);
						break;

					default:
						break;
				}

			handler_p -> ha_resource_p = NULL;
			handler_p -> ha_resource_mem = MF_ALREADY_FREED;
		}
}
