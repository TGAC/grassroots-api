#ifndef RESOURCE_H
#define RESOURCE_H

#include "wheatis_service_library.h"
#include "typedefs.h"

typedef struct Resource
{
	char *re_protocol_s;
	
	char *re_value_s;
	
} Resource;



#ifdef __cplusplus
extern "C" {
#endif

WHEATIS_SERVICE_API Resource *AllocateResource (void);

WHEATIS_SERVICE_API void InitResource (Resource *resource_p);

WHEATIS_SERVICE_API void FreeResource (Resource *resource_p);

WHEATIS_SERVICE_API void ClearResource (Resource *resource_p);

WHEATIS_SERVICE_API bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s);

WHEATIS_SERVICE_API bool CopyResource (const Resource * const src_p, Resource * const dest_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef RESOURCE_H */
