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
