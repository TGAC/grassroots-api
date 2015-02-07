#ifndef RESOURCE_H
#define RESOURCE_H

#include "jansson.h"

#include "wheatis_service_library.h"
#include "typedefs.h"

typedef struct Resource
{
	char *re_protocol_s;

	char *re_value_s;

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
	#define PREFIX WHEATIS_SERVICE_API
	#define VAL(x)	= x
#else
	#define PREFIX extern
	#define VAL(x)
#endif

PREFIX const char *RESOURCE_DELIMITER_S VAL(":");

PREFIX const char *PROTOCOL_IRODS_S VAL("irods");
PREFIX const char *PROTOCOL_FILE_S VAL("file");


#ifdef __cplusplus
extern "C" {
#endif

WHEATIS_SERVICE_API Resource *AllocateResource (const char *protocol_s, const char *value_s);

WHEATIS_SERVICE_API void InitResource (Resource *resource_p);

WHEATIS_SERVICE_API void FreeResource (Resource *resource_p);

WHEATIS_SERVICE_API void ClearResource (Resource *resource_p);

WHEATIS_SERVICE_API bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s);

WHEATIS_SERVICE_API bool CopyResource (const Resource * const src_p, Resource * const dest_p);

WHEATIS_SERVICE_API Resource *ParseStringToResource (const char * const resource_s);


WHEATIS_SERVICE_API json_t *GetResourceAsJSON (const char * const protocol_s, const char * const path_s);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef RESOURCE_H */
