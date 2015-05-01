#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "jansson.h"

#include "linked_list.h"
#include "parameter.h"
#include "json_util.h"



typedef struct ParameterGroupNode
{
	ListItem pgn_node;
	ParameterGroup *pgn_param_group_p;
} ParameterGroupNode;



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

	/**
	 * A LinkedList of ParamneterGroupNode for this
	 * ParameterSet.
	 */
	LinkedList *ps_grouped_params_p;
} ParameterSet;


typedef struct ParameterSetNode
{
	ListItem psn_node;
	ParameterSet *psn_param_set_p;
} ParameterSetNode;



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


WHEATIS_SERVICE_API Parameter *CreateAndAddParameterToParameterSet (ParameterSet *params_p, ParameterType type, const bool multi_valued_flag,
	const char * const name_s, const char * const display_name_s, const char * const description_s, uint32 tag, 
	ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p, 
	ParameterBounds *bounds_p, uint8 level,
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



WHEATIS_SERVICE_API uint32 GetCurrentParameterValues (const ParameterSet * const params_p, TagItem *tags_p);


WHEATIS_SERVICE_API Parameter *GetParameterFromParameterSetByTag (const ParameterSet * const params_p, const Tag tag);

WHEATIS_SERVICE_API Parameter *GetParameterFromParameterSetByName (const ParameterSet * const params_p, const char * const name_s);

WHEATIS_SERVICE_API bool GetParameterValueFromParameterSet (const ParameterSet * const params_p, const Tag tag, SharedType *value_p, const bool current_value_flag);


WHEATIS_SERVICE_API ParameterSetNode *AllocateParameterSetNode (ParameterSet *params_p);


WHEATIS_SERVICE_API void FreeParameterSetNode (ListItem *node_p);


WHEATIS_SERVICE_API bool AddParameterGroupToParameterSet (ParameterSet *param_set_p, const char *group_name_s, Parameter **params_pp, const uint32 num_params);


WHEATIS_SERVICE_API bool AddParameterGroupToParameterSetByName (ParameterSet *param_set_p, const char *group_name_s, const char ** const param_names_ss, const uint32 num_params);


WHEATIS_SERVICE_API bool CreateParameterGroupsFromJSON (ParameterSet *params_p, const json_t * const json_p);


WHEATIS_SERVICE_API json_t *GetParameterGroupsAsJSON (const LinkedList * const param_groups_p);


WHEATIS_SERVICE_API Parameter *DetachParameterByTag (ParameterSet *params_p, const Tag tag);



#ifdef __cplusplus
}
#endif



#endif		/* #ifndef PARAMETER_SET_H */

