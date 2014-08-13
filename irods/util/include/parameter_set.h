#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "linked_list.h"
#include "parameter.h"

typedef struct ParameterSet
{
	const char * const ps_name_s;

	const char * const ps_description_s;

	LinkedList *ps_params_p;
} ParameterSet;


#ifdef __cplusplus
extern "C"
{
#endif

IRODS_LIB_API bool AddParameterToParameterSet (ParameterSet *params_p, Parameter *param_p);

IRODS_LIB_API ParameterNode *AllocateParameterNode (Parameter *param_p);

IRODS_LIB_API void FreeParameterNode (ListNode *node_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef PARAMETER_SET_H */

