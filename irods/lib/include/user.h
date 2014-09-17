#ifndef IRODS_UTIL_USER_H
#define IRODS_UTIL_USER_H

#include <time.h>

#include <jansson.h>

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


IRODS_UTIL_API QueryResults *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s, const char *user_id_s);


IRODS_UTIL_API QueryResults *GetAllZoneNames (rcComm_t *connection_p);


IRODS_UTIL_API QueryResults *GetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from, const time_t to);


IRODS_UTIL_LOCAL QueryResults *RunQuery (rcComm_t *connection_p, const int *select_column_ids_p, const int num_select_columns, const int *where_column_ids_p, const char **where_column_values_pp, const char **where_ops_pp, const int num_where_columns);



IRODS_UTIL_API json_t *GetModifiedIRodsFiles  (const char * const username_s, const char * const password_s, const time_t from, const time_t to);


IRODS_UTIL_API json_t *GetInterestedServicesForIrodsDataObject (const char *services_path_s, char * const username_s, char * const password_s, const char *data_name_s);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_USER_H */
