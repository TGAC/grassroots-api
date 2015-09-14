#include <stdarg.h>

#include "streams.h"
#include "file_output_stream.h"


static OutputStream *s_log_stream_p = NULL;
static OutputStream *s_error_stream_p = NULL;



bool InitDefaultOutputStream (void)
{
	s_log_stream_p = AllocateFileOutputStream (NULL);
	
	if (s_log_stream_p)
		{
			s_error_stream_p = AllocateFileOutputStream (NULL);
			
			if (s_error_stream_p)
				{
					return true;
				}
				
			FreeOutputStream (s_log_stream_p);
			s_log_stream_p = NULL;
		}
		
	return false;
}


void FreeDefaultOutputStream (void)
{
	if (s_log_stream_p)
		{
			FreeOutputStream (s_log_stream_p);
			s_log_stream_p = NULL;
		}		

	if (s_error_stream_p)
		{
			FreeOutputStream (s_error_stream_p);
			s_error_stream_p = NULL;
		}		
}


void SetDefaultErrorStream (OutputStream *stream_p)
{
	if (s_error_stream_p)
		{
			FreeOutputStream (s_error_stream_p);
		}
		
	s_error_stream_p = stream_p;
}


void SetDefaultLogStream (OutputStream *stream_p)
{
	if (s_log_stream_p)
		{
			FreeOutputStream (s_log_stream_p);
		}
		
	s_log_stream_p = stream_p;
}


void FreeOutputStream (struct OutputStream *stream_p)
{
	stream_p -> st_free_stream_fn (stream_p);
}


int PrintToOutputStream (OutputStream *stream_p, const char *message_s, ...)
{
	int result = -1;

	va_list args;

	va_start (args, message_s);

	if (stream_p)
		{
			result = stream_p -> st_print_fn (stream_p, STM_LEVEL_ALL, message_s, args);
		}
	else
		{
			result = vfprintf (stdout, message_s, args);
		}

	va_end (args);

	return result;
}



/**
 * Print to the error FILE. If this has not
 * previously been set, it will default to stderr.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error FILE is NULL, then
 * this will return 0.
 */
int PrintErrors (const uint32 level, const char *message_s, ...)
{
	int result = -1;
	va_list args;

	va_start (args, message_s);

	if (s_error_stream_p)
		{
			result = s_error_stream_p -> st_print_fn (s_error_stream_p, level, message_s, args);
		}
	else
		{
			result = vfprintf (stdout, message_s, args); 
		}

	va_end (args);

	return result;
}


/**
 * Print to the log FILE.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the log FILE is NULL, then
 * this will return 0.
 */
int PrintLog (const uint32 level, const char *message_s, ...)
{
	int result = -1;
	va_list args;

	va_start (args, message_s);

	if (s_log_stream_p)
		{
			result = s_log_stream_p -> st_print_fn (s_log_stream_p, level, message_s, args);
		}
	else
		{
			result = vfprintf (stdout, message_s, args); 
		}

	va_end (args);

	return result;
}


bool FlushLog (void)
{
	bool success_flag = true;

	if (s_log_stream_p)
		{
			success_flag = s_log_stream_p -> st_flush_fn (s_log_stream_p);
		}

	return success_flag;
}



bool FlushErrors (void)
{
	bool success_flag = true;

	if (s_error_stream_p)
		{
			success_flag = s_error_stream_p -> st_flush_fn (s_error_stream_p);
		}

	return success_flag;
}
