#ifndef IRODS_STREAM_H
#define IRODS_STREAM_H

#include <time.h>

#include <jansson.h>


#include "rcConnect.h"
#include "dataObjInpOut.h"

#include "irods_util_library.h"

#include "linked_list.h"
#include "typedefs.h"
#include "query.h"
#include "handle.h"
#include "tags.h"


typedef struct IRodsHandle
{
	Handle irh_base_handle;
	rcComm_t *irh_connection_p;
	openedDataObjInp_t *irh_obj_p;
	HandleStatus irh_status;
} IRodsHandle;



#define TAG_IRODS_BASE 	(TAG_USER | 0x000000F0)

#define TAG_IRODS_USER  (TAG_IRODS_BASE | 0x00000001)
#define TAG_IRODS_PASSWORD  (TAG_IRODS_BASE | 0x00000002)
#define TAG_IRODS_CONNECTION  (TAG_IRODS_BASE | 0x00000003)



#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL IRodsResource *AllocateIRodsResource (void);

IRODS_UTIL void AllocateIRodsResource (IRodsResource *resource_p);


IRODS_UTIL_API Handle *GetIRodsHandle (const char * const username_s, const char * const password_s);

IRODS_UTIL_API Handle *AllocateIRodsHandle (rcComm_t *connection_p);

IRODS_UTIL_API void FreeIRodsHandle (Handle *handle_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_STREAM_H */
