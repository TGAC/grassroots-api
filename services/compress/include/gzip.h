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
	#define OS (255)
#endif
	


#ifdef __cplusplus
extern "C"
{
#endif

COMPRESS_SERVICE_API int CompressAsGZip (Handler *in_p, Handler *out_p, int level);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GZIP_H */
