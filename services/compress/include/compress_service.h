#ifndef COMPRESS_SERVICE_H
#define COMPRESS_SERVICE_H

#include "service.h"
#include "library.h"
#include "compress_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


COMPRESS_SERVICE_API Service *AllocateService (void);


COMPRESS_SERVICE_API void FreeService (Service *service_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef COMPRESS_SERVICE_H */
