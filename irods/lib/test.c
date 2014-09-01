#include <stdio.h>

#include "query.h"
#include "connect.h"
#include "user.h"

/*** STATIC PROTOTYPES ***/

static void TestGetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s);

static void TestGetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);

static void TestGetAllZonenames (rcComm_t *connection_p);

static void TestExecuteQueryString (rcComm_t *connection_p);

static void TestGetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from , const time_t to);

/*** METHODS ***/

int main (int argc, char *argv [])
{
	int result = 0;
	rcComm_t *connection_p = CreateConnection ();
	
	if (connection_p)
		{
			struct tm from;
			struct tm to;
			time_t from_time;
			time_t to_time;
			
			/* 00:00:00 01/08/2014 */
			from.tm_sec = 0;
			from.tm_min = 0; 
			from.tm_hour = 0;   /* hours since midnight (0 to 23) */
			from.tm_mday = 1;   /* day of the month (1 to 31) */
			from.tm_mon = 9;    /* months since January (0 to 11) */
			from.tm_year = 114;   /* years since 1900 */

			
			/* 00:00:00 02/08/2014 */
			to.tm_sec = 0;
			to.tm_min = 0; 
			to.tm_hour = 0;   /* hours since midnight (0 to 23) */
			to.tm_mday = 2;   /* day of the month (1 to 31) */
			to.tm_mon = 8;    /* months since January (0 to 11) */
			to.tm_year = 114;   /* years since 1900 */
			
			
			from_time = mktime (&from);
			to_time = mktime (&to);
			
//			TestGetAllCollectionsForUsername (connection_p, "tyrrells");


//			TestGetAllDataForUsername (connection_p, "tyrrells");
/*
			TestGetAllZonenames (connection_p);
*/
			TestGetAllModifiedDataForUsername (connection_p, "tyrrells", from_time, to_time);

			//TestExecuteQueryString (connection_p);
			
			CloseConnection (connection_p);
		}		/* if (connection_p) */

	return result;
}



/*** STATIC METHODS ***/

static void TestGetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s)
{
	QueryResults *results_p = GetAllCollectionsForUsername (connection_p, username_s);
	
	if (results_p)
		{							
			printf ("\nTestGetAllCollectionsForUsername : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}	
}


static void TestGetAllDataForUsername (rcComm_t *connection_p, const char * const username_s)
{
	QueryResults *results_p = GetAllDataForUsername (connection_p, username_s, "10039");
	
	if (results_p)
		{							
			printf ("\nTestGetAllDataForUsername : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}	
}

static void TestGetAllZonenames (rcComm_t *connection_p)
{
	QueryResults *results_p = GetAllZoneNames (connection_p);
	
	if (results_p)
		{							
			printf ("\nTestGetAllZonenames : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}	
}


static void TestGetAllModifiedDataForUsername (rcComm_t *connection_p, const char * const username_s, const time_t from, const time_t to)
{
	QueryResults *results_p = GetAllModifiedDataForUsername (connection_p, username_s, from, to);
	
	if (results_p)
		{							
			printf ("\nTestGetAllModifiedDataForUsername : -----------------------\n\n");
			PrintQueryResults (stdout, results_p);
			FreeQueryResults (results_p);
		}	
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
			* (query_args_pp + 4) = "' AND USER_ID = '10039'";
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




