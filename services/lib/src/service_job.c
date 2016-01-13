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
#include "service_job.h"
#include "service.h"

#include "string_utils.h"
#include "json_tools.h"

#include "jobs_manager.h"

#ifdef _DEBUG
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_FINER)
#else
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_NONE)
#endif


static bool AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s);

static bool AddValidJSON (json_t *parent_p, const char * const key_s, json_t *child_p, bool set_as_new_flag);

static bool AddStatusToServiceJobJSON (ServiceJob *job_p, json_t *value_p);



void InitServiceJob (ServiceJob *job_p, Service *service_p, const char *job_name_s, bool (*close_fn) (ServiceJob *job_p))
{
	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Initialising Job: %.16x\n", job_p);
	#endif

	job_p -> sj_service_p = service_p;
	uuid_generate (job_p -> sj_id);
	job_p -> sj_status = OS_IDLE;

	if (job_name_s)
		{
			job_p -> sj_name_s = CopyToNewString (job_name_s, 0, false);
		}
	else
		{
			job_p -> sj_name_s = NULL;
		}

	job_p -> sj_description_s = NULL;

	job_p -> sj_close_fn = close_fn;

	job_p -> sj_result_p = NULL;

	job_p -> sj_metadata_p = NULL;

	job_p -> sj_errors_p = NULL;

	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
		{
			char *uuid_s = GetUUIDAsString (job_p -> sj_id);

			if (uuid_s)
				{
					PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Job: %s\n", uuid_s);
					free (uuid_s);
				}
		}
	#endif
}


void ClearServiceJob (ServiceJob *job_p)
{
	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Clearing Job: %.16x\n", job_p);
	#endif

	if (job_p -> sj_name_s)
		{
			FreeCopiedString (job_p -> sj_name_s);
		}

	if (job_p -> sj_result_p)
		{
			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintJSONRefCounts (job_p -> sj_result_p, "pre decref res_p", STM_LEVEL_FINER, __FILE__, __LINE__);
			#endif

			json_decref (job_p -> sj_result_p);

			job_p -> sj_result_p = NULL;
		}

	if (job_p -> sj_metadata_p)
		{
			json_decref (job_p -> sj_metadata_p);
			job_p -> sj_metadata_p = NULL;
		}

	if (job_p -> sj_errors_p)
		{
			json_decref (job_p -> sj_errors_p);
			job_p -> sj_errors_p = NULL;
		}

	job_p -> sj_status = OS_CLEANED_UP;
}



bool SetServiceJobName (ServiceJob *job_p, const char * const name_s)
{
	bool success_flag = ReplaceStringValue (& (job_p -> sj_name_s), name_s);

	return success_flag;
}


bool SetServiceJobDescription (ServiceJob *job_p, const char * const description_s)
{
	bool success_flag = ReplaceStringValue (& (job_p -> sj_description_s), description_s);

	return success_flag;
}


ServiceJobSet *AllocateServiceJobSet (Service *service_p, const size_t num_jobs, bool (*close_job_fn) (ServiceJob *job_p))
{
	ServiceJob *jobs_p  = (ServiceJob *) AllocMemory (sizeof (ServiceJob) * num_jobs);

	if (jobs_p)
		{
			ServiceJobSet *job_set_p = (ServiceJobSet *) AllocMemory (sizeof (ServiceJobSet));

			if (job_set_p)
				{
					size_t i;

					job_set_p -> sjs_jobs_p = jobs_p;
					job_set_p -> sjs_num_jobs = num_jobs;
					job_set_p -> sjs_service_p = service_p;

					for (i = 0; i < num_jobs; ++ i)
						{
							InitServiceJob (jobs_p + i, service_p, NULL, close_job_fn);
						}

					return job_set_p;
				}

			FreeMemory (jobs_p);
		}

	return NULL;
}


void FreeServiceJobSet (ServiceJobSet *jobs_p)
{
	ServiceJob *job_p = jobs_p -> sjs_jobs_p;
	size_t i = jobs_p -> sjs_num_jobs;

	for ( ; i > 0; -- i, ++ job_p)
		{
			ClearServiceJob (job_p);
		}

	FreeMemory (jobs_p -> sjs_jobs_p);
	FreeMemory (jobs_p);
}


ServiceJob *GetJobById (const ServiceJobSet *jobs_p, const uuid_t job_id)
{
	if (jobs_p)
		{
			ServiceJob *job_p = jobs_p -> sjs_jobs_p;
			size_t i = jobs_p -> sjs_num_jobs;

			for ( ; i > 0; -- i, ++ job_p)
				{
					if (uuid_compare (job_p -> sj_id, job_id) == 0)
						{
							return job_p;
						}
				}
		}

	return NULL;
}


bool CloseServiceJob (ServiceJob *job_p)
{
	const bool success_flag = (job_p -> sj_close_fn (job_p));

	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Closing ServiceJob %.8X for %s", job_p, job_p -> sj_service_p);
	#endif

	if (job_p -> sj_result_p)
		{
			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintJSONRefCounts (job_p -> sj_result_p, "pre wipe results", STM_LEVEL_FINER, __FILE__, __LINE__);
			#endif

			WipeJSON (job_p -> sj_result_p);

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintJSONRefCounts (job_p -> sj_result_p, "post wipe results", STM_LEVEL_FINER, __FILE__, __LINE__);
			#endif

			job_p -> sj_result_p = NULL;
		}

	return success_flag;
}


void ClearServiceJobResults (ServiceJob *job_p, bool free_memory_flag)
{
	if (job_p -> sj_result_p)
		{
			if (free_memory_flag)
				{
					WipeJSON (job_p -> sj_result_p);
				}

			job_p -> sj_result_p = NULL;
		}
}



static bool AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s)
{
	bool success_flag = true;

	if (value_s)
		{
			if (json_object_set_new (parent_p, key_s, json_string (value_s)) != 0)
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set \"%s\": \"%s\" for json", key_s, value_s);
					success_flag = false;
				}
		}

	return success_flag;
}


static bool AddValidJSON (json_t *parent_p, const char * const key_s, json_t *child_p, bool set_as_new_flag)
{
	bool success_flag = true;

	if (child_p)
		{
			if (set_as_new_flag)
				{
					if (json_object_set_new (parent_p, key_s, child_p) != 0)
						{
							success_flag = false;
						}
				}
			else
				{
					if (json_object_set (parent_p, key_s, child_p) != 0)
						{
							success_flag = false;
						}
				}
		}

	if (!success_flag)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add child json for \"%s\" set_as_new_flag: %d", key_s, set_as_new_flag);
		}

	return success_flag;
}


static bool AddStatusToServiceJobJSON (ServiceJob *job_p, json_t *value_p)
{
	bool success_flag = false;

	if (json_is_object (value_p))
		{
			if (json_object_set_new (value_p, SERVICE_STATUS_VALUE_S, json_integer (job_p -> sj_status)) == 0)
				{
					const char *status_text_s = GetOperationStatusAsString (job_p -> sj_status);

					if (status_text_s)
						{
							if (json_object_set_new (value_p, SERVICE_STATUS_S, json_string (status_text_s)) == 0)
								{
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add status text \"%s\" to json", status_text_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get status text for ", job_p -> sj_status);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add status value \"%d\" to json", job_p -> sj_status);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Job status JSON can only be added to a object, value is of type %d", json_typeof (value_p));
		}

	return success_flag;
}


json_t *GetServiceJobAsJSON (ServiceJob *job_p)
{
	json_t *job_json_p = json_object ();

	if (job_json_p)
		{
			json_t *results_json_p = NULL;
			OperationStatus old_status = job_p -> sj_status;
			OperationStatus current_status = GetServiceJobStatus (job_p);
			bool success_flag = false;

			if (old_status == current_status)
				{
					results_json_p = job_p -> sj_result_p;
				}

			if (!results_json_p)
				{
					results_json_p = GetServiceResults (job_p -> sj_service_p, job_p -> sj_id);
					job_p -> sj_result_p = results_json_p;
				}

			if (AddValidJSON (job_json_p, JOB_RESULTS_S, results_json_p, false))
				{
					if (AddValidJSON (job_json_p, JOB_ERRORS_S, job_p -> sj_errors_p, false))
						{
							if (AddValidJSON (job_json_p, JOB_METADATA_S, job_p -> sj_metadata_p, false))
								{
									if (AddStatusToServiceJobJSON (job_p, job_json_p))
										{
											char buffer_s [UUID_STRING_BUFFER_SIZE];
											json_t *uuid_p = NULL;

											ConvertUUIDToString (job_p -> sj_id, buffer_s);

											uuid_p = json_string (buffer_s);

											if (uuid_p)
												{
													if (json_object_set_new (job_json_p, JOB_UUID_S, uuid_p) == 0)
														{
															if (AddValidJSONString (job_json_p, JOB_NAME_S, job_p -> sj_name_s))
																{
																	if (AddValidJSONString (job_json_p, JOB_DESCRIPTION_S, job_p -> sj_description_s))
																		{
																			success_flag = AddValidJSONString (job_json_p, JOB_SERVICE_S, GetServiceName (job_p -> sj_service_p));
																		}
																}
														}
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add uuid %s to json, uuid refcount: %d", buffer_s, uuid_p -> refcount);
															json_decref (uuid_p);
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid %s as json", buffer_s);
												}
										}		/* if (AddStatusToServiceJobJSON (job_p, job_json_p)) */
								}
						}
				}

			PrintJSONToLog (job_json_p, "service job: ", STM_LEVEL_FINER, __FILE__, __LINE__);
			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "success_flag %d", success_flag);
			#endif

			if (!success_flag)
				{
					json_decref (job_json_p);
					job_json_p = NULL;

				}
		}		/* if (job_json_p) */


	return job_json_p;
}


bool ProcessServiceJobSet (ServiceJobSet *jobs_p, json_t *res_p, bool *keep_service_p)
{
	bool success_flag = true;
	const size_t num_jobs = jobs_p -> sjs_num_jobs;
	size_t i;
	ServiceJob *job_p = jobs_p -> sjs_jobs_p;
	JobsManager *manager_p = GetJobsManager ();

	for (i = 0; i < num_jobs; ++ i, ++ job_p)
		{
			json_t *job_json_p = NULL;
			const OperationStatus job_status = GetServiceJobStatus (job_p);
			bool clear_service_job_results_flag = false;

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Job %d: status: %d", i, job_status);
			#endif


			if ((job_status == OS_SUCCEEDED) || (job_status == OS_PARTIALLY_SUCCEEDED))
				{
					job_json_p = GetServiceJobAsJSON (job_p);
					clear_service_job_results_flag = true;
				}
			else
				{
					job_json_p = GetServiceJobStatusAsJSON (job_p);

					/*
					 * If the job is running asynchronously and still going
					 * then we need to store it in the jobs table.
					 */
					if (! (job_p -> sj_service_p -> se_synchronous_flag))
						{
							if ((job_status == OS_PENDING) || (job_status == OS_STARTED))
								{
									if (keep_service_p)
										{
											*keep_service_p = true;
										}

									if (!AddServiceJobToJobsManager (manager_p, job_p -> sj_id, job_p))
										{
											char *uuid_s = GetUUIDAsString (job_p -> sj_id);

											if (uuid_s)
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job %s to jobs manager", uuid_s);
													FreeCopiedString (uuid_s);
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job %s to jobs manager", job_p -> sj_name_s);
												}
										}
								}
						}
				}

			if (job_json_p)
				{
					#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
					PrintJSONToLog (job_json_p, "Job JSON", STM_LEVEL_FINE, __FILE__, __LINE__);
					#endif

					#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
					PrintJSONRefCounts (job_json_p, "Job JSON", STM_LEVEL_FINE, __FILE__, __LINE__);
					#endif

					if (json_array_append_new (res_p, job_json_p) == 0)
						{
							#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
							PrintJSONRefCounts (job_json_p, "after array adding, job json ", STM_LEVEL_FINE, __FILE__, __LINE__);
							#endif

							/*
							if (clear_service_job_results_flag)
								{
									ClearServiceJobResults (job_p, false);
								}
							 */
						}
					else
						{
							char *uuid_s = GetUUIDAsString (job_p -> sj_id);

							if (uuid_s)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job results %s to json response", uuid_s);
									FreeCopiedString (uuid_s);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job results %s to json response", job_p -> sj_name_s);
								}

							WipeJSON (job_json_p);
						}
				}
			else
				{
					char *uuid_s = GetUUIDAsString (job_p -> sj_id);

					if (uuid_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to json response for json %s", uuid_s);
							FreeCopiedString (uuid_s);
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to json response for json %s", job_p -> sj_name_s);
						}
				}

		}		/* for (i = 0; i < num_jobs; ++ i, ++ job_p) */

	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
	PrintJSONToLog (res_p, "service json: ", STM_LEVEL_FINE, __FILE__, __LINE__);
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "success_flag %s keep service %s", success_flag ? "true" : "false", *keep_service_p ? "true" : "false");
	#endif

	return success_flag;
}




OperationStatus GetServiceJobStatus (ServiceJob *job_p)
{
	/* If job has been started or is waiting to run, check its status */
	/*
	OS_LOWER_LIMIT = -4,
	OS_FAILED = -3,
	OS_FAILED_TO_START = -2,
	OS_ERROR = -1,
	OS_IDLE = 0,
	OS_PENDING,
	OS_STARTED,
	OS_FINISHED,
	OS_PARTIALLY_SUCCEEDED,
	OS_SUCCEEDED,
	OS_CLEANED_UP,
	OS_UPPER_LIMIT,
	OS_NUM_STATUSES = OS_UPPER_LIMIT - OS_LOWER_LIMIT + 1
	 */
	switch (job_p -> sj_status)
		{
			case OS_IDLE:
			case OS_PENDING:
			case OS_STARTED:
				if (job_p -> sj_service_p -> se_get_status_fn)
					{
						job_p -> sj_status = job_p -> sj_service_p -> se_get_status_fn (job_p -> sj_service_p, job_p -> sj_id);
					}
				break;

			default:
				break;
		}

	return job_p -> sj_status;
}



json_t *GetServiceJobStatusAsJSON (ServiceJob *job_p)
{
	json_t *job_json_p = json_object ();

	if (job_json_p)
		{
			char *uuid_s = GetUUIDAsString (job_p -> sj_id);

			/* Make sure that the job's status is up to date */
			GetServiceJobStatus (job_p);

			if (uuid_s)
				{
					if (json_object_set_new (job_json_p, JOB_UUID_S, json_string (uuid_s)) == 0)
						{
							#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
							if (job_json_p)
								{
									PrintJSONToLog (job_json_p, "Job after adding uuid:", STM_LEVEL_FINER, __FILE__, __LINE__);
								}
							#endif

							if (AddStatusToServiceJobJSON (job_p, job_json_p))
								{
									const char *service_name_s = GetServiceName (job_p -> sj_service_p);

									#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
									if (job_json_p)
										{
											PrintJSONToLog (job_json_p, "Job after adding status:", STM_LEVEL_FINER, __FILE__, __LINE__);
										}
									#endif

									if (!AddValidJSONString (job_json_p, JOB_SERVICE_S, service_name_s))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add service %s to job status json", service_name_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (job_json_p, "Job after adding service_name_s:", STM_LEVEL_FINER, __FILE__, __LINE__);
	}
#endif

									if (!AddValidJSONString (job_json_p, JOB_NAME_S, job_p -> sj_name_s))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add job name %s to job status json", job_p -> sj_name_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (job_json_p, "Job after adding job_p -> sj_name_s:", STM_LEVEL_FINER, __FILE__, __LINE__);
	}
#endif
									if (!AddValidJSONString (job_json_p, JOB_DESCRIPTION_S, job_p -> sj_description_s))
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add job description %s to job status json", job_p -> sj_description_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (job_json_p, "Job after adding job_p -> sj_description_s:", STM_LEVEL_FINER, __FILE__, __LINE__);
	}
#endif
									if (!AddValidJSON (job_json_p, JOB_ERRORS_S, job_p -> sj_errors_p, false))
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job errors to job %s, %s", job_p -> sj_name_s, uuid_s);
										}

#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINER
if (job_json_p)
	{
		PrintJSONToLog (job_json_p, "Job after adding JOB_ERRORS_S:", STM_LEVEL_FINER, __FILE__, __LINE__);
	}
#endif
								}		/* if (AddStatusToServiceJobJSON (job_p, job_json_p)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to  add status for job uuid \"%s\" to json", uuid_s);
								}

						}		/* if (json_object_set_new (job_json_p, JOB_UUID_S, uuid_s) == 0) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set job uuid \"%s\" as json  for job %s", uuid_s, job_p -> sj_name_s ? job_p -> sj_name_s : "");
						}

					FreeUUIDString (uuid_s);
				}		/* if (uuid_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job uuid as string for job %s", job_p -> sj_name_s ? job_p -> sj_name_s : "");
				}

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
			if (job_json_p)
				{
					PrintJSONToLog (job_json_p, "Job:", STM_LEVEL_FINE, __FILE__, __LINE__);
				}
			#endif

		}		/* if (json_p) */

	return job_json_p;
}



ServiceJob *CreateServiceJobFromJSON (const json_t *json_p)
{
	ServiceJob *job_p = NULL;

	return job_p;
}


bool SetServiceJobFromJSON (ServiceJob *job_p, const json_t *json_p)
{
	bool success_flag = false;

	return success_flag;
}


json_t *GetServiceJobSetAsJSON (const ServiceJobSet *jobs_p)
{
	json_t *jobs_json_p = json_array ();

	if (jobs_json_p)
		{
			size_t i = jobs_p -> sjs_num_jobs;
			ServiceJob *job_p = jobs_p -> sjs_jobs_p;

			for ( ; i > 0; -- i, ++ job_p)
				{
					json_t *job_json_p = GetServiceJobAsJSON (job_p);

					if (job_json_p)
						{
							if (json_array_append_new (jobs_json_p, job_json_p) != 0)
								{
									char *uuid_s = GetUUIDAsString (job_p -> sj_id);

									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to append job %s to results", uuid_s ? uuid_s : "");

									if (uuid_s)
										{
											FreeUUIDString (uuid_s);
										}
								}
						}
					else
						{
							char *uuid_s = GetUUIDAsString (job_p -> sj_id);

							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job %s as json", uuid_s ? uuid_s : "");

							if (uuid_s)
								{
									FreeUUIDString (uuid_s);
								}

						}
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate json jobset array");
		}

	return jobs_json_p;
}



bool AreAnyJobsLive (const ServiceJobSet *jobs_p)
{
	size_t i = jobs_p -> sjs_num_jobs;
	const ServiceJob *job_p = jobs_p -> sjs_jobs_p;

	for ( ; i > 0; -- i, ++ job_p)
		{
			switch (job_p -> sj_status)
				{
					case OS_IDLE:
					case OS_PENDING:
					case OS_STARTED:
					case OS_SUCCEEDED:
					case OS_FINISHED:
						return true;

					default:
						break;
				}
		}

	return false;
}


