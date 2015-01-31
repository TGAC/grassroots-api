#include <string.h>
#include <stdarg.h>

#include "byte_buffer.h"
#include "memory_allocations.h"


ByteBuffer *AllocateByteBuffer (size_t initial_size)
{
	char *data_p = (char *) AllocMemoryArray (initial_size, sizeof (char));
	
	if (data_p)
		{
			ByteBuffer *buffer_p = (ByteBuffer *) AllocMemory (sizeof (ByteBuffer));
			
			if (buffer_p)
				{
					buffer_p -> bb_data_p = data_p;
					buffer_p -> bb_size  = initial_size;
					buffer_p -> bb_current_index = 0;
					
					return buffer_p;
				}
			
			FreeMemory (data_p);
		}
		
	return NULL;
}


void FreeByteBuffer (ByteBuffer *buffer_p)
{
	FreeMemory (buffer_p -> bb_data_p);
	FreeMemory (buffer_p);
}


bool ExtendByteBuffer (ByteBuffer *buffer_p, size_t increment)
{
	return ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + increment);
}


bool ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size)
{
	bool success_flag = false;
		
	char *new_data_p = (char *) AllocMemoryArray (new_size, sizeof (char));
	
	if (new_data_p)
		{
			if (new_size > buffer_p -> bb_current_index)
				{
					memcpy (new_data_p, buffer_p -> bb_data_p, buffer_p -> bb_current_index);
				}
				
			FreeMemory (buffer_p -> bb_data_p);			

			buffer_p -> bb_data_p = new_data_p;
			buffer_p -> bb_size = new_size;
			
			success_flag = true;
		}
	
	return success_flag;
}


bool AppendToByteBuffer (ByteBuffer *buffer_p, const void *data_p, const size_t data_length)
{
	const size_t space_remaining = GetRemainingSpaceInByteBuffer (buffer_p);
	bool success_flag = true;
	
	if (space_remaining <= data_length)
		{
			success_flag = ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + data_length - space_remaining);
		}
		
	if (success_flag)
		{
			char *current_data_p = (buffer_p -> bb_data_p) + (buffer_p -> bb_current_index);
			
			memcpy (current_data_p, data_p, data_length);			
			buffer_p -> bb_current_index += data_length;
		}
		
	return success_flag;
}


bool AppendStringsToByteBuffer (ByteBuffer *buffer_p, const char *value_s, ...)
{
	bool success_flag = true;
	bool loop_flag = true;
	va_list args;
	va_start (args, value_s);

	while (success_flag && loop_flag)
		{
			char *arg_s = va_arg (args, char *);

			if (arg_s)
				{
					success_flag = AppendToByteBuffer (buffer_p, arg_s, strlen (arg_s));
				}
			else
				{
					loop_flag = false;
				}
		}

	va_end (args);

	return success_flag;
}



void ResetByteBuffer (ByteBuffer *buffer_p)
{
	buffer_p -> bb_current_index = 0;
	memset (buffer_p -> bb_data_p, 0, buffer_p -> bb_size);
}


size_t GetRemainingSpaceInByteBuffer (const ByteBuffer * const buffer_p)
{
	return (buffer_p -> bb_size) - (buffer_p -> bb_current_index);
}


size_t GetByteBufferSize (const ByteBuffer * const buffer_p)
{
	return (buffer_p -> bb_current_index);
}


const char *GetByteBufferData (const ByteBuffer * const buffer_p)
{
	return buffer_p -> bb_data_p;
}

