/*
** Copyright 2014-2016 The Earlham Institute
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

/**
 * @file
 * @brief
 */
/**
 * @file irods_util_library.h
 * @defgroup iRODS iRODS Utility Library
 * @brief A collection of methods to access parts of an
 * iRODS system.
 */

#ifndef IRODS_UTIL_LIBRARY_H
#define IRODS_UTIL_LIBRARY_H

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
  #ifdef  IRODS_UTIL_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define IRODS_UTIL_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define IRODS_UTIL_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef IRODS_UTIL_LIBRARY_EXPORTS */
  #define IRODS_UTIL_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define IRODS_UTIL_API
  #define IRODS_UTIL_LOCAL
#endif /* #ifdef SHARED_LIBRARY */

#ifdef __cplusplus
}
#endif


#endif 	/* #ifndef IRODS_UTIL_LIBRARY_H */


