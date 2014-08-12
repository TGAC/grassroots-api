#ifndef BLAST_SERVICE_H
#define BLAST_SERVICE_H

#include "service.h"

#ifdef __cplusplus
extern "C"
{
#endif

IRODS_LIB_API Service *AllocateService (void);

IRODS_LIB_API void FreeService (Service *service_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef BLAST_SERVICE_H */

