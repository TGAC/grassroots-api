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
#ifndef WEB_SERVICE_LIBRARY_H
#define WEB_SERVICE_LIBRARY_H

#include "library.h"

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef WEB_SEARCH_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define WEB_SEARCH_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define WEB_SEARCH_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef WEB_SEARCH_LIBRARY_EXPORTS */
  #define WEB_SEARCH_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define WEB_SEARCH_SERVICE_API
  #define WEB_SEARCH_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */


#endif		/* #ifndef WEB_SERVICE_LIBRARY_H */
