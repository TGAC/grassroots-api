#ifndef WHEATIS_RESOURCE_H
#define WHEATIS_RESOURCE_H

#include "jansson.h"

#include "network_library.h"
#include "typedefs.h"

typedef struct Resource
{
	char *re_protocol_s;

	char *re_value_s;

	char *re_title_s;

	json_t *re_data_p;

	bool re_owns_data_flag;
} Resource;

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
 * ALLOCATE_RESOURCE_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * resource.c.
 */
#ifdef ALLOCATE_RESOURCE_TAGS
	#define RESOURCE_PREFIX WHEATIS_NETWORK_API
	#define RESOURCE_VAL(x)	= x
#else
	#define RESOURCE_PREFIX extern
	#define RESOURCE_VAL(x)
#endif

RESOURCE_PREFIX const char *RESOURCE_DELIMITER_S RESOURCE_VAL("://");

RESOURCE_PREFIX const char *PROTOCOL_IRODS_S RESOURCE_VAL("irods");
RESOURCE_PREFIX const char *PROTOCOL_FILE_S RESOURCE_VAL("file");
RESOURCE_PREFIX const char *PROTOCOL_HTTP_S RESOURCE_VAL("http");
RESOURCE_PREFIX const char *PROTOCOL_HTTPS_S RESOURCE_VAL("https");
RESOURCE_PREFIX const char *PROTOCOL_INLINE_S RESOURCE_VAL("inline");


#ifdef __cplusplus
extern "C" {
#endif

WHEATIS_NETWORK_API Resource *AllocateResource (const char *protocol_s, const char *value_s, const char *title_s);

WHEATIS_NETWORK_API void InitResource (Resource *resource_p);

WHEATIS_NETWORK_API void FreeResource (Resource *resource_p);

WHEATIS_NETWORK_API void ClearResource (Resource *resource_p);

WHEATIS_NETWORK_API bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s, const char *title_s);

WHEATIS_NETWORK_API bool SetResourceData (Resource *resource_p, json_t *data_p, const bool owns_data_flag);

WHEATIS_NETWORK_API bool CopyResource (const Resource * const src_p, Resource * const dest_p);

WHEATIS_NETWORK_API Resource *ParseStringToResource (const char * const resource_s);

WHEATIS_NETWORK_API bool GetResourceProtocolAndPath (const char * const resource_s, char ** const path_ss, char ** const protocol_ss);

WHEATIS_NETWORK_API json_t *GetResourceAsJSONByParts (const char * const protocol_s, const char * const path_s, const char * const title_s, json_t *data_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef WHEATIS_RESOURCE_H */
