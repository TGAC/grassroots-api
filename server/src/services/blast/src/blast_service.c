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
#include <string.h>

#define ALLOCATE_BLAST_SERVICE_CONSTANTS (1)
#include "base_blast_service.h"

#include "nucleotide_blast_service.h"
#include "protein_blast_service.h"

#include "memory_allocations.h"

#include "service_job_set_iterator.h"
#include "string_utils.h"
#include "grassroots_config.h"
#include "temp_file.hpp"
#include "json_tools.h"
#include "blast_formatter.h"
#include "blast_service_job.h"
#include "paired_blast_service.h"
#include "blast_service_params.h"
#include "blast_tool_factory.hpp"
#include "jobs_manager.h"
#include "blast_service_job.h"
#include "blast_service_params.h"

#include "servers_pool.h"
#include "remote_parameter_details.h"




/***************************************/

static bool InitBlastService (Service *blast_service_p, ServicesArray *services_p, size_t *index_p);



/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *  UNUSED_PARAM (config_p))
{
	ServicesArray *services_p = NULL;
	Service *nucleotide_service_p = GetNucleotideBlastService ();
	Service *protein_service_p = GetProteinBlastService ();
	size_t num_services = 0;

	if (nucleotide_service_p)
		{
			++ num_services;
		}

	if (protein_service_p)
		{
			++ num_services;
		}


	services_p = AllocateServicesArray (num_services);

	if (services_p)
		{
			bool success_flag = true;
			size_t i = 0;

			if (nucleotide_service_p && success_flag)
				{
					success_flag = InitBlastService (nucleotide_service_p, services_p, &i);
				}

			if (protein_service_p && success_flag)
				{
					success_flag = InitBlastService (protein_service_p, services_p, &i);
				}

			if (!success_flag)
				{

				}

		}		/* if (services_p) */
	else
		{

			if (nucleotide_service_p)
				{
					FreeService (nucleotide_service_p);
				}

			if (protein_service_p)
				{
					FreeService (protein_service_p);
				}
		}

	return services_p;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


/*
 * STATIC FUNCTIONS 
 */


static bool InitBlastService (Service *blast_service_p, ServicesArray *services_p, size_t *index_p)
{
	bool success_flag = true;

	blast_service_p -> se_synchronous_flag = IsBlastToolFactorySynchronous (((BlastServiceData *) (blast_service_p -> se_data_p)) -> bsd_tool_factory_p);

	blast_service_p -> se_deserialise_job_json_fn = BuildBlastServiceJob;
	blast_service_p -> se_serialise_job_json_fn = BuildBlastServiceJobJSON;

	blast_service_p -> se_get_value_from_job_fn = GetValueFromBlastServiceJobOutput;

	if (*index_p < services_p -> sa_num_services)
		{
			* ((services_p -> sa_services_pp) + (*index_p)) = blast_service_p;
			++ (*index_p);
		}
	else
		{
			success_flag = false;
		}

	return success_flag;
}



void ReleaseBlastServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}




static ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *  UNUSED_PARAM (user_p), ProvidersStateTable *providers_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	SharedType param_value;

	memset (&param_value, 0, sizeof (SharedType));

	/* Are we retrieving previously run jobs? */
	if ((GetParameterValueFromParameterSet (param_set_p, BS_JOB_ID.npt_name_s, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s)))
		{
			service_p -> se_jobs_p  = CreateJobsForPreviousResults (param_set_p, param_value.st_string_value_s, blast_data_p);
		}		/* if ((GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_JOB_ID, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s))) */
	else
		{
			service_p -> se_jobs_p = AllocateServiceJobSet (service_p);

			if (service_p -> se_jobs_p)
				{
					/* Get all of the selected databases and create a BlastServiceJob for each one */
					PrepareBlastServiceJobs (blast_data_p -> bsd_databases_p, false, param_set_p, service_p -> se_jobs_p, blast_data_p);

					if (GetServiceJobSetSize (service_p -> se_jobs_p) > 0)
						{
							/*
							 * Get the absolute path and filename stem e.g.
							 *
							 *  file_stem_s = /usr/foo/bar/job_id
							 *
							 *  which will be used to build the input, output and other associated filenames e.g.
							 *
							 *  input file = file_stem_s + ".input"
							 *  output_file = file_stem_s + ".output"
							 *
							 *  As each job will have the same input file name it using the first job's id
							 *
							 */
							ServiceJobSetIterator iterator;
							BlastServiceJob *job_p = NULL;

							InitServiceJobSetIterator (&iterator, service_p -> se_jobs_p);
							job_p = (BlastServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);

							if (job_p)
								{
									TempFile *input_p = GetInputTempFile (param_set_p, blast_data_p -> bsd_working_dir_s, job_p -> bsj_job.sj_id);

									if (input_p)
										{
											const char *input_filename_s = input_p -> GetFilename ();

											if (input_filename_s)
												{
													bool loop_flag = true;

													while (loop_flag)
														{
															BlastTool *tool_p = job_p -> bsj_tool_p;

															job_p -> bsj_job.sj_status = OS_FAILED_TO_START;

															if (tool_p)
																{
																	if (tool_p -> SetInputFilename (input_filename_s))
																		{
																			if (tool_p -> SetUpOutputFile ())
																				{
																					if (tool_p -> ParseParameters (param_set_p))
																						{
																							if (RunBlast (tool_p))
																								{
																									/* If the status needs updating, refresh it */
																									if ((job_p -> bsj_job.sj_status == OS_PENDING) || (job_p -> bsj_job.sj_status == OS_STARTED))
																										{
																											job_p -> bsj_job.sj_status = tool_p -> GetStatus ();
																										}

																									switch (job_p -> bsj_job.sj_status)
																										{
																											case OS_SUCCEEDED:
																											case OS_PARTIALLY_SUCCEEDED:
																												if (!DetermineBlastResult (service_p, job_p))
																													{
																														char job_id_s [UUID_STRING_BUFFER_SIZE];

																														ConvertUUIDToString (job_p -> bsj_job.sj_id, job_id_s);
																														PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get results for %s", job_id_s);
																													}
																												break;

																											case OS_PENDING:
																											case OS_STARTED:
																												{
																													JobsManager *jobs_manager_p = GetJobsManager ();

																													if (jobs_manager_p)
																														{
																															if (!AddServiceJobToJobsManager (jobs_manager_p, job_p -> bsj_job.sj_id, (ServiceJob *) job_p))
																																{
																																	char job_id_s [UUID_STRING_BUFFER_SIZE];

																																	ConvertUUIDToString (job_p -> bsj_job.sj_id, job_id_s);
																																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job \"%s\" to JobsManager", job_id_s);
																																}
																														}
																												}
																												break;

																											default:
																												break;
																										}		/* switch (job_p -> bsj_job.sj_status) */

																								}
																							else
																								{
																									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to run blast tool \"%s\"", job_p -> bsj_job.sj_name_s);
																								}

																						}		/* if (tool_p -> ParseParameters (param_set_p, input_filename_s)) */
																					else
																						{
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to parse parameters for blast tool \"%s\"", job_p -> bsj_job.sj_name_s);
																						}

																				}		/* if (tool_p -> SetOutputFilename ()) */
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set output filename for blast tool \"%s\"", job_p -> bsj_job.sj_name_s);
																				}

																		}		/* if (tool_p -> SetInputFilename (input_filename_s)) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set input filename for blast tool \"%s\" to \"%s\"", job_p -> bsj_job.sj_name_s, input_filename_s);
																		}

																}		/* if (tool_p) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast tool for \"%s\"", job_p -> bsj_job.sj_name_s);
																}

															job_p = (BlastServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);
															loop_flag = (job_p != NULL);

														}		/* while (loop_flag) */

												}		/* if (input_filename_s) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get input filename for blast tool \"%s\"", job_p -> bsj_job.sj_name_s);
												}

											delete input_p;
										}		/* if (input_p) */
									else
										{
											const char * const error_s = "Failed to create input temp file for blast tool";
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create input temp file for blast tool \"%s\" in \"%s\"", job_p -> bsj_job.sj_name_s, blast_data_p -> bsd_working_dir_s);

											/* Since we couldn't save the input sequence, all jobs need to be set to have an error status */
											if (!AddErrorToServiceJob (& (job_p -> bsj_job), JOB_ERRORS_S, error_s))
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add error to job", job_p -> bsj_job.sj_name_s);
												}

											while ((job_p = (BlastServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator)) != NULL)
												{
													if (!AddErrorToServiceJob (& (job_p -> bsj_job), JOB_ERRORS_S, error_s))
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add error to job", job_p -> bsj_job.sj_name_s);
														}
												}
										}

								}		/* if (job_p) */

						}		/* if (GetServiceJobSetSize (service_p -> se_jobs_p) > 0) */

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
													int32 res = RunRemoteBlastJobs (service_p, service_p -> se_jobs_p, param_set_p, paired_service_p, providers_p);

													if (res >= 0)
														{
															#if BLAST_SERVICE_DEBUG >= STM_LEVEL_FINER
															PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Got " UINT32_FMT " results from \"%s\" at \"%s\"", res, paired_service_p -> ps_name_s, paired_service_p -> ps_server_uri_s);
															#endif
														}		/* if (res < 0) */
													else
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Got " UINT32_FMT " error from \"%s\" at \"%s\"", res, paired_service_p -> ps_name_s, paired_service_p -> ps_server_uri_s);
														}


												}		/* if (IsServiceInProvidersStateTable (providers_p, paired_service_p -> ps_server_uri_s, service_name_s)) */

											node_p = (PairedServiceNode *) (node_p -> psn_node.ln_next_p);
										}		/* while (node_p) */

								}		/* if (service_name_s) */


						}		/* if (service_p -> se_paired_services.ll_size > 0) */


					if (GetServiceJobSetSize (service_p -> se_jobs_p) == 0)
						{
							PrintErrors (STM_LEVEL_INFO, __FILE__, __LINE__, "No jobs specified");
						}

				}		/* if (service_p -> se_jobs_p) */

		}		/* if ((GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_JOB_ID, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s))) else */

	return service_p -> se_jobs_p;
}
