#ifndef WHEATIS_SERVICE_H
#define WHEATIS_SERVICE_H

#include "linked_list.h"

/**
 * A datatype detailing the addon services
 * that the wheatis offers. These are the
 * equivalent to iRods microservices.
 */

 /* forward declaration */
struct Service;
 
 
struct Service
{
	int (*se_run_fn) (const char * const filename_s, ParameterSet *param_set_p);

	bool (*se_match_fn) (const char * const filename_s);

	ParameterSet * (*se_get_params_fn) (void);
	
};


typedef struct 
{
	ListNode sn_node;
	Service *service_p;
} ServiceNode;



ServiceNode *AllocateServiceNode (void);

void FreeServiceNode (ListNode *node_p);

LinkedList *LoadServices (const char * const path_s);



#endif		/* #ifndef WHEATIS_SERVICE_H */
