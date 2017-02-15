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

/**
 * parameter_option.c
 *
 *  Created on: 14 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */


#include "parameter_option.h"
#include "streams.h"
#include "string_utils.h"
#include "memory_allocations.h"



ParameterOption *AllocateParameterOption (SharedType value, const char * const description_s, const ParameterType param_type)
{
	bool success_flag = true;
	char *new_description_s  = NULL;

	if (description_s)
		{
			new_description_s = CopyToNewString (description_s, 0, true);

			if (!new_description_s)
				{
					success_flag = false;
				}
		}


	if (success_flag)
		{
			ParameterOption *option_p = (ParameterOption *) AllocMemory (sizeof (option_p));

			if (option_p)
				{
					if (CopySharedType (value, & (option_p -> po_value), param_type))
						{
							option_p -> po_description_s = new_description_s;
							option_p -> po_type = param_type;

							return option_p;
						}

					FreeMemory (option_p);
				}		/* if (option_p) */

		}

	return NULL;
}


void FreeParameterOption (ParameterOption *option_p)
{
	if (option_p -> po_description_s)
		{
			FreeCopiedString (option_p -> po_description_s);
		}

	ClearSharedType (& (option_p -> po_value), option_p -> po_type);
}


LinkedList *CreateProgramOptionsList (void)
{
	return AllocateLinkedList (FreeParameterOptionNode);
}


ParameterOptionNode *AllocateParameterOptionNode (ParameterOption *option_p)
{
	ParameterOptionNode *node_p = (ParameterOptionNode *) AllocMemory (sizeof (ParameterOptionNode));

	if (node_p)
		{
			node_p -> pon_node.ln_prev_p = NULL;
			node_p -> pon_node.ln_next_p = NULL;

			node_p -> pon_option_p = option_p;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate ParameterMultiOptionNode for option \"%s\"", option_p -> po_description_s);
		}

	return node_p;
}


void FreeParameterOptionNode (ListItem *item_p)
{
	ParameterOptionNode *node_p = (ParameterOptionNode *) item_p;

	if (node_p -> pon_option_p)
		{
			FreeParameterOption (node_p -> pon_option_p);
		}

	FreeMemory (node_p);
}



bool CreateAndAddParameterOption (LinkedList *options_p, SharedType value, const char * const description_s, const ParameterType param_type)
{
	bool success_flag = false;

	ParameterOption *option_p = AllocateParameterOption (value, description_s, param_type);

	if (option_p)
		{
			ParameterOptionNode *node_p = AllocateParameterOptionNode (option_p);

			if (node_p)
				{
					LinkedListAddTail (options_p, & (node_p -> pon_node));
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate option node with description \"%s\"", description_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate option with description \"%s\"", description_s);
		}

	return success_flag;
}




/*
bool SetParameterMultiOption (ParameterMultiOptionArray *options_p, const uint32 i, const char * const description_s, SharedType value, bool copy_value_flag)
{
	ParameterOption *option_p = (options_p -> pmoa_options_p) + i;
	bool success_flag = true;

	if (description_s)
		{
			char *new_description_s = CopyToNewString (description_s, 0, true);

			if (new_description_s)
				{
					if (option_p -> pmo_description_s)
						{
							FreeCopiedString (option_p -> pmo_description_s);
						}

					option_p -> pmo_description_s = new_description_s;
				}
			else
				{
					success_flag = false;
				}
		}

	if (success_flag)
		{
			if (copy_value_flag)
				{
					if (!CopySharedType (value, & (option_p -> po_value), options_p -> pmoa_values_type))
						{
							success_flag = false;
						}
				}
			else
				{
					option_p -> po_value = value;
				}
		}

	return success_flag;
}
*/
