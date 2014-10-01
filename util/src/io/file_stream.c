#include "file_stream.h"
#include "memory_allocations.h"

static bool OpenFileStream (struct Stream *stream_p, const char * const filename_s, const char * const mode_s);

static size_t ReadFromFileStream (struct Stream *stream_p, void *buffer_p, const size_t length);

static size_t WriteToFileStream (struct Stream *stream_p, void *buffer_p, const size_t length);

static bool SeekFileStream (struct Stream *stream_p, long offset, int whence);

static bool CloseFileStream (struct Stream *stream_p);

static StreamStatus GetFileStreamStatus (struct Stream *stream_p);


Stream *AllocateFileStream (void)
{
	FileStream *stream_p = (FileStream *) AllocMemory (sizeof (FileStream));
	
	if (stream_p)
		{
			stream_p -> fs_base_stream.st_open_fn = OpenFileStream;
			stream_p -> fs_base_stream.st_close_fn = CloseFileStream;
			stream_p -> fs_base_stream.st_read_fn = ReadFromFileStream;
			stream_p -> fs_base_stream.st_write_fn = WriteToFileStream;
			stream_p -> fs_base_stream.st_seek_fn = SeekFileStream;
			stream_p -> fs_base_stream.st_status_fn = GetFileStreamStatus;
			
			stream_p -> fs_stream_f = NULL;
		}
		
	return ((Stream *) stream_p);
}


void FreeFileStream (Stream *stream_p)
{
	FileStream *file_stream_p = (FileStream *) stream_p;
	
	if (file_stream_p -> fs_stream_f)
		{
			CloseFileStream (stream_p);
		}
		
	FreeMemory (stream_p);
}


static bool OpenFileStream (struct Stream *stream_p, const char * const filename_s, const char * const mode_s)
{
	FileStream *file_stream_p = (FileStream *) stream_p;

	if (file_stream_p -> fs_stream_f)
		{
			CloseFileStream (stream_p);
		}
	
	file_stream_p -> fs_stream_f = fopen (filename_s, mode_s);
	
	return (file_stream_p -> fs_stream_f != NULL);
}


static size_t ReadFromFileStream (struct Stream *stream_p, void *buffer_p, const size_t length)
{
	size_t res = 0;
	FileStream *file_stream_p = (FileStream *) stream_p;
	
	if (file_stream_p -> fs_stream_f)
		{
			res = fread (buffer_p, 1, length, file_stream_p -> fs_stream_f);
		}

	return res;
}


static size_t WriteToFileStream (struct Stream *stream_p, void *buffer_p, const size_t length)
{
	size_t res = 0;
	FileStream *file_stream_p = (FileStream *) stream_p;
	
	if (file_stream_p -> fs_stream_f)
		{
			res = fwrite (buffer_p, 1, length, file_stream_p -> fs_stream_f);
		}

	return res;
}



static bool SeekFileStream (struct Stream *stream_p, long offset, int whence)
{
	bool success_flag = false;
	FileStream *file_stream_p = (FileStream *) stream_p;
	
	if (file_stream_p -> fs_stream_f)
		{
			success_flag = (fseek (file_stream_p -> fs_stream_f, offset, whence) == 0);
		}

	return success_flag;
}


static bool CloseFileStream (struct Stream *stream_p)
{
	bool success_flag = true;
	FileStream *file_stream_p = (FileStream *) stream_p;
	
	if (file_stream_p -> fs_stream_f)
		{
			success_flag = (fclose (file_stream_p -> fs_stream_f) == 0);
			file_stream_p -> fs_stream_f = NULL;
		}

	return success_flag;
}


static StreamStatus GetFileStreamStatus (struct Stream *stream_p)
{
	FileStream *file_stream_p = (FileStream *) stream_p;
	StreamStatus status = SS_GOOD;
	
	if (feof (file_stream_p -> fs_stream_f))
		{
			status = SS_FINISHED;
		}
	else if (ferror (file_stream_p -> fs_stream_f))
		{
			status = SS_BAD;
		}
				
	return status;
}
