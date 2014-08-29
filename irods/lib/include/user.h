#ifndef IRODS_UTIL_USER_H
#define IRODS_UTIL_USER_H

#include <time.h>

#include "irods_util_library.h"
#include "rcConnect.h"

#include "linked_list.h"
#include "typedefs.h"
#include "query.h"


#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL_API const char *GetUsernameForId (const int64 user_id);


IRODS_UTIL_API bool FindIdForUsername (const char * const username_s);


IRODS_UTIL_API QueryResults *GetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API QueryResults *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API QueryResults *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API QueryResults *GetAllZoneNames (rcComm_t *connection_p);


IRODS_UTIL_API QueryResults *GetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from);



#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_USER_H */
