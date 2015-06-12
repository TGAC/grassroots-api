/**@file file_output_stream.h
*/

#ifndef FILE_OUTPUT_STREAM_H
#define FILE_OUTPUT_STREAM_H

#include "wheatis_util_library.h"
#include "streams.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief An OutputStream that writes to user-specified files
 */
typedef struct FileOutputStream
{
	/** The base OutputStream */
	OutputStream fos_stream;

	/** The filename that this OutputStream will write to */
	char *fos_filename_s;

	/** The FILE for this OutputStream */
	FILE *fos_out_f;

	/**
	 * <code>true</code> if the fos_out_f will be closed on exit, <code>false</code>
	 * otherwise.
	 */
	bool fos_close_on_exit_flag;
} FileOutputStream;

/**
 * @brief Allocate a FileOutputStream.
 *
 * @param filename_s The filename to write to. If this is <code>NULL</code> or cannot
 * be opened for writing, the FileOutputStream will write to stdout instead.
 * @return A newly-allocated FileOutputStream or <code>NULL</code> on error.
 * @memberof FileOutputStream
 * @see DeallocateFileOutputStream
 */
WHEATIS_UTIL_API OutputStream *AllocateFileOutputStream (const char * const filename_s);

/**
 * Free a FileOutputStream
 *
 * @param stream_p The FileOutputStream to free.
 */
WHEATIS_UTIL_API void DeallocateFileOutputStream (OutputStream *stream_p);

#ifdef __cplusplus
}
#endif

#endif	/* FILE_OUTPUT_STREAM_H */

