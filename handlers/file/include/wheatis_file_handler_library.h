/**@file wheatis_util_library.h
*/ 

#ifndef WHEATIS_FILE_HANDLER_LIBRARY_H
#define WHEATIS_FILE_HANDLER_LIBRARY_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "library.h"

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef  FILE_HANDLER_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define FILE_HANDLER_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define FILE_HANDLER_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef FILE_HANDLER_EXPORTS */
  #define FILE_HANDLER_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define FILE_HANDLER_API
  #define FILE_HANDLER_LOCAL
#endif /* #ifdef SHARED_LIBRARY */

#ifdef __cplusplus
}
#endif


#endif 	/* #ifndef WHEATIS_FILE_HANDLER_LIBRARY_H */

