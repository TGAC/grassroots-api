#ifndef ENSEMBL_REST_SERVICE_H
#define ENSEMBL_REST_SERVICE_H

#include "service.h"
#include "library.h"
#include "ensembl_rest_service_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


ENSEMBL_REST_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


ENSEMBL_REST_SERVICE_API void ReleaseServices (ServicesArray *service_p);


ENSEMBL_REST_SERVICE_LOCAL const char * const ENSEMBL_REST_URI_S = "http://rest.ensemblgenomes.org/sequence/";


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef ENSEMBL_REST_SERVICE_H */
