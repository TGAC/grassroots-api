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
 * paired_blast_service.c
 *
 *  Created on: 9 Feb 2016
 *      Author: billy
 */
#include <string.h>

#include "typedefs.h"
#include "paired_blast_service.h"
#include "blast_service.h"
#include "servers_pool.h"
#include "parameter.h"
#include "parameter_group.h"
#include "json_tools.h"



static char *CreateGroupName (PairedService *paired_service_p);

/**
 * Add a database if it is a non-database parameter or if
 * refers to a database at the paired service sent in as data_p.
 *
 * @param param_p
 * @param data_p
 * @return
 */
static bool AddRemoteServiceParametersToJSON (const Parameter *param_p, void *data_p);



/***********************************/
/******** PUBLIC FUNCTIONS *********/
/***********************************/


bool AddPairedServiceParameters (Service *service_p, ParameterSet *internal_params_p, uint16 db_counter)
{
	bool success_flag = true;

	if (service_p -> se_paired_services.ll_size > 0)
		{
			ServersManager *servers_manager_p = GetServersManager ();
			PairedServiceNode *node_p = (PairedServiceNode *) (service_p -> se_paired_services.ll_head_p);

			while (node_p)
				{
					/*
					 * Try and add the external server's databases
					 */
					PairedService *paired_service_p = node_p -> psn_paired_service_p;
					ExternalServer *external_server_p = GetExternalServerFromServersManager (servers_manager_p, paired_service_p -> ps_uri_s, NULL);

					if (external_server_p)
						{
							ParameterGroup *db_group_p = GetParameterGroupFromParameterSetByGroupName (paired_service_p -> ps_params_p, BS_DATABASE_GROUP_NAME_S);

							if (db_group_p)
								{
									if (db_group_p -> pg_num_params > 0)
										{
											Parameter **src_param_pp = db_group_p -> pg_params_pp;
											Parameter **dest_params_pp = (Parameter **) AllocMemoryArray (1 + (db_group_p -> pg_num_params), sizeof (Parameter *));

											if (dest_params_pp)
												{
													SharedType def;
													Parameter **dest_param_pp = dest_params_pp;
													char *group_name_s = NULL;
													uint32 tag = MAKE_TAG ('D', 'B', 0, 1);
													uint32 num_added_dbs = 0;

													tag += db_counter;
													memset (&def, 0, sizeof (SharedType));

													while (*src_param_pp)
														{
															/* Add the database to our list */
															Parameter *external_param_p = *src_param_pp;
															Parameter *param_p = NULL;

															def.st_boolean_value = external_param_p -> pa_current_value.st_boolean_value;

															param_p = CreateAndAddParameterToParameterSet (internal_params_p, PT_BOOLEAN, false, external_param_p -> pa_name_s, external_param_p -> pa_description_s, external_param_p -> pa_name_s, tag, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ALL, NULL);

															if (param_p)
																{
																	if (CopyRemoteParameterDetails (external_param_p, param_p))
																		{
																			if (dest_param_pp)
																				{
																					*dest_param_pp = param_p;
																					++ dest_param_pp;
																				}		/* if (dest_param_pp) */

																		}		/* if (CopyRemoteParameterDetails (external_param_p, param_p)) */
																	else
																		{

																		}

																	++ num_added_dbs;
																	++ db_counter;
																}		/* if (param_p) */

															++ src_param_pp;
														}		/* while (*src_param_pp) */

													group_name_s = CreateGroupName (paired_service_p);

													if (group_name_s)
														{
															if (!AddParameterGroupToParameterSet (internal_params_p, group_name_s, paired_service_p -> ps_uri_s, dest_params_pp, num_added_dbs))
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", BS_DATABASE_GROUP_NAME_S);
																	FreeMemory (dest_params_pp);
																}

															FreeCopiedString (group_name_s);
														}		/* if (group_name_s) */

												}		/* if (grouped_params_pp) */

										}		/* if (num_dbs > 0) */

								}		/* if (db_group_p) */

						}		/* if (external_server_p) */

					node_p = (PairedServiceNode *) (node_p -> psn_node.ln_next_p);
				}		/* while (node_p) */

		}		/* if (service_p -> se_paired_services.ll_size > 0) */

	return success_flag;
}


json_t *PrepareRemoteJobsForRunning (Service *service_p, ParameterSet *params_p, PairedService *paired_service_p)
{
	json_t *params_json_p = GetParameterSetSelectionAsJSON (params_p, false, paired_service_p, AddRemoteServiceParametersToJSON);

	return params_json_p;
}


static PrintJSONToErrors (const json_t *json_p, const char *prefix_s)
{
	char *dump_s = json_dumps (service_results_p, 0);

	if (dump_s)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get OperationStatus from \"%s\"", dump_s);
			free (dump_s);
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get OperationStatus");
		}

	PrintJSONToLog ();

}


int32 AddRemoteResultsToServiceJobs (const json_t *server_response_p, ServiceJobSet *jobs_p, PairedService *service_p)
{
	int32 num_successful_runs = 0;

	if (server_response_p)
		{
			if (json_is_array (server_response_p))
				{
					size_t i;
					json_t *service_results_p;

					json_array_foreach (server_response_p, i, service_results_p)
						{
							const char *service_name_s = GetJSONString (service_results_p, SERVICE_NAME_S);
	
							if (service_name_s)
								{
									if (strcmp (service_name_s = service_p -> ps_name_s) == 0)
										{
											OperationStatus status;

											if (GetStatusFromJSON (service_results_p, &status))
												{
													switch (status)
														{
															case OS_SUCCEEDED:
																{

																}		/* case OS_SUCCEEDED: */
																break;

															default:
																break;
														}		/* switch (status) */

												}		/* if (GetStatusFromJSON (service_results_p, &status)) */
											else
												{
													PrintJSONToError (service_results_p, "Failed to get OperationStatus", STM_LEVEL_WARNING, __FILE__, __LINE__);
												}

										}		/* if (strcmp (service_name_s = service_p -> ps_name_s) == 0) */
								}		/* if (service_name_s) */
							else
								{
									PrintJSONToError (service_results_p, "Failed to get service name", STM_LEVEL_WARNING, __FILE__, __LINE__);
								}
						}		/* json_array_foreach (server_response_p, i, service_results_p) */

				}		/* if (json_is_array (server_response_p)) */
			else
				{
					PrintJSONToError (server_response_p, "Service response is not an array", STM_LEVEL_WARNING, __FILE__, __LINE__);
				}

		}		/* if (server_response_p) */

	return num_successful_runs;
}


int32 RunRemoteBlastJobs (Service *service_p, ServiceJobSet *jobs_p, ParameterSet *params_p, PairedService *paired_service_p)
{
	int32 num_successful_runs = -1;
	Connection *connection_p = AllocateWebServerConnection (paired_service_p -> ps_uri_s);

	if (connection_p)
		{
			/*
			 * Only send the databases that the external paired service knows about
			 */
			json_t *req_p = GetServiceRunRequest (service_p, params_p, true);

			if (req_p)
				{
					json_t *res_p = MakeRemoteJsonCall (req_p, connection_p);

					if (res_p)
						{
							num_successful_runs = AddRemoteResultsToServiceJobs (res_p, jobs_p);
						}		/* if (res_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON fragment from MakeRemoteJsonCall");
						}

					json_decref (req_p);
				}		/* if (req_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON fragment from GetServiceRunRequest");
				}

			FreeConnection (connection_p);
		}		/* if (connection_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create connection to paired service at \"%s\"", paired_service_p -> ps_uri_s);
		}

	return num_successful_runs;
}


/***********************************/
/******** STATIC FUNCTIONS *********/
/***********************************/


/**
 * Add a database if it is a non-database parameter or if
 * refers to a database at the paired service sent in as data_p.
 *
 * @param param_p
 * @param data_p
 * @return
 */
static bool AddRemoteServiceParametersToJSON (const Parameter *param_p, void *data_p)
{
	bool add_flag = true;
	PairedService *paired_service_p = (PairedService *) data_p;

	if (param_p -> pa_group_p)
		{
			char *paired_service_group_name_s = CreateGroupName (paired_service_p);

			if (paired_service_group_name_s)
				{
					const char *param_group_name_s = param_p -> pa_group_p -> pg_name_s;

					if (strcmp (paired_service_group_name_s, param_group_name_s) == 0)
						{
							add_flag = true;
						}

					FreeCopiedString (paired_service_group_name_s);
				}		/* if (paired_service_group_name_s) */

		}		/* if (param_p -> pa_group_p) */

	return add_flag;
}


static char *CreateGroupName (PairedService *paired_service_p)
{
	return ConcatenateVarargsStrings (BS_DATABASE_GROUP_NAME_S, " provided by ", paired_service_p -> ps_name_s, " at ", paired_service_p -> ps_uri_s, NULL);
}
