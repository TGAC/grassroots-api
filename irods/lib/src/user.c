//#include "rodsGenQuery.h"

#include "user.h"
#include "query.h"


const char *GetUsernameForId (const int64 user_id)
{
	const char *username_s = NULL;
	
	return username_s;
}


bool FindIdForUsername (rcComm_t *connection_p, const char * const username_s, int64 *id_p)
{
	bool success_flag = false;
	char *query_s = NULL;
	
	//const char "SELECT USER_ID WHERE USER_NAME = '";
	
	
	genQueryOut_t *result_p = ExecuteQueryString (connection_p, query_s);
	
	if (result_p)
		{
			
		}
	
	return success_flag;
}


QueryResults *RunQuery (rcComm_t *connection_p, const int *select_column_ids_p, const int num_select_columns, const int *where_column_ids_p, const char **where_column_values_pp, const int num_where_columns)
{
	QueryResults *results_p = NULL;
	genQueryInp_t in_query;

	InitQuery (&in_query);	
	
	if (SetQuerySelectClauses (&in_query, num_select_columns, select_column_ids_p, NULL))
		{			
			if (SetQueryWhereClauses (&in_query, num_where_columns, where_column_ids_p, where_column_values_pp))
				{
					genQueryOut_t *output_p = ExecuteQuery (connection_p, &in_query);
					
					if (output_p)
						{
							results_p = GenerateQueryResults (output_p);
						}		/* if (output_p) */
					
				}		/* if (SetQueryWhereClauses (&in_query, 1, where_column_ids, where_columns_p, where_quote_clauses)) */	
			
		}		/* if (SetQuerySelectClauses (&in_query, 2, select_column_ids, NULL)) */
	
	ClearQuery (&in_query);
	
	return results_p;	
}




QueryResults *GetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s)
{
	const int select_column_ids [] = { COL_COLL_ID, COL_COLL_NAME };
	const int where_column_ids [] = { COL_USER_NAME };
	const char *where_columns_pp [] = { username_s };

	return RunQuery (connection_p, select_column_ids, 2, where_column_ids, where_columns_pp, 1);
}




QueryResults *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s)
{
	const int select_column_ids [] = { COL_D_DATA_ID, COL_COLL_ID, COL_DATA_NAME };
	const int where_column_ids [] = { COL_USER_NAME };
	const char *where_columns_pp [] = { username_s };

	return RunQuery (connection_p, select_column_ids, 3, where_column_ids, where_columns_pp, 1);
}



QueryResults *GetAllZoneNames (rcComm_t *connection_p)
{
	const int select_column_ids [] = { COL_ZONE_ID, COL_ZONE_NAME };

	return RunQuery (connection_p, select_column_ids, 2, NULL, NULL, 0);
}







