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
#ifndef GZIP_H
#define GZIP_H


#include "zlib.h"
#include "handler.h"


#include "compress_service_library.h"


/*
 * 
 * The format is explained at
 * 
 * http://www.gzip.org/zlib/rfc-gzip.html
 * 
 */
#ifdef UNIX
	#define OS (3)
#elif defined WINDOWS	
	#define OS (0)
#elif defined AMIGA
	#define OS (1)
#elif defined APPLE
	#define OS (7)
#else
	/**
	 * The numeric value specifying the operating system
	 * that the Service is running on.
	 */
	#define OS (255)
#endif
	


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Compress some data using the gzip algorithm.
 *
 * @param in_p The Handler for the data to compress.
 * @param out_p The Handler where the compressed data will be written to.
 * @param level The level of compression to apply
 * @return 0 upon success, non-zero error code upon error.
 */
COMPRESS_SERVICE_API int CompressAsGZip (Handler *in_p, Handler *out_p, int level);


COMPRESS_SERVICE_LOCAL bool InitGzipCompressor (z_stream *strm_p, int level, gz_header *gzip_header_p, const char *filename_s, const time_t last_modified);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GZIP_H */
