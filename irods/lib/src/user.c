#include "rodsGenQuery.h"

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
	
	
	genQueryOut_t *result_p = ExecuteQuery (connection_p, query_s);

}


LinkedList *GetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s)
{
	LinkedList *result_p = NULL;
	
	return result_p;
}



LinkedList *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s)
{
	LinkedList *result_p = NULL;
	
	return result_p;	
}



LinkedList *GetAllZoneNames (rcComm_t *connection_p)
{
	LinkedList *result_p = NULL;
	
	return result_p;
}







