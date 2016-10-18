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
 * parameter_group.c
 *
 *  Created on: 9 Feb 2016
 *      Author: billy
 */

#define ALLOCATE_PARAMETER_GROUP_TAGS_H
#include "parameter_group.h"
#include "string_utils.h"
#include "json_util.h"
#include "service.h"


ParameterGroupNode *AllocateParameterGroupNode (const char *name_s, const char *group_key_s, ServiceData *service_data_p, ParameterSet *param_set_p)
{
	ParameterGroup *group_p = AllocateParameterGroup (name_s, group_key_s, service_data_p);

	if (group_p)
		{
			ParameterGroupNode *param_group_node_p = AllocateParameterGroupNodeForExistingParameterGroup (group_p);

			if (param_group_node_p)
				{
					return param_group_node_p;
				}		/* if (param_group_node_p) */

			FreeParameterGroup (group_p);
		}		/* if (group_p) */

	return NULL;
}


ParameterGroupNode *AllocateParameterGroupNodeForExistingParameterGroup (ParameterGroup *group_p)
{
	ParameterGroupNode *param_group_node_p = (ParameterGroupNode *) AllocMemory (sizeof (ParameterGroupNode));

	if (param_group_node_p)
		{
			param_group_node_p -> pgn_param_group_p = group_p;
			param_group_node_p -> pgn_node.ln_prev_p = NULL;
			param_group_node_p -> pgn_node.ln_next_p = NULL;

			return param_group_node_p;
		}		/* if (param_group_node_p) */

	return NULL;
}



void FreeParameterGroupNode (ListItem *node_p)
{
	ParameterGroupNode *param_group_node_p = (ParameterGroupNode *) node_p;

	FreeParameterGroup (param_group_node_p -> pgn_param_group_p);
	FreeMemory (param_group_node_p);
}


ParameterGroup *AllocateParameterGroup (const char *name_s, const char *key_s, ServiceData *service_data_p)
{
	char *copied_name_s = CopyToNewString (name_s, 0, false);

	if (copied_name_s)
		{
			LinkedList *children_p = NULL;
			char *copied_key_s = NULL;

			if (key_s)
				{
					copied_key_s = CopyToNewString (key_s, 0, false);

					if (!copied_key_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy key \"%s\"", key_s);
							FreeCopiedString (copied_name_s);
							return NULL;
						}
				}

			children_p = AllocateLinkedList (FreeParameterGroupNode);

			if (children_p)
				{
					LinkedList *params_p = AllocateLinkedList (FreeParameterNode);

					if (params_p)
						{
							ParameterGroup *param_group_p = (ParameterGroup *) AllocMemory (sizeof (ParameterGroup));

							if (param_group_p)
								{
									param_group_p -> pg_name_s = copied_name_s;
									param_group_p -> pg_key_s = copied_key_s;
									param_group_p -> pg_params_p = params_p;
									param_group_p -> pg_visible_flag = true;

									param_group_p -> pg_full_display_flag = true;
									param_group_p -> pg_vertical_layout_flag = true;
									param_group_p -> pg_child_groups_p = children_p;

									if (service_data_p)
										{
											GetParameterGroupVisibility (service_data_p, name_s, & (param_group_p -> pg_visible_flag));
										}

									return param_group_p;
								}

							FreeLinkedList (params_p);
						}		/* if (params_p) */

					FreeLinkedList (children_p);
				}


			FreeCopiedString (copied_name_s);
		}		/* if (copied_name_s) */

	return NULL;
}


void FreeParameterGroup (ParameterGroup *param_group_p)
{
	FreeCopiedString (param_group_p -> pg_name_s);

	if (param_group_p -> pg_key_s)
		{
			FreeCopiedString (param_group_p -> pg_key_s);
		}

	FreeLinkedList (param_group_p -> pg_child_groups_p);

	FreeLinkedList (param_group_p -> pg_params_p);

	FreeMemory (param_group_p);
}


ParameterGroup *CreateAddAddParameterGroupToParameterSet (const char *name_s, const char *key_s, struct ServiceData *service_data_p, ParameterSet *param_set_p)
{
	ParameterGroup *group_p = AllocateParameterGroup (name_s, key_s, service_data_p);

	if (group_p)
		{
			if (!AddParameterGroupToParameterSet (param_set_p, group_p))
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add \"%s\" group to \"%s\" param set", name_s, param_set_p -> ps_name_s);
					FreeParameterGroup (group_p);
					group_p = NULL;
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create \"%s\" group", name_s);
		}

	return group_p;
}


json_t *GetParameterGroupAsJSON (ParameterGroup *param_group_p)
{
	json_error_t err;
	json_t *value_p = json_pack_ex (&err, 0, "{s:s,s:b}", PARAM_GROUP_S, param_group_p -> pg_name_s, PARAM_GROUP_VISIBLE_S, param_group_p -> pg_visible_flag);

	if (!value_p)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetParameterGroupAsJSON failed for \"%s\"", param_group_p -> pg_name_s);
		}

	return value_p;
}



bool AddParameterToParameterGroup (ParameterGroup *group_p, Parameter *param_p)
{
	bool success_flag = false;
	ParameterNode *node_p = AllocateParameterNode (param_p);

	if (node_p)
		{
			LinkedListAddTail (group_p, node_p);
		}

	return success_flag;
}


bool AddParameterGroupChild (ParameterGroup *parent_group_p, ParameterGroup *child_group_p)
{
	bool success_flag = false;
	ParameterGroupNode *node_p = AllocateParameterGroupNodeForExistingParameterGroup (child_group_p);

	if (node_p)
		{
			LinkedListAddTail (parent_group_p -> pg_child_groups_p, node_p);
			success_flag = true;
		}

	return success_flag;
}

