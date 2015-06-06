#include "httpd.h"
#include "http_log.h"

#include "apr_strings.h"

#include "apache_output_stream.h"
#include "memory_allocations.h"


static int PrintToApacheStream (OutputStream *stream_p, const uint32 level, const char *message_s, va_list args);

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



static int PrintToApacheStream (OutputStream *stream_p, const uint32 level, const char *message_s, va_list args)
{
	ApacheOutputStream *apache_stream_p = (ApacheOutputStream *) stream_p;
	char *value_s = apr_pvsprintf (apache_stream_p -> aos_server_p -> process -> pool, message_s, args);
	int ap_level;

	switch (level)
		{
			case STM_LEVEL_SEVERE:
				ap_level = APLOG_CRIT;
				break;

			case STM_LEVEL_WARNING:
				ap_level = APLOG_WARNING;
				break;

			case STM_LEVEL_INFO:
				ap_level = APLOG_NOERRNO |APLOG_INFO;
				break;

			case STM_LEVEL_FINE:
			case STM_LEVEL_FINER:
			case STM_LEVEL_FINEST:
			case STM_LEVEL_ALL:
				ap_level = APLOG_NOERRNO | APLOG_DEBUG;
				break;

			default:
				ap_level = APLOG_NOERRNO | APLOG_DEBUG;
				break;
		}

	ap_log_error (APLOG_MARK, ap_level, APR_SUCCESS, apache_stream_p -> aos_server_p, "mod_wheatis: %s", value_s);

	return 0;
}


static bool FlushApacheStream (OutputStream *stream_p)
{
	return true;
}
