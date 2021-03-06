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

#include <string.h>

#include "linked_service.h"
#include "memory_allocations.h"
#include "streams.h"

#include "json_util.h"
#include "string_utils.h"
#include "service.h"
#include "schema_version.h"
#include "user_details.h"
#include "service_config.h"
#include "service_job.h"
//#include "resource.h"


LinkedService *AllocateLinkedService (const char *input_service_s, const char *input_key_s)
{
	char *input_service_copy_s = CopyToNewString (input_service_s, 0, false);

	if (input_service_copy_s)
		{
			bool continue_flag = true;
			char *input_key_copy_s = NULL;


			if (input_key_s)
				{
					input_key_copy_s = CopyToNewString (input_key_s, 0, false);

					if (!input_key_copy_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy input key \"%s\"", input_key_s);
							continue_flag = false;
						}
				}

			if (continue_flag)
				{
					LinkedList *list_p = AllocateLinkedList (FreeMappedParameterNode);

					if (list_p)
						{
							LinkedService *linked_service_p = (LinkedService *) AllocMemory (sizeof (LinkedService));

							if (linked_service_p)
								{
									linked_service_p -> ls_output_service_s = input_service_copy_s;
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

				}		/* if (input_key_copy_s) */

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
	if (linked_service_p -> ls_output_service_s)
		{
			FreeCopiedString (linked_service_p -> ls_output_service_s);
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
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate LinkedServiceNode for \"%s\"", linked_service_p -> ls_output_service_s);
		}

	return node_p;
}



void FreeLinkedServiceNode (ListItem *node_p)
{
	LinkedServiceNode *ls_node_p = (LinkedServiceNode *) node_p;

	FreeLinkedService (ls_node_p -> lsn_linked_service_p);
	FreeMemory (node_p);
}


bool AddLinkedServiceToRequestJSON (json_t *request_p, LinkedService *linked_service_p, ParameterSet *output_params_p)
{
	bool success_flag = false;
	json_t *linked_services_array_p = json_object_get (request_p, LINKED_SERVICES_S);

	if (!linked_services_array_p)
		{
			linked_services_array_p = json_array ();

			if (linked_services_array_p)
				{
					if (json_object_set (request_p, LINKED_SERVICES_S, linked_services_array_p) != 0)
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, request_p, "Failed to add empty linked services array for \"%s\"", LINKED_SERVICES_S);
							json_decref (linked_services_array_p);
							linked_services_array_p = NULL;
						}
				}

		}

	if (linked_services_array_p)
		{
			json_t *wrapper_p = json_object ();

			if (wrapper_p)
				{
					json_t *run_service_p = GetInterestedServiceJSON (linked_service_p -> ls_output_service_s, NULL, output_params_p, false);

					if (run_service_p)
						{
							if (json_object_set_new (wrapper_p, SERVICES_NAME_S, run_service_p) == 0)
								{
									if (json_array_append_new (linked_services_array_p, wrapper_p) == 0)
										{
											success_flag = true;
										}
								}
							else
								{
									json_decref (run_service_p);
								}
						}

					if (!success_flag)
						{
							json_decref (wrapper_p);
						}
				}


		}		/* if (linked_services_array_p) */

	return success_flag;
}


MappedParameter *GetMappedParameterByInputParamName (const LinkedService *linked_service_p, const char * const name_s)
{
	MappedParameterNode *param_node_p = (MappedParameterNode *) (linked_service_p -> ls_mapped_params_p -> ll_head_p);

	while (param_node_p)
		{
			MappedParameter *mapped_param_p = param_node_p -> mpn_mapped_param_p;

			if (strcmp (mapped_param_p -> mp_input_param_s, name_s) == 0)
				{
					return mapped_param_p;
				}

			param_node_p = (MappedParameterNode *) param_node_p -> mpn_node.ln_next_p;
		}

	return NULL;
}



bool ProcessLinkedService (LinkedService *linked_service_p, ServiceJob *job_p)
{
	bool success_flag = false;

	/* Does the input service have a custom process function? */
	if (job_p -> sj_service_p -> se_process_linked_services_fn)
		{
			success_flag = job_p -> sj_service_p -> se_process_linked_services_fn (job_p -> sj_service_p, job_p, linked_service_p);
		}

	return success_flag;
}


LinkedService *CreateLinkedServiceFromJSON (const json_t *linked_service_json_p)
{
	LinkedService *linked_service_p = NULL;
	const char *service_s = GetJSONString (linked_service_json_p, SERVICE_NAME_S);

	if (service_s)
		{
			const json_t *params_p = json_object_get (linked_service_json_p, PARAM_SET_PARAMS_S);

			if (params_p)
				{
					if (json_is_object (params_p))
						{
							const char *input_root_s = GetJSONString (params_p, MAPPED_PARAMS_ROOT_S);
							json_t *mapped_params_json_p = json_object_get (params_p, MAPPED_PARAMS_LIST_S);

							if (mapped_params_json_p)
								{
									linked_service_p = AllocateLinkedService (service_s, input_root_s);

									if (linked_service_p)
										{
											bool success_flag = true;
											const size_t size = json_array_size (mapped_params_json_p);
											size_t i;

											for (i = 0; i < size; ++ i)
												{
													const json_t *mapped_param_json_p = json_array_get (mapped_params_json_p, i);
													MappedParameter *mapped_param_p = CreateMappedParameterFromJSON (mapped_param_json_p);

													if (mapped_param_p)
														{
															if (!AddMappedParameterToLinkedService (linked_service_p, mapped_param_p))
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add mapped param \"%s\":\"%s\" for \"%s\"", mapped_param_p -> mp_input_param_s, mapped_param_p -> mp_output_param_s, linked_service_p -> ls_output_service_s);
																	i = size; 		/* force exit from loop */
																	success_flag = false;
																}		/* if (!AddMappedParameterToLinkedService (linked_service_p, mapped_param_p)) */

														}		/* if (mapped_param_p) */
													else
														{
															PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, mapped_param_json_p, "Failed to create mapped parameter");
															i = size; 		/* force exit from loop */
															success_flag = false;
														}

												}		/* for (i = 0; i < size; ++ i) */

											if (!success_flag)
												{
													FreeLinkedService (linked_service_p);
													linked_service_p = NULL;
												}

										}		/* if (linked_service_p) */

								}		/* if (mapped_params_json_p) */


						}		/* if (json_is_object (params_p)) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, params_p, "Fragment is not a JSON object");
						}

				}		/* if (params_p) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, linked_service_json_p, "Failed to get \"%s\"", PARAM_SET_PARAMS_S);
				}

		}		/* if (service_s) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, linked_service_json_p, "Failed to get %s", SERVICE_NAME_S);
		}

	return linked_service_p;
}


bool CreateAndAddMappedParameterToLinkedService (LinkedService *linked_service_p, const char *input_s, const char *output_s, bool required_flag, bool multi_flag)
{
	MappedParameter *mapped_param_p = AllocateMappedParameter (input_s, output_s, required_flag, multi_flag);

	if (mapped_param_p)
		{
			if (AddMappedParameterToLinkedService (linked_service_p, mapped_param_p))
				{
					return true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add MappedParameter for \"%s\"=\"%s\" to \"%s\"", input_s, output_s, linked_service_p -> ls_output_service_s);
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
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory of MappedParameterNode for \"%s\"=\"%s\" to \"%s\"", mapped_param_p -> mp_input_param_s, mapped_param_p -> mp_output_param_s, linked_service_p -> ls_output_service_s);
		}

	return success_flag;
}



json_t *GetLinkedServiceAsJSON (LinkedService *linked_service_p)
{
	json_t *res_p = NULL;
	json_error_t json_err;

	res_p = json_pack_ex (&json_err, 0, "{s:s,s:b}", JOB_SERVICE_S, linked_service_p -> ls_output_service_s, SERVICE_RUN_S, true);

	if (res_p)
		{
			const SchemaVersion *sv_p = GetSchemaVersion ();
			Service *service_p = GetServiceByName (linked_service_p -> ls_output_service_s);

			if (service_p)
				{
					Resource *resource_p = NULL;
					UserDetails *user_p = NULL;
					ParameterSet *params_p = GetServiceParameters (service_p, resource_p, user_p);

					if (params_p)
						{
							json_t *params_json_p = GetParameterSetAsJSON (params_p, sv_p, false);

							if (params_json_p)
								{
									if (!json_object_set_new (res_p, PARAM_SET_KEY_S, params_json_p) == 0)
										{
											json_decref (params_json_p);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add parameters JSON to InterestedServiceJSON");
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetParameterSetAsJSON failed");
								}

							ReleaseServiceParameters (service_p, params_p);
						}		/* if (params_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetServiceParameters failed for \"%s\"", GetServiceName (service_p));
						}

					FreeService (service_p);
				}		/* if (service_p) */
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetInterestedServiceJSON failed, %s", json_err.text);
		}

	return res_p;
}

