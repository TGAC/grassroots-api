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
 * linked_service.c
 *
 *  Created on: 8 Sep 2016
 *      Author: tyrrells
 */

#include "linked_service.h"
#include "memory_allocations.h"
#include "streams.h"


LinkedService *AllocateLinkedService (const char *input_service_s)
{
	char *input_service_copy_s = CopyToNewString (input_service_s, 0, false);

	if (input_service_copy_s)
		{
			LinkedList *list_p = AllocateLinkedList (FreeMappedParameterNode);

			if (list_p)
				{
					LinkedService *linked_service_p = (LinkedService *) AllocMemory (sizeof (LinkedService));

					if (linked_service_p)
						{
							linked_service_p -> ls_input_service_s = input_service_copy_s;
							linked_service_p -> ls_mapped_params_p = list_p;

							return linked_service_p;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate LinkedService for \"%s\"", input_service_s);

						}

					FreeLinkedList (list_p);
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate list of mapped params for \"%s\"", input_service_s);
				}

			FreeCopiedString (input_service_copy_s);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy input service name \"%s\"", input_service_s);
		}

	return NULL;
}


void FreeLinkedService (LinkedService *linked_service_p)
{
	if (linked_service_p -> ls_input_service_s)
		{
			FreeCopiedString (linked_service_p -> ls_input_service_s);
		}

	if (linked_service_p -> ls_mapped_params_p)
		{
			FreeLinkedList (linked_service_p -> ls_mapped_params_p);
		}

	FreeMemory (linked_service_p);
}


LinkedServiceNode *AllocateLinkedServiceNode (LinkedService *linked_service_p)
{
	LinkedServiceNode *node_p = (LinkedServiceNode *) AllocMemory (sizeof (LinkedServiceNode));

	if (node_p)
		{
			node_p -> lsn_node.ln_prev_p = NULL;
			node_p -> lsn_node.ln_next_p = NULL;

			node_p -> lsn_linked_service_p = linked_service_p;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate LinkedServiceNode for \"%s\"", linked_service_p -> ls_input_service_s);
		}

	return node_p;
}


void FreeLinkedServiceNode (ListItem *node_p)
{
	LinkedServiceNode *ls_node_p = (LinkedServiceNode *) node_p;

	FreeLinkedService (ls_node_p -> lsn_linked_service_p);
	FreeMemory (node_p);
}


bool CreateAndAddMappedParameterToLinkedService (LinkedService *linked_service_p, const char *input_s, const char *output_s)
{
	MappedParameter *mapped_param_p = AllocateMappedParameter (input_s, output_s);

	if (mapped_param_p)
		{
			if (AddMappedParameterToLinkedService (linked_service_p, mapped_param_p))
				{
					return true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add MappedParameter for \"%s\"=\"%s\" to \"%s\"", input_s, output_s, linked_service_p -> ls_input_service_s);
				}

			FreeMappedParameter (mapped_param_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate MappedParameter for \"%s\"=\"%s\"", input_s, output_s);
		}

	return false;
}


bool AddMappedParameterToLinkedService (LinkedService *linked_service_p, MappedParameter *mapped_param_p)
{
	bool success_flag = false;
	MappedParameterNode *node_p = AllocateMappedParameterNode (mapped_param_p);

	if (node_p)
		{
			LinkedListAddTail (linked_service_p -> ls_mapped_params_p, (ListItem *) node_p);
			success_flag  = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory of MappedParameterNode for \"%s\"=\"%s\" to \"%s\"", mapped_param_p -> mp_input_param_s, mapped_param_p -> mp_output_param_s, linked_service_p -> ls_input_service_s);
		}

	return success_flag;
}
