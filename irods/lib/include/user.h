#ifndef IRODS_UTIL_USER_H
#define IRODS_UTIL_USER_H

#ifdef __cplusplus
extern "C" 
{
#endif

IRODS_UTIL_API LinkedList *GetAllCollectionsForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API LinkedList *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);


IRODS_UTIL_API LinkedList *GetAllDataForUsername (rcComm_t *connection_p, const char * const username_s);



#ifdef __cplusplus
}
#endif


#endif		/* #ifndef IRODS_UTIL_USER_H */