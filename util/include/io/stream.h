#ifndef WHEATIS_STREAM_H
#define WHEATIS_STREAM_H

#include <stddef.h>

#include "typedefs.h"

struct Stream;

typedef struct Stream
{
	bool (*st_open_fn) (struct Stream *stream_p, const char * const filename_s, const char * const mode_s);
	size_t (*st_read_fn) (struct Stream *stream_p, void *buffer_p, const size_t length);
	bool (*st_seek_fn) (struct Stream *stream_p, long offset, int whence);
	bool (*st_close_fn) (struct Stream *stream_p);
} Stream;

#ifdef __cplusplus
extern "C" 
{
#endif

bool OpenStream (struct Stream *stream_p, const char * const filename_s, const char * const mode_s);

size_t ReadFromStream (struct Stream *stream_p, void *buffer_p, const size_t length);

size_t SeekStream (struct Stream *stream_p, size_t offset, int whence);

bool CloseStream (struct Stream *stream_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_STREAM_H */
