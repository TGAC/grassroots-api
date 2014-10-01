#ifndef WHEATIS_FILE_STREAM_H
#define WHEATIS_FILE_STREAM_H

#include <stdio.h>

#include "wheatis_util_library.h"
#include "stream.h"


typedef struct FileStream
{
	Stream fs_base_stream;
	FILE *fs_stream_f;
} FileStream;


#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_UTIL_API Stream *AllocateFileStream (void);

WHEATIS_UTIL_API void FreeFileStream (Stream *stream_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_FILE_STREAM_H */
