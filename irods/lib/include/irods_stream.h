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
#include "stream.h"


typedef struct IRodsStream
{
	Stream irs_base_stream;
	rcComm_t *irs_connection_p;
	openedDataObjInp_t *irs_obj_p;
} IRodsStream;


#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL_API Stream *GetIRodsStream (const char * const username_s, const char * const password_s);

IRODS_UTIL_API Stream *AllocateIRodsStream (rcComm_t *connection_p);

IRODS_UTIL_API void FreeIRodsStream (Stream *stream_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_STREAM_H */
