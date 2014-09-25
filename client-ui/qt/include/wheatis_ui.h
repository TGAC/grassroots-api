
#ifndef WHEATIS_QT_CLIENT_LIBRARY_H
#define WHEATIS_QT_CLIENT_LIBRARY_H

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
	#ifdef  WHEATIS_QT_CLIENT_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
		#define WHEATIS_CLIENT_API LIB_HELPER_SYMBOL_EXPORT
	#else
		#define WHEATIS_CLIENT_API LIB_HELPER_SYMBOL_IMPORT
	#endif /* #ifdef WHEATIS_QT_CLIENT_LIBRARY_EXPORTS */
	#define WHEATIS_CLIENT_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
	#define WHEATIS_CLIENT_API
	#define WHEATIS_CLIENT_LOCAL
#endif /* #ifdef SHARED_LIBRARY */

#ifdef __cplusplus
}
#endif


#endif 	/* #ifndef WHEATIS_QT_CLIENT_LIBRARY_H */

