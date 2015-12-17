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
/**@file samtools_service.h
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



#define TAG_SAMTOOLS_FILENAME MAKE_TAG ('S', 'T', 'F', 'L')
#define TAG_SAMTOOLS_BLASTDB_FILENAME MAKE_TAG ('S', 'T', 'B', 'F')
#define TAG_SAMTOOLS_SCAFFOLD MAKE_TAG ('S', 'T', 'S', 'C')
#define TAG_SAMTOOLS_SCAFFOLD_LINE_BREAK MAKE_TAG ('S', 'T', 'L', 'B')

#ifdef __cplusplus
extern "C"
{
#endif


SAMTOOLS_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


SAMTOOLS_SERVICE_API void ReleaseServices (ServicesArray *services_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef SAMTOOLS_SERVICE_H */
