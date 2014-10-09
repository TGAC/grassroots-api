#include <sys/types.h>

#include "crc.h"
#include "typedefs.h"

/* Table of CRCs of all 8-bit messages. */
static uint32 s_crc_table [256];

/* Flag: has the table been computed? Initially false. */
static bool s_crc_table_computed = false;


static void MakeCRCTable (void);



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
uint32 UpdateCRC (uint32 crc, const uint8 *buffer_p, size_t len)
{
  uint32 c = crc ^ 0xffffffffL;

  if (!s_crc_table_computed)
		{
			MakeCRCTable ();
		}

  for ( ; len > 0; -- len, ++ buffer_p) 
		{
			c = s_crc_table [(c ^ (*buffer_p)) & 0xff] ^ (c >> 8);
		}
  
  return c ^ 0xffffffffL;
}

/* Return the CRC of the bytes buf[0..len-1]. */
uint32 GetCRC (const uint8 *buffer_p, size_t len)
{
  return UpdateCRC (0L, buffer_p, len);
}


/* Make the table for a fast CRC. */
static void MakeCRCTable (void)
{
  uint32 c;
  uint32 n, k;
	uint32 *table_p = s_crc_table;
	
  for (n = 0; n < 256; ++ n, ++ table_p) 
		{
			c = n;
			
			for (k = 0; k < 8; ++ k) 
				{
					if (c & 1) 
						{
							c = 0xedb88320L ^ (c >> 1);
						}	 
					else 
						{
							c = c >> 1;
						}	
				}

			*table_p = c;
		}

  s_crc_table_computed = true;
}
