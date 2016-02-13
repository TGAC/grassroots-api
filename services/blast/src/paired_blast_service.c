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
#include "service_job.h"
#include "blast_service_job.h"


#ifdef _DEBUG
	#define PAIRED_BLAST_SERVICE_DEBUG	(STM_LEVEL_FINER)
#else
	#define PAIRED_BLAST_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


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
											uint32 i = db_group_p -> pg_num_params;
											tag += db_counter;
											memset (&def, 0, sizeof (SharedType));

											while (i > 0)
												{
													/* Add the database to our list */
													Parameter *external_param_p = *src_param_pp;
													Parameter *param_p = NULL;

													def.st_boolean_value = external_param_p -> pa_current_value.st_boolean_value;

													param_p = CreateAndAddParameterToParameterSet (internal_params_p, PT_BOOLEAN, false, external_param_p -> pa_name_s, external_param_p -> pa_display_name_s, external_param_p -> pa_description_s, tag, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ALL, NULL);

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
													-- i;
												}		/* while (i > 0) */

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


int32 AddRemoteResultsToServiceJobs (const json_t *server_response_p, ServiceJobSet *jobs_p, PairedService *paired_service_p)
{
	int32 num_successful_runs = 0;

	if (server_response_p)
		{
			if (json_is_array (server_response_p))
				{
					size_t i;
					json_t *service_results_p;

					if (paired_service_p -> ps_name_s)
						{
							json_array_foreach (server_response_p, i, service_results_p)
								{
									const char *service_name_s = GetJSONString (service_results_p, SERVICE_NAME_S);

									if (service_name_s)
										{
											if (strcmp (service_name_s, paired_service_p -> ps_name_s) == 0)
												{
													OperationStatus status;

													if (GetStatusFromJSON (service_results_p, &status))
														{
															switch (status)
																{
																	case OS_SUCCEEDED:
																		{
																			/* Get the results and add them to our list of jobs */
																			json_t *results_p = json_object_get (service_results_p, SERVICE_RESULTS_S);

																			if (results_p)
																				{
																					if (json_is_array (results_p))
																						{
																							size_t j;
																							json_t *job_json_p;
																							Service *service_p = jobs_p -> sjs_service_p;
																							const char *name_s = GetJSONString (service_results_p, JOB_NAME_S);
																							const char *description_s = GetJSONString (service_results_p, JOB_DESCRIPTION_S);
																							const char *remote_id_s = GetJSONString (service_results_p, JOB_UUID_S);

																							json_array_foreach (results_p, j, job_json_p)
																								{
																									BlastServiceJob *job_p = CreateBlastServiceJobFromResultsJSON (job_json_p, service_p, name_s, description_s, status, NULL);

																									if (job_p)
																										{
																											job_p -> bsj_job.sj_remote_uri_s = CopyToNewString (paired_service_p -> ps_uri_s, 0, false);

																											if (! (job_p -> bsj_job.sj_remote_uri_s))
																												{
																													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add remote uri \"%s\"", paired_service_p -> ps_uri_s);
																												}		/* if (! (job_p -> bsj_job.sj_remote_uri_s)) */

																											if (remote_id_s)
																												{
																													if (uuid_parse (remote_id_s, job_p -> bsj_job.sj_id) != 0)
																														{
																															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to generate uuid from \"%s\"", remote_id_s);
																														}		/* if (uuid_parse (remote_id_s, job_p -> bsj_job.sj_id) != 0) */

																												}		/* if (remote_id_s) */


																											if (AddServiceJobToServiceJobSet (jobs_p, & (job_p -> bsj_job)))
																												{
																													++ num_successful_runs;
																												}
																											else
																												{
																													PrintJSONToErrors (service_results_p, "Failed to add ServiceJob to ServiceJobSet ", STM_LEVEL_SEVERE, __FILE__, __LINE__);
																													FreeBlastServiceJob (& (job_p -> bsj_job));
																												}

																										}		/* if (job_p) */
																									else
																										{
																											PrintJSONToErrors (service_results_p, "Failed to create ServiceJob ", STM_LEVEL_SEVERE, __FILE__, __LINE__);
																										}

																								}		/* json_array_foreach (results_p, j, job_json_p) */

																						}		/* if (json_is_array (results_p)) */

																				}		/* if (results_p) */
																			else
																				{
																					PrintJSONToErrors (service_results_p, "Failed to get SERVICE_RESULTS_S ", STM_LEVEL_SEVERE, __FILE__, __LINE__);
																				}

																		}		/* case OS_SUCCEEDED: */
																		break;

																	default:
																		break;
																}		/* switch (status) */

														}		/* if (GetStatusFromJSON (service_results_p, &status)) */
													else
														{
															PrintJSONToErrors (service_results_p, "Failed to get OperationStatus", STM_LEVEL_WARNING, __FILE__, __LINE__);
														}

												}		/* if (strcmp (service_name_s = service_p -> ps_name_s) == 0) */
										}		/* if (service_name_s) */
									else
										{
											PrintJSONToErrors (service_results_p, "Failed to get service name", STM_LEVEL_WARNING, __FILE__, __LINE__);
										}
								}		/* json_array_foreach (server_response_p, i, service_results_p) */

						}		/* if (service_p -> ps_name_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "PairedService for \"%s\" has no name set", paired_service_p -> ps_uri_s);
						}

				}		/* if (json_is_array (server_response_p)) */
			else
				{
					PrintJSONToErrors (server_response_p, "Service response is not an array", STM_LEVEL_WARNING, __FILE__, __LINE__);
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
			json_t *req_p = json_object ();

			if (req_p)
				{
					/*
					 * Only send the databases that the external paired service knows about
					 */
					const char *service_name_s = GetServiceName (service_p);
					json_t *service_req_p = GetServiceRunRequest (service_name_s, params_p, true);

					if (service_req_p)
						{
							if (json_object_set_new (req_p, SERVICES_NAME_S, service_req_p) == 0)
								{
									json_t *res_p = MakeRemoteJsonCall (req_p, connection_p);

									if (res_p)
										{
											num_successful_runs = AddRemoteResultsToServiceJobs (res_p, jobs_p, paired_service_p);
										}		/* if (res_p) */
									else
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
	char *group_name_s = ConcatenateVarargsStrings (BS_DATABASE_GROUP_NAME_S, " provided by ", paired_service_p -> ps_name_s, " at ", paired_service_p -> ps_uri_s, NULL);

	if (group_name_s)
		{
			#if PAIRED_BLAST_SERVICE_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Created group name \"%s\" for \"%s\" and \"%s\"", group_name_s, paired_service_p -> ps_name_s, paired_service_p -> ps_uri_s);
			#endif
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create group name for \"%s\" and \"%s\"", paired_service_p -> ps_name_s, paired_service_p -> ps_uri_s);
		}

	return group_name_s;
}
