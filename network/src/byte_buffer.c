#include <string.h>

#include "byte_buffer.h"
#include "memory_allocations.h"


ByteBuffer *AllocateByteBuffer (size_t initial_size)
{
	char *data_p = (char *) AllocMemory (initial_size);
	
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


bool ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size)
{
	bool success_flag = false;
	char *new_data_p = (char *) AllocMemory (new_size);
	
	if (new_data_p)
		{
			strcpy (new_data_p, buffer_p -> bb_data_p);
			FreeMemory (buffer_p -> bb_data_p);			
			buffer_p -> bb_data_p = new_data_p;
			
			success_flag = true;
		}
	
	return success_flag;
}


bool AppendToByteBuffer (ByteBuffer *buffer_p, char *value_s)
{
	const size_t space_remaining = (buffer_p -> bb_size) - (buffer_p -> bb_current_index);
	const size_t value_length = strlen (value_s);
	bool success_flag = true;
	
	if (space_remaining <= value_length)
		{
			success_flag = ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + value_length - space_remaining);
		}
		
	if (success_flag)
		{
			char *current_data_p = (buffer_p -> bb_data_p) + (buffer_p -> bb_current_index);
			
			strcpy (current_data_p, value_s);			
			buffer_p -> bb_current_index += value_length;
		}
		
	return success_flag;
}
