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


IRODS_UTIL_API genQueryOut_t *ExecuteQuery (rcComm_t *connection_p, char *query_s);


IRODS_UTIL_API char *BuildQuery (const char **args_ss);


IRODS_UTIL_API void FreeBuiltQuery (char *query_s);



#ifdef __cplusplus
}
#endif

#endif		/* #ifndef IRODS_QUERY_H */
