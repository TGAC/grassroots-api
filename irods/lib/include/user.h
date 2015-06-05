/**
 * @file user.h
 * @addtogroup iRODS
 * @brief Routines to query user data objects and collections
 * @{
 */
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


/**
 * Get the username for an iRODS user id.
 *
 * @user_id The iRODS numeric user id.
 * @return The corresponding username or <code>NULL</code> upon error.
 * @memberof iRODS
 */
IRODS_UTIL_API const char *GetUsernameForId (const int64 user_id);

/**
 * Get the iRODS numeric user id for a given username.
 *
 * @param connection_p The connection to the iRODS server.
 * @param username_s The username to get the id for.
 * @param id_p Pointer to where the id will be stored if it can be found.
 * @return <code>true</code> if the id was found successfully, <code>false</code> otherwise.
 */
IRODS_UTIL_API bool FindIdForUsername (rcComm_t *connection_p, const char * const username_s, int64 *id_p);


/**
 * Get all collections for a given user.
 *
 * @param connection_p The connection to the iRODS server.
 * @param username_s The username to get the collections for.
 * @return The QueryResults for the matching collections. The results
 * have following columns: COL_COLL_ID & COL_COLL_NAME.
 */
IRODS_UTIL_API QueryResults *GetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s);


/**
 * Get all data objects for a given user.
 *
 * @param connection_p The connection to the iRODS server.
 * @param username_s The username to get the data objects for.
 * @param user_id_s The user id to get the data objects for.
 * @return The QueryResults for the matching data objects. The results
 * have following columns: COL_D_DATA_ID, COL_COLL_ID & COL_DATA_NAME .
 */
IRODS_UTIL_API QueryResults *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s, const char *user_id_s);


/**
 * Get all zones on an iRODS server.
 *
 * @param connection_p The connection to the iRODS server.
 * @return The QueryResults for the matcshing data objects. The results
 * have the following columns: COL_ZONE_ID & COL_ZONE_NAME.
 */
IRODS_UTIL_API QueryResults *GetAllZoneNames (rcComm_t *connection_p);


/**
 * Get all data objects for a given user modified within a given time interval
 *
 * @param connection_p The connection to the iRODS server.
 * @param username_s The username to get the data objects for.
 * @param from The start of the time interval.
 * @param to The end of the time interval.
 * @return The QueryResults for the matching data objects. The results
 * have following columns: COL_D_DATA_ID, COL_COLL_ID, COL_DATA_NAME
 * & COL_D_MODIFY_TIME.
 */
IRODS_UTIL_API QueryResults *GetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from, const time_t to);


/** @private */
IRODS_UTIL_LOCAL QueryResults *RunQuery (rcComm_t *connection_p, const int *select_column_ids_p, const int num_select_columns, const int *where_column_ids_p, const char **where_column_values_pp, const char **where_ops_pp, const int num_where_columns);


/**
 * Get all data objects for a given user modified within a given time interval.
 * This calls GetAllModifiedDataForUsername before constructing a json object
 * from these results.
 *
 * @param connection_p The connection to the iRODS server.
 * @param username_s The username to get the data objects for.
 * @param from The start of the time interval.
 * @param to The end of the time interval.
 * @return A json-based representation of the matching data objects.
 * @see GetAllModifiedDataForUsername
 */
IRODS_UTIL_API json_t *GetModifiedIRodsFiles  (const char * const username_s, const char * const password_s, const time_t from, const time_t to);


/**
 * Get all Services that can work on a given data object.
 *
 * @param services_path_s The directory containing the Service modules.
 * @param username_s The username to get the data objects for.
 * @param password_s The iRODS password. FIXME! This needs to change to being encrypted.
 * @param data_name_s The name of the data object.
 * @return A json-based representation of the matching Services.
 */
IRODS_UTIL_API json_t *GetInterestedServicesForIrodsDataObject (const char *services_path_s, char * const username_s, char * const password_s, const char *data_name_s);

/** @} */

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_USER_H */
