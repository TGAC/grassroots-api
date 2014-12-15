/**@file blast_service.h
*/ 

#ifndef BLAST_SERVICE_H
#define BLAST_SERVICE_H

#include "service.h"
#include "library.h"

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef BLAST_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define BLAST_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define BLAST_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef BLAST_LIBRARY_EXPORTS */
  #define BLAST_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define BLAST_SERVICE_API
  #define BLAST_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */



#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_API ServicesArray *GetServices (void);


BLAST_SERVICE_API void ReleaseServices (ServicesArray *services_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef BLAST_SERVICE_H */
