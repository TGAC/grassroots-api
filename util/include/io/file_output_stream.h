/**@file file_output_stream.h
*/ 

#ifndef FILE_OUTPUT_STREAM_H
#define FILE_OUTPUT_STREAM_H

#include "wheatis_util_library.h"
#include "streams.h"

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct FileOutputStream
{
	OutputStream fos_stream;
	char *fos_filename_s;
	FILE *fos_out_f;
	bool fos_close_on_exit_flag;
} FileOutputStream;


WHEATIS_UTIL_API OutputStream *AllocateFileOutputStream (const char * const filename_s);


WHEATIS_UTIL_API void DeallocateFileOutputStream (OutputStream *stream_p);

#ifdef __cplusplus
}
#endif

#endif	/* FILE_OUTPUT_STREAM_H */

