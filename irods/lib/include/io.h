#ifndef IRODS_UTIL_IO_H
#define IRODS_UTIL_IO_H

#include <time.h>

#include <jansson.h>


#include "rcConnect.h"
#include "dataObjInpOut.h"

#include "irods_util_library.h"

#include "linked_list.h"
#include "typedefs.h"
#include "query.h"
#include "stream.h"


#ifdef __cplusplus
extern "C" 
{
#endif



IRODS_UTIL_API Stream *GetIRodsStream (char * const username_s, char * const password_s);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_IO_H */
