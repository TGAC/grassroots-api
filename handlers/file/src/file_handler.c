#include "file_handler.h"
#include "memory_allocations.h"

static bool OpenFileHandler (struct Handler *handler_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromFileHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

static size_t WriteToFileHandler (struct Handler *handler_p, void *buffer_p, const size_t length);

static bool SeekFileHandler (struct Handler *handler_p, long offset, int whence);

static bool CloseFileHandler (struct Handler *handler_p);

static HandlerStatus GetFileHandlerStatus (struct Handler *handler_p);


Handler *AllocateFileHandler (void)
{
	FileHandler *handler_p = (FileHandler *) AllocMemory (sizeof (FileHandler));
	
	if (handler_p)
		{
			handler_p -> fh_base_handler.ha_open_fn = OpenFileHandler;
			handler_p -> fh_base_handler.ha_close_fn = CloseFileHandler;
			handler_p -> fh_base_handler.ha_read_fn = ReadFromFileHandler;
			handler_p -> fh_base_handler.ha_write_fn = WriteToFileHandler;
			handler_p -> fh_base_handler.ha_seek_fn = SeekFileHandler;
			handler_p -> fh_base_handler.ha_status_fn = GetFileHandlerStatus;
			
			handler_p -> fh_handler_f = NULL;
		}
		
	return ((Handler *) handler_p);
}


void FreeFileHandler (Handler *handler_p)
{
	FileHandler *file_handler_p = (FileHandler *) handler_p;
	
	if (file_handler_p -> fh_handler_f)
		{
			CloseFileHandler (handler_p);
		}
		
	FreeMemory (handler_p);
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
