#include "file_handler.h"
#include "memory_allocations.h"
#include "resource.h"


static bool OpenFileHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromFileHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

static size_t WriteToFileHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

static bool SeekFileHandler (struct Handler *handler_p, long offset, int whence);

static bool CloseFileHandler (struct Handler *handler_p);

static HandlerStatus GetFileHandlerStatus (struct Handler *handler_p);

static bool IsResourceForFileHandler (struct Handler *handler_p, const Resource * resource_p);

static const char *GetFileHandlerProtocol (struct Handler *handler_p);

static const char *GetFileHandlerName (struct Handler *handler_p);

static const char *GetFileHandlerDescription (struct Handler *handler_p);

static void FreeFileHandler (struct Handler *handler_p);

static bool IsResourceForFileHandler (struct Handler *handler_p, const Resource * resource_p);



Handler *GetHandler (void)
{
	FileHandler *handler_p = (FileHandler *) AllocMemory (sizeof (FileHandler));
	
	if (handler_p)
		{
			InitialiseHandler (& (handler_p -> fh_base_handler),
				IsResourceForFileHandler,
				GetFileHandlerProtocol,
				GetFileHandlerName,
				GetFileHandlerDescription,
				OpenFileHandler,
				ReadFromFileHandler,
				WriteToFileHandler,
				SeekFileHandler,
				CloseFileHandler,
				GetFileHandlerStatus,
				FreeFileHandler);

			handler_p -> fh_handler_f = NULL;
		}
		
	return ((Handler *) handler_p);
}


void ReleaseHandler (Handler *handler_p)
{
	FreeFileHandler (handler_p);
}


static bool OpenFileHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s)
{
	FileHandler *file_handler_p = (FileHandler *) handler_p;

	if (file_handler_p -> fh_handler_f)
		{
			CloseFileHandler (handler_p);
		}
	
	file_handler_p -> fh_handler_f = fopen (filename_s, mode_s);
	
	return (file_handler_p -> fh_handler_f != NULL);
}


static size_t ReadFromFileHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	size_t res = 0;
	FileHandler *file_handler_p = (FileHandler *) handler_p;
	
	if (file_handler_p -> fh_handler_f)
		{
			res = fread (buffer_p, 1, length, file_handler_p -> fh_handler_f);
		}

	return res;
}


static size_t WriteToFileHandler (struct Handler *handler_p, void *buffer_p, const size_t length)
{
	size_t res = 0;
	FileHandler *file_handler_p = (FileHandler *) handler_p;
	
	if (file_handler_p -> fh_handler_f)
		{
			res = fwrite (buffer_p, 1, length, file_handler_p -> fh_handler_f);
		}

	return res;
}



static bool SeekFileHandler (struct Handler *handler_p, long offset, int whence)
{
	bool success_flag = false;
	FileHandler *file_handler_p = (FileHandler *) handler_p;
	
	if (file_handler_p -> fh_handler_f)
		{
			success_flag = (fseek (file_handler_p -> fh_handler_f, offset, whence) == 0);
		}

	return success_flag;
}


static bool CloseFileHandler (struct Handler *handler_p)
{
	bool success_flag = true;
	FileHandler *file_handler_p = (FileHandler *) handler_p;
	
	if (file_handler_p -> fh_handler_f)
		{
			success_flag = (fclose (file_handler_p -> fh_handler_f) == 0);
			file_handler_p -> fh_handler_f = NULL;
		}

	return success_flag;
}


static HandlerStatus GetFileHandlerStatus (struct Handler *handler_p)
{
	FileHandler *file_handler_p = (FileHandler *) handler_p;
	HandlerStatus status = HS_GOOD;
	
	if (feof (file_handler_p -> fh_handler_f))
		{
			status = HS_FINISHED;
		}
	else if (ferror (file_handler_p -> fh_handler_f))
		{
			status = HS_BAD;
		}
				
	return status;
}


static bool IsResourceForFileHandler (struct Handler *handler_p, const Resource * resource_p)
{
	bool match_flag = false;
	
	match_flag = (resource_p -> re_protocol == FILE_LOCATION_LOCAL) || (resource_p -> re_protocol == FILE_LOCATION_REMOTE);
		
	return match_flag;
}


static const char *GetFileHandlerProtocol (struct Handler *handler_p)
{
	return "file";
}


static const char *GetFileHandlerName (struct Handler *handler_p)
{
	return "File Handler";
}


static const char *GetFileHandlerDescription (struct Handler *handler_p)
{
	return "A Handler for files mounted locally and remotely";
}


static void FreeFileHandler (struct Handler *handler_p)
{
	FileHandler *file_handler_p = (FileHandler *) handler_p;
	
	CloseFileHandler (handler_p);
	FreeMemory (file_handler_p);
}

