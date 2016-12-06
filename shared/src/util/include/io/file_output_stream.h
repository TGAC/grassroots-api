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
/**@file file_output_stream.h
*/

#ifndef FILE_OUTPUT_STREAM_H
#define FILE_OUTPUT_STREAM_H

#include "grassroots_util_library.h"
#include "streams.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief An OutputStream that writes to user-specified files
 *
 * @extends OutputStream
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
GRASSROOTS_UTIL_API OutputStream *AllocateFileOutputStream (const char * const filename_s);

/**
 * Free a FileOutputStream
 *
 * @param stream_p The FileOutputStream to free.
 */
GRASSROOTS_UTIL_API void DeallocateFileOutputStream (OutputStream *stream_p);

#ifdef __cplusplus
}
#endif

#endif	/* FILE_OUTPUT_STREAM_H */

