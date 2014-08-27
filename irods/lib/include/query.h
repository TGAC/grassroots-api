#ifndef IRODS_QUERY_H
#define IRODS_QUERY_H


#include "rcConnect.h"
#include "rodsGenQuery.h"

#include "irods_util_library.h"
#include "linked_list.h"




#ifdef __cplusplus
extern "C" 
{
#endif


IRODS_UTIL_API void InitQuery (genQueryInp_t *query_p);


IRODS_UTIL_API void ClearQuery (genQueryInp_t *query_p);


IRODS_UTIL_API genQueryOut_t *ExecuteQueryString (rcComm_t *connection_p, char *query_s);


IRODS_UTIL_API genQueryOut_t *ExecuteQuery (rcComm_t *connection_p, genQueryInp_t * const query_p);


IRODS_UTIL_API char *BuildQueryString (const char **args_ss);


IRODS_UTIL_API void FreeBuiltQueryString (char *query_s);


IRODS_UTIL_API bool SetQuerySelectClauses (genQueryInp_t *in_query_p, int num_columns, const int * const columns_p, const int * const values_p);


IRODS_UTIL_API bool SetQueryWhereClauses (genQueryInp_t *in_query_p, int num_columns, const int *columns_p, const char **clauses_ss, const bool *quote_clause_p);


IRODS_UTIL_API int PrintQueryOutput (FILE *out_f, const genQueryOut_t *query_result_p);


#ifdef __cplusplus
}
#endif

#endif		/* #ifndef IRODS_QUERY_H */
