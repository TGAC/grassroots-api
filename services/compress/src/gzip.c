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
	bool success_flag = true;
	FileInformation file_info;

	if (CalculateFileInformationFromHandler (in_p, &file_info))
		{
			if (WriteGZipHeader (out_p, file_info.fi_last_modified))
				{
					#define BUFFER_SIZE (4096)
					size_t output_buffer_size = BUFFER_SIZE;
					Bytef *output_buffer_p = (Bytef *) AllocMemory (output_buffer_size * sizeof (Bytef));
					uint32 crc = 0;
					
					if (output_buffer_p)
						{
							bool loop_flag = true;							
							Bytef input_buffer [BUFFER_SIZE];
							uLongf output_size;
																					
							/*
								Loop whilst there is input data to compress and
								it is being compressed successfully
							*/

							while (loop_flag && success_flag)
								{
									size_t num_read = ReadFromHandler (in_p, input_buffer, BUFFER_SIZE);
									int res;
									
									crc = crc32 (crc, input_buffer, num_read);
									
									/* Compress the data */
									res = compress (output_buffer_p, &output_size, input_buffer, BUFFER_SIZE);
									while ((res == Z_BUF_ERROR) && success_flag)
										{
											FreeMemory (output_buffer_p);
											output_buffer_size <<= 1;
											
											output_buffer_p = (Bytef *) AllocMemory (output_buffer_size * sizeof (Bytef));
											
											if (output_buffer_p)
												{
													res = compress (output_buffer_p, &output_size, input_buffer, BUFFER_SIZE);
												}
											else
												{
													success_flag = false;
												}
										}

									if (success_flag)
										{
											if (WriteToHandler (out_p, output_buffer_p, output_size) == output_size)
												{
													HandlerStatus hs = GetHandlerStatus (in_p);
													
													switch (hs)
														{
															case HS_FINISHED:
																loop_flag = false;
																break;
																
															case HS_BAD:
																success_flag = false;
																break;
															
															default:
																break;
														}
												}
											else
												{
													success_flag = false;
												}
										}

								}		/* while (loop_flag && success_flag) */							
							
							if (output_buffer_p)
								{
									FreeMemory (output_buffer_p);
								}
							
						}		/* if (output_buffer_p) */


					if (success_flag)
						{
							success_flag = WriteGZipFooter (out_p, crc, file_info.fi_size);
						}

				}		/* if (WriteGZipHeader (out_p, file_info.fi_last_modified) */
			else
				{
					success_flag = false;
				}
			
		}		/* if (CalculateFileInformation (in_p, &file_info))*/
	else
		{
			success_flag = false;
		}

	return (success_flag ? Z_OK : Z_STREAM_ERROR);
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
