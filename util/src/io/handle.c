#include "handle.h"


bool OpenHandle (struct Handle *handle_p, const char * const filename_s, const char * const mode_s)
{
	return (handle_p -> ha_open_fn (handle_p, filename_s, mode_s));
}


size_t ReadFromHandle (struct Handle *handle_p, void *buffer_p, const size_t length)
{
	return (handle_p -> ha_read_fn (handle_p, buffer_p, length));
}


size_t WriteToHandle (struct Handle *handle_p, void *buffer_p, const size_t length)
{
	return (handle_p -> ha_write_fn (handle_p, buffer_p, length));
}


bool CloseHandle (struct Handle *handle_p)
{
	return (handle_p -> ha_close_fn (handle_p));
}


size_t SeekHandle (struct Handle *handle_p, size_t offset, int whence)
{
	return (handle_p -> ha_seek_fn (handle_p, offset, whence));
}


HandleStatus GetHandleStatus (struct Handle *handle_p)
{
	return (handle_p -> ha_status_fn (handle_p));	
}
