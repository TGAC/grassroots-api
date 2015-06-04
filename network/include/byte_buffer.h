#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <stddef.h>

#include "network_library.h"
#include "typedefs.h"

/**
 * A datatype to allow an automatically growing buffer for appending
 * data to.
 */
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

/**
 * Allocate a ByteBuffer.
 *
 * @param initial_size The initial size of the data buffer that this ByteBuffer will have.
 * @return The newly-allocated ByteBuffer or <code>NULL</code> on error.
 */
WHEATIS_NETWORK_API ByteBuffer *AllocateByteBuffer (size_t initial_size);


/**
 * Free a ByteBuffer.
 *
 * @param buffer_p The ByteBuffer to free.
 */
WHEATIS_NETWORK_API void FreeByteBuffer (ByteBuffer *buffer_p);


/**
 * Resize a ByteBuffer.
 * The content of the data buffer will be preserved.
 *
 * @param buffer_p The ByteBuffer to resize.
 * @param new_size The new size of the ByteBuffer's data.
 * @return <code>true</code> if the resize was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 */
WHEATIS_NETWORK_API bool ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size);


/**
 * Increase the size of a ByteBuffer's data buffer.
 * The content of the data buffer will be preserved.
 *
 * @param buffer_p The ByteBuffer to resize.
 * @param increment The amount to increase the ByteBuffer's data buffer.
 * @return <code>true</code> if the resize was successful <code>false</code>
 * upon failure. If the call failed, the contents of the data buffer are preserved.
 */
WHEATIS_NETWORK_API bool ExtendByteBuffer (ByteBuffer *buffer_p, size_t increment);


WHEATIS_NETWORK_API bool AppendToByteBuffer (ByteBuffer *buffer_p, const void *data_p, const size_t data_length);


WHEATIS_NETWORK_API bool AppendStringToByteBuffer (ByteBuffer *buffer_p, const char * const value_s);


WHEATIS_NETWORK_API bool AppendStringsToByteBuffer (ByteBuffer *buffer_p, const char *value_s, ...);


WHEATIS_NETWORK_API void ResetByteBuffer (ByteBuffer *buffer_p);


WHEATIS_NETWORK_API size_t GetRemainingSpaceInByteBuffer (const ByteBuffer * const buffer_p);


WHEATIS_NETWORK_API size_t GetByteBufferSize (const ByteBuffer * const buffer_p);


WHEATIS_NETWORK_API const char *GetByteBufferData (const ByteBuffer * const buffer_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BYTE_BUFFER_H */
