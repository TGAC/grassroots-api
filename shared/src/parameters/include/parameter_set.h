/*
L** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
/**
 * @file
 */
#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "jansson.h"

#include "linked_list.h"
#include "parameter.h"
#include "json_util.h"


struct ServiceData;

#include "parameter_group.h"


/**
 * @brief A set of Parameters along with an optional name
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
	 * A LinkedList of ParameterGroupNodes for this
	 * ParameterSet.
	 */
	LinkedList *ps_grouped_params_p;
} ParameterSet;


/**
 * A datatype for storing a ParameterSet on a LinkedList
 *
 * @extends ListItem
 */
typedef struct ParameterSetNode
{
	/** The base list node */
	ListItem psn_node;

	/** Pointer to the associated ParameterSet */
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
 * @return The newly created ParameterSet or <code>NULL</code> upon error.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API ParameterSet *AllocateParameterSet (const char *name_s, const char *description_s);


/**
 * Free a ParameterSet and all of its Parameters.
 *
 * @param params_p The ParameterSet to free.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API void FreeParameterSet (ParameterSet *params_p);


/**
 * Add a Parameter to a ParameterSet.
 *
 * @param params_p The ParameterSet to amend.
 * @param param_p The Parameter to add.
 * @return <code>true</code> if the Parameter was added successfully, <code>false</code> otherwise.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API bool AddParameterToParameterSet (ParameterSet *params_p, Parameter *param_p);


/**
 * Allocate a new Parameter and add it to a ParameterSet.
 *
 * @param service_data_p The ServiceData for the Service that is allocating this Parmeter.
 * @param params_p The ParameterSet to add the new Parameter to.
 * @param group_p The ParameterGroup to add this Parameter to. This can be <code>NULL</code> in which case
 * the Parameter will not be placed within any ParameterGroup.
 * @param type The ParameterType for this Parameter.
 * @param multi_valued_flag If this is <code>true</code> then the Parameter can hold multiple values. For single value Parameters, set this to <code>false</code>.
 * @param name_s The name of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param display_name_s An optional name to display for the Parameter for use in Clients. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * This can be <code>NULL</code>.
 * @param description_s The description of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param options_p This can be used to constrain the Parameter to a fixed set of values. If this is <code>NULL</code> then the Parameter can be set to any value.
 * @param default_value The default value for this Parameter.
 * @param current_value_p If this is not <code>NULL</code>, then copy this value as the current value of the Parameter. If this is <code>NULL</code>, then current value for this Parameter
 * will be set to be a copy of its default value.
 * @param bounds_p If this is not <code>NULL</code>, then this will be used to specify the minimum and maximum values that this Parameter can take. If this is <code>NULL</code>,
 * then the Parameter can take any value.
 * @param level The ParameterLevel for this Parameter. This determines when the Client should display this Parameter to the user.
 * @param check_value_fn If this is not <code>NULL</code>, then this will be used to check whether the Parameter has been set to a valid value.
 * @return A newly-allocated Parameter or <code>NULL</code> upon error.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API Parameter *CreateAndAddParameterToParameterSet (const struct ServiceData *service_data_p, ParameterSet *params_p, ParameterGroup *group_p, ParameterType type, const bool multi_valued_flag,
  const char * const name_s, const char * const display_name_s, const char * const description_s,
	ParameterMultiOptionArray *options_p, SharedType default_value, SharedType *current_value_p,
	ParameterBounds *bounds_p, uint8 level,
	const char *(*check_value_fn) (const Parameter * const parameter_p, const void *value_p));



/**
 * Allocate a new Parameter and add it to a ParameterSet. This is suitable for most Parameters that are single-valued. For more
 * complex Parameters, use <code>CreateAndAddParameterToParameterSet</code>.
 *
 * @param service_data_p The ServiceData for the Service that is allocating this Parmeter.
 * @param params_p The ParameterSet to add the new Parameter to.
 * @param group_p The ParameterGroup to add this Parameter to. This can be <code>NULL</code> in which case
 * the Parameter will not be placed within any ParameterGroup.
 * @param type The ParameterType for this Parameter.
 * @param name_s The name of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param display_name_s An optional name to display for the Parameter for use in Clients. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * This can be <code>NULL</code>.
 * @param description_s The description of the Parameter. The Parameter will store a copy of this string so this value does not need to remain in scope.
 * @param default_value The default value for this Parameter.
 * @param level The ParameterLevel for this Parameter. This determines when the Client should display this Parameter to the user.
 * @return A newly-allocated Parameter or <code>NULL</code> upon error.
 * @see CreateAndAddParameterToParameterSet
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API Parameter *EasyCreateAndAddParameterToParameterSet (const struct ServiceData *service_data_p, ParameterSet *params_p, ParameterGroup *group_p, ParameterType type,
	const char * const name_s, const char * const display_name_s, const char * const description_s, SharedType default_value, uint8 level);



/**
 * Generate a json-based description of a ParameterSet. This uses the Swagger definitions
 * as much as possible.
 *
 * @param param_set_p The ParameterSet to generate the description for.
 * @param sv_p If you wish to create a JSON fragment for a different version of the Grassroots system,
 * then you can set this value to the version that you require. If this is <code>NULL</code>, then the
 * current version of the running Grassroots system will be used.
 * @param full_definition_flag If this is <code>true</code> then all of the details for each of
 * the Parameters will get added. If this is <code>false</code> then just the name and
 * current value of each Parameter will get added. This is useful is you just want to send
 * the values to use when running a service.
 * @return The json-based representation of the ParameterSet or <code>NULL</code> if there was
 * an error.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API json_t *GetParameterSetAsJSON (const ParameterSet * const param_set_p, const SchemaVersion * const sv_p, const bool full_definition_flag);




/**
 * Generate a json-based description of a selection of ParameterSet.
 *
 * @param param_set_p The ParameterSet to generate the description for.
 * @param sv_p If you wish to create a JSON fragment for a different version of the Grassroots system,
 * then you can set this value to the version that you require. If this is <code>NULL</code>, then the
 * current version of the running Grassroots system will be used.
 * @param full_definition_flag If this is <code>true</code> then all of the details for each of
 * the Parameters will get added. If this is <code>false</code> then just the name and
 * current value of each Parameter will get added. This is useful is you just want to send
 * the values to use when running a service.
 * @param data_p If some custom data is needed by add_param_fn, then this can be used. It will be
 * passed to each call of add_param_fn.
 * @param add_param_fn A callback function that determines whether a Parameter should be added
 * to the generated JSON. This will be called for each Parameter in the ParameterSet along with data_p
 * and if returns <code>true</code> then the Parameter's JSON will get added. If it returns
 * <code>false</code>, then the Parameter will be skipped.
 * @return The json-based representation of the ParameterSet or <code>NULL</code> if there was
 * an error.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API json_t *GetParameterSetSelectionAsJSON (const ParameterSet * const param_set_p, const SchemaVersion * const sv_p, const bool full_definition_flag, void *data_p, bool (*add_param_fn) (const Parameter *param_p, void *data_p));


/**
 * Create a new ParameterSet from a json-based description. This uses the Swagger definitions
 * as much as possible.
 *
 * @param json_p The json-based representation of the ParameterSet.
 * @param concise_flag If this is <code>true</code>, then any Parameters within this
 * ParameterSet will just have the values that are needed to run the Service are added.
 * If this is <code>false</code> then user-facing attributes such as description,
 * parameter level, group, etc. will be added.
 * @return  The newly-generated ParameterSet or <code>NULL</code> if there was
 * an error.
 * @memberof ParameterSet
 * @see CreateParameterFromJSON
 */
GRASSROOTS_PARAMS_API ParameterSet *CreateParameterSetFromJSON (const json_t * const json_p, const bool concise_flag);

/**
 * Get the Parameter with a given name from a ParameterSet.
 *
 * @param params_p The ParameterSet to search.
 * @param name_s The Parameter name to try and match.
 * @return  The Parameter with the matching name or <code>NULL</code> if it could not
 * be found
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API Parameter *GetParameterFromParameterSetByName (const ParameterSet * const params_p, const char * const name_s);


/**
 * Get the ParameterNode for a Parameter with a given name from a ParameterSet.
 *
 * @param params_p The ParameterSet to search.
 * @param name_s The Parameter name to try and match.
 * @return  The ParameterNode with the matching Parameter name or <code>NULL</code> if it could not
 * be found
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API ParameterNode *GetParameterNodeFromParameterSetByName (const ParameterSet * const params_p, const char * const name_s);



/**
 * Get the value of a Parameter within a ParameterSet
 *
 * @param params_p The ParameterSet to get the Parameter from.
 * @param name_s The Parameter name to try and match.
 * @param value_p Where the Parameter value will be stored upon success.
 * @param current_value_flag If this is <code>true</code> then the current value
 * of the Parameter will be retrieved. If this is <code>false</code> then the default
 * value of the Parameter will be retrieved instead.
 * @return <code>true</code> if the Parameter value was retrieved successfully, <code>false</code>
 * otherwise.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API bool GetParameterValueFromParameterSet (const ParameterSet * const params_p, const char * const name_s, SharedType *value_p, const bool current_value_flag);


/**
 * Allocate a new ParameterSetNode to point to the given ParameterSet
 *
 * @param params_p The ParameterSet that this ParameterSetNode will point to.
 * @return  The newly-generated ParameterSetNode or <code>NULL</code> if there was
 * an error.
 * @memberof ParameterSetNode
 */
GRASSROOTS_PARAMS_API ParameterSetNode *AllocateParameterSetNode (ParameterSet *params_p);


/**
 * Free a ParameterSetNode and its associated ParameterSet.
 *
 * @param node_p The ParameterSetNode to free.
 * @see FreeParameterSet
 * @memberof ParameterSetNode
 */
GRASSROOTS_PARAMS_API void FreeParameterSetNode (ListItem *node_p);



/**
 * Remove the Parameter with a given name from a ParameterSet.
 *
 * @param params_p The ParameterSet to search.
 * @param name_s The name of the Parameter to try and match.
 * @return If found, the Parameter with the matching Tag will be be removed from the ParameterSet
 * and returned. If it could not be found <code>NULL</code> will be returned.
 * be found.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API Parameter *DetachParameterByName (ParameterSet *params_p, const char * const name_s);


/**
 * Get all of the Parameters within a given ParameterGroup.
 *
 * @param params_p The ParameterSet to search.
 * @param name_s The name of the ParameterGroup to get the Parameters for.
 * @return Upon success, an array of matching Parameter pointers terminated by a <code>NULL</code>.
 * Upon failure a <code>NULL</code> is returned.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API struct ParameterGroup *GetParameterGroupFromParameterSetByGroupName (const ParameterSet * const params_p, const char * const name_s);


/**
 * Add a ParameterGroup to a ParameterSet.
 *
 * @param param_set_p The ParameterSet to add the ParameterGroup to.
 * @param group_p The ParameterGroup to add.
 * @return <code>true</code> if the ParameterGroup was added successfully, <code>false</code>
 * otherwise.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API bool AddParameterGroupToParameterSet (ParameterSet *param_set_p, ParameterGroup *group_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef PARAMETER_SET_H */

