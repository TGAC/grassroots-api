#include "handler.h"


bool OpenHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s)
{
	return (handler_p -> ha_open_fn (handler_p, filename_s, mode_s));
}


size_t ReadFromHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	return (handler_p -> ha_read_fn (handler_p, buffer_p, length));
}


size_t WriteToHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	return (handler_p -> ha_write_fn (handler_p, buffer_p, length));
}


bool CloseHandler (struct Handler *handler_p)
{
	return (handler_p -> ha_close_fn (handler_p));
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
}


const char *GetHandlerProtocol (struct Handler *handler_p)
{
	return (handler_p -> ha_get_protocol_fn (handler_p));
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

	FreeHandler (handler_node_p -> hn_handler_p);
	FreeMemory (handler_node_p);
}
