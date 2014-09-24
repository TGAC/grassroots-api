#ifndef JSON_UTIL_H
#define JSON_UTIL_H


#include <stdio.h>

#include "wheatis_util_library.h"

#include "jansson.h"


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
#ifdef ALLOCATE_JSON_TAGS
	#define PREFIX WHEATIS_UTIL_API
	#define VAL(x)	= x
#else
	#define PREFIX extern
	#define VAL(x)	
#endif



PREFIX const char *SERVER_OPERATIONS_S VAL("operations");

PREFIX const char *SERVICE_NAME_S VAL("path");
PREFIX const char *SERVICE_DESCRIPTION_S VAL("description");

PREFIX const char *PARAM_NAME_S VAL("name");
PREFIX const char *PARAM_DESCRIPTION_S VAL("description");
PREFIX const char *PARAM_TYPE_S VAL("type");
PREFIX const char *PARAM_WHEATIS_TYPE_INFO_S VAL("wheatis_type");
PREFIX const char *PARAM_DEFAULT_VALUE_S  VAL("default_value");


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_UTIL_API int PrintJSON (FILE *out_f, const json_t * const json_p, const char * const prefix_s);

WHEATIS_UTIL_API const char *GetJSONString (const json_t *json_p, const char * const key_s);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef JSON_UTIL_H */
