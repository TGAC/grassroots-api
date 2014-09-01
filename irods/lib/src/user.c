//#include "rodsGenQuery.h"

#include "user.h"
#include "query.h"


#include "string_utils.h"



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


QueryResults *RunQuery (rcComm_t *connection_p, const int *select_column_ids_p, const int num_select_columns, const int *where_column_ids_p, const char **where_column_values_pp, const char **where_ops_pp, const int num_where_columns)
{
	QueryResults *results_p = NULL;
	genQueryInp_t in_query;

	InitQuery (&in_query);	
	
	if (SetQuerySelectClauses (&in_query, num_select_columns, select_column_ids_p, NULL))
		{			
			if (SetQueryWhereClauses (&in_query, num_where_columns, where_column_ids_p, where_column_values_pp, where_ops_pp))
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

	return RunQuery (connection_p, select_column_ids, 2, where_column_ids, where_columns_pp, NULL, 1);
}




QueryResults *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s, const char *user_id_s)
{
	const int select_column_ids [] = { COL_D_DATA_ID, COL_COLL_ID, COL_DATA_NAME };
	const int where_column_ids [] = { COL_USER_NAME, COL_USER_ID };
	const char *where_columns_pp [] = { username_s, user_id_s };

	return RunQuery (connection_p, select_column_ids, 3, where_column_ids, where_columns_pp, NULL, 2);
}



QueryResults *GetAllZoneNames (rcComm_t *connection_p)
{
	const int select_column_ids [] = { COL_ZONE_ID, COL_ZONE_NAME };

	return RunQuery (connection_p, select_column_ids, 2, NULL, NULL, NULL, 0);
}



QueryResults *GetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from, const time_t to)
{
	QueryResults *results_p = NULL;
	const int select_column_ids [] = { COL_D_DATA_ID, COL_COLL_ID, COL_DATA_NAME, COL_D_MODIFY_TIME };
	const int where_column_ids [] = { COL_USER_NAME, COL_D_MODIFY_TIME, COL_D_MODIFY_TIME };	
	char *from_s = ConvertIntegerToString ((int32) from);
	
	if (from_s)
		{
			char *to_s = ConvertIntegerToString ((int32) to);
			
			if (to_s)
				{
					const char *where_columns_pp [] = { username_s, from_s, to_s };
					const char *where_ops_pp [] = { " = \'", " >= \'", " <= \'" };
					
					results_p = RunQuery (connection_p, select_column_ids, 3, where_column_ids, where_columns_pp, where_ops_pp, 3);
					
					free (to_s);
				}
						
			free (from_s);
		}
	
	return results_p;
}




