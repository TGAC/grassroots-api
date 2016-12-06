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
#include <stdarg.h>
#include <string.h>

#include "memory_allocations.h"
#include "file_output_stream.h"
#include "string_utils.h"


/*********************************************/
/************ STATIC PROTOTYTPES *************/
/*********************************************/

static int PrintToFileStream (OutputStream *stream_p, const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args);

static bool FlushFileStream (OutputStream *stream_p);


OutputStream *AllocateFileOutputStream (const char * const filename_s)
{
	FileOutputStream *fos_p = (FileOutputStream *) AllocMemory (sizeof (FileOutputStream));

	if (fos_p)
		{
			fos_p -> fos_out_f = NULL;
			fos_p -> fos_close_on_exit_flag = false;

			if (filename_s)
				{
					fos_p -> fos_out_f = fopen (filename_s, "w");
					fos_p -> fos_filename_s = CopyToNewString (filename_s, 0, false);
				}

			if (fos_p -> fos_out_f)
				{
					fos_p -> fos_close_on_exit_flag  = true;
				}
			else
				{
					fos_p -> fos_out_f = stdout;
					fos_p -> fos_filename_s = "stdout";
					fos_p -> fos_close_on_exit_flag  = false;
				}

			fos_p -> fos_stream.st_print_fn = PrintToFileStream;
			fos_p -> fos_stream.st_flush_fn = FlushFileStream;
			fos_p -> fos_stream.st_free_stream_fn = DeallocateFileOutputStream;

			return ((OutputStream *) fos_p);
		}

	return NULL;
}


void DeallocateFileOutputStream (OutputStream *stm_p)
{
	FileOutputStream *stream_p = (FileOutputStream *) stm_p;
	
	if (stream_p -> fos_close_on_exit_flag)
		{
			int res = fclose (stream_p -> fos_out_f);
			
			if (res != 0)
				{
					fprintf (stderr, "failed to close output stream\n");
				}				
		}
		
	if (stream_p -> fos_filename_s)
		{
			if (strcmp ("stdout", stream_p -> fos_filename_s) != 0)
				{
					FreeCopiedString (stream_p -> fos_filename_s);
				}
		}

	FreeMemory (stream_p);
}


static int PrintToFileStream (OutputStream *stream_p, const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args)
{
	FILE *out_f = ((FileOutputStream *) stream_p) -> fos_out_f;
	int res = fprintf (out_f, "%s:%d " UINT32_FMT, filename_s, line_number, level);

	if (res >= 0)
		{
			res = vfprintf (out_f, message_s, args);
		}

	return res;
}


static bool FlushFileStream (OutputStream *stream_p)
{
	return (fflush (((FileOutputStream *) stream_p) -> fos_out_f) == 0);
}

