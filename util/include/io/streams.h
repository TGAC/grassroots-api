/**@file streams.h
*/

#ifndef STREAMS_H
#define STREAMS_H

#include <stdarg.h>
#include <stdio.h>
#include "grassroots_util_library.h"
#include "typedefs.h"

/**
 * The Stream levels to use
 */

/** Do not print a message */
#define STM_LEVEL_NONE			(0)

/** A severe, probably terminal error */
#define STM_LEVEL_SEVERE	(1024)

/** A recoverable error where some functionality may be reduced. */
#define STM_LEVEL_WARNING	(1025)

/** An general information message */
#define STM_LEVEL_INFO		(1026)

/** A fine level message */
#define STM_LEVEL_FINE		(1027)

/** A more finely-grained level message */
#define STM_LEVEL_FINER		(1028)

/** An  even more finely-grained level message */
#define STM_LEVEL_FINEST	(1029)

/** Print all messages */
#define STM_LEVEL_ALL		(0xFFFFFFFF)



/******** FORWARD DECLARATION ******/
struct OutputStream;


/**
 * An datatype to abstract out the process of writing log and error messages
 * to the appropriate processes and places on a given Server.
 */
typedef struct OutputStream
{
	/**
	 * Print to an OutputStream
	 *
	 * @param stream_p The OutputStream to print to.
	 * @param level The
	 * @param message_s The message to print. This can include the standard
	 * printf format specifiers.
	 * @param args Any arguments to match against the format specifiers.
	 * @return If successful, the total number of characters written. If this
	 * is negative then an error has occurred.
	 * @memberof OutputStream
	 * @see PrintToOutputStream
	 */
	int (*st_print_fn) (struct OutputStream *stream_p, const uint32 level, const char *message_s, va_list args);

	/**
	 * Flush aany pending writes to an OutputStream.
	 *
	 * @param stream_p The OutputStream to flush
	 * @return <code>true</code> if the OutputStream was flushed succesfully,
	 * <code>false</code> otherwise.
	 */
	bool (*st_flush_fn) (struct OutputStream *stream_p);

	/**
	 * Callback function to free the OutputStream if it needs any custom behaviour.
	 *
	 * @param stream_p The OutputStream to free.
	 */
	void (*st_free_stream_fn) (struct OutputStream *stream_p);
} OutputStream;


#ifdef __cplusplus
	extern "C" {
#endif


/**
 * @brief Initialise the default OutputStreams ready for use.
 *
 * This should be called when starting up a Server.
 *
 * @return <code>true</code> if the initialisation was succesful,
 * <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool InitDefaultOutputStream (void);


/**
 * Free the default OutputStreams
 *
 * This should be called when stopping a Server.
 *
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API void FreeDefaultOutputStream (void);


/**
 * Set the Server-side OutputStream for printing error messages to.
 *
 * @param stream_p The OutputStream to use.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API void SetDefaultErrorStream (OutputStream *stream_p);


/**
 * Set the Server-side OutputStream for printing error messages to.
 *
 * @param stream_p The OutputStream to use.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API void SetDefaultLogStream (OutputStream *stream_p);


/**
 * Free an OutputStream
 *
 * This will call st_free_fn.
 *
 * @param stream_p The OutputStream to free
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API void FreeOutputStream (struct OutputStream *stream_p);


/**
 * Print to a given error OutputStream.
 *
 * This will call st_print_fn.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 * @see st_print_fn
 */
GRASSROOTS_UTIL_API int PrintToOutputStream (OutputStream *stream_p, const char *message_s, ...);


/**
 * Print to the error OutputStream. If this has not
 * previously been set, it will default to stderr.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API int PrintErrors (const uint32 level, const char *message, ...);


/**
 * Print to the log OutputStream. If this has not
 * previously been set, it will default to stdout.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the log OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API int PrintLog (const uint32 level, const char *message, ...);


/**
 * Get where logging messages will be sent.
 *
 * @return The OutputStream to send results to.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API OutputStream *GetLogOutput (void);


/**
 * Get where error messages will be sent.
 *
 * @return The OutputStream to send results to.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API OutputStream *GetErrorsOutput (void);


/**
 * Flush an OutputStream.
 *
 * @param stream_p The OutputStream to flush.
 * @return <code>true</code> if the OutputStream was flushed successfully,
 * <code>falsse</code> otherwise.
 * @memberof OutputStream
 * @see st_flush_fn
 */
GRASSROOTS_UTIL_API bool FlushOutputStream (OutputStream *stream_p);


GRASSROOTS_UTIL_API bool FlushLog (void);


GRASSROOTS_UTIL_API bool FlushErrors (void);


#ifdef __cplusplus
}
#endif

#endif	/* STREAMS_H */

