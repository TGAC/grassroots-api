/*
** Copyright 2014-2015 The Genome Analysis Centre
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
 */
#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include "jansson.h"

#include "linked_list.h"
#include "parameter.h"
#include "json_util.h"



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
	 * A LinkedList of ParamneterGroupNode for this
	 * ParameterSet.
	 */
	LinkedList *ps_grouped_params_p;
} ParameterSet;


/**
 * A datatype for storing a ParameterSet on a LinkedList
 */
typedef struct ParameterSetNode
{
	/** The base list node */
	ListItem psn_node;

	/** Pointer to the associated ParameterSet */
	ParameterSet *psn_param_set_p;
} ParameterSetNode;



/**
 * A datatype for storing a ParameterGroup on a LinkedList
 */
typedef struct ParameterGroupNode
{
	/** The base list node */
	ListItem pgn_node;

	/** Pointer to the associated ParameterGroup */
	ParameterGroup *pgn_param_group_p;
} ParameterGroupNode;





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


GRASSROOTS_PARAMS_API Parameter *CreateAndAddParameterToParameterSet (ParameterSet *params_p, ParameterType type, const bool multi_valued_flag,
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
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API json_t *GetParameterSetAsJSON (const ParameterSet * const param_set_p, const bool full_definition_flag);


/**
 * Create a new ParameterSet from a json-based description. This uses the Swagger definitions
 * as much as possible.
 *
 * @param json_p The json-based representation of the ParameterSet..
 * @return  The newly-generated ParameterSet or <code>NULL</code> if there was
 * an error.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API ParameterSet *CreateParameterSetFromJSON (const json_t * const json_p);



GRASSROOTS_PARAMS_API uint32 GetCurrentParameterValues (const ParameterSet * const params_p, TagItem *tags_p);


/**
 * Get the Parameter with a given tag from a ParameterSet.
 *
 * @param params_p The ParameterSet to search.
 * @param tag The Tag to try and match.
 * @return  The Parameter with the matching Tag or <code>NULL</code> if it could not
 * be found
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API Parameter *GetParameterFromParameterSetByTag (const ParameterSet * const params_p, const Tag tag);


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


GRASSROOTS_PARAMS_API bool GetParameterValueFromParameterSet (const ParameterSet * const params_p, const Tag tag, SharedType *value_p, const bool current_value_flag);


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


GRASSROOTS_PARAMS_API bool AddParameterGroupToParameterSet (ParameterSet *param_set_p, const char *group_name_s, Parameter **params_pp, const uint32 num_params);


GRASSROOTS_PARAMS_API bool AddParameterGroupToParameterSetByName (ParameterSet *param_set_p, const char *group_name_s, const char ** const param_names_ss, const uint32 num_params);


GRASSROOTS_PARAMS_API bool CreateParameterGroupsFromJSON (ParameterSet *params_p, const json_t * const json_p);


GRASSROOTS_PARAMS_API json_t *GetParameterGroupsAsJSON (const LinkedList * const param_groups_p);


/**
 * Remove the Parameter with a given tag from a ParameterSet.
 *
 * @param params_p The ParameterSet to search.
 * @param tag The Tag to try and match.
 * @return If found, the Parameter with the matching Tag will be be removed from the ParameterSet
 * and returned. If it could not be found <code>NULL</code> will be returned.
 * be found.
 * @see GetParameterFromParameterSetByTag
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API Parameter *DetachParameterByTag (ParameterSet *params_p, const Tag tag);


/**
 * Get all of the Parameters within a given ParameterGroup.
 *
 * @param params_p The ParameterSet to search.
 * @param name_s The name of the ParameterGroup to get the Parameters for.
 * @return Upon success, an array of matching Parameter pointers terminated by a <code>NULL</code>.
 * Upon failure a <code>NULL</code> is returned.
 * @memberof ParameterSet
 */
GRASSROOTS_PARAMS_API ParameterGroup *GetParameterGroupFromParameterSetByGroupName (const ParameterSet * const params_p, const char * const name_s);



GRASSROOTS_PARAMS_API bool MergeParameterSets (const ParameterSet * const src_p, ParameterSet * const destination_p);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef PARAMETER_SET_H */

