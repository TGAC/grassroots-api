#ifndef WHEATIS_STREAM_H
#define WHEATIS_STREAM_H

#include <stddef.h>

#include "typedefs.h"
#include "wheatis_util_library.h"

struct Stream;

typedef struct Stream
{
	bool (*st_open_fn) (struct Stream *stream_p, const char * const filename_s, const char * const mode_s);
	size_t (*st_read_fn) (struct Stream *stream_p, void *buffer_p, const size_t length);
	size_t (*st_write_fn) (struct Stream *stream_p, void *buffer_p, const size_t length);	
	bool (*st_seek_fn) (struct Stream *stream_p, long offset, int whence);
	bool (*st_close_fn) (struct Stream *stream_p);
	bool (*st_status_fn) (struct Stream *stream_p);
} Stream;

#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_UTIL_API bool OpenStream (struct Stream *stream_p, const char * const filename_s, const char * const mode_s);

WHEATIS_UTIL_API size_t ReadFromStream (struct Stream *stream_p, void *buffer_p, const size_t length);

WHEATIS_UTIL_API size_t WriteToStream (struct Stream *stream_p, void *buffer_p, const size_t length);

WHEATIS_UTIL_API size_t SeekStream (struct Stream *stream_p, size_t offset, int whence);

WHEATIS_UTIL_API bool CloseStream (struct Stream *stream_p);

WHEATIS_UTIL_API bool IsStreamGood (struct Stream *stream_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_STREAM_H */
