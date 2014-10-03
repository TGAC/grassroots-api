#ifndef GZIP_H
#define GZIP_H


#include "zlib.h"
#include "handle.h"


#include "compress_service_library.h"



#ifdef __cplusplus
extern "C"
{
#endif

COMPRESS_SERVICE_API int CompressAsGZip (Handle *in_p, Handle *out_p, int level);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GZIP_H */
