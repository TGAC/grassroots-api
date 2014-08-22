#ifndef IRODS_UTIL_USER_H
#define IRODS_UTIL_USER_H

#include "irods_util_library.h"
#include "rcConnect.h"

#include "linked_list.h"
#include "typedefs.h"

#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL_API const char *GetUsernameForId (const int64 user_id);


IRODS_UTIL_API bool FindIdForUsername (const char * const username_s);


IRODS_UTIL_API LinkedList *GetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API LinkedList *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API LinkedList *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API LinkedList *GetAllZoneNames (rcComm_t *connection_p);


IRODS_UTIL_API LinkedList *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);



#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_USER_H */
