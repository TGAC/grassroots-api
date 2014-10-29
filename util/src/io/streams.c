#include <stdarg.h>

#include "streams.h"
#include "file_output_stream.h"


static OutputStream *s_stdout_stream_p = NULL;

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

	OutputStreamerModule *streamer_p = GetStreamer ();
	OutputStream *stream_p = NULL;

	if (streamer_p)
		{
			stream_p = streamer_p -> osm_get_errors_stream_fn (streamer_p);
		}

	va_start (args, message_s);

	if (stream_p)
		{
			result = stream_p -> st_print_fn (stream_p, level, message_s, args);
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

	OutputStreamerModule *streamer_p = GetStreamer ();
	OutputStream *stream_p = NULL;

	if (streamer_p)
		{
			stream_p = streamer_p -> osm_get_log_stream_fn (streamer_p);
		}

	va_start (args, message_s);

	if (stream_p)
		{
			result = stream_p -> st_print_fn (stream_p, level, message_s, args);
		}
	else
		{
			result = vfprintf (stdout, message_s, args); 
		}

	va_end (args);

	return result;
}


/**
 * Print to the results FILE.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the results FILE is NULL, then
 * this will return 0.
 */
int PrintResults (const uint32 level, const char *message_s, ...)
{
	int result = -1;
	va_list args;

	OutputStreamerModule *streamer_p = GetStreamer ();
	OutputStream *stream_p = NULL;

	if (streamer_p)
		{
			stream_p = streamer_p -> osm_get_results_stream_fn (streamer_p);
		}

	va_start (args, message_s);

	if (stream_p)
		{
			result = stream_p -> st_print_fn (stream_p, level, message_s, args);
		}
	else
		{
			result = vfprintf (stdout, message_s, args); 
		}

	va_end (args);

	return result;
}


/**
 * Get where result messages will be sent.
 *
 * @return The FILE to send results to.
 */
OutputStream *GetResultsOutput (void)
{
	OutputStreamerModule *streamer_p = GetStreamer ();

	if (streamer_p)
		{
			return streamer_p -> osm_get_results_stream_fn (streamer_p);
		}
	else
		{
			if (!s_stdout_stream_p)
				{
					s_stdout_stream_p = (OutputStream *) AllocateFileOutputStream (NULL);
				}

			return s_stdout_stream_p;
		}
}


/**
 * Get where logging messages will be sent.
 *
 * @return The FILE to send results to.
 */
OutputStream *GetLogOutput (void)
{
	OutputStreamerModule *streamer_p = GetStreamer ();

	if (streamer_p)
		{
			return streamer_p -> osm_get_log_stream_fn (streamer_p);
		}
	else
		{
			if (!s_stdout_stream_p)
				{
					s_stdout_stream_p = (OutputStream *) AllocateFileOutputStream (NULL);
				}

			return s_stdout_stream_p;
		}
}

/**
 * Get where error messages will be sent.
 *
 * @return The FILE to send results to.
 */
OutputStream *GetErrorsOutput (void)
{
	OutputStreamerModule *streamer_p = GetStreamer ();

	if (streamer_p)
		{
			return streamer_p -> osm_get_errors_stream_fn (streamer_p);
		}
	else
		{
			if (!s_stdout_stream_p)
				{
					s_stdout_stream_p = (OutputStream *) AllocateFileOutputStream (NULL);
				}

			return s_stdout_stream_p;
		}
}


bool FlushOutputStream (OutputStream *stream_p)
{
	return (stream_p -> st_flush_fn (stream_p));
}
