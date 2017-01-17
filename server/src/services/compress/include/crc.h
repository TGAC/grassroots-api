/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
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
