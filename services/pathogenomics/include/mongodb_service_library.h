#ifndef MONGODB_SERVICE_LIBRARY_H
#define MONGODB_SERVICE_LIBRARY_H

#include "library.h"

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef MONGODB_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define MONGODB_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define MONGODB_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef MONGODB_LIBRARY_EXPORTS */
  #define MONGODB_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define MONGODB_SERVICE_API
  #define MONGODB_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */


#endif		/* #ifndef MONGODB_SERVICE_LIBRARY_H */
