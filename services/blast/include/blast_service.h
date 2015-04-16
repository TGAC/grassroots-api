/**@file blast_service.h
*/ 

#ifndef BLAST_SERVICE_H
#define BLAST_SERVICE_H

#include "service.h"
#include "library.h"

/*
** Now we use the generic helper definitions above to define LIB_API and LIB_LOCAL.
** LIB_API is used for the public API symbols. It either DLL imports or DLL exports
** (or does nothing for static build)
** LIB_LOCAL is used for non-api symbols.
*/

#ifdef SHARED_LIBRARY /* defined if LIB is compiled as a DLL */
  #ifdef BLAST_LIBRARY_EXPORTS /* defined if we are building the LIB DLL (instead of using it) */
    #define BLAST_SERVICE_API LIB_HELPER_SYMBOL_EXPORT
  #else
    #define BLAST_SERVICE_API LIB_HELPER_SYMBOL_IMPORT
  #endif /* #ifdef BLAST_LIBRARY_EXPORTS */
  #define BLAST_SERVICE_LOCAL LIB_HELPER_SYMBOL_LOCAL
#else /* SHARED_LIBRARY is not defined: this means LIB is a static lib. */
  #define BLAST_SERVICE_API
  #define BLAST_SERVICE_LOCAL
#endif /* #ifdef SHARED_LIBRARY */



#define TAG_BLAST_INPUT_QUERY MAKE_TAG ('B', 'Q', 'U', 'Y')
#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'I', 'N', 'F')
#define TAG_BLAST_OUTPUT_FILE MAKE_TAG ('B', 'O', 'U', 'F')
#define TAG_BLAST_MAX_SEQUENCES MAKE_TAG ('B', 'M', 'S', 'Q')
#define TAG_BLAST_SHORT_QUERIES MAKE_TAG ('B', 'S', 'H', 'Q')
#define TAG_BLAST_EXPECT_THRESHOLD MAKE_TAG ('B', 'E', 'X', 'T')
#define TAG_BLAST_WORD_SIZE MAKE_TAG ('B', 'W', 'D', 'S')
#define TAG_BLAST_MAX_RANGE_MATCHES MAKE_TAG ('B', 'M', 'R', 'G')
#define TAG_BLAST_SUBRANGE_FROM MAKE_TAG ('B', 'Q', 'F', 'R')
#define TAG_BLAST_SUBRANGE_TO MAKE_TAG ('B', 'Q', 'T', 'O')
#define TAG_BLAST_MATCH_SCORE MAKE_TAG ('B', 'M', 'T', 'C')
#define TAG_BLAST_MISMATCH_SCORE MAKE_TAG ('B', 'M', 'S', 'M')

#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


BLAST_SERVICE_API void ReleaseServices (ServicesArray *services_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef BLAST_SERVICE_H */
