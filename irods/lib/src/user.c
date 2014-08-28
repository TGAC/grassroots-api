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


QueryResults *GetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s)
{
	QueryResults *result_p = NULL;
	genQueryInp_t in_query;
	const int select_column_ids [] = { COL_COLL_ID, COL_COLL_NAME };

	InitQuery (&in_query);	
	
	if (SetQuerySelectClauses (&in_query, 2, select_column_ids, NULL))
		{
			const int where_column_ids [] = { COL_USER_NAME };
			const char *where_columns_p [] = { username_s };
			
			if (SetQueryWhereClauses (&in_query, 1, where_column_ids, where_columns_p))
				{
					genQueryOut_t *output_p = ExecuteQuery (connection_p, &in_query);
					
					if (output_p)
						{
							
							
							PrintQueryOutput (stdout, output_p);
						}		/* if (output_p) */
					
				}		/* if (SetQueryWhereClauses (&in_query, 1, where_column_ids, where_columns_p, where_quote_clauses)) */	
			
		}		/* if (SetQuerySelectClauses (&in_query, 2, select_column_ids, NULL)) */
	
	ClearQuery (&in_query);
	
	return result_p;
}




QueryResults *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s)
{
	QueryResults *result_p = NULL;
	
	
	return result_p;	
}



QueryResults *GetAllZoneNames (rcComm_t *connection_p)
{
	QueryResults *result_p = NULL;
	genQueryInp_t in_query;
	
	InitQuery (&in_query);
	
	
	return result_p;
}







