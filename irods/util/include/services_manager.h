#ifndef SERVICES_MANAGER_H
#define SERVICES_MANAGER_H


typedef struct ServicesManager 
{
	
	
} ServicesManager;



#ifdef __cplusplus
extern "C"
{
#endif	

IRODS_LIB_API LinkedList *LoadServices (const char * const path_s);


IRODS_LIB_API char *GetServicesListAsJSON (LinkedList *services_list_p);


IRODS_LIB_API void FreeServicesString (char *services_json_s);


/*
 * irods ids are 8-byte signed ints = int64_t
 */




#ifdef __cplusplus
}
#endif	



#endif		/* #ifndef SERVICES_MANAGER_H */

