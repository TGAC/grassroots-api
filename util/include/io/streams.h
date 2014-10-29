/**@file streams.h
*/

#ifndef STREAMS_H
#define STREAMS_H

#include <stdarg.h>
#include <stdio.h>
#include "wheatis_util_library.h"
#include "typedefs.h"


#define STM_LEVEL_NONE			(0)
#define STM_LEVEL_SEVERE	(1024)
#define STM_LEVEL_WARNING	(1025)
#define STM_LEVEL_INFO		(1026)
#define STM_LEVEL_FINE		(1027)
#define STM_LEVEL_FINER		(1028)
#define STM_LEVEL_FINEST	(1029)
#define STM_LEVEL_ALL		(0xFFFFFFFF)


#ifdef __cplusplus
	extern "C" {
#endif


/******** FORWARD DECLARATION ******/
struct OutputStream;

typedef struct OutputStream
{
	int (*st_print_fn) (struct OutputStream *stream_p, const uint32 level, const char *message_s, va_list args);
	BOOLEAN (*st_flush_fn) (struct OutputStream *stream_p);
} OutputStream;


WHEATIS_UTIL_API int PrintToOutputStream (OutputStream *stream_p, const char *message_s, ...);


/**
 * Print to the error OutputStream. If this has not
 * previously been set, it will default to stderr.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error OutputStream is NULL, then
 * this will return 0.
 */
WHEATIS_UTIL_API int PrintErrors (const uint32 level, const char *message, ...);


/**
 * Print to the log OutputStream.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the log OutputStream is NULL, then
 * this will return 0.
 */
WHEATIS_UTIL_API int PrintLog (const uint32 level, const char *message, ...);


/**
 * Get where logging messages will be sent.
 *
 * @return The OutputStream to send results to.
 */
WHEATIS_UTIL_API OutputStream *GetLogOutput (void);


/**
 * Get where error messages will be sent.
 *
 * @return The OutputStream to send results to.
 */
WHEATIS_UTIL_API OutputStream *GetErrorsOutput (void);


/**
 * Flush an OutputStream.
 *
 * @param stream_p The OutputStream to flush.
 * @return TRUE if the OutputStream was flushed successfully, FALSE otherwise.
 */
WHEATIS_UTIL_API BOOLEAN FlushOutputStream (OutputStream *stream_p);

#ifdef __cplusplus
}
#endif

#endif	/* STREAMS_H */

