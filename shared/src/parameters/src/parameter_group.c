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

#include "parameter_group.h"
#include "string_utils.h"
#include "json_util.h"
#include "service.h"


ParameterGroupNode *AllocateParameterGroupNode (const char *name_s, const char *group_key_s, Parameter **params_pp, const uint32 num_params, ServiceData *service_data_p)
{
	ParameterGroup *group_p = AllocateParameterGroup (name_s, group_key_s, params_pp, num_params, service_data_p);

	if (group_p)
		{
			ParameterGroupNode *param_group_node_p = (ParameterGroupNode *) AllocMemory (sizeof (ParameterGroupNode));

			if (param_group_node_p)
				{
					param_group_node_p -> pgn_param_group_p = group_p;
					param_group_node_p -> pgn_node.ln_prev_p = NULL;
					param_group_node_p -> pgn_node.ln_next_p = NULL;

					return param_group_node_p;
				}		/* if (param_group_node_p) */

			FreeParameterGroup (group_p);
		}		/* if (group_p) */

	return NULL;
}


void FreeParameterGroupNode (ListItem *node_p)
{
	ParameterGroupNode *param_group_node_p = (ParameterGroupNode *) node_p;

	FreeParameterGroup (param_group_node_p -> pgn_param_group_p);
	FreeMemory (param_group_node_p);
}


ParameterGroup *AllocateParameterGroup (const char *name_s, const char *key_s, Parameter **params_pp, const uint32 num_params, ServiceData *service_data_p)
{
	char *copied_name_s = CopyToNewString (name_s, 0, false);

	if (copied_name_s)
		{
			ParameterGroup *param_group_p = NULL;
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

			param_group_p = (ParameterGroup *) AllocMemory (sizeof (ParameterGroup));

			if (param_group_p)
				{
					param_group_p -> pg_name_s = copied_name_s;
					param_group_p -> pg_key_s = copied_key_s;
					param_group_p -> pg_params_pp = params_pp;
					param_group_p -> pg_num_params = num_params;
					param_group_p -> pg_visible_flag = true;

					if (service_data_p)
						{
							GetParameterGroupVisibility (service_data_p, name_s, & (param_group_p -> pg_visible_flag));
						}

					return param_group_p;
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

	FreeMemory (param_group_p -> pg_params_pp);
	FreeMemory (param_group_p);
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

