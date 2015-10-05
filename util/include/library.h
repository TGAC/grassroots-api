/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
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
