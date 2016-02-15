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
#include "connection.h"
#include "json_tools.h"
#include "service.h"


#ifdef _DEBUG
	#define PAIRED_SERVICE_DEBUG	(STM_LEVEL_FINER)
#else
	#define PAIRED_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


PairedService *AllocatePairedService (const uuid_t id, const char *name_s, const char *uri_s, const json_t *op_p)
{
	if (name_s && uri_s)
		{
			if (op_p)
				{
					ParameterSet *param_set_p = CreateParameterSetFromJSON (op_p);

					if (param_set_p)
						{
							char *copied_name_s = CopyToNewString (name_s, 0, false);

							if (copied_name_s)
								{
									char *copied_uri_s = CopyToNewString (uri_s, 0, false);

									if (copied_uri_s)
										{
											PairedService *paired_service_p = (PairedService *) AllocMemory (sizeof (PairedService));

											if (paired_service_p)
												{
													uuid_copy (paired_service_p -> ps_extenal_server_id, id);
													paired_service_p -> ps_name_s = copied_name_s;
													paired_service_p -> ps_uri_s = copied_uri_s;
													paired_service_p -> ps_params_p = param_set_p;

													#if PAIRED_SERVICE_DEBUG >= STM_LEVEL_FINER
														{
															char uuid_s [UUID_STRING_BUFFER_SIZE];

															ConvertUUIDToString (paired_service_p -> ps_extenal_server_id, uuid_s);
															PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Added paired service at \"%s\" with name \"%s\" for server \"%s\"", paired_service_p -> ps_uri_s, paired_service_p -> ps_name_s, uuid_s);

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
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy uri \"%s\"", uri_s);
										}

									FreeCopiedString (copied_name_s);
								}		/* if (copied_name_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy name \"%s\"", name_s);
								}

							FreeParameterSet (param_set_p);
						}		/* if (param_set_p) */
					else
						{

						}

				}		/* if (op_p) */

		}		/* if (name_s && uri_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Name \"%s\" and URI \"%s\" must exist", name_s ? name_s : "NULL", uri_s ? uri_s : "NULL");
		}

	return NULL;
}



void FreePairedService (PairedService *paired_service_p)
{
	if (paired_service_p -> ps_name_s)
		{
			FreeCopiedString (paired_service_p -> ps_name_s);
		}

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



PairedServiceNode *AllocatePairedServiceNodeByParts (const uuid_t id, const char *name_s, const char *uri_s, const json_t *op_p)
{
	PairedService *paired_service_p = AllocatePairedService (id, name_s, uri_s, op_p);

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



json_t *MakeRemotePairedServiceCall (const char * const service_name_s, ParameterSet *params_p, const char * const paired_service_uri_s)
{
	json_t *res_p = NULL;
	Connection *connection_p = AllocateWebServerConnection (paired_service_uri_s);

	if (connection_p)
		{
			json_t *req_p = json_object ();

			if (req_p)
				{
					/*
					 * Only send the databases that the external paired service knows about
					 */
					json_t *service_req_p = GetServiceRunRequest (service_name_s, params_p, true);

					if (service_req_p)
						{
							if (json_object_set_new (req_p, SERVICES_NAME_S, service_req_p) == 0)
								{
									res_p = MakeRemoteJsonCall (req_p, connection_p);

									if (!res_p)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON fragment from MakeRemoteJsonCall");
										}

								}		/* if (json_object_set_new (req_p, SERVICES_NAME_S, service_req_p) == 0) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add service request to josn request");
									json_decref (service_req_p);
								}

						}		/* if (service_req_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON fragment from GetServiceRunRequest");
						}

					json_decref (req_p);
				}		/* if (req_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create JSON array for req_p");
				}

			FreeConnection (connection_p);
		}		/* if (connection_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create connection to paired service at \"%s\"", paired_service_uri_s);
		}

	return res_p;
}

