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
 */

#ifndef SAMTOOLS_SERVICE_H
#define SAMTOOLS_SERVICE_H

#include "service.h"
#include "library.h"

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/




#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef SAMTOOLS_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define SAMTOOLS_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define SAMTOOLS_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef SAMTOOLS_LIBRARY_EXPORTS */
  #define SAMTOOLS_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define SAMTOOLS_SERVICE_API
  #define SAMTOOLS_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */



/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_JSON_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_JSON_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * json_util.c.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_SAMTOOLS_TAGS
	#define SAMTOOLS_PREFIX SAMTOOLS_SERVICE_API
	#define SAMTOOLS_VAL(x,y)	= { x, y}
#else
	#define SAMTOOLS_PREFIX extern
	#define SAMTOOLS_VAL(x,y)
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */


SAMTOOLS_PREFIX NamedParameterType SS_INDEX SAMTOOLS_VAL ("input_file", PT_STRING);


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Get the ServicesArray containing the SamTools Services.
 *
 * @param config_p The service configuration data.
 * @return The ServicesArray containing all of the SamTools Services or
 * <code>NULL</code> upon error.
 * @ingroup samtools_service
 */
SAMTOOLS_SERVICE_API ServicesArray *GetServices (UserDetails *user_p);


/**
 * Free the ServicesArray containing the SamTools Services.
 *
 * @param services_p The ServicesArray to free.
 * @ingroup samtools_service
 */
SAMTOOLS_SERVICE_API void ReleaseServices (ServicesArray *services_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef SAMTOOLS_SERVICE_H */
