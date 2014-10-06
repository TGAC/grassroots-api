#ifndef GZIP_H
#define GZIP_H


#include "zlib.h"
#include "handler.h"


#include "compress_service_library.h"



#ifdef __cplusplus
extern "C"
{
#endif

COMPRESS_SERVICE_API int CompressAsGZip (Handler *in_p, Handler *out_p, int level);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GZIP_H */
