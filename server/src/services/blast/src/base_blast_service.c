/*
 * base_blast_service.c
 *
 *  Created on: 13 Oct 2016
 *      Author: billy
 */


#include <string.h>


#include "base_blast_service.h"

#include "../include/blast_app_parameters.h"
#include "blast_formatter.h"
#include "blast_service_params.h"
#include "paired_blast_service.h"
#include "jobs_manager.h"
#include "string_utils.h"
#include "blast_tool_factory.hpp"
#include "service_job_set_iterator.h"


ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *  UNUSED_PARAM (user_p), ProvidersStateTable *providers_p, BlastAppParameters *app_params_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	SharedType param_value;

	memset (&param_value, 0, sizeof (SharedType));

	/*
	 * We will check for all of our parameters, such as previous job ids, etc. first, until
	 * we are left with the blast-specific parameters
	 */

	/* Are we retrieving previously run jobs? */
	if (GetParameterValueFromParameterSet (param_set_p, BS_JOB_ID.npt_name_s, &param_value, true))
		{
			if (!!IsStringEmpty (param_value.st_string_value_s))
				{
					service_p -> se_jobs_p  = CreateJobsForPreviousResults (param_set_p, param_value.st_string_value_s, blast_data_p);
				}
		}		/* if (GetParameterValueFromParameterSet (param_set_p, BS_JOB_ID.npt_name_s, &param_value, true)) */

	if (! (service_p -> se_jobs_p))
		{
			service_p -> se_jobs_p = AllocateServiceJobSet (service_p);

			if (service_p -> se_jobs_p)
				{
					/* Get all of the selected databases and create a BlastServiceJob for each one */
					PrepareBlastServiceJobs (blast_data_p -> bsd_databases_p, param_set_p, service_p -> se_jobs_p, blast_data_p);

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
																					if (tool_p -> ParseParameters (param_set_p, app_params_p))
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

LinkedList *GetUUIDSList (const char *ids_s)
{
	LinkedList *ids_p = AllocateLinkedList (FreeStringListNode);

	if (ids_p)
		{
			bool loop_flag = true;
			const char *start_p = ids_s;
			const char *end_p = NULL;

			while (loop_flag)
				{
					/* scroll to the start of the token */
					while (isspace (*start_p))
						{
							++ start_p;
						}

					if (*start_p != '\0')
						{
							end_p = start_p;

							/* scroll to the end of the token */
							while ((isalnum (*end_p)) || (*end_p == '-'))
								{
									++ end_p;
								}

							/* Is the region the correct size for a uuid? */
							if (end_p - start_p == UUID_STRING_BUFFER_SIZE - 1)
								{
									StringListNode *node_p = NULL;
									char buffer_s [UUID_STRING_BUFFER_SIZE];

									memcpy (buffer_s, start_p, UUID_STRING_BUFFER_SIZE - 1);
									* (buffer_s + (UUID_STRING_BUFFER_SIZE - 1)) = '\0';

									node_p = AllocateStringListNode (buffer_s, MF_DEEP_COPY);

									if (node_p)
										{
											LinkedListAddTail (ids_p, (ListItem *) node_p);
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add uuid \"%s\" to list", buffer_s);
										}

								}		/* if (end_p - start_p == UUID_STRING_BUFFER_SIZE - 1) */
							else
								{
									char *c_p =  (char *) end_p;
									char c = *c_p;

									*c_p = '\0';
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "token \"%s\" is " SIZET_FMT " bytes long, not %d", start_p, end_p - start_p, UUID_STRING_BUFFER_SIZE - 1);
									*c_p = c;
								}

							if (*end_p != '\0')
								{
									start_p = end_p + 1;
								}
							else
								{
									loop_flag = false;
								}

						}		/* if (*start_p != '\0') */
					else
						{
							loop_flag = false;
						}

				}		/* while (loop_flag) */

			if (ids_p -> ll_size == 0)
				{
					FreeLinkedList (ids_p);
					ids_p = NULL;
				}		/* if (ids_p -> ll_size == 0) */

		}		/* if (ids_p) */

	return ids_p;
}



ServiceJobSet *CreateJobsForPreviousResults (ParameterSet *params_p, const char *ids_s, BlastServiceData *blast_data_p)
{
	LinkedList *ids_p = GetUUIDSList (ids_s);
	ServiceJobSet *jobs_p = NULL;

	if (ids_p)
		{
			SharedType param_value;
			uint32 output_format_code = BS_DEFAULT_OUTPUT_FORMAT;

			memset (&param_value, 0, sizeof (SharedType));

			if (GetParameterValueFromParameterSet (params_p, BS_OUTPUT_FORMAT.npt_name_s, &param_value, true))
				{
					output_format_code = param_value.st_ulong_value;
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Couldn't get requested output format code, using " UINT32_FMT " instead", output_format_code);
				}

			jobs_p = GetPreviousJobResults (ids_p, blast_data_p, output_format_code);

			if (!jobs_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get ServiceJobSet for previously run blast job \"%s\"", param_value.st_string_value_s);
				}

			FreeLinkedList (ids_p);
		}		/* if (ids_p) */
	else
		{
			jobs_p = AllocateServiceJobSet (blast_data_p -> bsd_base_data.sd_service_p);

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to parse \"%s\" to get uuids", ids_s);

			if (jobs_p)
				{
					ServiceJob *job_p = CreateAndAddServiceJobToServiceJobSet (jobs_p, ids_s, "Failed UUID lookup", NULL, NULL);

					if (job_p)
						{
							bool added_error_flag = false;
							char *errors_s = ConcatenateVarargsStrings ("Failed to parse \"", ids_s, "\" to get uuids", NULL);

							if (errors_s)
								{
									added_error_flag = AddErrorToServiceJob (job_p, ids_s, errors_s);
									FreeCopiedString (errors_s);
								}
							else
								{
									added_error_flag = AddErrorToServiceJob (job_p, ids_s, "Failed to parse uuids");
								}

							if (!added_error_flag)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add errors to job for \"%s\" uuids", ids_s);
								}

						}		/* if (job_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add failed job for \"%s\" uuids", ids_s);
							FreeServiceJobSet (jobs_p);
							jobs_p = NULL;
						}

				}		/* if (service_p -> se_jobs_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate job set");
				}
		}

	return jobs_p;
}


bool CloseBlastService (Service *service_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

	FreeBlastServiceData (blast_data_p);

	return true;
}


bool AddBaseBlastServiceParameters (Service *blast_service_p, ParameterSet *param_set_p, const DatabaseType db_type)
{
	bool success_flag = false;
	BlastServiceData *blast_data_p = (BlastServiceData *) (blast_service_p -> se_data_p);

	if (AddQuerySequenceParams (blast_data_p, param_set_p))
		{
			uint16 num_dbs = AddDatabaseParams (blast_data_p, param_set_p, db_type);

			num_dbs = AddPairedServiceParameters (blast_service_p, param_set_p, num_dbs);

			if (num_dbs > 0)
				{
					success_flag = true;
				}
		}

	return success_flag;
}



TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t id)
{
	TempFile *input_file_p = NULL;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	/* Input query */
	if (GetParameterValueFromParameterSet (params_p, BS_INPUT_QUERY.npt_name_s, &value, true))
		{
			char *sequence_s = value.st_string_value_s;

			if (!IsStringEmpty (sequence_s))
				{
					input_file_p = TempFile :: GetTempFile (working_directory_s, id, BS_INPUT_SUFFIX_S);

					if (input_file_p)
						{
							bool success_flag = true;

							if (!input_file_p -> IsOpen ())
								{
									success_flag = input_file_p -> Open ("w");
								}

							if (success_flag)
								{
									success_flag = input_file_p -> Print (sequence_s);
									input_file_p -> Close ();
								}

							if (!success_flag)
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast service failed to write to temp file \"%s\" for query \"%s\"", input_file_p -> GetFilename (), sequence_s);
									delete input_file_p;
									input_file_p = NULL;
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast service failed to open temp file for query \"%s\"", sequence_s);
						}



				}		/* if (!IsStringEmpty (sequence_s)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid as string");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast input query is empty");
		}

	return input_file_p;
}


ServiceJobSet *GetPreviousJobResults (LinkedList *ids_p, BlastServiceData *blast_data_p, const uint32 output_format_code)
{
	char *error_s = NULL;
	Service *service_p = blast_data_p -> bsd_base_data.sd_service_p;
	ServiceJobSet *jobs_p = AllocateServiceJobSet (service_p);

	if (jobs_p)
		{
			ServiceJob *job_p = CreateAndAddServiceJobToServiceJobSet (jobs_p, "Previous Blast Results", NULL, NULL, NULL);

			if (job_p)
				{
					uuid_t job_id;
					StringListNode *node_p = (StringListNode *) (ids_p -> ll_head_p);
					uint32 num_successful_jobs = 0;

					while (node_p)
						{
							const char * const job_id_s = node_p -> sln_string_s;

							if (uuid_parse (job_id_s, job_id) == 0)
								{
									char *result_s = GetBlastResultByUUIDString (blast_data_p, job_id_s, output_format_code);
									job_p -> sj_status = OS_FAILED;

									if (result_s)
										{
											json_t *result_json_p = json_string (result_s);

											if (result_json_p)
												{
													json_t *blast_result_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, job_id_s, result_json_p);

													if (blast_result_json_p)
														{
															if (AddResultToServiceJob (job_p, blast_result_json_p))
																{
																	++ num_successful_jobs;
																}
															else
																{
																	error_s = ConcatenateVarargsStrings ("Failed to add blast result \"", job_id_s, "\" to json results array", NULL);
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add blast result \"%s\" to json results array", job_id_s);
																}
														}
													else
														{
															error_s = ConcatenateVarargsStrings ("Failed to get full blast result as json \"", job_id_s, "\"", NULL);
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get full blast result as json \"%s\"", job_id_s);
														}

													json_decref (result_json_p);
												}
											else
												{
													error_s = ConcatenateVarargsStrings ("Failed to get blast result as json \"", job_id_s, "\"", NULL);
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result as json \"%s\"", job_id_s);
												}

											FreeCopiedString (result_s);
										}		/* if (result_s) */
									else
										{
											error_s = ConcatenateVarargsStrings ("Failed to get blast result for \"", job_id_s, "\"", NULL);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result for \"%s\"", job_id_s);
										}
								}		/* if (uuid_parse (param_value.st_string_value_s, job_id) == 0) */
							else
								{
									error_s = ConcatenateVarargsStrings ("Failed to convert \"", job_id_s, "\" to a valid uuid", NULL);
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to a valid uuid", job_id_s);
								}

							if (error_s)
								{
									if (!AddErrorToServiceJob (job_p, job_id_s, error_s))
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create json error string for \"%s\"", job_id_s);
										}

									FreeCopiedString (error_s);
								}		/* if (error_s) */


							node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
						}		/* while (node_p) */

					#if BLAST_SERVICE_DEBUG >= STM_LEVEL_FINE
					PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Num input jobs " UINT32_FMT " num successful json results " UINT32_FMT, ids_p -> ll_size, num_successful_jobs);
					#endif

					if (num_successful_jobs == ids_p -> ll_size)
						{
							job_p -> sj_status = OS_SUCCEEDED;
						}
					else
						{
							job_p -> sj_status = OS_PARTIALLY_SUCCEEDED;
						}

				}		/* if (job_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate service job");
				}

		}		/* if (jobs_p) */

	return jobs_p;
}



void PrepareBlastServiceJobs (const DatabaseInfo *db_p, const ParameterSet * const param_set_p, ServiceJobSet *jobs_p, BlastServiceData *data_p)
{
	if (db_p)
		{
			while (db_p -> di_name_s)
				{
					Parameter *param_p = GetParameterFromParameterSetByName (param_set_p, db_p -> di_name_s);

					/* Do we have a matching parameter? */
					if (param_p)
						{
							/* Is the database selected to search against? */
							if (param_p -> pa_current_value.st_boolean_value)
								{
									BlastServiceJob *job_p = AllocateBlastServiceJob (jobs_p -> sjs_service_p, db_p -> di_name_s, db_p -> di_description_s, data_p);

									if (job_p)
										{
											if (!AddServiceJobToServiceJobSet (jobs_p, (ServiceJob *) job_p))
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add ServiceJob to the ServiceJobSet for \"%s\"", db_p -> di_name_s);
													FreeBlastServiceJob (& (job_p -> bsj_job));
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create ServiceJob for \"%s\"", db_p -> di_name_s);
										}

								}		/* if (param_p -> pa_current_value.st_boolean_value) */

						}		/* if (param_p) */

					++ db_p;
				}		/* while (db_p) */

		}		/* if (db_p) */
}


ParameterSet *IsResourceForBlastService (Service *service_p, Resource *resource_p, Handler * UNUSED_PARAM (handler_p))
{
	ParameterSet *params_p = NULL;

	if (strcmp (resource_p -> re_protocol_s, PROTOCOL_FILE_S) == 0)
		{
			/*
			 * @TODO
			 * We could check if the file is on a remote filesystem and if so
			 * make a full or partial local copy for analysis.
			 */

			/*
			 * We can check on file extension and also the content of the file
			 * to determine if we want to blast this file.
			 */
			if (resource_p -> re_value_s)
				{
					const char *extension_s = Stristr (resource_p -> re_value_s, ".");

					if (extension_s)
						{
							/* move past the . */
							++ extension_s;

							/* check that the file doesn't end with the . */
							if (*extension_s != '\0')
								{
									if (strcmp (extension_s, "fa") == 0)
										{
											/* Get the parameters */
										}

								}		/* if (*extension_s != '\0') */

						}		/* if (extension_s) */

				}		/* if (filename_s) */

		}		/* if (strcmp (resource_p -> re_protocol_s, PROTOCOL_FILE_S)) */
	else if (strcmp (resource_p -> re_protocol_s, PROTOCOL_TEXT_S) == 0)
		{
			if (resource_p -> re_value_s)
				{
					BlastServiceData *blast_data_p = (BlastServiceData *) service_p -> se_data_p;
					DatabaseInfo *db_p = blast_data_p -> bsd_databases_p;

					/*
					 * Scroll through the databases and see if the phrase is in either the
					 * database title or description
					 */
					if (db_p)
						{
							params_p = NULL; //GetBaseBlastServiceParameters (service_p, NULL, NULL);

							if (params_p)
								{
									bool matched_db_flag = false;

									db_p = blast_data_p -> bsd_databases_p;

									while (db_p -> di_name_s)
										{
											Parameter *param_p = GetParameterFromParameterSetByName (params_p, db_p -> di_name_s);

											if (param_p)
												{
													if (param_p -> pa_type == PT_BOOLEAN)
														{
															/* Set the matching databases to active */
															bool active_flag = false;

															if ((Stristr (db_p -> di_name_s, resource_p -> re_value_s)) ||
																	(Stristr (db_p -> di_description_s, resource_p -> re_value_s)))
																{
																	active_flag = true;
																	matched_db_flag = true;
																}

															if (!SetParameterValue (param_p, &active_flag, true))
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set Parameter \"%s\" to true", param_p -> pa_name_s);
																}
														}
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Parameter \"%s\" is of type \"%s\" not boolean", param_p -> pa_name_s, GetGrassrootsTypeAsString (param_p -> pa_type));
														}
												}		/* if (param_p) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to find parameter \"%s\"", db_p -> di_name_s);
												}

											++ db_p;
										}		/* while (db_p -> di_name_s) */

									if (!matched_db_flag)
										{
											ReleaseServiceParameters (service_p, params_p);
										}

								}		/* if (params_p) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetBlastServiceParameters failed");
								}

						}		/* if (db_p) */

				}		/* if (resource_p -> re_value_s) */

		}		/* else if (strcmp (resource_p -> re_protocol_s, PROTOCOL_INLINE_S)) */


	return params_p;
}


OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t job_id)
{
	OperationStatus status = OS_ERROR;
	BlastServiceJob *job_p = NULL;
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	ConvertUUIDToString (job_id, uuid_s);

	if (service_p -> se_jobs_p)
		{
			job_p = (BlastServiceJob *) GetServiceJobFromServiceJobSetById (service_p -> se_jobs_p, job_id);
		}

	if (!job_p)
		{
			JobsManager *jobs_manager_p = GetJobsManager ();

			if (jobs_manager_p)
				{
					ServiceJob *previous_job_p = GetServiceJobFromJobsManager (jobs_manager_p, job_id);

					if (previous_job_p)
						{
							job_p = (BlastServiceJob *) previous_job_p;
						}
				}		/* if (jobs_manager_p) */
			else
				{

				}

		}		/* if (!job_p) */


	if (job_p)
		{
			status = GetCachedServiceJobStatus (& (job_p -> bsj_job));
		}		/* if (job_p) */
	else
		{

		}

	return status;
}






bool DetermineBlastResult (Service *service_p, BlastServiceJob *job_p)
{
	bool success_flag = false;
	BlastTool *tool_p = job_p -> bsj_tool_p;
	OperationStatus status = tool_p -> GetStatus ();
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);

	if (status == OS_SUCCEEDED)
		{
			char *result_s = tool_p -> GetResults (blast_data_p -> bsd_formatter_p);

			if (result_s)
				{
					json_t *result_json_p = json_string (result_s);

					if (result_json_p)
						{
							json_t *blast_result_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, uuid_s, result_json_p);

							if (blast_result_json_p)
								{
									if (AddResultToServiceJob (& (job_p -> bsj_job), blast_result_json_p))
										{
											success_flag = true;
										}
									else
										{
											json_decref (blast_result_json_p);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to append blast result for \"%s\"", uuid_s);
										}

								}		/* if (blast_result_json_p) */

							json_decref (result_json_p);
						}		/* if (result_json_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result as json from \"%s\" for \"%s\"", result_s, uuid_s);
						}

					FreeCopiedString (result_s);
				}		/* if (result_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result for \"%s\"", uuid_s);
				}

		}		/* if (status == OS_SUCCEEDED) */
	else
		{
			success_flag = true;
		}

	return success_flag;
}


char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id, const uint32 output_format_code)
{
	char job_id_s [UUID_STRING_BUFFER_SIZE];
	char *result_s = NULL;

	ConvertUUIDToString (job_id, job_id_s);

	result_s = GetBlastResultByUUIDString (data_p, job_id_s, output_format_code);

	return result_s;
}



char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s, const uint32 output_format_code)
{
	char *result_s = NULL;
	char *job_output_filename_s = GetPreviousJobFilename (data_p, job_id_s, BS_OUTPUT_SUFFIX_S);

	if (job_output_filename_s)
		{
			/* Does the file already exist? */
			char *converted_filename_s = BlastFormatter :: GetConvertedOutputFilename (job_output_filename_s, output_format_code, NULL);

			if (converted_filename_s)
				{
					FILE *job_f = fopen (converted_filename_s, "r");

					if (job_f)
						{
							result_s = GetFileContentsAsString (job_f);

							if (!result_s)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get content of job file \"%s\"", job_output_filename_s);
								}

							if (fclose (job_f) != 0)
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Couldn't close job file \"%s\"", job_output_filename_s);
								}
						}		/* if (job_f) */

					FreeCopiedString (converted_filename_s);
				}		/* if (converted_filename_s) */

			if (!result_s)
				{
					/*
					 * We haven't got the output in the desired output format so we need to run the formatter.
					 */

					/* Is it a local job? */
					if (IsPathValid (job_output_filename_s))
						{
							if (data_p -> bsd_formatter_p)
								{
									result_s = data_p -> bsd_formatter_p -> GetConvertedOutput (job_output_filename_s, output_format_code);
								}		/* if (data_p -> bsd_formatter_p) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "No formatter specified");
								}

						}		/* if (IsPathValid (job_output_filename_s)) */

				}		/* if (!result_s) */

			FreeCopiedString (job_output_filename_s);
		}		/* if (job_output_filename_s) */


	if (!result_s)
		{
			/* Is it a remote job? */
			result_s = GetPreviousRemoteBlastServiceJob (job_id_s, output_format_code, data_p);
		}

	return result_s;
}


json_t *BuildBlastServiceJobJSON (Service * UNUSED_PARAM (service_p), const ServiceJob *service_job_p)
{
	return ConvertBlastServiceJobToJSON ((BlastServiceJob *) service_job_p);
}



BlastServiceData *AllocateBlastServiceData (Service * UNUSED_PARAM (blast_service_p), DatabaseType database_type)
{
	BlastServiceData *data_p = (BlastServiceData *) AllocMemory (sizeof (BlastServiceData));

	if (data_p)
		{
			memset (& (data_p -> bsd_base_data), 0, sizeof (ServiceData));

			data_p -> bsd_working_dir_s = NULL;
			data_p -> bsd_databases_p = NULL;
			data_p -> bsd_formatter_p = NULL;
			data_p -> bsd_tool_factory_p = NULL;
			data_p -> bsd_type = database_type;
		}

	return data_p;
}


ServiceJob *BuildBlastServiceJob (struct Service *service_p, const json_t *service_job_json_p)
{
	BlastServiceData *config_p = (BlastServiceData*) (service_p -> se_data_p);
	BlastServiceJob *job_p = GetBlastServiceJobFromJSON (service_job_json_p, config_p);

	if (job_p)
		{
			BlastTool *tool_p = job_p -> bsj_tool_p;

			OperationStatus old_status = GetCachedServiceJobStatus (& (job_p -> bsj_job));
			OperationStatus current_status = tool_p -> GetStatus ();

			if (old_status != current_status)
				{
					switch (current_status)
						{
							case OS_SUCCEEDED:
							case OS_PARTIALLY_SUCCEEDED:
								{
									if (!DetermineBlastResult (service_p, job_p))
										{
											char job_id_s [UUID_STRING_BUFFER_SIZE];

											ConvertUUIDToString (job_p -> bsj_job.sj_id, job_id_s);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get results for %s", job_id_s);
										}
								}
								break;

							case OS_FAILED:
							case OS_FAILED_TO_START:
								{
									AddErrorToBlastServiceJob (job_p);
								}
								break;

							default:
								break;
						}
				}

			return & (job_p -> bsj_job);
		}		/* if (job_p) */

	return NULL;
}


void CustomiseBlastServiceJob (Service * UNUSED_PARAM (service_p), ServiceJob *job_p)
{
	job_p -> sj_update_fn = UpdateBlastServiceJob;
	job_p -> sj_free_fn = FreeBlastServiceJob;
}



char *GetValueFromBlastServiceJobOutput (Service *service_p, ServiceJob *job_p, const char * const input_s)
{
	char *result_s = NULL;
	char *raw_result_s = NULL;

	/*
	 * Get the result. Ideally we'd like to get this in a format that we can parse, so to begin with we'll use the single json format
	 * available in blast 2.3+
	 */
	raw_result_s = GetBlastResultByUUID ((BlastServiceData *) (service_p -> se_data_p), job_p -> sj_id, BOF_SINGLE_FILE_JSON_BLAST);

	if (raw_result_s)
		{
			json_error_t err;
			json_t *blast_output_p = json_loads (raw_result_s, 0, &err);

			if (blast_output_p)
				{
					/*
					 * For the SamTools service, we want the database and scaffold names
					 */
					const json_t *value_p = GetCompoundJSONObject (blast_output_p, input_s);

					if (value_p)
						{
							if (json_is_string (value_p))
								{
									const char *value_s = json_string_value (value_p);

									if (value_s)
										{
											result_s = CopyToNewString (value_s, 0, false);

											if (!result_s)
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy \"%s\"", value_s);
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to copy \"%s\"", value_s);
										}
								}		/* if (json_is_string (value_p)) */

						}		/* if (value_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to find \"%s\" in \"%s\"", input_s, raw_result_s);
						}

					json_decref (blast_output_p);
				}		/* if (blast_output_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load \"%s\" as json, error at %d, %d", raw_result_s, err.line, err.column);
				}

			FreeCopiedString (raw_result_s);
		}		/* if (raw_result_s) */
	else
		{
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_p -> sj_id, uuid_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast results for id \"%s\" in %d format", input_s, BOF_SINGLE_FILE_JSON_BLAST);
		}


	return result_s;
}


bool GetBlastServiceConfig (BlastServiceData *data_p)
{
	bool success_flag = false;
	const json_t *blast_config_p = data_p -> bsd_base_data.sd_config_p;

	if (blast_config_p)
		{
			data_p -> bsd_working_dir_s = GetJSONString (blast_config_p, "working_directory");

			if (data_p -> bsd_working_dir_s)
				{
					json_t *value_p = json_object_get (blast_config_p, "databases");

					if (value_p)
						{
							if (json_is_array (value_p))
								{
									size_t i = json_array_size (value_p);
									DatabaseInfo *databases_p = (DatabaseInfo *) AllocMemoryArray (i + 1, sizeof (DatabaseInfo));

									if (databases_p)
										{
											json_t *db_json_p;
											DatabaseInfo *db_p = databases_p;

											json_array_foreach (value_p, i, db_json_p)
											{
												const char *name_s = GetJSONString (db_json_p, "name");

												if (name_s)
													{
														const char *description_s = GetJSONString (db_json_p, "description");

														if (description_s)
															{
																const char *type_s = GetJSONString (db_json_p, "type");

																db_p -> di_name_s = name_s;
																db_p -> di_description_s = description_s;
																db_p -> di_active_flag = true;
																db_p -> di_type = DT_NUCLEOTIDE;

																GetJSONBoolean (db_json_p, "active", & (db_p -> di_active_flag));

																if (type_s)
																	{
																		if (strcmp (type_s, "protein") == 0)
																			{
																				db_p -> di_type = DT_PROTEIN;
																			}
																	}

																success_flag = true;
																++ db_p;
															}		/* if (description_p) */

													}		/* if (name_p) */

											}		/* json_array_foreach (value_p, i, db_json_p) */

											if (success_flag)
												{
													data_p -> bsd_databases_p = databases_p;
												}
											else
												{
													FreeMemory (databases_p);
												}

										}		/* if (databases_p) */

									if (success_flag)
										{
											data_p -> bsd_tool_factory_p = BlastToolFactory :: GetBlastToolFactory (blast_config_p);

											if (! (data_p -> bsd_tool_factory_p))
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetBlastToolFactory failed");
													success_flag = false;
												}
										}

								}		/* if (json_is_array (value_p)) */

						}		/* if (value_p) */

				}		/* if (value_s) */

			if (success_flag)
				{
					const char *value_s = GetJSONString (blast_config_p, "blast_formatter");

					if (value_s)
						{
							if (strcmp (value_s, "system") == 0)
								{
									const json_t *formatter_config_p = json_object_get (blast_config_p, "system_formatter_config");

									data_p -> bsd_formatter_p = SystemBlastFormatter :: Create (formatter_config_p);
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Unknown BlastFormatter type \"%s\"", value_s);
								}
						}
				}

		}		/* if (blast_config_p) */

	return success_flag;
}



void FreeBlastServiceData (BlastServiceData *data_p)
{
	if (data_p -> bsd_databases_p)
		{
			FreeMemory (data_p -> bsd_databases_p);
		}

	if (data_p -> bsd_formatter_p)
		{
			delete (data_p -> bsd_formatter_p);
		}

	if (data_p -> bsd_tool_factory_p)
		{
			delete (data_p -> bsd_tool_factory_p);
		}

	FreeMemory (data_p);
}
