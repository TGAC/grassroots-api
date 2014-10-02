#ifndef GZIP_H
#define GZIP_H


#include "zlib.h"
#include "stream.h"


#include "compress_service_library.h"



#ifdef __cplusplus
extern "C"
{
#endif

COMPRESS_SERVICE_API int CompressAsGZip (Stream *in_stm_p, Stream *out_stm_p, int level);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef GZIP_H */
