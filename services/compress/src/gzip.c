#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "gzip.h"
#include "crc.h"


static bool WriteGZipHeader (Handler *out_p, const char * const filename_s, const time_t file_last_modified);

static bool WriteGZipFooter (Handler *out_p, uint32 crc, uint32 original_file_size);

static bool GzipData (z_stream *strm_p, Bytef **output_buffer_pp, size_t *output_buffer_size_p, const int flush);

static bool InitGzipCompressor (z_stream *strm_p, int level, gz_header *gzip_header_p, const char *filename_s, const time_t last_modified);



/*
 *
 * The format is explained at
 *
 * http://www.gzip.org/zlib/rfc-gzip.html
 *
 */


static bool InitGzipCompressor (z_stream *strm_p, int level, gz_header *gzip_header_p, const char *filename_s, const time_t last_modified)
{
	int i;
	
	#define WINDOW_BITS 15
	#define GZIP_ENCODING 16
	
	strm_p -> zalloc = Z_NULL;
	strm_p -> zfree  = Z_NULL;
	strm_p -> opaque = Z_NULL; 
	 
	i = deflateInit2 (strm_p, level, Z_DEFLATED, WINDOW_BITS | GZIP_ENCODING, Z_DEFLATED, Z_DEFAULT_STRATEGY);
	
	if (i == Z_OK)
		{	
			memset (gzip_header_p, 0, sizeof (gz_header));

			gzip_header_p -> os = OS;

			if (filename_s)
				{
					gzip_header_p -> name = (Bytef *) filename_s;
					gzip_header_p -> name_max = strlen (filename_s) + 1;
				}
				
			gzip_header_p -> time = last_modified;

			i = deflateSetHeader (strm_p, gzip_header_p);
		}
 				
	return (i == Z_OK);
}


static bool GzipData (z_stream *strm_p, Bytef **output_buffer_pp, size_t *output_buffer_size_p, const int flush)
{
	int res;
	bool success_flag = true;
	Bytef *output_buffer_p = *output_buffer_pp;
	size_t output_buffer_size = *output_buffer_size_p;
		
	res = deflate (strm_p, flush);

	while ((res == Z_BUF_ERROR) && success_flag)
		{
			FreeMemory (output_buffer_p);
			
			output_buffer_size <<= 1;
			output_buffer_p = (Bytef *) AllocMemoryArray (output_buffer_size, sizeof (Bytef));
			
			if (output_buffer_p)
				{
					*output_buffer_pp = output_buffer_p;
					*output_buffer_size_p = output_buffer_size;
					
					strm_p -> next_out = output_buffer_p;
					strm_p -> avail_out = output_buffer_size;
					
					res = deflate (strm_p, flush);					
				}
			else
				{
					success_flag = false;
				}
		}
		
	return (success_flag && (res != Z_STREAM_ERROR));
}


int CompressAsGZip (Handler *in_p, Handler *out_p, int level)
{
	bool success_flag = true;
	FileInformation file_info;

	if (CalculateFileInformationFromHandler (in_p, &file_info))
		{
			#define BUFFER_SIZE (4096)
			size_t output_buffer_size = BUFFER_SIZE;
			Bytef *output_buffer_p = (Bytef *) AllocMemoryArray (output_buffer_size, sizeof (Bytef));
			uint32 crc = 0;
			
			if (output_buffer_p)
				{
					bool loop_flag = true;							
					Bytef input_buffer [BUFFER_SIZE];
					z_stream strm;
					gz_header header;
												
					success_flag = InitGzipCompressor (&strm, level, &header, in_p -> ha_filename_s, file_info.fi_last_modified);
																		
					/*
						Loop whilst there is input data to compress and
						it is being compressed successfully
					*/
					while (loop_flag && success_flag)
						{
							size_t num_read = ReadFromHandler (in_p, input_buffer, BUFFER_SIZE - 1);
							int flush = Z_NO_FLUSH;
							HandlerStatus hs = GetHandlerStatus (in_p);
							
							switch (hs)
								{
									case HS_FINISHED:
										loop_flag = (strm.avail_out != 0);
										flush = Z_FINISH;
										break;
										
									case HS_BAD:
										success_flag = false;
										break;
									
									default:
										break;
								}									
							
							if (success_flag)
								{
									crc = crc32 (crc, input_buffer, num_read);
									
									* (input_buffer + num_read) = '\0';
									
									strm.next_in = input_buffer;
									strm.avail_in = num_read;

									/* Compress the data */
									do 
										{
											strm.next_out = output_buffer_p;
											strm.avail_out = output_buffer_size;

											/* Compress the data from the input_buffer into the output buffer */
											if (GzipData (&strm, &output_buffer_p, &output_buffer_size, flush))
												{
													const size_t output_size = output_buffer_size - strm.avail_out;
													
												#if GZIP_DEBUG >= DL_FINER
													{
														size_t j = 0;
														const Bytef *ptr = (const Bytef *) output_buffer_p;

														fprintf (stdout, "- BEGIN POST GZIP -------\n");
														for (j = 0; j < output_size; ++ j, ++ ptr)
															{
																int k = *ptr;
																fprintf (stdout, "j %lu = \'%x\'\n", j, k);
															}
														fprintf (stdout, "- END POST GZIP -------\n\n");
														fflush (stdout);
													}
												#endif															
													
													success_flag = (WriteToHandler (out_p, output_buffer_p, output_size) == output_size);
												}		/* if (output_size > 0) */
											else
												{
													success_flag = false;
												}
										}
									while (strm.avail_out == 0);	
									
								}		/* if (success_flag) */
							

						}		/* while (loop_flag && success_flag) */							

					
					if (output_buffer_p)
						{
							FreeMemory (output_buffer_p);
						}

					success_flag = (deflateEnd (&strm) == Z_OK);
					
				}		/* if (output_buffer_p) */

		}		/* if (CalculateFileInformation (in_p, &file_info))*/
	else
		{
			success_flag = false;
		}

	return (success_flag ? Z_OK : Z_STREAM_ERROR);
}




static bool WriteGZipHeader (Handler *out_p, const char * const filename_s, const time_t file_last_modified)
{
	bool success_flag = false;
	/* ID1 | ID2 | CM | FLG */
	uint32 header = htonl (0x1F8B0808);

	if (WriteToHandler (out_p, &header, sizeof (header)))
		{
			uint32 t = htonl ((uint32) file_last_modified);
			
			if (WriteToHandler (out_p, &t, sizeof (t)))
				{
					uint16 i = htons (OS);

					if (WriteToHandler (out_p, &i, sizeof (i)))
						{
							if (WriteToHandler (out_p, filename_s, strlen (filename_s)))
								{
									uint8 null_char = '\0';
								
									if (WriteToHandler (out_p, &null_char, 1))
										{
											success_flag = true;
										}
								}
						}		/* if (WriteToHandler (out_p, &file_last_modified, sizeof (file_last_modified))) */

				}		/* if (WriteToHandler (out_p, &file_last_modified, sizeof (file_last_modified))) */

		}		/* if (WriteToHandler (out_p, &header, sizeof (header))) */

	return success_flag;
}



static bool WriteGZipFooter (Handler *out_p, uint32 crc, uint32 original_file_size)
{
	return true;
/*
	return ((WriteToHandler (out_p, &crc, sizeof (crc)) == sizeof (crc)) &&
		(WriteToHandler (out_p, &original_file_size, sizeof (original_file_size)) == sizeof (original_file_size)));
*/
}
