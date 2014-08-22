#include <stdio.h>

#include "query.h"
#include "connect.h"


static int PrintResults (FILE *out_f, const genQueryOut_t *query_result_p);


int main (int argc, char *argv [])
{
	int result = 0;
	const char **query_args_pp = (const char **) calloc (6, sizeof (const char *));
	
	if (query_args_pp)
		{
			char *full_query_s = NULL;
			
			*query_args_pp = "SELECT DATA_NAME, DATA_ID, COLL_NAME, COLL_ID where ";
			* (query_args_pp + 1) = "USER_NAME";
			* (query_args_pp + 2) = " = '";
			* (query_args_pp + 3) = "tyrrells";
			* (query_args_pp + 4) = "'";
			* (query_args_pp + 5) = NULL;
			
			full_query_s = BuildQuery (query_args_pp);
			
			if (full_query_s)
				{
					rcComm_t *connection_p = CreateConnection ();
					
					if (connection_p)
						{
							genQueryOut_t *query_result_p = ExecuteQuery (connection_p, full_query_s);
							
							if (query_result_p)
								{
									PrintResults (stdout, query_result_p);
								}		/* if (query_result_p) */
							
							CloseConnection (connection_p);
						}		/* if (connection_p) */
									
					FreeBuiltQuery (full_query_s);
				}		/* if (full_query_s) */
			
			free (query_args_pp);
		}		/* if (query_args_pp) */

	return result;
}


static int PrintResults (FILE *out_f, const genQueryOut_t *query_result_p)
{
	int i;
	
	for (i = 0;i < query_result_p -> rowCnt; ++ i) 
		{
			int j;
			const sqlResult_t *sql_result_p = query_result_p -> sqlResult;

			if (i > 0) 
				{
					fprintf (out_f, "----\n");
				}
				
			for (j = 0; j < query_result_p -> attriCnt; ++ j, ++ sql_result_p) 
				{
					char *result_s = NULL;
					result_s = sql_result_p -> value;

					if (fprintf (out_f, "%s\n", result_s) < 0)
						{
							return -1;
						}

					result_s += i * (sql_result_p -> len);
				}
		}
		
	return 0;
}

