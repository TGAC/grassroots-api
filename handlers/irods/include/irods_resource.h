#ifndef IRODS_RESOURCE_H
#define IRODS_RESOURCE_H

#include "rcConnect.h"
#include "resource.h"

#include "irods_handler_library.h"

typedef struct IRodsResource
{
	Resource irr_base_resource;
	char *irr_username_s;
	char *irr_password_s;
	rcComm_t *irr_connection_p;
} IRodsResource;



#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_HANDLER_API Resource *GetIRodsResource (const char * const username_s, const char * const password_s);

IRODS_HANDLER_API Resource *AllocateIRodsResource (rcComm_t *connection_p);

IRODS_HANDLER_API void FreeIRodsResource (Resource *resource_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_RESOURCE_H */
