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



typedef struct QueryResult
{
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


typedef struct QueryResults
{
	int qr_num_results;
	QueryResult *qr_values_p;
} QueryResults;




#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL_API void InitGenQuery (genQueryInp_t *query_p);


IRODS_UTIL_API void ClearGenQuery (genQueryInp_t *query_p);


IRODS_UTIL_API genQueryOut_t *ExecuteQueryString (rcComm_t *connection_p, char *query_s);


IRODS_UTIL_API genQueryOut_t *ExecuteGenQuery (rcComm_t *connection_p, genQueryInp_t * const query_p);


IRODS_UTIL_API char *BuildQueryString (const char **args_ss);


IRODS_UTIL_API void FreeBuiltQueryString (char *query_s);


IRODS_UTIL_API bool SetQuerySelectClauses (genQueryInp_t *in_query_p, int num_columns, const int * const columns_p, const int * const values_p);


IRODS_UTIL_API bool SetQueryWhereClauses (genQueryInp_t *in_query_p, int num_columns, const int *columns_p, const char **clauses_ss, const char **ops_ss);


IRODS_UTIL_API int PrintQueryOutput (FILE *out_f, const genQueryOut_t *query_result_p);


IRODS_UTIL_API void PrintQueryResults (FILE *out_f, const QueryResults * const result_p);


IRODS_UTIL_API void PrintQueryResult (FILE *out_f, const QueryResult * const result_p);


/** @private */
IRODS_UTIL_LOCAL QueryResults *GenerateQueryResults (const genQueryOut_t *results_p);


IRODS_UTIL_API void FreeQueryResults (QueryResults *result_p);


IRODS_UTIL_API const char *GetColumnNameForId (const int id);


/** @private */
IRODS_UTIL_LOCAL void ClearQueryResult (QueryResult *result_p);


/** @private */
IRODS_UTIL_LOCAL bool FillInQueryResult (QueryResult *query_result_p, const sqlResult_t *sql_result_p, const int num_rows);


//IRODS_UTIL_LOCAL const columnName_t *GetColumnById (const int id);



IRODS_UTIL_API json_t *GetQueryResultAsJSON (const QueryResults * const qrs_p);


IRODS_UTIL_API json_t *GetQueryResultAsResourcesJSON (const QueryResults * const qrs_p);


IRODS_UTIL_API QueryResults *GetAllMetadataDataAttributeNames (rcComm_t *connection_p);


IRODS_UTIL_API QueryResults *GetAllMetadataDataAttributeValues (rcComm_t *connection_p, const char * const names_s);


IRODS_UTIL_API QueryResults *GetAllMetadataCollectionAttributeNames (rcComm_t *connection_p);


IRODS_UTIL_API QueryResults *GetAllMetadataUserAttributeNames (rcComm_t *connection_p);


IRODS_UTIL_API QueryResults *GetAllMetadataAttributeNames (rcComm_t *connection_p, const int col_id);


IRODS_UTIL_API QueryResults *GetAllMetadataAttributeValues (rcComm_t *connection_p, const int key_id, const char * const key_s, const int value_id);


/** @} */


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef IRODS_QUERY_H */
