/*
** Copyright 2014-2016 The Earlham Institute
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
 * mapped_parameter.c
 *
 *  Created on: 8 Sep 2016
 *      Author: tyrrells
 */


#include "mapped_parameter.h"
#include "memory_allocations.h"
#include "streams.h"
#include "string_utils.h"
#include "json_util.h"
#include "service_job.h"
#include "service.h"


MappedParameter *AllocateMappedParameter (const char *input_s, const char *output_s, bool required_flag, bool multi_flag)
{
	MappedParameter *param_p = (MappedParameter *) AllocMemory (sizeof (MappedParameter));

	if (param_p)
		{
			char *input_copy_s = CopyToNewString (input_s, 0, false);

			if (input_copy_s)
				{
					char *output_copy_s = CopyToNewString (output_s, 0, false);

					if (output_copy_s)
						{
							param_p -> mp_input_param_s = input_copy_s;
							param_p -> mp_output_param_s = output_copy_s;
							param_p -> mp_required_flag = required_flag;
							param_p -> mp_multiple_flag = multi_flag;

							return param_p;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy output param \"%s\"", output_s);
						}

					FreeCopiedString (output_copy_s);
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy input param \"%s\"", input_s);
				}

			FreeMemory (param_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for \"%s\"=\"%s\"", input_s, output_s);
		}

	return NULL;
}



void FreeMappedParameter (MappedParameter *mapped_param_p)
{
	FreeCopiedString (mapped_param_p -> mp_input_param_s);
	FreeCopiedString (mapped_param_p -> mp_output_param_s);
	FreeMemory (mapped_param_p);
}


MappedParameterNode *AllocateMappedParameterNode (MappedParameter *mapped_param_p)
{
	MappedParameterNode *node_p = (MappedParameterNode *) AllocMemory (sizeof (MappedParameterNode));

	if (node_p)
		{
			node_p -> mpn_node.ln_prev_p = NULL;
			node_p -> mpn_node.ln_next_p = NULL;

			node_p -> mpn_mapped_param_p = mapped_param_p;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate MappedParameterNode for \"%s\"=\"%s\"", mapped_param_p -> mp_input_param_s, mapped_param_p -> mp_output_param_s);
		}

	return node_p;
}


void FreeMappedParameterNode (ListItem *node_p)
{
	MappedParameterNode *mp_node_p = (MappedParameterNode *) node_p;

	FreeMappedParameter (mp_node_p -> mpn_mapped_param_p);
	FreeMemory (node_p);
}


MappedParameter *CreateMappedParameterFromJSON (const json_t *mapped_param_json_p)
{
	const char *input_s = GetJSONString (mapped_param_json_p, MAPPED_PARAM_INPUT_S);

	if (input_s)
		{
			const char *output_s = GetJSONString (mapped_param_json_p, MAPPED_PARAM_OUTPUT_S);

			if (output_s)
				{
					bool required_flag = true;
					bool multi_flag = false;

					GetJSONBoolean (mapped_param_json_p, MAPPED_PARAM_REQUIRED_S, &required_flag);
					GetJSONBoolean (mapped_param_json_p, MAPPED_PARAM_MULTI_VALUED_S, &multi_flag);

					return AllocateMappedParameter (input_s, output_s, required_flag, multi_flag);
				}		/* if (output_s) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, mapped_param_json_p, "Failed to get %s", MAPPED_PARAM_OUTPUT_S);
				}

		}		/* if (input_s) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, mapped_param_json_p, "Failed to get %s", MAPPED_PARAM_INPUT_S);
		}

	return NULL;
}


bool ProcessMappedParameter (MappedParameter *mapped_param_p, ServiceJob *job_p, ParameterSet *output_params_p)
{
	bool success_flag = false;
	char *param_value_s = NULL; //GenerateLinkedServiceResults (job_p, mapped_param_p -> mp_input_param_s);

	if (param_value_s)
		{
			/*
			 * We now have the value to set for linked service
			 */
			Parameter *output_parameter_p = GetParameterFromParameterSetByName (output_params_p, mapped_param_p -> mp_output_param_s);

			if (output_parameter_p)
				{
					if (SetParameterValueFromString (output_parameter_p, param_value_s))
						{
							success_flag = true;
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set \"%s\" to \"%s\"", output_parameter_p -> pa_name_s, param_value_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get parameter \"%s\"", mapped_param_p -> mp_output_param_s);
				}

		}
	else
		{
			if (mapped_param_p -> mp_required_flag)
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get the value for \"%s\" from \"%s\"", mapped_param_p -> mp_input_param_s, GetServiceName (job_p -> sj_service_p));
					success_flag = false;
				}
			else
				{
					success_flag = true;
					#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
					PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Could not get value for optional parameter \"%s\" ", mapped_param_p -> mp_input_param_s);
					#endif
				}
		}

	return success_flag;
}


