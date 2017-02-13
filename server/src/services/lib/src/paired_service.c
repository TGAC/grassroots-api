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
 * paired_service.c
 *
 *  Created on: 1 Feb 2016
 *      Author: billy
 */

#include <string.h>

#include "paired_service.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "connection.h"
#include "json_tools.h"
#include "service.h"
#include "service_config.h"
#include "remote_service_job.h"


#ifdef USE_PTHREADS
#include "pthread.h"
#endif


#ifdef _DEBUG
	#define PAIRED_SERVICE_DEBUG	(STM_LEVEL_FINER)
#else
	#define PAIRED_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


PairedService *AllocatePairedService (const uuid_t id, const char *service_name_s, const char *uri_s, const char * const server_name_s, const json_t *op_p, const json_t *provider_p)
{
	if (service_name_s && uri_s && server_name_s)
		{
			if (op_p)
				{
					ParameterSet *param_set_p = CreateParameterSetFromJSON (op_p, false);

					if (param_set_p)
						{
							char *copied_name_s = CopyToNewString (service_name_s, 0, false);

							if (copied_name_s)
								{
									char *copied_uri_s = CopyToNewString (uri_s, 0, false);

									if (copied_uri_s)
										{
											char *copied_server_name_s = CopyToNewString (server_name_s, 0, false);

											if (copied_server_name_s)
												{
													json_t *copied_provider_p = json_deep_copy (provider_p);

													if (copied_provider_p)
														{
															PairedService *paired_service_p = (PairedService *) AllocMemory (sizeof (PairedService));

															if (paired_service_p)
																{
																	uuid_copy (paired_service_p -> ps_extenal_server_id, id);
																	paired_service_p -> ps_name_s = copied_name_s;
																	paired_service_p -> ps_server_name_s = copied_server_name_s;
																	paired_service_p -> ps_server_uri_s = copied_uri_s;
																	paired_service_p -> ps_params_p = param_set_p;
																	paired_service_p -> ps_provider_p = copied_provider_p;

																	#if PAIRED_SERVICE_DEBUG >= STM_LEVEL_FINER
																		{
																			char uuid_s [UUID_STRING_BUFFER_SIZE];

																			ConvertUUIDToString (paired_service_p -> ps_extenal_server_id, uuid_s);
																			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Added paired service at \"%s\" with name \"%s\" for server \"%s\"", paired_service_p -> ps_server_uri_s, paired_service_p -> ps_name_s, uuid_s);

																			ConvertUUIDToString (id, uuid_s);
																			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Original id \"%s\"", uuid_s);
																		}
																	#endif

																	return paired_service_p;
																}		/* if (paired_service_p) */

															json_decref (copied_provider_p);
														}		/* if (copied_provider_p) */

													FreeCopiedString (copied_server_name_s);
												}		/* copied_server_name_s */

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
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy name \"%s\"", service_name_s);
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
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Name \"%s\" and URI \"%s\" must exist", service_name_s ? service_name_s : "NULL", uri_s ? uri_s : "NULL");
		}

	return NULL;
}



void FreePairedService (PairedService *paired_service_p)
{
	if (paired_service_p -> ps_name_s)
		{
			FreeCopiedString (paired_service_p -> ps_name_s);
		}

	if (paired_service_p -> ps_server_name_s)
		{
			FreeCopiedString (paired_service_p -> ps_server_name_s);
		}


	if (paired_service_p -> ps_server_uri_s)
		{
			FreeCopiedString (paired_service_p -> ps_server_uri_s);
		}


	if (paired_service_p -> ps_params_p)
		{
			FreeParameterSet (paired_service_p -> ps_params_p);
		}

	if (paired_service_p -> ps_provider_p)
		{
			json_decref (paired_service_p -> ps_provider_p);
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



PairedServiceNode *AllocatePairedServiceNodeByParts (const uuid_t id, const char *name_s, const char *server_uri_s, const char *server_name_s, const json_t *op_p, const json_t *provider_p)
{
	PairedService *paired_service_p = AllocatePairedService (id, name_s, server_uri_s, server_name_s, op_p, provider_p);

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



json_t *MakeRemotePairedServiceCall (const char * const service_name_s, ParameterSet *params_p, const char * const paired_service_uri_s, ProvidersStateTable *providers_p)
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
					const SchemaVersion *sv_p = GetSchemaVersion ();
					json_t *service_req_p = GetServiceRunRequest (service_name_s, params_p, sv_p, true);

					if (service_req_p)
						{
							if (json_object_set_new (req_p, SERVICES_NAME_S, service_req_p) == 0)
								{
									bool success_flag = true;

									if (providers_p)
										{
											success_flag = AddProvidersStateTableToRequest (providers_p, req_p);

											if (!success_flag)
												{
													PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, req_p, "Failed to add providers request");
												}
										}

									if (success_flag)
										{
											res_p = MakeRemoteJsonCall (req_p, connection_p);

											if (!res_p)
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get JSON fragment from MakeRemoteJsonCall");
												}

										}		/* if (success_flag) */

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



int32 RunPairedServices (Service *service_p, ParameterSet *param_set_p, ProvidersStateTable *providers_p, bool (*save_job_fn) (RemoteServiceJob *job_p, const ServiceData *service_data_p))
{
	int num_services_ran = 0;

	/* Are there any remote jobs to run? */
	if (service_p -> se_paired_services.ll_size > 0)
		{
			const char *service_name_s = GetServiceName (service_p);

			if (service_name_s)
				{
					PairedServiceNode *node_p = (PairedServiceNode *) (service_p -> se_paired_services.ll_head_p);

					while (node_p)
						{
							PairedService *paired_service_p = node_p -> psn_paired_service_p;

							if (!IsServiceInProvidersStateTable (providers_p, paired_service_p -> ps_server_uri_s, service_name_s))
								{
									json_t *res_p = MakeRemotePairedServiceCall (GetServiceName (service_p), param_set_p, paired_service_p -> ps_server_uri_s, providers_p);

									if (res_p)
										{
											int res = AddRemoteResultsToServiceJobs (res_p, service_p -> se_jobs_p, paired_service_p -> ps_name_s, paired_service_p -> ps_server_uri_s, service_p -> se_data_p, save_job_fn);

											if (res >= 0)
												{
													num_services_ran += num_services_ran;

													#if BLAST_SERVICE_DEBUG >= STM_LEVEL_FINER
													PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Got " UINT32_FMT " results from \"%s\" at \"%s\"", res, paired_service_p -> ps_name_s, paired_service_p -> ps_server_uri_s);
													#endif
												}		/* if (res < 0) */
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Got " UINT32_FMT " error from \"%s\" at \"%s\"", res, paired_service_p -> ps_name_s, paired_service_p -> ps_server_uri_s);
												}

											json_decref (res_p);
										}		/* if (res_p) */

								}		/* if (IsServiceInProvidersStateTable (providers_p, paired_service_p -> ps_server_uri_s, service_name_s)) */

							node_p = (PairedServiceNode *) (node_p -> psn_node.ln_next_p);
						}		/* while (node_p) */

				}		/* if (service_name_s) */


		}		/* if (service_p -> se_paired_services.ll_size > 0) */

	return num_services_ran;
}


int32 AddRemoteResultsToServiceJobs (const json_t *server_response_p, ServiceJobSet *jobs_p, const char * const remote_service_s, const char * const remote_uri_s, const ServiceData *service_data_p, bool (*save_job_fn) (RemoteServiceJob *job_p, const ServiceData *service_data_p))
{
	int32 num_successful_runs = 0;

	if (server_response_p)
		{
			const json_t *service_results_p = json_object_get (server_response_p, SERVICE_RESULTS_S);

			if (service_results_p)
				{
					if (json_is_array (service_results_p))
						{
							size_t i;
							json_t *service_result_p;

							if (remote_service_s)
								{
									json_array_foreach (service_results_p, i, service_result_p)
										{
											const char *service_name_s = GetJSONString (service_result_p, SERVICE_NAME_S);

											if (service_name_s)
												{
													if (strcmp (service_name_s, remote_service_s) == 0)
														{
															OperationStatus status;

															if (GetStatusFromJSON (service_result_p, &status))
																{
																	switch (status)
																		{
																			case OS_SUCCEEDED:
																				{
																					/* Get the results and add them to our list of jobs */
																					json_t *results_p = json_object_get (service_result_p, SERVICE_RESULTS_S);

																					if (results_p)
																						{
																							if (json_is_array (results_p))
																								{
																									bool id_flag = false;
																									uuid_t remote_id;
																									size_t j;
																									json_t *job_json_p;
																									Service *service_p = jobs_p -> sjs_service_p;
																									const char *name_s = GetJSONString (service_result_p, JOB_NAME_S);
																									const char *description_s = GetJSONString (service_result_p, JOB_DESCRIPTION_S);
																									const char *remote_id_s = GetJSONString (service_result_p, JOB_UUID_S);

																									if (remote_id_s)
																										{
																											if (uuid_parse (remote_id_s, remote_id) == 0)
																												{
																													id_flag = true;
																												}
																										}

																									json_array_foreach (results_p, j, job_json_p)
																										{
																											RemoteServiceJob *job_p = CreateRemoteServiceJobFromResultsJSON (job_json_p, service_p, name_s, description_s, status);

																											if (job_p)
																												{
																													bool added_flag = false;
																													/*
																													 * Save the details to access the remote service with a file named using
																													 * the local uuid
																													 */
																													if ((job_p -> rsj_uri_s = CopyToNewString (remote_uri_s, 0, false)) != NULL)
																														{
																															if ((job_p -> rsj_service_name_s = CopyToNewString (remote_service_s, 0, false)) != NULL)
																																{
																																	if (id_flag)
																																		{
																																			uuid_copy (job_p -> rsj_job_id, remote_id);

																																			if (AddServiceJobToServiceJobSet (jobs_p, & (job_p -> rsj_job)))
																																				{
																																					if (!save_job_fn (job_p, service_data_p))
																																						{
																																							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to save remote info file for \"%s\"", remote_id_s);
																																						}

																																					added_flag = true;
																																					++ num_successful_runs;
																																				}
																																			else
																																				{
																																					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, service_results_p, "Failed to add ServiceJob to ServiceJobSet ");
																																				}

																																		}
																																}
																															else
																																{
																																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add remote service name \"%s\"", remote_service_s);
																																}		/* if (! (job_p -> bsj_job.sj_remote_uri_s)) */
																														}
																													else
																														{
																															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add remote uri \"%s\"", remote_uri_s);
																														}		/* if (! (job_p -> bsj_job.sj_remote_uri_s)) */

																													if (!added_flag)
																														{
																															FreeServiceJob (& (job_p -> rsj_job));
																														}

																												}		/* if (job_p) */
																											else
																												{
																													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, service_results_p, "Failed to create ServiceJob ");
																												}

																										}		/* json_array_foreach (results_p, j, job_json_p) */

																								}		/* if (json_is_array (results_p)) */

																						}		/* if (results_p) */
																					else
																						{
																							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, service_results_p, "Failed to get SERVICE_RESULTS_S ");
																						}

																				}		/* case OS_SUCCEEDED: */
																				break;

																			default:
																				break;
																		}		/* switch (status) */

																}		/* if (GetStatusFromJSON (service_results_p, &status)) */
															else
																{
																	PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, service_results_p, "Failed to get OperationStatus");
																}

														}		/* if (strcmp (service_name_s = service_p -> ps_name_s) == 0) */
												}		/* if (service_name_s) */
											else
												{
													PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, service_results_p, "Failed to get service name");
												}
										}		/* json_array_foreach (server_response_p, i, service_results_p) */

								}		/* if (service_p -> ps_name_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "PairedService for \"%s\" has no name set", remote_uri_s);
								}

						}		/* if (json_is_array (server_response_p)) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, server_response_p, "Service results is not an array");
						}

				}		/* if (results_p) */

		}		/* if (server_response_p) */

	return num_successful_runs;
}


