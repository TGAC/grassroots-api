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
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "gzip.h"
#include "compress_service.h"



/*
 *
 * The format is explained at
 *
 * http://www.gzip.org/zlib/rfc-gzip.html
 *
 */


bool InitGzipCompressor (z_stream *strm_p, int level, gz_header *gzip_header_p, const char *filename_s, const time_t last_modified)
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


