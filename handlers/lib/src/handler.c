#include "handler.h"

#include "string_utils.h"

static bool SetFilename (struct Handler *handler_p, const char * const filename_s);

static void FreeFilename (struct Handler *handler_p);



void InitialiseHandler (Handler * const handler_p,
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
	void (*free_handler_fn) (struct Handler *handler_p))
{
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

	handler_p -> ha_filename_s = NULL;
	handler_p -> ha_plugin_p = NULL;
}



bool InitHandler (struct Handler *handler_p)
{
	return (handler_p -> ha_init_fn (handler_p));
}


bool OpenHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s)
{
	bool success_flag = false;
	
	if (handler_p -> ha_open_fn (handler_p, filename_s, mode_s))
		{
			success_flag = SetFilename (handler_p, filename_s);
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
	
	FreeFilename (handler_p);
	
	return success_flag;
}


size_t SeekHandler (struct Handler *handler_p, size_t offset, int whence)
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
	FreeFilename (handler_p);
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
Handler *GetHandlerFromPlugin (Plugin * const plugin_p, const json_t *tags_p)
{
	if (!plugin_p -> pl_handler_p)
		{
			void *symbol_p = GetSymbolFromPlugin (plugin_p, "GetHandler");

			if (symbol_p)
				{
					Handler *(*fn_p) (const json_t *) = (Handler *(*) (const json_t *)) symbol_p;

					plugin_p -> pl_handler_p = fn_p (tags_p);

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


static bool SetFilename (struct Handler *handler_p, const char * const filename_s)
{
	if (handler_p -> ha_filename_s)
		{
			FreeCopiedString (handler_p -> ha_filename_s);
		}
		
	handler_p -> ha_filename_s = CopyToNewString (filename_s, 0, false);

	return (handler_p -> ha_filename_s != NULL);	
}


static void FreeFilename (struct Handler *handler_p)
{
	if (handler_p -> ha_filename_s)
		{
			FreeCopiedString (handler_p -> ha_filename_s);
			handler_p -> ha_filename_s = NULL;
		}
}
