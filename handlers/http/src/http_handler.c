#include <string.h>

#include "http_handler.h"
#include "memory_allocations.h"
#include "data_resource.h"


static bool InitHttpHandler (struct Handler *handler_p, json_t *credentials_p);


static bool OpenHttpHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromHttpHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

static size_t WriteToHttpHandler (struct Handler *handler_p, const void *buffer_p, const size_t length);

static bool SeekHttpHandler (struct Handler *handler_p, long offset, int whence);

static bool CloseHttpHandler (struct Handler *handler_p);

static HandlerStatus GetHttpHandlerStatus (struct Handler *handler_p);

static bool IsResourceForHttpHandler (struct Handler *handler_p, const Resource * resource_p);

static const char *GetHttpHandlerProtocol (struct Handler *handler_p);

static const char *GetHttpHandlerName (struct Handler *handler_p);

static const char *GetHttpHandlerDescription (struct Handler *handler_p);

static void FreeHttpHandler (struct Handler *handler_p);

static bool IsResourceForHttpHandler (struct Handler *handler_p, const Resource * resource_p);

static bool CalculateHttpInformationFromHttpHandler (struct Handler *handler_p, HttpInformation *info_p);


Handler *GetHandler (const json_t *tags_p)
{
	HttpHandler *handler_p = (HttpHandler *) AllocMemory (sizeof (HttpHandler));

	if (handler_p)
		{
			InitialiseHandler (& (handler_p -> fh_base_handler),
				InitHttpHandler,
				IsResourceForHttpHandler,
				GetHttpHandlerProtocol,
				GetHttpHandlerName,
				GetHttpHandlerDescription,
				OpenHttpHandler,
				ReadFromHttpHandler,
				WriteToHttpHandler,
				SeekHttpHandler,
				CloseHttpHandler,
				GetHttpHandlerStatus,
				CalculateHttpInformationFromHttpHandler,
				FreeHttpHandler);

			handler_p -> fh_handler_f = NULL;
		}

	return ((Handler *) handler_p);
}


void ReleaseHandler (Handler *handler_p)
{
	FreeHttpHandler (handler_p);
}



static bool InitHttpHandler (struct Handler *handler_p, json_t *credentials_p)
{
	bool success_flag = true;
	
	return success_flag;
}



static bool OpenHttpHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s)
{
	HttpHandler *http_handler_p = (HttpHandler *) handler_p;

	if (http_handler_p -> fh_handler_f)
		{
			CloseHttpHandler (handler_p);
		}

	http_handler_p -> fh_handler_f = fopen (filename_s, mode_s);

	return (http_handler_p -> fh_handler_f != NULL);
}


static size_t ReadFromHttpHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	size_t res = 0;
	HttpHandler *http_handler_p = (HttpHandler *) handler_p;

	if (http_handler_p -> fh_handler_f)
		{
			res = fread (buffer_p, 1, length, http_handler_p -> fh_handler_f);
		}

	return res;
}


static size_t WriteToHttpHandler (struct Handler *handler_p, const void *buffer_p, const size_t length)
{
	size_t res = 0;
	HttpHandler *http_handler_p = (HttpHandler *) handler_p;

	if (http_handler_p -> fh_handler_f)
		{
			res = fwrite (buffer_p, 1, length, http_handler_p -> fh_handler_f);
		}

	return res;
}



static bool SeekHttpHandler (struct Handler *handler_p, long offset, int whence)
{
	bool success_flag = false;
	HttpHandler *http_handler_p = (HttpHandler *) handler_p;

	if (http_handler_p -> fh_handler_f)
		{
			success_flag = (fseek (http_handler_p -> fh_handler_f, offset, whence) == 0);
		}

	return success_flag;
}


static bool CloseHttpHandler (struct Handler *handler_p)
{
	bool success_flag = true;
	HttpHandler *http_handler_p = (HttpHandler *) handler_p;

	if (http_handler_p -> fh_handler_f)
		{
			success_flag = (fclose (http_handler_p -> fh_handler_f) == 0);
			http_handler_p -> fh_handler_f = NULL;
		}

	return success_flag;
}


static HandlerStatus GetHttpHandlerStatus (struct Handler *handler_p)
{
	HttpHandler *http_handler_p = (HttpHandler *) handler_p;
	HandlerStatus status = HS_GOOD;

	if (feof (http_handler_p -> fh_handler_f))
		{
			status = HS_FINISHED;
		}
	else if (ferror (http_handler_p -> fh_handler_f))
		{
			status = HS_BAD;
		}

	return status;
}


static bool IsResourceForHttpHandler (struct Handler *handler_p, const Resource * resource_p)
{
	bool match_flag = false;

	if (resource_p -> re_protocol_s)
		{
			match_flag = (strcmp (GetHttpHandlerProtocol (handler_p), resource_p -> re_protocol_s) == 0);
		}

	return match_flag;
}


static const char *GetHttpHandlerProtocol (struct Handler *handler_p)
{
	return "http";
}


static const char *GetHttpHandlerName (struct Handler *handler_p)
{
	return "Http Handler";
}


static const char *GetHttpHandlerDescription (struct Handler *handler_p)
{
	return "A Handler for http pages";
}


static bool CalculateHttpInformationFromHttpHandler (struct Handler *handler_p, HttpInformation *info_p)
{
	bool success_flag = false;

	if (handler_p -> ha_filename_s)
		{
			success_flag = CalculateHttpInformation (handler_p -> ha_filename_s, info_p);
		}

	return success_flag;
}


static void FreeHttpHandler (struct Handler *handler_p)
{
	HttpHandler *http_handler_p = (HttpHandler *) handler_p;

	CloseHttpHandler (handler_p);
	FreeMemory (http_handler_p);
}

