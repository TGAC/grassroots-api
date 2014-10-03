#include "file_handle.h"
#include "memory_allocations.h"

static bool OpenFileHandle (struct Handle *handle_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromFileHandle (struct Handle *handle_p, void *buffer_p, const size_t length);

static size_t WriteToFileHandle (struct Handle *handle_p, void *buffer_p, const size_t length);

static bool SeekFileHandle (struct Handle *handle_p, long offset, int whence);

static bool CloseFileHandle (struct Handle *handle_p);

static HandleStatus GetFileHandleStatus (struct Handle *handle_p);


Handle *AllocateFileHandle (void)
{
	FileHandle *handle_p = (FileHandle *) AllocMemory (sizeof (FileHandle));
	
	if (handle_p)
		{
			handle_p -> fh_base_handle.ha_open_fn = OpenFileHandle;
			handle_p -> fh_base_handle.ha_close_fn = CloseFileHandle;
			handle_p -> fh_base_handle.ha_read_fn = ReadFromFileHandle;
			handle_p -> fh_base_handle.ha_write_fn = WriteToFileHandle;
			handle_p -> fh_base_handle.ha_seek_fn = SeekFileHandle;
			handle_p -> fh_base_handle.ha_status_fn = GetFileHandleStatus;
			
			handle_p -> fh_handle_f = NULL;
		}
		
	return ((Handle *) handle_p);
}


void FreeFileHandle (Handle *handle_p)
{
	FileHandle *file_handle_p = (FileHandle *) handle_p;
	
	if (file_handle_p -> fh_handle_f)
		{
			CloseFileHandle (handle_p);
		}
		
	FreeMemory (handle_p);
}


static bool OpenFileHandle (struct Handle *handle_p, const char * const filename_s, const char * const mode_s)
{
	FileHandle *file_handle_p = (FileHandle *) handle_p;

	if (file_handle_p -> fh_handle_f)
		{
			CloseFileHandle (handle_p);
		}
	
	file_handle_p -> fh_handle_f = fopen (filename_s, mode_s);
	
	return (file_handle_p -> fh_handle_f != NULL);
}


static size_t ReadFromFileHandle (struct Handle *handle_p, void *buffer_p, const size_t length)
{
	size_t res = 0;
	FileHandle *file_handle_p = (FileHandle *) handle_p;
	
	if (file_handle_p -> fh_handle_f)
		{
			res = fread (buffer_p, 1, length, file_handle_p -> fh_handle_f);
		}

	return res;
}


static size_t WriteToFileHandle (struct Handle *handle_p, void *buffer_p, const size_t length)
{
	size_t res = 0;
	FileHandle *file_handle_p = (FileHandle *) handle_p;
	
	if (file_handle_p -> fh_handle_f)
		{
			res = fwrite (buffer_p, 1, length, file_handle_p -> fh_handle_f);
		}

	return res;
}



static bool SeekFileHandle (struct Handle *handle_p, long offset, int whence)
{
	bool success_flag = false;
	FileHandle *file_handle_p = (FileHandle *) handle_p;
	
	if (file_handle_p -> fh_handle_f)
		{
			success_flag = (fseek (file_handle_p -> fh_handle_f, offset, whence) == 0);
		}

	return success_flag;
}


static bool CloseFileHandle (struct Handle *handle_p)
{
	bool success_flag = true;
	FileHandle *file_handle_p = (FileHandle *) handle_p;
	
	if (file_handle_p -> fh_handle_f)
		{
			success_flag = (fclose (file_handle_p -> fh_handle_f) == 0);
			file_handle_p -> fh_handle_f = NULL;
		}

	return success_flag;
}


static HandleStatus GetFileHandleStatus (struct Handle *handle_p)
{
	FileHandle *file_handle_p = (FileHandle *) handle_p;
	HandleStatus status = HS_GOOD;
	
	if (feof (file_handle_p -> fh_handle_f))
		{
			status = HS_FINISHED;
		}
	else if (ferror (file_handle_p -> fh_handle_f))
		{
			status = HS_BAD;
		}
				
	return status;
}
