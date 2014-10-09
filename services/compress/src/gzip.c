#include "gzip.h"
#include "crc.h"

static bool WriteGZipHeader (Handler *out_p);

static bool WriteGZipFooter (Handler *out_p, uint32 crc, uint32 original_file_size);


/*
 * 
 * The format is explained at
 * 
 * http://www.gzip.org/zlib/rfc-gzip.html
 * 
 */

int CompressAsGZip (Handler *in_p, Handler *out_p, int level)
{
	int res = Z_OK;
	uint32 crc  = 0;
	uint32 file_size = 0;
	
	
	return res;
}




static bool WriteGZipHeader (Handler *out_p)
{
	bool success_flag = false;
	/* ID1 | ID2 | CM | FLG */
	uint32 header = 0x1F8B0808;
	
	if (WriteToHandler (out_p, &header, sizeof (header)))
		{
			time_t file_last_modified = GetLastModifiedTimeFromHandler (out_p);
			
			if (WriteToHandler (out_p, &file_last_modified, sizeof (file_last_modified)))
				{
					uint16 i = OS;
					
					if (WriteToHandler (out_p, &i, sizeof (i)))
						{
							success_flag = true;
						}		/* if (WriteToHandler (out_p, &file_last_modified, sizeof (file_last_modified))) */
					
				}		/* if (WriteToHandler (out_p, &file_last_modified, sizeof (file_last_modified))) */
				
		}		/* if (WriteToHandler (out_p, &header, sizeof (header))) */
		
	return success_flag;
}



static bool WriteGZipFooter (Handler *out_p, uint32 crc, uint32 original_file_size)
{
	return ((WriteToHandler (out_p, &crc, sizeof (crc)) == sizeof (crc)) &&
		(WriteToHandler (out_p, &original_file_size, sizeof (original_file_size)) == sizeof (original_file_size)));
}
