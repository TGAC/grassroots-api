#ifndef COMPRESS_SERVICE_LIBRARY_H
#define COMPRESS_SERVICE_LIBRARY_H

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef COMPRESS_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define COMPRESS_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define COMPRESS_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef COMPRESS_LIBRARY_EXPORTS */
  #define COMPRESS_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define COMPRESS_SERVICE_API
  #define COMPRESS_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */


#endif		/* #ifndef COMPRESS_SERVICE_LIBRARY_H */
