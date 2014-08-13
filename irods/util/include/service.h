#ifndef WHEATIS_SERVICE_H
#define WHEATIS_SERVICE_H

#include "linked_list.h"
#include "parameter_set.h"
#include "typedefs.h"

/**
 * A datatype detailing the addon services
 * that the wheatis offers. These are the
 * equivalent to iRods microservices.
 */

/* forward declarations */
struct Plugin;
struct Service;
 
typedef struct ServiceData
{
	/** The service that owns this data. */
	struct Service *md_service_p;
} ServiceData;
 

typedef struct Service
{
	struct Plugin *se_plugin_p;
	
	int (*se_run_fn) (const char * const filename_s, ParameterSet *param_set_p);

	bool (*se_match_fn) (const char * const filename_s);

 	/** Get the user-friendly name of the service. */
	const char *(*se_get_service_name_fn) (void);

	/** Get the user-friendly description of the service. */
	const char *(*se_get_service_description_fn) (void);

	ParameterSet * (*se_get_params_fn) (void);	
	
	ServiceData *se_data_p;
	
} Service;


typedef struct 
{
	ListNode sn_node;
	Service *sn_service_p;
} ServiceNode;



int RunService (Service *service_p, const char * const filename_s, ParameterSet *param_set_p);

bool DoesFileMatchService (Service *service_p, const char * const filename_s);

/** Get the user-friendly name of the service. */
const char *GetServiceName (Service *service_p);

/** Get the user-friendly description of the service. */
const char *GetServiceDescription (Service *service_p);

ParameterSet *GetServiceParameters (Service *service_p);	



void FreeService (Service *service_p);


ServiceNode *AllocateServiceNode (Service *service_p);

void FreeServiceNode (ListNode *node_p);

LinkedList *LoadServices (const char * const path_s);



#endif		/* #ifndef WHEATIS_SERVICE_H */
