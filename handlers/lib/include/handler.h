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
} HandlerStatus;

struct Handler;


typedef struct HandlerData
{
	/** The Client that owns this data. */
	struct Handler *hd_handler_p;
} HandlerData;

typedef struct Handler
{
	/**
	 * The platform-specific plugin that the code for the Client is
	 * stored in.
	 */
	struct Plugin *ha_plugin_p;
	
	bool (*ha_open_fn) (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);
	size_t (*ha_read_fn) (struct Handler *handler_p, void *buffer_p, const size_t length);
	size_t (*ha_write_fn) (struct Handler *handler_p, void *buffer_p, const size_t length);	
	bool (*ha_seek_fn) (struct Handler *handler_p, long offset, int whence);
	bool (*ha_close_fn) (struct Handler *handler_p);
	HandlerStatus (*ha_status_fn) (struct Handler *handler_p);
	
	HandlerData *ha_data_p;
} Handler;

#ifdef __cplusplus
extern "C" 
{
#endif

WHEATIS_UTIL_API bool OpenHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);

WHEATIS_UTIL_API size_t ReadFromHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

WHEATIS_UTIL_API size_t WriteToHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

WHEATIS_UTIL_API size_t SeekHandler (struct Handler *handler_p, size_t offset, int whence);

WHEATIS_UTIL_API bool CloseHandler (struct Handler *handler_p);

WHEATIS_UTIL_API HandlerStatus GetHandlerStatus (struct Handler *handler_p);

WHEATIS_UTIL_API HandlerStatus GetHandlerStatus (struct Handler *handler_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef WHEATIS_HANDLE_H */
