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
#include "httpd.h"
#include "http_log.h"

#include "apr_strings.h"

#include "apache_output_stream.h"
#include "memory_allocations.h"


static int PrintToApacheStream (OutputStream *stream_p, const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args);

static bool FlushApacheStream (OutputStream *stream_p);



OutputStream *AllocateApacheOutputStream (server_rec *server_p)
{
	ApacheOutputStream *stream_p = (ApacheOutputStream *) AllocMemory (sizeof (ApacheOutputStream));

	if (stream_p)
		{
			stream_p -> aos_stream.st_print_fn = PrintToApacheStream;
			stream_p -> aos_stream.st_flush_fn = FlushApacheStream;
			stream_p -> aos_stream.st_free_stream_fn = DeallocateApacheOutputStream;

			stream_p -> aos_server_p = server_p;

			return ((OutputStream *) stream_p);
		}

	return NULL;

}


void DeallocateApacheOutputStream (OutputStream *stream_p)
{
	ApacheOutputStream *apache_stream_p = (ApacheOutputStream *) stream_p;
	FreeMemory (apache_stream_p);
}



static int PrintToApacheStream (OutputStream *stream_p, const uint32 level, const char * const filename_s, const int line_number, const char *message_s, va_list args)
{
	ApacheOutputStream *apache_stream_p = (ApacheOutputStream *) stream_p;
	char *value_s = apr_pvsprintf (apache_stream_p -> aos_server_p -> process -> pool, message_s, args);
	int ap_level;

	/* filenames and line numbers are only printed if APLOG_DEBUG is set, so use this all of the time */
	switch (level)
		{
			case STM_LEVEL_SEVERE:
				ap_level = APLOG_DEBUG;
				break;

			case STM_LEVEL_WARNING:
				ap_level = APLOG_DEBUG;
				break;

			case STM_LEVEL_INFO:
			case STM_LEVEL_FINE:
				ap_level = APLOG_NOERRNO | APLOG_DEBUG;
				break;

			case STM_LEVEL_FINER:
			case STM_LEVEL_FINEST:
			case STM_LEVEL_ALL:
				ap_level = APLOG_NOERRNO | APLOG_DEBUG;
				break;

			default:
				ap_level = APLOG_NOERRNO | APLOG_DEBUG;
				break;
		}

	ap_log_error (filename_s, line_number, APLOG_MODULE_INDEX, ap_level, APR_SUCCESS, apache_stream_p -> aos_server_p, "mod_grassroots: %s", value_s);

	return 0;
}


static bool FlushApacheStream (OutputStream *stream_p)
{
	return true;
}
