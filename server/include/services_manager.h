#ifndef SERVICES_MANAGER_H
#define SERVICES_MANAGER_H


#include "jansson.h"
#include "linked_list.h"
#include "wheatis_service_manager_library.h"

typedef struct ServicesManager 
{
	
	
} ServicesManager;



#ifdef __cplusplus
extern "C"
{
#endif	


WHEATIS_SERVICE_MANAGER_API json_t *GetServicesListAsJSON (LinkedList *services_list_p);


WHEATIS_SERVICE_MANAGER_API void FreeServicesString (char *services_json_s);


/*
 * irods ids are 8-byte signed ints = int64_t
 */




#ifdef __cplusplus
}
#endif	



#endif		/* #ifndef SERVICES_MANAGER_H */

