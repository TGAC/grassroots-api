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

/*
 * parameter_group.h
 *
 *  Created on: 9 Feb 2016
 *      Author: billy
 */

#ifndef PARAMETERS_INCLUDE_PARAMETER_GROUP_H_
#define PARAMETERS_INCLUDE_PARAMETER_GROUP_H_


#include "linked_list.h"

#include "parameter.h"


/**
 * A datatype to tell the system that certain
 * parameters should be grouped together in the
 * client's user interface if possible.
 */
typedef struct ParameterGroup
{
	/** The name of the ParameterGroup */
	char *pg_name_s;

	/** An internal key to denote this ParameterGroup */
	char *pg_key_s;

	/** The number of Parameters in this ParameterGroup */
	uint32 pg_num_params;

	/**
	 * An array of pointers to the Parameters that make up
	 * this ParameterGroup.
	 */
	struct Parameter **pg_params_pp;
} ParameterGroup;



/**
 * A datatype for storing a ParameterGroup on a LinkedList.
 *
 * @extends ListItem
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


GRASSROOTS_PARAMS_API ParameterGroupNode *AllocateParameterGroupNode (const char *name_s,  const char *key_s, Parameter **params_pp, const uint32 num_params);


GRASSROOTS_PARAMS_API void FreeParameterGroupNode (ListItem *node_p);


GRASSROOTS_PARAMS_API ParameterGroup *AllocateParameterGroup (const char *name_s, const char *key_s, Parameter **params_pp, const uint32 num_params);


GRASSROOTS_PARAMS_API void FreeParameterGroup (ParameterGroup *param_group_p);


#ifdef __cplusplus
}
#endif


#endif /* PARAMETERS_INCLUDE_PARAMETER_GROUP_H_ */
