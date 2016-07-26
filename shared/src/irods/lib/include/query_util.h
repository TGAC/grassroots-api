/*
** Copyright 2014-2015 The Genome Analysis Centre
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
/*
 * query_utils.h
 *
 *  Created on: 18 Feb 2016
 *      Author: tyrrells
 */

#ifndef IRODS_LIB_INCLUDE_QUERY_UTIL_H_
#define IRODS_LIB_INCLUDE_QUERY_UTIL_H_


#include "irods_util_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Prepare a genQueryInp_t ready for use
 *
 * @param query_p The genQueryInp_t to initialise.
 */
IRODS_UTIL_LOCAL void InitGenQuery (genQueryInp_t *query_p);


/**
 * Clear a genQueryInp_t.
 *
 * @param query_p The genQueryInp_t to clear.
 */
IRODS_UTIL_LOCAL void ClearGenQuery (genQueryInp_t *query_p);


/**
 * Run a given SQL query string on an iRODS server.
 *
 * @param connection_p The connection to the iRODS server.
 * @param query_s The query string that will be used to fill in a genQueryInp_t.
 * @return Pointer to a newly-allocated genQueryOut_t containing the results of
 * the executed query.
 * @see ExecuteGenQuery
 */
IRODS_UTIL_LOCAL genQueryOut_t *ExecuteQueryString (rcComm_t *connection_p, char *query_s);


/**
 * Run a given SQL query string on an iRODS server.
 *
 * @param connection_p The connection to the iRODS server.
 * @param query_p The input query.
 * @return Pointer to a newly-allocated genQueryOut_t containing the results of
 * the executed query.
 */
IRODS_UTIL_LOCAL genQueryOut_t *ExecuteGenQuery (rcComm_t *connection_p, genQueryInp_t * const query_p);



/**
 * Set the SELECT clauses for a query.
 *
 * @param in_query_p The query to be set.
 * @param num_columns The number of columns to set the SELECT for.
 * @param columns_p Pointer to an array of column ids.
 * @param values_p Pointer to an array of values.
 * @return <code>true</code> if the clauses were set successfully, <code>false</code> otherwise.
 */
IRODS_UTIL_LOCAL bool SetQuerySelectClauses (genQueryInp_t *in_query_p, int num_columns, const int * const columns_p, const int * const values_p);


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
IRODS_UTIL_LOCAL bool SetQueryWhereClauses (genQueryInp_t *in_query_p, int num_columns, const int *columns_p, const char **clauses_ss, const char **ops_ss);


/**
 * Print a genQueryOut_t to an output FILE.
 *
 * @param out_f The FILE to write to.
 * @param query_result_p The genQueryOut_t to print.
 */
IRODS_UTIL_LOCAL int PrintQueryOutput (FILE *out_f, const genQueryOut_t *query_result_p);


/** @private */
IRODS_UTIL_LOCAL QueryResults *GenerateQueryResults (const genQueryOut_t *results_p);



/** @private */
IRODS_UTIL_LOCAL void ClearQueryResult (QueryResult *result_p);


/** @private */
IRODS_UTIL_LOCAL bool FillInQueryResult (QueryResult *query_result_p, const sqlResult_t *sql_result_p, const int num_rows);



/** @} */


#ifdef __cplusplus
}
#endif

#endif /* IRODS_LIB_INCLUDE_QUERY_UTIL_H_ */
