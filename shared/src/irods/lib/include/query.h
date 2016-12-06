/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
/**
 * @file
 * @addtogroup iRODS
 * @brief Routines to query data objects and collections
 * @{
 */
#ifndef IRODS_QUERY_H
#define IRODS_QUERY_H


#include <jansson.h>


#include "irods_util_library.h"
#include "linked_list.h"


/* forward declaration */
struct IRodsConnection;


/**
 * A datatype for accessing a single result of a search on an iRODS server.
 */
typedef struct QueryResult
{
	/** @privatesection */
	/*
	 * Due to the columnNames array being in the same file as the
	 * definition of columnName_t (rodsGenQueryNames.h) we get
	 * multiple definition errors if we try to set qr_column_p to be
	 * columnName_t, so we need to have it as void in the definition.
	 * If rodsGenQueryNames.h gets refactored or if the struct that
	 * columnName_t is a typedef of ceases to be anonymous, then we can
	 * change this.
	 */
	const void *qr_column_p;
	char **qr_values_pp;
	int qr_num_values;
} QueryResult;


/**
 * A datatype for holding an array of the results of a search on an iRODS server.
 */
typedef struct QueryResults
{
	/** The number of results in the array */
	int qr_num_results;

	/** The array of results */
	QueryResult *qr_values_p;
} QueryResults;


#ifdef __cplusplus
extern "C"
{
#endif



/**
 * Build a new query string to use to execute a query.
 *
 * @param args_ss The array of strings to build query string from terminated by a NULL.
 * @return A newly-allocated string containing the query or <code>NULL</code> upon error.
 */
IRODS_UTIL_API char *BuildQueryString (const char **args_ss);


/**
 * Free a previously generated query string.
 *
 * @param query_s The string to free.
 * @see BuildQueryString
 */
IRODS_UTIL_API void FreeBuiltQueryString (char *query_s);


/**
 * Print a QueryResults to an output FILE.
 *
 * @param out_f The FILE to write to.
 * @param result_p The QueryResults to print.
 * @memberof QueryResults
 */
IRODS_UTIL_API void PrintQueryResults (FILE *out_f, const QueryResults * const result_p);


/**
 * Print a QueryResult to an output FILE.
 *
 * @param out_f The FILE to write to.
 * @param result_p The QueryResult to print.
 * @memberof QueryResults
 */
IRODS_UTIL_API void PrintQueryResult (FILE *out_f, const QueryResult * const result_p);



/**
 * Free a QueryResults
 *
 * @param result_p The QueryResults to free.
 * @memberof QueryResults
 */
IRODS_UTIL_API void FreeQueryResults (QueryResults *result_p);


IRODS_UTIL_LOCAL const char *GetColumnNameForId (const int id);


/** @private */
IRODS_UTIL_LOCAL void ClearQueryResult (QueryResult *result_p);


/**
 * @brief Get the json representation of a QueryResults.

 * @param results_p The QueryResults to get the json for.
 * @return The json_t object representing the QueryResults
 * or <code>NULL</code> upon error.
 * @memberof QueryResults
 */
IRODS_UTIL_API json_t *GetQueryResultAsJSON (const QueryResults * const results_p);


/**
 * @brief Get the json representation of a QueryResults in the format of a Resource.

 * @param qrs_p The QueryResults to get the json for.
 * @return The json_t object representing the QueryResults
 * or <code>NULL</code> upon error.
 * @memberof QueryResults
 */
IRODS_UTIL_API json_t *GetQueryResultAsResourcesJSON (const QueryResults * const qrs_p);


/**
 * Get the full path to all collections and data objects in the given QueryResults.
 *
 * @param qrs_p The QueryResults to get the values from.
 * @return A LinkedList of StringListNodes where each node contains the full path
 * to each of the values in the given QueryResults.
 * @memberof QueryResults
 */
IRODS_UTIL_API LinkedList *GetQueryResultsPaths (const QueryResults * const qrs_p);


/**
 * Get all of the data attribute names within the iCAT system of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @return A newly-allocated QueryResults listing all of the data attribute names
 * or <code>NULL</code> upon error.
 * @see GetAllAttributeNames
 */
IRODS_UTIL_API QueryResults *GetAllMetadataDataAttributeNames (struct IRodsConnection *connection_p);



/**
 * Get all of the data attribute values for the given name within the iCAT system
 * of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @param name_s The metadata attribute name to get all of the values for.
 * @return A newly-allocated QueryResults listing all of the data attribute values
 * or <code>NULL</code> upon error.
 */
IRODS_UTIL_API QueryResults *GetAllMetadataDataAttributeValues (struct IRodsConnection *connection_p, const char * const name_s);


/**
 * Get all of the collection attribute names within the iCAT system of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @return A newly-allocated QueryResults listing all of the collection attribute names
 * or <code>NULL</code> upon error.
 */
IRODS_UTIL_API QueryResults *GetAllMetadataCollectionAttributeNames (struct IRodsConnection *connection_p);


/**
 * Get all of the user attribute names within the iCAT system of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @return A newly-allocated QueryResults listing all of the user attribute names
 * or <code>NULL</code> upon error.
 */
IRODS_UTIL_API QueryResults *GetAllMetadataUserAttributeNames (struct IRodsConnection *connection_p);


/**
 * Get all of the data attribute names within the iCAT system of an iRODS server for
 * give column id.
 *
 * @param connection_p The connection to an iRODS server.
 * @param col_id The id of the column to get all of the attribute names for.
 * @return A newly-allocated QueryResults listing all of the data attribute names
 * or <code>NULL</code> upon error.
 * @see GetAllMetadataDataAttributeNames
 */
IRODS_UTIL_LOCAL QueryResults *GetAllAttributeNames (struct IRodsConnection *connection_p, const int col_id);


/**
 * Get all of the data attribute names within the iCAT system of an iRODS server for
 * give column id.
 *
 * @param connection_p The connection to an iRODS server.
 * @param key_id The id of the column to use as the key for.
 * @param key_s The name of the key column.
 * @param value_id The id of the column to get values for.
 * @return A newly-allocated QueryResults listing all of the data attribute names
 * or <code>NULL</code> upon error.
 * @see GetAllMetadataDataAttributeValues
 */
IRODS_UTIL_API QueryResults *GetAllMetadataAttributeValues (struct IRodsConnection *connection_p, const int key_id, const char * const key_s, const int value_id);


/** @} */


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef IRODS_QUERY_H */
