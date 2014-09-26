#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "jansson.h"

#include "linked_list.h"
#include "parameter.h"
#include "json_util.h"

/**
 * A set of Parameters along with an optional name
 * and description.
 */
typedef struct ParameterSet
{
	/** 
	 * An optional name for the ParameterSet. 
	 * This can be NULL.
	 */
	const char *ps_name_s;

	/** 
	 * An optional description for the ParameterSet. 
	 * This can be NULL.
	 */
	const char *ps_description_s;

	/**
	 * A LinkedList of ParameterNodes containing the
	 * Parameters.
	 */
	LinkedList *ps_params_p;
} ParameterSet;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Create a new ParameterSet containing no parameters.
 * 
 * @param name_s The name to use for the ParameterSet.
 * @param description_s The description to use for the ParameterSet.
 * @return The newly created ParameterSet or NULL upon error.
 */
WHEATIS_SERVICE_API ParameterSet *AllocateParameterSet (const char *name_s, const char *description_s);


/**
 * Free a ParameterSet and all of its Parameters.
 * 
 * @param params_p The ParameterSet to free.
 */
WHEATIS_SERVICE_API void FreeParameterSet (ParameterSet *params_p);


/**
 * Add a Parameter to a ParameterSet.
 * 
 * @param params_p The ParameterSet to amend.
 * @param param_p The Parameter to add.
 * @return <code>true</code> if the Parameter was added successfully, <code>false</code> otherwise.
 */
WHEATIS_SERVICE_API bool AddParameterToParameterSet (ParameterSet *params_p, Parameter *param_p);


WHEATIS_SERVICE_API bool CreateAndAddParameterToParameterSet (ParameterSet *params_p, ParameterType type, 
	const char * const name_s, const char * const description_s, ParameterMultiOptionArray *options_p, 
	SharedType default_value, ParameterBounds *bounds_p, ParameterLevel level, 
	const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));


/**
 * Generate a json-based description of a ParameterSet. This uses the Swagger definitions 
 * as much as possible.
 * 
 * @param param_set_p The ParameterSet to generate the description for.
 * @param full_definition_flag If this is <code>true</code> then all of the details for each of
 * the Parameters will get added. If this is <code>false</code> then just the name and 
 * current value of each Parameter will get added. This is useful is you just want to send 
 * the values to use when running a service. 
 * @return The json-based representation of the ParameterSet or <code>NULL</code> if there was
 * an error.
 */
WHEATIS_SERVICE_API json_t *GetParameterSetAsJSON (const ParameterSet * const param_set_p, const bool full_definition_flag);


/**
 * Create a new ParameterSet from a json-based description. This uses the Swagger definitions 
 * as much as possible.
 * 
 * @param json_p The json-based representation of the ParameterSet..
 * @return  The newly-generated ParameterSet or <code>NULL</code> if there was
 * an error.
 */
WHEATIS_SERVICE_API ParameterSet *CreateParameterSetFromJSON (const json_t * const json_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef PARAMETER_SET_H */

