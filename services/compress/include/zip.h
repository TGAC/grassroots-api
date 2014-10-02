#ifndef ZIP1_H
#define ZIP1_H


#include "zlib.h"
#include "stream.h"


#include "compress_service_library.h"



#ifdef __cplusplus
extern "C"
{
#endif

COMPRESS_SERVICE_API int CompressAsZip (Stream *in_stm_p, Stream *out_stm_p, int level);

COMPRESS_SERVICE_API int CompressAsZip1 (Stream *in_stm_p, Stream *out_stm_p, int level);

#ifdef __cplusplus
}
#endif

#endif		/* #ifndef ZIP1_H */
