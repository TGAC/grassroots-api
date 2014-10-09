#include "gzip.h"
#include "crc.h"


static bool WriteGZipHeader (Handler *out_p, const time_t file_last_modified);

static bool WriteGZipFooter (Handler *out_p, const uint32 crc, const uint32 original_file_size);


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
	FileInformation file_info;

	if (CalculateFileInformation (in_p, &file_info))
		{
			if (WriteGZipHeader (out_p, file_info.fi_last_modified)
				{
					uint32 crc = 0;
					bool success_flag = true;

					/*
						Loop whilst there is input data to compress and
						it is being compressed successfully
					*/


					if (success_flag)
						{
							success_flag = WriteGZipFooter (out_p, crc, file_info.fi_size);
						}

				}		/* if (WriteGZipHeader (out_p, file_info.fi_last_modified) */

		}		/* if (CalculateFileInformation (in_p, &file_info))*/

	return res;
}




static bool WriteGZipHeader (Handler *out_p, const time_t file_last_modified)
{
	bool success_flag = false;
	/* ID1 | ID2 | CM | FLG */
	uint32 header = 0x1F8B0808;

	if (WriteToHandler (out_p, &header, sizeof (header)))
		{
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



static bool WriteGZipFooter (Handler *out_p, const uint32 crc, const uint32 original_file_size)
{
	return ((WriteToHandler (out_p, &crc, sizeof (crc)) == sizeof (crc)) &&
		(WriteToHandler (out_p, &original_file_size, sizeof (original_file_size)) == sizeof (original_file_size)));
}
