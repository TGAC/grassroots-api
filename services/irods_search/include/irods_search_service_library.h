#ifndef IRODS_SEARCH_SERVICE_LIBRARY_H
#define IRODS_SEARCH_SERVICE_LIBRARY_H

#include "library.h"
#include "service.h"

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef IRODS_SEARCH_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define IRODS_SEARCH_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define IRODS_SEARCH_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef IRODS_SEARCH_LIBRARY_EXPORTS */
  #define IRODS_SEARCH_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define IRODS_SEARCH_SERVICE_API
  #define IRODS_SEARCH_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */


#ifdef __cplusplus
extern "C"
{
#endif

IRODS_SEARCH_SERVICE_API ServicesArray *GetServices (const json_t *config_p);

IRODS_SEARCH_SERVICE_API void ReleaseServices (ServicesArray *services_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_SEARCH_SERVICE_LIBRARY_H */
