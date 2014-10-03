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


typedef struct IRodsHandle
{
	Handle irh_base_handle;
	rcComm_t *irh_connection_p;
	openedDataObjInp_t *irh_obj_p;
	HandleStatus irh_status;
} IRodsHandle;


#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL_API Handle *GetIRodsHandle (const char * const username_s, const char * const password_s);

IRODS_UTIL_API Handle *AllocateIRodsHandle (rcComm_t *connection_p);

IRODS_UTIL_API void FreeIRodsHandle (Handle *handle_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_STREAM_H */
