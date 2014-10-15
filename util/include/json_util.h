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
PREFIX const char *SERVER_OPERATION_S VAL("operation");

PREFIX const char *SERVICES_NAME_S VAL("services");
PREFIX const char *SERVICE_NAME_S VAL("path");
PREFIX const char *SERVICE_DESCRIPTION_S VAL("description");
PREFIX const char *SERVICE_RUN_S VAL("run");

PREFIX const char *PARAM_SET_NAME_S VAL("param_set");
PREFIX const char *PARAM_SET_DESCRIPTION_S VAL("description");
PREFIX const char *PARAM_SET_PARAMS_S VAL("params");

PREFIX const char *PARAM_NAME_S VAL("param");
PREFIX const char *PARAM_CONCISE_DEFINITION_S VAL("concise");
PREFIX const char *PARAM_DESCRIPTION_S VAL("description");
PREFIX const char *PARAM_TAG_S VAL("tag");
PREFIX const char *PARAM_TYPE_S VAL("type");
PREFIX const char *PARAM_WHEATIS_TYPE_INFO_S VAL("wheatis_type");
PREFIX const char *PARAM_DEFAULT_VALUE_S  VAL("default_value");
PREFIX const char *PARAM_CURRENT_VALUE_S  VAL("current_value");
PREFIX const char *PARAM_OPTIONS_S  VAL("enum");
PREFIX const char *PARAM_MIN_S  VAL("min");
PREFIX const char *PARAM_MAX_S  VAL("max");

PREFIX const char *SHARED_TYPE_DESCRIPTION_S  VAL("description");
PREFIX const char *SHARED_TYPE_VALUE_S  VAL("value");

PREFIX const char *RESOURCE_PROTOCOL_S  VAL("protocol");
PREFIX const char *RESOURCE_VALUE_S  VAL("value");

PREFIX const char *TAG_INPUT_NAME_S VAL("input");
PREFIX const char *TAG_OUTPUT_NAME_S VAL("output");

PREFIX const char *CREDENTIALS_S VAL("credentials");
PREFIX const char *CREDENTIALS_USERNAME_S VAL("user");
PREFIX const char *CREDENTIALS_PASSWORD_S VAL("pass");
PREFIX const char *CREDENTIALS_TOKEN_S VAL("token");
PREFIX const char *CREDENTIALS_ENCRYPTION_METHOD_S VAL("encrypt_method");
PREFIX const char *CREDENTIALS_ENCRYPTION_KEY_S VAL("encrypt_key");



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
