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
 * paired_service.c
 *
 *  Created on: 1 Feb 2016
 *      Author: billy
 */


#include "paired_service.h"
#include "memory_allocations.h"
#include "string_utils.h"


#ifdef _DEBUG
	#define PAIRED_SERVICE_DEBUG	(STM_LEVEL_FINER)
#else
	#define PAIRED_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif

PairedService *AllocatePairedService (const uuid_t id, const char *uri_s, const json_t *op_p)
{
	if (uri_s)
		{
			if (op_p)
				{
					ParameterSet *param_set_p = CreateParameterSetFromJSON (op_p);

					if (param_set_p)
						{
							char *copied_uri_s = CopyToNewString (uri_s, 0, false);

							if (copied_uri_s)
								{
									PairedService *paired_service_p = (PairedService *) AllocMemory (sizeof (PairedService));

									if (paired_service_p)
										{
											uuid_copy (paired_service_p -> ps_extenal_server_id, id);
											paired_service_p -> ps_uri_s = copied_uri_s;
											paired_service_p -> ps_params_p = param_set_p;

											#if PAIRED_SERVICE_DEBUG >= STM_LEVEL_FINER
												{
													char uuid_s [UUID_STRING_BUFFER_SIZE];

													ConvertUUIDToString (paired_service_p -> ps_extenal_server_id, uuid_s);
													PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Added paired service at \"%s\" for server \"%s\"", paired_service_p -> ps_uri_s, uuid_s);

													ConvertUUIDToString (id, uuid_s);
													PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Original id \"%s\"", uuid_s);
												}
											#endif

											return paired_service_p;
										}

									FreeCopiedString (copied_uri_s);
								}		/* if (copied_uri_s) */
							else
								{

								}

							FreeParameterSet (param_set_p);
						}		/* if (param_set_p) */

				}		/* if (op_p) */
		}

	return NULL;
}



void FreePairedService (PairedService *paired_service_p)
{
	if (paired_service_p -> ps_uri_s)
		{
			FreeCopiedString (paired_service_p -> ps_uri_s);
		}

	if (paired_service_p -> ps_params_p)
		{
			FreeParameterSet (paired_service_p -> ps_params_p);
		}

	FreeMemory (paired_service_p);
}



PairedServiceNode *AllocatePairedServiceNode (PairedService *paired_service_p)
{
	PairedServiceNode *node_p = AllocMemory (sizeof (PairedServiceNode));

	if (node_p)
		{
			node_p -> psn_node.ln_prev_p = NULL;
			node_p -> psn_node.ln_next_p = NULL;
			node_p -> psn_paired_service_p = paired_service_p;
		}

	return node_p;
}



PairedServiceNode *AllocatePairedServiceNodeByParts (const uuid_t id, const char *uri_s, const json_t *op_p)
{
	PairedService *paired_service_p = AllocatePairedService (id, uri_s, op_p);

	if (paired_service_p)
		{
			PairedServiceNode *node_p = AllocatePairedServiceNode (paired_service_p);

			if (node_p)
				{
					return node_p;
				}

			FreePairedService (paired_service_p);
		}

	return NULL;
}



void FreePairedServiceNode (ListItem *node_p)
{
	PairedServiceNode *ps_node_p = (PairedServiceNode *) node_p;

	FreePairedService (ps_node_p -> psn_paired_service_p);
	FreeMemory (ps_node_p);
}
