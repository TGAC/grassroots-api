#include <stdarg.h>
#include <string.h>

#include "memory_allocations.h"
#include "file_output_stream.h"
#include "string_utils.h"


/*********************************************/
/************ STATIC PROTOTYTPES *************/
/*********************************************/

static int PrintToFileStream (OutputStream *stream_p, const uint32 level, const char *message_s, va_list args);

static bool FlushFileStream (OutputStream *stream_p);


FileOutputStream *AllocateFileOutputStream (const char * const filename_s)
{
	FileOutputStream *fos_p = (FileOutputStream *) AllocMemory (sizeof (FileOutputStream));

	if (fos_p)
		{
			fos_p -> fos_out_f = NULL;
			fos_p -> fos_close_on_exit_flag = FALSE;

			if (filename_s)
				{
					fos_p -> fos_out_f = fopen (filename_s, "w");
					fos_p -> fos_filename_s = CopyToNewString (filename_s, 0, FALSE);
				}

			if (fos_p -> fos_out_f)
				{
					fos_p -> fos_close_on_exit_flag  = TRUE;
				}
			else
				{
					fos_p -> fos_out_f = stdout;
					fos_p -> fos_filename_s = "stdout";
					fos_p -> fos_close_on_exit_flag  = FALSE;
				}

			fos_p -> fos_stream.st_print_fn = PrintToFileStream;
			fos_p -> fos_stream.st_flush_fn = FlushFileStream;

			return fos_p;
		}

	return NULL;
}


void DeallocateFileOutputStream (FileOutputStream *stream_p)
{
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


static int PrintToFileStream (OutputStream *stream_p, const uint32 level, const char *message_s, va_list args)
{
	return vfprintf (((FileOutputStream *) stream_p) -> fos_out_f, message_s, args);
}


static bool FlushFileStream (OutputStream *stream_p)
{
	return (fflush (((FileOutputStream *) stream_p) -> fos_out_f) == 0);
}

