#ifndef JSON_UTIL_H
#define JSON_UTIL_H


#include <stdio.h>

#include "irods_library.h"

#include "jansson.h"

#ifdef __cplusplus
extern "C"
{
#endif

IRODS_LIB_API int PrintJSON (FILE *out_f, const json_t * const json_p, const char * const prefix_s);

#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_UTIL_H */
