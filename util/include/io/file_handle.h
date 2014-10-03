#ifndef WHEATIS_FILE_HANDLE_H
#define WHEATIS_FILE_HANDLE_H

#include <stdio.h>

#include "wheatis_util_library.h"
#include "handle.h"


typedef struct FileHandle
{
	Handle fh_base_handle;
	FILE *fh_handle_f;
} FileHandle;


#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_UTIL_API Handle *AllocateFileHandle (void);

WHEATIS_UTIL_API void FreeFileHandle (Handle *handle_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_FILE_HANDLE_H */
