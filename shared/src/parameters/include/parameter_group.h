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

#include "jansson.h"
#include "linked_list.h"

#include "parameter.h"



/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_JSON_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_JSON_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * json_util.c.
 */
#ifdef ALLOCATE_PARAMETER_GROUP_TAGS_H
	#define PARAMETER_GROUP_PREFIX GRASSROOTS_PARAMS_API
	#define PARAMETER_GROUP_VAL(x)	= x
#else
	#define PARAMETER_GROUP_PREFIX extern
	#define PARAMETER_GROUP_VAL(x)
#endif



PARAMETER_GROUP_PREFIX const char *PARAM_GROUP_DELIMITER_S PARAMETER_GROUP_VAL("/");


/* forward declaration */
struct ServiceData;
struct ParameterGroup;
struct ParameterSet;

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

	/** Should this ParameterGroup initially be visible? */
	bool pg_visible_flag;


	/**
	 * Should this ParameterGroup have a full display e.g. a titled
	 * legend or simply be a minimal grouping
	 */
	bool pg_full_display_flag;


	/**
	 * Should the ParameterGroup layout its child parameters
	 * horizontally or vertically?
	 */
	bool pg_vertical_layout_flag;


	/** The number of Parameters in this ParameterGroup */
	uint32 pg_num_params;

	/**
	 * A list of ParameterNodes for the parameters in this group
	 */
	LinkedList *pg_params_p;


	/**
	 * A list of ParmameterGroupNodes for any
	 * child parameter groups.
	 */
	LinkedList *pg_child_groups_p;

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



GRASSROOTS_PARAMS_API ParameterGroupNode *AllocateParameterGroupNode (ParameterGroup *group_p);


GRASSROOTS_PARAMS_API void FreeParameterGroupNode (ListItem *node_p);


GRASSROOTS_PARAMS_API ParameterGroup *AllocateParameterGroup (const char *name_s, const char *key_s, struct ServiceData *service_data_p);


GRASSROOTS_PARAMS_API void FreeParameterGroup (ParameterGroup *param_group_p);


GRASSROOTS_PARAMS_API json_t *GetParameterGroupAsJSON (ParameterGroup *param_group_p);


GRASSROOTS_PARAMS_API bool AddParameterGroupChild (ParameterGroup *parent_group_p, ParameterGroup *child_group_p);


GRASSROOTS_PARAMS_API bool AddParameterToParameterGroup (ParameterGroup *parent_group_p, Parameter *param_p);


GRASSROOTS_PARAMS_API ParameterGroup *CreateAndAddParameterGroupToParameterSet (const char *name_s, const char *key_s, struct ServiceData *service_data_p, struct ParameterSet *param_set_p);




#ifdef __cplusplus
}
#endif


#endif /* PARAMETERS_INCLUDE_PARAMETER_GROUP_H_ */
