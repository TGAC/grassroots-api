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
//#include "rodsGenQuery.h"

#include "rodsGenQuery.h"

#include "user.h"
#include "user_util.h"
#include "query.h"
#include "query_util.h"
#include "irods_connection.h"
#include "connect.h"

#include "string_utils.h"
//#include "irods_handler.h"

#include <time.h>

#include <jansson.h>

#include "irods_util_library.h"


#include "linked_list.h"
#include "typedefs.h"



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

	InitGenQuery (&in_query);	
	
	if (SetQuerySelectClauses (&in_query, num_select_columns, select_column_ids_p, NULL))
		{			
			if (SetQueryWhereClauses (&in_query, num_where_columns, where_column_ids_p, where_column_values_pp, where_ops_pp))
				{
					genQueryOut_t *output_p = ExecuteGenQuery (connection_p, &in_query);
					
					if (output_p)
						{
							results_p = GenerateQueryResults (output_p);
						}		/* if (output_p) */
					
				}		/* if (SetQueryWhereClauses (&in_query, 1, where_column_ids, where_columns_p, where_quote_clauses)) */	
			
		}		/* if (SetQuerySelectClauses (&in_query, 2, select_column_ids, NULL)) */
	
	ClearGenQuery (&in_query);
	
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
	
	/* 
	 * iRods stores timestamps as strings rather than numbers so we need to convert the times 
	 * to strings. They are stored as strings of length 11. 
	 */
	char from_s [12];
	
	if (sprintf (from_s, "%011ld", from) >= 0)
		{
			char to_s [12];
				
			if (sprintf (to_s, "%011ld", to) >= 0)
				{
					const char *where_columns_pp [] = { username_s, from_s, to_s };
					const char *where_ops_pp [] = { " = \'", " >= \'", " <= \'" };
					
					results_p = RunQuery (connection_p, select_column_ids, 3, where_column_ids, where_columns_pp, where_ops_pp, 3);
				}		
		}
		
	return results_p;
}



json_t *GetModifiedIRodsFiles (const char * const username_s, const char * const password_s, const time_t from, const time_t to)
{
	json_t *json_p = NULL;
	IRODSConnection *connection_p = CreateIRODSConnection (username_s, password_s);
	
	if (connection_p)
		{
			QueryResults *qr_p = GetAllModifiedDataForUsername (connection_p -> ic_connection_p, username_s, from, to);
			
			if (qr_p)
				{
					json_p = GetQueryResultAsJSON (qr_p);
					
					FreeQueryResults (qr_p);
				}
			
			FreeIRODSConnection (connection_p);
		}
	
	return json_p;
}


json_t *GetInterestedServicesForIrodsDataObject (const char *services_path_s, char * const username_s, char * const password_s, const char *data_name_s)
{
	json_t *json_p = NULL;
	IRODSConnection *connection_p = CreateIRODSConnection (username_s, password_s);
	
	if (connection_p)
		{
		/*
			Handle *handle_p = AllocateIRodsHandle (connection_p);
			
			if (handle_p)
				{
					
					FreeIRodsHandle (handle_p);
				}
			*/
			FreeIRODSConnection (connection_p);
		}
	
	return json_p;
}




