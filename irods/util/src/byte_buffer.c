#include "byte_buffer.h"



bool AppendStringToBuffer (ByteBuffer *buffer_p, const char *data_s)
{
	BufferBlockListNode *node_p = (BufferBlockListNode *) (byte_buffer_p -> bb_blocks_p -> ll_head_p);
	BufferBlock *block_p = node_p -> bbln_buffer_p;
	
	size_t remaining_space = (block_p -> bb_size) - (block_p -> bb_used_size);
	size_t data_length = strlen (data_s);
	
	if (data_length > remaining_space)
		{
			
		}
	else
		{
			
		}	
}


char *GetStringFromByteBuffer (const ByteBuffer * const byte_buffer_p)
{
	char *value_s = NULL;
	
	if (byte_buffer_p != NULL)
		{
			/* initialise it to 1 for the terminating \0 */
			size_t total_size = 1;
			BufferBlockListNode *node_p = (BufferBlockListNode *) (byte_buffer_p -> bb_blocks_p -> ll_head_p);

			/*
			* Get the total length needed
			*/
			while (node_p)
				{
					total_size += node_p -> bbln_buffer_p -> bb_used_size;
					node_p = (BufferBlockListNode *) (node_p -> bbln_node.ln_next_p);
				}
			
			if (total_size > 1)
				{
					value_s = (char *) AllocMemory (total_size);
					
					if (value_s)
						{
							char *current_position_p = value_s;
							node_p = (BufferBlockListNode *) (byte_buffer_p -> bb_blocks_p -> ll_head_p);
							
							while (node_p)
								{
									strncpy (current_position_p, node_p -> bbln_buffer_p -> bb_data_p);
									current_position_p += node_p -> bbln_buffer_p -> bb_used_size;

									node_p = (BufferBlockListNode *) (node_p -> bbln_node.ln_next_p);
								}
	
							*current_position_p = '\0';
						}
				}
					
		}		/* if (byte_buffer_p != NULL) */
	 
	return value_s;
}


void FreeByteBuffer (ByteBuffer *buffer_p)
{
	if (buffer_p)
		{
			ClearLinkedList (&buffer_p -> bb_blocks);
			FreeMemory (buffer_p);
		}		/* if (buffer_p) */
		
}
