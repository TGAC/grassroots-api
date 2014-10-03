#ifndef RESOURCE_H
#define RESOURCE_H

#include "wheatis_service_library.h"
#include "typedefs.h"

typedef struct Resource
{
	FileLocation re_protocol;
	
	char *re_value_s;
	
} Resource;



#ifdef __cplusplus
extern "C" {
#endif

WHEATIS_SERVICE_API Resource *AllocateResource (void);

WHEATIS_SERVICE_API void InitResource (Resource *resource_p);

WHEATIS_SERVICE_API void FreeResource (Resource *resource_p);

WHEATIS_SERVICE_API void ClearResource (Resource *resource_p);

WHEATIS_SERVICE_API bool SetResourceValue (Resource *resource_p, const FileLocation fl, const char *value_s);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef RESOURCE_H */
