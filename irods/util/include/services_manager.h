#ifndef SERVICES_MANAGER_H
#define SERVICES_MANAGER_H


typedef struct ServicesManager 
{
	
	
} ServicesManager;





LinkedList *LoadServices (const char * const path_s);


char *GetServicesListAsJSON (LinkedList *services_list_p);


void FreeServicesString (char *services_json_s);


/*
 * irods ids are 8-byte signed ints = int64_t
 */





#endif		/* #ifndef SERVICES_MANAGER_H */

