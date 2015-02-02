#ifndef COMPRESS_SERVICE_H
#define COMPRESS_SERVICE_H

#include "service.h"
#include "library.h"
#include "compress_service_library.h"
#include "zlib.h"

#ifdef __cplusplus
extern "C"
{
#endif


COMPRESS_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


COMPRESS_SERVICE_API void ReleaseServices (ServicesArray *service_p);


COMPRESS_SERVICE_LOCAL bool CompressData (z_stream *strm_p, Bytef **output_buffer_pp, size_t *output_buffer_size_p, const int flush);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef COMPRESS_SERVICE_H */
