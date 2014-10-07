#ifndef IRODS_STREAM_H
#define IRODS_STREAM_H

#include <time.h>

#include <jansson.h>


#include "rcConnect.h"
#include "dataObjInpOut.h"

#include "irods_handler_library.h"
#include "irods_resource.h"


#include "linked_list.h"
#include "typedefs.h"
#include "query.h"
#include "handler.h"
#include "tags.h"


typedef struct IRodsHandler
{
	Handler irh_base_handler;
	rcComm_t *irh_connection_p;
	openedDataObjInp_t *irh_obj_p;
	HandlerStatus irh_status;
} IRodsHandler;



#define TAG_IRODS_BASE 	(TAG_USER | 0x000000F0)

#define TAG_IRODS_USER  (TAG_IRODS_BASE | 0x00000001)
#define TAG_IRODS_PASSWORD  (TAG_IRODS_BASE | 0x00000002)
#define TAG_IRODS_CONNECTION  (TAG_IRODS_BASE | 0x00000003)



#ifdef __cplusplus
extern "C" 
{
#endif

IRODS_HANDLER_API Handler *GetHandler (json_t *tags_p);

IRODS_HANDLER_API void ReleaseHandler (Handler *handler_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_STREAM_H */
