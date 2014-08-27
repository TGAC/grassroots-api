#include <stdio.h>

#include "query.h"
#include "connect.h"
#include "user.h"

/*** STATIC PROTOTYPES ***/

static void TestGetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s);


static void TestExecuteQueryString (rcComm_t *connection_p);


/*** METHODS ***/

int main (int argc, char *argv [])
{
	int result = 0;
	rcComm_t *connection_p = CreateConnection ();
	
	if (connection_p)
		{
			printf ("--- TestGetAllCollectionsForUsername ---\n");
			TestGetAllCollectionsForUsername (connection_p, "tyrrells");

			printf ("--- TestExecuteQueryString ---\n");
			TestExecuteQueryString (connection_p);
			
			CloseConnection (connection_p);
		}		/* if (connection_p) */

	return result;
}



/*** STATIC METHODS ***/

static void TestGetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s)
{
	GetAllCollectionsForUsername (connection_p, username_s);
}


static void TestExecuteQueryString (rcComm_t *connection_p)
{
	const char **query_args_pp = (const char **) calloc (6, sizeof (const char *));
	
	if (query_args_pp)
		{
			char *full_query_s = NULL;
			
			*query_args_pp = "SELECT DATA_NAME, DATA_ID, COLL_NAME, COLL_ID where ";
			* (query_args_pp + 1) = "USER_NAME";
			* (query_args_pp + 2) = " = '";
			* (query_args_pp + 3) = "tyrrells";
			* (query_args_pp + 4) = "' AND USER_ID = 10039";
			* (query_args_pp + 5) = NULL;
			
			full_query_s = BuildQueryString (query_args_pp);
			
			if (full_query_s)
				{
					genQueryOut_t *query_result_p = ExecuteQueryString (connection_p, full_query_s);
					
					if (query_result_p)
						{
							PrintQueryOutput (stdout, query_result_p);
						}		/* if (query_result_p) */
							
					FreeBuiltQueryString (full_query_s);
				}		/* if (full_query_s) */
			
			free (query_args_pp);
		}		/* if (query_args_pp) */	
}




