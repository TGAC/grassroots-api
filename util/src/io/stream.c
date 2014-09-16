#include "stream.h"


bool OpenStream (struct Stream *stream_p, const char * const filename_s, const char * const mode_s)
{
	return (stream_p -> st_open_fn (stream_p, filename_s, mode_s));
}


size_t ReadFromStream (struct Stream *stream_p, void *buffer_p, const size_t length)
{
	return (stream_p -> st_read_fn (stream_p, buffer_p, length));
}


bool CloseStream (struct Stream *stream_p)
{
	return (stream_p -> st_close_fn (stream_p));
}


size_t SeekStream (struct Stream *stream_p, size_t offset, int whence)
{
	return (stream_p -> st_seek_fn (stream_p, offset, whence));
}
