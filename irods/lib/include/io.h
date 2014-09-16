#ifndef IRODS_UTIL_IO_H
#define IRODS_UTIL_IO_H

#include <time.h>

#include <jansson.h>


#include "rcConnect.h"
#include "dataObjInpOut.h"

#include "irods_util_library.h"

#include "linked_list.h"
#include "typedefs.h"
#include "query.h"
#include "stream.h"

typedef struct IRodsStream
{
	Stream irs_base_stream;
	rcComm_t *connection_p;
	openedDataObjInp_t *irs_obj_p;
} IRodsStream;


#ifdef __cplusplus
extern "C" 
{
#endif



Stream *AllocateIRodsStream (void);

void FreeFileStream (Stream *stream_p);


/**
 * Open an IRODS data object.
 * 
 * @param connection_p The connection to the IRODS server.
 * @param filename_s The path of the data object to open.
 * @param resource_name_s The name of the resource that the file is in.
 * @param flags The flags to use when opening the file. These are the flags used when open is 
 * called e.g. O_RDONLY, O_APPEND, etc.
 * @return The handle for the data object is it was opened successfully or NULL upon failure.
 */
IRODS_UTIL_API openedDataObjInp_t *OpenIRodsDataObject (rcComm_t *connection_p, char * const filename_s, char * const resource_name_s, int flags);


/**
 * Close the handle for an IRODS data object.
 * 
 * @param connection_p The connection to the IRODS server.
 * @param data_obj_p The handle to close.
 * @param deallocate_flag If this is true, then the memory pointed to by data_obj_p will be 
 * freed.
 * @return O upon success, non-zero otherwise.
 */
IRODS_UTIL_API int CloseIRodsDataObject (rcComm_t *connection_p, openedDataObjInp_t *data_obj_p, bool deallocate_flag);


/**
 * Read from an IRODS data object.
 * 
 * @param connection_p The connection to the IRODS server.
 * @param data_obj_p The handle for the data object to read from.
 * @param buffer_p The buffer to store the read data into.
 * @param length_to_read The maximum number of bytes requested. buffer_p must be large enough to
 * store this.
 * @return Upon success, the number of bytes read. Less than zero indicates failure.
 */
IRODS_UTIL_API int ReadIRodsDataObject (rcComm_t *connection_p, openedDataObjInp_t *data_obj_p, bytesBuf_t *buffer_p, const int length_to_read);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_IO_H */
