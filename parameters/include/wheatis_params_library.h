/**
 * @file
 * @defgroup Parameters Parameters Library
 * @brief The dataypes and methods for dealing
 * with parameters.
 */
#ifndef WHEATIS_PARAMS_LIBRARY_H
#define WHEATIS_PARAMS_LIBRARY_H

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
  #ifdef  WHEATIS_PARAMS_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define WHEATIS_PARAMS_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define WHEATIS_PARAMS_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef WHEATIS_PARAMS_MANAGER_LIBRARY_EXPORTS */
  #define WHEATIS_PARAMS_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define WHEATIS_PARAMS_API
  #define WHEATIS_PARAMS_LOCAL
#endif /* #ifdef SHARED_LIBRARY */

#ifdef __cplusplus
}
#endif


#endif 	/* #ifndef WHEATIS_PARAMS_LIBRARY_H */


