/**@file grassroots_util_library.h
*/ 

#ifndef GRASSROOTS_SERVICE_LIBRARY_H
#define GRASSROOTS_SERVICE_LIBRARY_H


#include "library.h"

#ifdef __cplusplus
	extern "C" {
#endif



/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef  GRASSROOTS_SERVICE_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define GRASSROOTS_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define GRASSROOTS_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef GRASSROOTS_SERVICE_MANAGER_LIBRARY_EXPORTS */
  #define GRASSROOTS_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define GRASSROOTS_SERVICE_API
  #define GRASSROOTS_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */

#ifdef __cplusplus
}
#endif


#endif 	/* #ifndef GRASSROOTS_SERVICE_LIBRARY_H */


