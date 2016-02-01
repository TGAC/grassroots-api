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


PairedService *AllocatePairedService (const uuid_t id, const char *uri_s, const json_t *op_p)
{
	if (uri_s)
		{
			if (op_p)
				{
					char *copied_uri_s = CopyToNewString (uri_s, 0, false);

					if (copied_uri_s)
						{
							json_t *copied_op_p = json_deep_copy (op_p);

							if (copied_op_p)
								{
									PairedService *paired_service_p = (PairedService *) AllocMemory (sizeof (PairedService));

									if (paired_service_p)
										{
											uuid_copy (paired_service_p -> ps_extenal_server_id, id);
											paired_service_p -> ps_uri_s = copied_uri_s;
											paired_service_p -> ps_op_json_p = copied_op_p;

											return paired_service_p;
										}

									json_decref (copied_op_p);
								}		/* if (copied_op_p) */
							else
								{

								}

						}		/* if (copied_uri_s) */
					else
						{

						}

					FreeCopiedString (copied_uri_s);
				}
		}

	return NULL;
}



void FreePairedService (PairedService *paired_service_p)
{
	if (paired_service_p -> ps_uri_s)
		{
			FreeCopiedString (paired_service_p -> ps_uri_s);
		}

	if (paired_service_p -> ps_op_json_p)
		{
			json_decref (paired_service_p -> ps_op_json_p);
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
