#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <sys/types.h>

#include "linked_list.h"

typedef struct 
{
	char *bb_data_p;
	size_t bb_size;
	size_t bb_used_size;
	size_t bb_current_index;
} BufferBlock;


typedef struct 
{
	ListNode bbln_node;
	
	BufferBlock *bbln_buffer_p;
	
} BufferBlockListNode;


typedef struct 
{
	LinkedList bb_blocks;
	size_t bb_default_block_size;
} ByteBuffer;



bool AppendStringToBuffer (ByteBuffer *buffer_p, const char *data_s);


char *GetStringFromByteBuffer (const ByteBuffer * const byte_buffer_p);





#endif		/* #ifndef BYTE_BUFFER_H */
