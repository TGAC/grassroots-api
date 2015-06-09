/**
 * @file
 * @addtogroup iRODS
 * @brief Routines to query data objects and collections
 * @{
 */
#ifndef IRODS_QUERY_H
#define IRODS_QUERY_H


#include <jansson.h>

#include "rcConnect.h"
#include "rodsGenQuery.h"
//#include "rodsGenQueryNames.h"

#include "irods_util_library.h"
#include "linked_list.h"


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
 * Prepare a genQueryInp_t ready for use
 *
 * @param query_p The genQueryInp_t to initialise.
 */
IRODS_UTIL_API void InitGenQuery (genQueryInp_t *query_p);


/**
 * Clear a genQueryInp_t.
 *
 * @param query_p The genQueryInp_t to clear.
 */
IRODS_UTIL_API void ClearGenQuery (genQueryInp_t *query_p);


/**
 * Run a given SQL query string on an iRODS server.
 *
 * @param connection_p The connection to the iRODS server.
 * @param query_s The query string that will be used to fill in a genQueryInp_t.
 * @return Pointer to a newly-allocated genQueryOut_t containing the results of
 * the executed query.
 * @see ExecuteGenQuery
 */
IRODS_UTIL_API genQueryOut_t *ExecuteQueryString (rcComm_t *connection_p, char *query_s);


/**
 * Run a given SQL query string on an iRODS server.
 *
 * @param connection_p The connection to the iRODS server.
 * @param query_p The input query.
 * @return Pointer to a newly-allocated genQueryOut_t containing the results of
 * the executed query.
 */
IRODS_UTIL_API genQueryOut_t *ExecuteGenQuery (rcComm_t *connection_p, genQueryInp_t * const query_p);


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
 * Set the SELECT clauses for a query.
 *
 * @param in_query_p The query to be set.
 * @param num_columns The number of columns to set the SELECT for.
 * @param columns_p Pointer to an array of column ids.
 * @param values_p Pointer to an array of values.
 * @return <code>true</code> if the clauses were set successfully, <code>false</code> otherwise.
 */
IRODS_UTIL_API bool SetQuerySelectClauses (genQueryInp_t *in_query_p, int num_columns, const int * const columns_p, const int * const values_p);


/**
 * Set the WHERE clauses for a query.
 *
 * @param in_query_p The query to be set.
 * @param num_columns The number of columns to set the WHERE for.
 * @param columns_p Pointer to an array of column ids.
 * @param clauses_ss Pointer to an array of strings for the WHERE clause values.
 * @param opss_ss Pointer to an array of strings specifying the WHERE clause operators.
 * @return <code>true</code> if the clauses were set successfully, <code>false</code> otherwise.
 */
IRODS_UTIL_API bool SetQueryWhereClauses (genQueryInp_t *in_query_p, int num_columns, const int *columns_p, const char **clauses_ss, const char **ops_ss);


/**
 * Print a genQueryOut_t to an output FILE.
 *
 * @param out_f The FILE to write to.
 * @param query_result_p The genQueryOut_t to print.
 */
IRODS_UTIL_API int PrintQueryOutput (FILE *out_f, const genQueryOut_t *query_result_p);


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
 * @param query_result_p The QueryResult to print.
 * @memberof QueryResults
 */
IRODS_UTIL_API void PrintQueryResult (FILE *out_f, const QueryResult * const result_p);


/** @private */
IRODS_UTIL_LOCAL QueryResults *GenerateQueryResults (const genQueryOut_t *results_p);


/**
 * Free a QueryResults
 *
 * @param result_p The QueryResults to free.
 * @memberof QueryResults
 */
IRODS_UTIL_API void FreeQueryResults (QueryResults *result_p);


IRODS_UTIL_API const char *GetColumnNameForId (const int id);


/** @private */
IRODS_UTIL_LOCAL void ClearQueryResult (QueryResult *result_p);


/** @private */
IRODS_UTIL_LOCAL bool FillInQueryResult (QueryResult *query_result_p, const sqlResult_t *sql_result_p, const int num_rows);


/**
 * @brief Get the json representation of a QueryResults.

 * @param result_p The QueryResults to get the json for.
 * @return The json_t object representing the QueryResults
 * or <code>NULL</code> upon error.
 * @memberof QueryResults
 */
IRODS_UTIL_API json_t *GetQueryResultAsJSON (const QueryResults * const qrs_p);


/**
 * @brief Get the json representation of a QueryResults in the format of a Resource.

 * @param qrs_p The QueryResults to get the json for.
 * @return The json_t object representing the QueryResults
 * or <code>NULL</code> upon error.
 * @memberof QueryResults
 */
IRODS_UTIL_API json_t *GetQueryResultAsResourcesJSON (const QueryResults * const qrs_p);


/**
 * Get all of the data attribute names within the iCAT system of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @return A newly-allocated QueryResults listing all of the data attribute names
 * or <code>NULL</code> upon error.
 * @see GetAllMetadataAttributeNames
 */
IRODS_UTIL_API QueryResults *GetAllMetadataDataAttributeNames (rcComm_t *connection_p);


/**
 * Get all of the data attribute values for the given name within the iCAT system
 * of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @param name_s The metadata attribute name to get all of the values for.
 * @return A newly-allocated QueryResults listing all of the data attribute values
 * or <code>NULL</code> upon error.
 */
IRODS_UTIL_API QueryResults *GetAllMetadataDataAttributeValues (rcComm_t *connection_p, const char * const name_s);


/**
 * Get all of the collection attribute names within the iCAT system of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @return A newly-allocated QueryResults listing all of the collection attribute names
 * or <code>NULL</code> upon error.
 */
IRODS_UTIL_API QueryResults *GetAllMetadataCollectionAttributeNames (rcComm_t *connection_p);


/**
 * Get all of the user attribute names within the iCAT system of an iRODS server.
 *
 * @param connection_p The connection to an iRODS server.
 * @return A newly-allocated QueryResults listing all of the user attribute names
 * or <code>NULL</code> upon error.
 */
IRODS_UTIL_API QueryResults *GetAllMetadataUserAttributeNames (rcComm_t *connection_p);


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
IRODS_UTIL_API QueryResults *GetAllMetadataAttributeNames (rcComm_t *connection_p, const int col_id);


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
IRODS_UTIL_API QueryResults *GetAllMetadataAttributeValues (rcComm_t *connection_p, const int key_id, const char * const key_s, const int value_id);


/** @} */


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef IRODS_QUERY_H */
