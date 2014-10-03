#ifndef WHEATIS_HANDLE_H
#define WHEATIS_HANDLE_H

#include <stddef.h>

#include "typedefs.h"
#include "wheatis_util_library.h"


typedef enum
{
	HS_GOOD,
	HS_FINISHED,
	HS_BAD
} HandleStatus;

struct Handle;

typedef struct Handle
{
	bool (*ha_open_fn) (struct Handle *handle_p, const char * const filename_s, const char * const mode_s);
	size_t (*ha_read_fn) (struct Handle *handle_p, void *buffer_p, const size_t length);
	size_t (*ha_write_fn) (struct Handle *handle_p, void *buffer_p, const size_t length);	
	bool (*ha_seek_fn) (struct Handle *handle_p, long offset, int whence);
	bool (*ha_close_fn) (struct Handle *handle_p);
	HandleStatus (*ha_status_fn) (struct Handle *handle_p);
} Handle;

#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_UTIL_API bool OpenHandle (struct Handle *handle_p, const char * const filename_s, const char * const mode_s);

WHEATIS_UTIL_API size_t ReadFromHandle (struct Handle *handle_p, void *buffer_p, const size_t length);

WHEATIS_UTIL_API size_t WriteToHandle (struct Handle *handle_p, void *buffer_p, const size_t length);

WHEATIS_UTIL_API size_t SeekHandle (struct Handle *handle_p, size_t offset, int whence);

WHEATIS_UTIL_API bool CloseHandle (struct Handle *handle_p);

WHEATIS_UTIL_API HandleStatus GetHandleStatus (struct Handle *handle_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_HANDLE_H */
