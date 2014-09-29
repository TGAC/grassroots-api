#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <stddef.h>

#include "typedefs.h"

typedef struct ByteBuffer 
{
	char *bb_data_p;
	size_t bb_size;
	size_t bb_current_index;
} ByteBuffer;

#ifdef __cplusplus
extern "C"
{
#endif

ByteBuffer *AllocateByteBuffer (size_t initial_size);


void FreeByteBuffer (ByteBuffer *buffer_p);


bool ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size);


bool AppendToByteBuffer (ByteBuffer *buffer_p, char *value_s);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BYTE_BUFFER_H */
