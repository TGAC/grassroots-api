#ifndef COMPRESS_CRC_H
#define COMPRESS_CRC_H

#include "typedefs.h"
#include "compress_service_library.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*
   Update a running crc with the bytes buf[0..len-1] and return
 the updated crc. The crc should be initialized to zero. Pre- and
 post-conditioning (one's complement) is performed within this
 function so it shouldn't be done by the caller. Usage example:

   unsigned long crc = 0L;

   while (read_buffer(buffer, length) != EOF) {
     crc = update_crc(crc, buffer, length);
   }
   if (crc != original_crc) error();
*/
COMPRESS_SERVICE_LOCAL uint32 UpdateCRC (uint32 crc, const uint8 *buffer_p, size_t len);

/* Return the CRC of the bytes buf[0..len-1]. */
COMPRESS_SERVICE_LOCAL uint32 GetCRC (const uint8 *buffer_p, size_t len);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef COMPRESS_CRC_H */
