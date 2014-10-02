/* Based upon zpipe.c taken from http://www.zlib.net/zlib_how.html */

/* zpipe.c: example of proper use of zlib's inflate() and deflate()
   Not copyrighted -- provided to the public domain
   Version 1.4  11 December 2005  Mark Adler */

/* Version history:
   1.0  30 Oct 2004  First version
   1.1   8 Nov 2004  Add void casting for unused return values
					 Use switch statement for inflate() return values
   1.2   9 Nov 2004  Add assertions to document zlib guarantees
   1.3   6 Apr 2005  Remove incorrect assertion in inf()
   1.4  11 Dec 2005  Add hack to avoid MSDOS end-of-line conversions
					 Avoid some compiler warnings for input and output buffers
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK_SIZE (16384)


#include "zip.h"


/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
//int def(FILE *source, FILE *dest, int level)

int CompressAsZip (Stream *in_stm_p, Stream *out_stm_p, int level)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char input_buffer [CHUNK_SIZE];
	unsigned char output_buffer [CHUNK_SIZE];

	/* Initialise the zip options */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit (&strm, level);

	if (ret == Z_OK)
		{
			bool loop_flag = true;
			
			while (loop_flag && (ret != Z_ERRNO))
				{
					/* Read in the next input chunk */
					strm.avail_in = ReadFromStream (in_stm_p, input_buffer, CHUNK_SIZE);

					switch (GetStreamStatus (in_stm_p))
						{
							case SS_BAD:
								deflateEnd (&strm);
								ret = Z_ERRNO;	
								loop_flag = false;			
								break;
								
							case SS_FINISHED:
								flush = Z_FINISH;
								break;
								
							case SS_GOOD:
								flush = Z_NO_FLUSH;
								break;							
						}

					if (loop_flag && (ret != Z_ERRNO))
						{
							bool write_loop_flag = true;
							
							strm.next_in = input_buffer;

							
							while (write_loop_flag && (ret != Z_ERRNO))
								{
									size_t i;
									
									strm.avail_out = CHUNK_SIZE;
									strm.next_out = output_buffer;

									ret = deflate (&strm, flush);	/* no bad return value */

									assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

									have = CHUNK_SIZE - strm.avail_out;
									
									i = WriteToStream (out_stm_p, output_buffer, have);
									
									if ((i != have) || (GetStreamStatus (out_stm_p) == SS_BAD))
										{
											ret = Z_ERRNO;
										}
									
									write_loop_flag = (strm.avail_out == 0);									
								}		/* while (write_loop_flag) */


							/* has all input been used? */
							if (strm.avail_in != 0)
								{
									ret = Z_ERRNO;
								}
					
							loop_flag = (flush != Z_FINISH);
						}		/* if (loop_flag) */
				}
			

			if (ret != Z_STREAM_END)
				{
					/* error */
				}

			/* clean up */
			deflateEnd (&strm);			
		}
	

	return ret;
}




/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
int inf(FILE *source, FILE *dest)
{
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK_SIZE];
	unsigned char out[CHUNK_SIZE];

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	/* decompress until deflate stream ends or end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK_SIZE, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = CHUNK_SIZE;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;	 /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
			have = CHUNK_SIZE - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)inflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
void zerr(int ret)
{
	fputs("zpipe: ", stderr);
	switch (ret) {
	case Z_ERRNO:
		if (ferror(stdin))
			fputs("error reading stdin\n", stderr);
		if (ferror(stdout))
			fputs("error writing stdout\n", stderr);
		break;
	case Z_STREAM_ERROR:
		fputs("invalid compression level\n", stderr);
		break;
	case Z_DATA_ERROR:
		fputs("invalid or incomplete deflate data\n", stderr);
		break;
	case Z_MEM_ERROR:
		fputs("out of memory\n", stderr);
		break;
	case Z_VERSION_ERROR:
		fputs("zlib version mismatch!\n", stderr);
	}
}

