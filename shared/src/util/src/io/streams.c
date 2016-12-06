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

	if (stream_p == s_error_stream_p)
		{
			s_error_stream_p = NULL;
		}
	else if (stream_p == s_log_stream_p)
		{
			s_log_stream_p = NULL;
		}
}


int PrintToOutputStream (OutputStream *stream_p, const char * const filename_s, const int line_number, const char *message_s, ...)
{
	int result = -1;

	va_list args;

	va_start (args, message_s);

	if (stream_p)
		{
			result = stream_p -> st_print_fn (stream_p, STM_LEVEL_ALL, filename_s, line_number, message_s, args);
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
int PrintErrors (const uint32 level, const char * const filename_s, const int line_number, const char *message_s, ...)
{
	int result = -1;
	va_list args;

	va_start (args, message_s);

	result = PrintErrorsVarArgs (level, filename_s, line_number, message_s, args);

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
int PrintLog (const uint32 level, const char * const filename_s, const int line_number, const char *message_s, ...)
{
	int result = -1;
	va_list args;

	va_start (args, message_s);

	result = PrintLogVarArgs (level, filename_s, line_number, message_s, args);

	va_end (args);

	return result;
}



int PrintErrorsVarArgs (const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args)
{
	int result = -1;

	if (s_error_stream_p)
		{
			result = s_error_stream_p -> st_print_fn (s_error_stream_p, level, filename_s, line_number, message_s, args);
		}
	else
		{
			result = vfprintf (stderr, message_s, args);
		}

	return result;
}


int PrintLogVarArgs (const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args)
{
	int result = -1;

	if (s_log_stream_p)
		{
			result = s_log_stream_p -> st_print_fn (s_error_stream_p, level, filename_s, line_number, message_s, args);
		}
	else
		{
			result = vfprintf (stdout, message_s, args);
		}

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
