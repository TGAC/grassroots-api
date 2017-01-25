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
 * providers_state_table.c
 *
 *  Created on: 6 May 2016
 *      Author: tyrrells
 */

#include "providers_state_table.h"

#include "memory_allocations.h"
#include "json_util.h"
#include "streams.h"
#include "provider.h"
#include "grassroots_config.h"


#include "service.h"

#ifdef _DEBUG
	#define PROVIDERS_STATE_TABLE_DEBUG	(STM_LEVEL_FINE)
#else
	#define PROVIDERS_STATE_TABLE_DEBUG	(STM_LEVEL_NONE)
#endif


ProvidersStateTable *AllocateProvidersStateTable (const json_t *req_p)
{
	ProvidersStateTable *providers_p = (ProvidersStateTable *) AllocMemory (sizeof (ProvidersStateTable));

	if (providers_p)
		{
			providers_p -> pst_values_p = NULL;

			if (InitProvidersStateTableFromRequest (providers_p, req_p))
				{
					return providers_p;
				}
			else
				{
					PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, req_p, "Failed to inti providers table from request");
				}

			FreeMemory (providers_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to allocate providers table");
		}

	return NULL;
}



bool InitProvidersStateTableFromRequest (ProvidersStateTable * const providers_p, const json_t * const servers_p)
{
	json_t *inited_obj_p = NULL;

	if (servers_p)
		{
			if (json_is_object (servers_p))
				{
					inited_obj_p = json_deep_copy (servers_p);

					if (!inited_obj_p)
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, servers_p, "Failed to copy servers");
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Servers key is not an object, %d", json_typeof (servers_p));
				}

		}		/* if (servers_p) */
	else
		{
			inited_obj_p = json_object ();
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "No existing servers object, creating a new one");
		}

	if (providers_p -> pst_values_p)
		{
			json_decref (providers_p -> pst_values_p);
		}

	providers_p -> pst_values_p = inited_obj_p;


	return (providers_p -> pst_values_p != NULL);
}



void FreeProvidersStateTable (ProvidersStateTable *providers_p)
{
	json_decref (providers_p -> pst_values_p);
	FreeMemory (providers_p);
}


ProvidersStateTable *GetInitialisedProvidersStateTableForSingleService (const json_t *req_p, const char * const server_uri_s, const char * const service_s)
{
	ProvidersStateTable *providers_p = AllocateProvidersStateTable (req_p);

	if (providers_p)
		{
			if (AddToProvidersStateTable (providers_p, server_uri_s, service_s))
				{
					return providers_p;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\" : \"%s\" to ProvidersStateTable", server_uri_s, service_s);
				}

			FreeProvidersStateTable (providers_p);
		}		/* if (providers_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate ProvidersStateTable");
		}

	return NULL;
}


ProvidersStateTable *GetInitialisedProvidersStateTable (const json_t *req_p, LinkedList *services_p)
{
	ProvidersStateTable *providers_p = AllocateProvidersStateTable (req_p);

	if (providers_p)
		{
			bool success_flag = true;

			if (services_p)
				{
					success_flag = AddServicesListToProvidersStateTable (providers_p, services_p);
				}

			if (success_flag)
				{
					return providers_p;
				}

			FreeProvidersStateTable (providers_p);
		}		/* if (providers_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate ProvidersStateTable");
		}


	return NULL;
}


bool ReinitProvidersStateTable (ProvidersStateTable *providers_p, const json_t * const req_p, const char *server_uri_s, const char *service_name_s)
{
	if (ClearProvidersStateTable (providers_p))
		{
			if (InitProvidersStateTableFromRequest (providers_p, req_p))
				{
					if (AddToProvidersStateTable (providers_p, server_uri_s, service_name_s))
						{
							return true;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\":\"%s\" to ProvidersStateTable", server_uri_s, service_name_s);
						}

				}		/* if (InitProvidersStateTableFromRequest (providers_p, req_p)) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, req_p, "Failed to init ProvidersStateTable from request");
				}

		}		/* if (ClearProvidersStateTable (providers_p)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to clear ProvidersStateTable");
		}

	return false;
}


bool ClearProvidersStateTable (ProvidersStateTable *providers_p)
{
	bool success_flag = false;

	if (json_object_clear (providers_p -> pst_values_p) == 0)
		{
			success_flag = true;
		}
	else
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, providers_p -> pst_values_p, "Failed to clear ProvidersStateTable");
		}

	return success_flag;
}


bool AddServicesListToProvidersStateTable (ProvidersStateTable *providers_p, LinkedList *services_p)
{
	bool success_flag = false;
	const char *server_s = GetServerProviderURI ();

	if (server_s)
		{
			ServiceNode *node_p = (ServiceNode *) (services_p -> ll_head_p);

			success_flag = true;

			while (node_p && success_flag)
				{
					const char *service_s = GetServiceName (node_p -> sn_service_p);

					if (AddToProvidersStateTable (providers_p, server_s, service_s))
						{
							node_p = (ServiceNode *) (node_p -> sn_node.ln_next_p);
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s:%s to ProvidersStateTable", server_s, service_s);
							success_flag = false;
						}

				}		/* while (node_p && success_flag) */

		}		/* if (server_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetServerProviderName failed");
		}

	return success_flag;
}



bool AddToProvidersStateTable (ProvidersStateTable *providers_p, const char *server_uri_s, const char *service_s)
{
	bool success_flag;
	json_t *server_json_p = json_object_get (providers_p -> pst_values_p, server_uri_s);

	if (server_json_p)
		{
			if (json_object_get (server_json_p, service_s))
				{
					success_flag = true;
				}
			else
				{
					if (json_object_set_new (server_json_p, service_s, json_true ()) == 0)
						{
							success_flag = true;
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set %s -> %s in providers table", server_uri_s, service_s);
						}
				}
		}
	else
		{
			server_json_p = json_object ();

			if (server_json_p)
				{
					if (json_object_set_new (server_json_p, service_s, json_true ()) == 0)
						{
							if (json_object_set_new (providers_p -> pst_values_p, server_uri_s, server_json_p) == 0)
								{
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s to providers table", server_uri_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set %s -> %s in providers table", server_uri_s, service_s);
						}
				}
		}

	#if PROVIDERS_STATE_TABLE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Adding %s -> %s in providers table, %s", server_s, service_s, success_flag ? "true" : "false");
	#endif


	return success_flag;
}


bool IsServiceInProvidersStateTable (const ProvidersStateTable *providers_p, const char *server_uri_s, const char *service_s)
{
	bool success_flag = false;
	const json_t *server_p = json_object_get (providers_p -> pst_values_p, server_uri_s);

	if (server_p)
		{
			const json_t *service_p = json_object_get (server_p, service_s);

			if (service_p)
				{
					success_flag = true;
				}
		}

	#if PROVIDERS_STATE_TABLE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Checking for %s -> %s in providers table, %s", server_s, service_s, success_flag ? "true" : "false");
	#endif

	return success_flag;
}


json_t *GetProvidersStateTableAsJSON (const ProvidersStateTable *providers_p)
{
	return providers_p -> pst_values_p;
}



bool AddProvidersStateTableToRequest (const ProvidersStateTable * providers_p, json_t *req_p)
{
	bool success_flag = false;

	json_t *providers_json_p = GetProvidersStateTableAsJSON (providers_p);

	if (providers_json_p)
		{
			if (json_object_set (req_p, SERVERS_S, providers_json_p) == 0)
				{
					success_flag = true;
				}
			else
				{
					json_decref (providers_json_p);
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add providers json to request");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create providers json");
		}

	return success_flag;
}


json_t *GetAvailableServicesRequestForAllProviders (const ProvidersStateTable *providers_p, UserDetails *user_p, const SchemaVersion * const sv_p)
{
	json_t *op_p = GetAvailableServicesRequest (user_p, sv_p);

	if (op_p)
		{
			/*
			 * If there any providers that have already declared services,
			 * add them to the json
			 */
			if (providers_p)
				{
					if (AddProvidersStateTableToRequest (providers_p, op_p))
						{
							return op_p;
						}		/* if (AddProvidersStateTableToRequest (providers_p, op_p)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add providers json");
						}

				}		/* if (providers_p) */
			else
				{
					return op_p;
				}

			json_decref (op_p);
		}		/* if (op_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON for OP_LIST_ALL_SERVICES");
		}

	return NULL;
}

