/**@file library.h
*/ 

#ifndef LIBRARY_H
#define LIBRARY_H

#ifdef __cplusplus
	extern "C" {
#endif

/* Generic helper definitions for shared library support */

#if defined _WIN32 || defined __CYGWIN__
	#define LIB_HELPER_SYMBOL_IMPORT __declspec(dllimport)
	#define LIB_HELPER_SYMBOL_EXPORT __declspec(dllexport)
	#define LIB_HELPER_SYMBOL_LOCAL
#elif __GNUC__ >= 4
	#define LIB_HELPER_SYMBOL_IMPORT __attribute__ ((visibility("default")))
	#define LIB_HELPER_SYMBOL_EXPORT __attribute__ ((visibility("default")))
	#define LIB_HELPER_SYMBOL_LOCAL  __attribute__ ((visibility("hidden")))
#else
	#define LIB_HELPER_SYMBOL_IMPORT
	#define LIB_HELPER_SYMBOL_EXPORT
	#define LIB_HELPER_SYMBOL_LOCAL
#endif

#ifdef __cplusplus
}
#endif

#endif	/* #ifndef LIBRARY_H */
