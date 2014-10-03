#ifndef ZIP1_H
#define ZIP1_H


#include "zlib.h"
#include "handle.h"


#include "compress_service_library.h"



#ifdef __cplusplus
extern "C"
{
#endif

COMPRESS_SERVICE_API int CompressAsZip (Handle *in_p, Handle *out_p, int level);

COMPRESS_SERVICE_API int CompressAsZip1 (Handle *in_p, Handle *out_p, int level);

#ifdef __cplusplus
}
#endif

#endif		/* #ifndef ZIP1_H */
