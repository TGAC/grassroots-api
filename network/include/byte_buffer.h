#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <stddef.h>

#include "network_library.h"
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


WHEATIS_NETWORK_API ByteBuffer *AllocateByteBuffer (size_t initial_size);


WHEATIS_NETWORK_API void FreeByteBuffer (ByteBuffer *buffer_p);


WHEATIS_NETWORK_API bool ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size);


WHEATIS_NETWORK_API bool AppendToByteBuffer (ByteBuffer *buffer_p, char *value_s);


WHEATIS_NETWORK_API void ResetByteBuffer (ByteBuffer *buffer_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BYTE_BUFFER_H */
