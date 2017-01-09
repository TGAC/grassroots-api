/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
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

/**
 * Do not print a message
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_NONE			(0)


/**
 * A severe, probably terminal error
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_SEVERE	(1024)

/**
 * A recoverable error where some functionality may be reduced.
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_WARNING	(1025)

/**
 * An general information message
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_INFO		(1026)

/**
 * A fine level message
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_FINE		(1027)

/**
 * A more finely-grained level message
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_FINER		(1028)

/**
 * An even more finely-grained level message
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_FINEST	(1029)


/**
 * Print all messages
 *
 * @ingroup utility_group
 */
#define STM_LEVEL_ALL		(0xFFFFFFFF)



/******** FORWARD DECLARATION ******/
struct OutputStream;


/**
 * An datatype to abstract out the process of writing log and error messages
 * to the appropriate processes and places on a given Server.
 *
 * @ingroup utility_group
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
	int (*st_print_fn) (struct OutputStream *stream_p, const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args);

	/**
	 * Flush any pending writes to an OutputStream.
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
 *
 * @ingroup utility_group
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
 * @param stream_p The OutputStream to print to.
 * @param filename_s The name of the file calling this function.
 * @param line_number The line in the file from which this function was called.
 * @param message_s A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 * @see st_print_fn
 */
GRASSROOTS_UTIL_API int PrintToOutputStream (OutputStream *stream_p, const char * const filename_s, const int line_number, const char *message_s, ...);


/**
 * Print to the error OutputStream. If this has not
 * previously been set, it will default to stderr.
 *
 * @param level The stream level to use.
 * @param filename_s The name of the file calling this function.
 * @param line_number The line in the file from which this function was called.
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API int PrintErrors (const uint32 level, const char * const filename_s, const int line_number, const char *message, ...);


/**
 * Print to the log OutputStream. If this has not
 * previously been set, it will default to stdout.
 *
 * @param level The stream level to use.
 * @param filename_s The name of the file calling this function.
 * @param line_number The line in the file from which this function was called.
 * @param message_s A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the log OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API int PrintLog (const uint32 level, const char * const filename_s, const int line_number, const char *message_s, ...);


/**
 * Print to the error OutputStream. If this has not
 * previously been set, it will default to stderr.
 *
 * @param level The stream level to use.
 * @param filename_s The name of the file calling this function.
 * @param line_number The line in the file from which this function was called.
 * @param message_s A char * using the same format as printf, etc.
 * @param args A va_list of the values to print.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API int PrintErrorsVarArgs (const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args);


/**
 * Print to the log OutputStream. If this has not
 * previously been set, it will default to stdout.
 *
 * @param level The stream level to use.
 * @param filename_s The name of the file calling this function.
 * @param line_number The line in the file from which this function was called.
 * @param message_s A char * using the same format as printf, etc.
 * @param args A va_list of the values to print.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the log OutputStream is NULL, then
 * this will return 0.
 * @memberof OutputStream
 */
GRASSROOTS_UTIL_API int PrintLogVarArgs (const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args);

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


/**
 * Flush the logging OutputStream.
 *
 * @return <code>true</code> if the logging OutputStream was flushed successfully,
 * <code>falsse</code> otherwise.
 * @memberof OutputStream
 * @see FlushOutputStream
 */
GRASSROOTS_UTIL_API bool FlushLog (void);


/**
 * Flush the errors OutputStream.
 *
 * @return <code>true</code> if the errors OutputStream was flushed successfully,
 * <code>falsse</code> otherwise.
 * @memberof OutputStream
 * @see FlushOutputStream
 */
GRASSROOTS_UTIL_API bool FlushErrors (void);


#ifdef __cplusplus
}
#endif

#endif	/* STREAMS_H */

