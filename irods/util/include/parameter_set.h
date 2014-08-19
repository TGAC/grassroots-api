#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "jansson.h"

#include "linked_list.h"
#include "parameter.h"

typedef struct ParameterSet
{
	const char *ps_name_s;

	const char *ps_description_s;

	LinkedList *ps_params_p;
} ParameterSet;


#ifdef __cplusplus
extern "C"
{
#endif

IRODS_LIB_API ParameterSet *AllocateParameterSet (const char *name_s, const char *description_s);

IRODS_LIB_API void FreeParameterSet (ParameterSet *params_p);

IRODS_LIB_API bool AddParameterToParameterSet (ParameterSet *params_p, Parameter *param_p);


IRODS_LIB_API bool CreateAndAddParameterToParameterSet (ParameterSet *params_p, ParameterType type, 
	const char * const name_s, const char * const description_s, ParameterMultiOptionArray *options_p, 
	SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, 
	const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


IRODS_LIB_API json_t *GetParameterSetAsJSON (const ParameterSet * const param_set_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef PARAMETER_SET_H */

