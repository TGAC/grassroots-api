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
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_FINE)
#else
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_NONE)
#endif


static bool AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s);

static bool AddValidJSON (json_t *parent_p, const char * const key_s, json_t *child_p);


void InitServiceJob (ServiceJob *job_p, Service *service_p, const char *job_name_s, bool (*close_fn) (ServiceJob *job_p))
{
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
					PrintLog (STM_LEVEL_FINE, "Job: %s\n", uuid_s);
					free (uuid_s);
				}
		}
	#endif
}


void ClearServiceJob (ServiceJob *job_p)
{
	if (job_p -> sj_name_s)
		{
			FreeCopiedString (job_p -> sj_name_s);
		}

	if (job_p -> sj_result_p)
		{
			WipeJSON (job_p -> sj_result_p);
			job_p -> sj_result_p = NULL;
		}

	if (job_p -> sj_metadata_p)
		{
			WipeJSON (job_p -> sj_metadata_p);
			job_p -> sj_metadata_p = NULL;
		}

	if (job_p -> sj_errors_p)
		{
			WipeJSON (job_p -> sj_errors_p);
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

	if (job_p -> sj_result_p)
		{
			WipeJSON (job_p -> sj_result_p);
			job_p -> sj_result_p = NULL;
		}

	return success_flag;
}



static bool AddValidJSONString (json_t *parent_p, const char * const key_s, const char * const value_s)
{
	bool success_flag = true;

	if (value_s)
		{
			if (json_object_set_new (parent_p, key_s, json_string (value_s)) != 0)
				{
					success_flag = false;
				}
		}

	return success_flag;
}


static bool AddValidJSON (json_t *parent_p, const char * const key_s, json_t *child_p)
{
	bool success_flag = true;

	if (child_p)
		{
			if (json_object_set_new (parent_p, key_s, child_p) != 0)
				{
					success_flag = false;
				}
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
			bool success_flag = true;

			if (old_status == current_status)
				{
					results_json_p = job_p -> sj_result_p;
				}

			if (!results_json_p)
				{
					results_json_p = GetServiceResults (job_p -> sj_service_p, job_p -> sj_id);
					job_p -> sj_result_p = results_json_p;
				}

			success_flag = AddValidJSON (job_json_p, JOB_RESULTS_S, results_json_p);

			if (success_flag)
				{
					success_flag = AddValidJSON (job_json_p, JOB_ERRORS_S, job_p -> sj_errors_p);

					if (success_flag)
						{
							success_flag = AddValidJSON (job_json_p, JOB_METADATA_S, job_p -> sj_metadata_p);

							if (success_flag)
								{
									const char *status_s = GetOperationStatusAsString (job_p -> sj_status);

									if (status_s)
										{
											success_flag = (json_object_set_new (job_json_p, SERVICE_STATUS_S, json_string (status_s)) == 0);
										}
									else
										{
											success_flag = (json_object_set (job_json_p, SERVICE_STATUS_VALUE_S, json_integer (current_status)) == 0);
										}

									if (success_flag)
										{
											char buffer_s [UUID_STRING_BUFFER_SIZE];

											ConvertUUIDToString (job_p -> sj_id, buffer_s);
											success_flag = (json_object_set_new (job_json_p, JOB_UUID_S, json_string (buffer_s)) == 0);
										}

									if (success_flag)
										{
											success_flag = AddValidJSONString (job_json_p, JOB_NAME_S, job_p -> sj_name_s);
										}

									if (success_flag)
										{
											success_flag = AddValidJSONString (job_json_p, JOB_DESCRIPTION_S, job_p -> sj_description_s);
										}

									if (success_flag)
										{
											success_flag = AddValidJSONString (job_json_p, JOB_SERVICE_S, GetServiceName (job_p -> sj_service_p));
										}
								}
						}
				}

		}		/* if (job_json_p) */

	PrintJSONToLog (job_json_p, "service job: ", STM_LEVEL_FINER);

	return job_json_p;
}


bool ProcessServiceJobSet (ServiceJobSet *jobs_p, json_t *res_p, bool *keep_service_p)
{
	bool success_flag = true;
	const size_t num_jobs = jobs_p -> sjs_num_jobs;
	size_t i;
	ServiceJob *job_p = jobs_p -> sjs_jobs_p;
	JobsManager *manager_p = GetJobsManager ();

	json_t *service_json_p = json_object ();

	if (service_json_p)
		{
			json_t *jobs_array_p = json_array ();

			if (jobs_array_p)
				{
					if (json_object_set_new (service_json_p, SERVICE_JOBS_S, jobs_array_p) == 0)
						{
							if (!AddServiceResponseHeader (jobs_p -> sjs_service_p, service_json_p))
								{
									PrintErrors (STM_LEVEL_WARNING, "Failed to add service response header from %s", GetServiceName (jobs_p -> sjs_service_p));
								}

							success_flag = true;

							for (i = 0; i < num_jobs; ++ i, ++ job_p)
								{
									json_t *job_json_p = NULL;
									const OperationStatus job_status = GetServiceJobStatus (job_p);

									if ((job_status == OS_SUCCEEDED) || (job_status == OS_PARTIALLY_SUCCEEDED))
										{
											job_json_p = GetServiceJobAsJSON (job_p);
										}
									else
										{
											/*
											 * If the job is running asynchronously and still going
											 * then we need to store it in the jobs table.
											 */
											if (job_p -> sj_service_p -> se_synchronous_flag)
												{
													job_json_p = GetServiceJobStatusAsJSON (job_p);
												}
											else
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
																			PrintErrors (STM_LEVEL_SEVERE, "Failed to add job %s to jobs manager", uuid_s);
																			FreeCopiedString (uuid_s);
																		}
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, "Failed to add job %s to jobs manager", job_p -> sj_name_s);
																		}
																}
														}
													else
														{
															job_json_p = GetServiceJobStatusAsJSON (job_p);
														}
												}
										}

									if (job_json_p)
										{
											if (json_array_append_new (res_p, job_json_p) != 0)
												{
													char *uuid_s = GetUUIDAsString (job_p -> sj_id);

													if (uuid_s)
														{
															PrintErrors (STM_LEVEL_SEVERE, "Failed to add job results %s to json response", uuid_s);
															FreeCopiedString (uuid_s);
														}
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, "Failed to add job results %s to json response", job_p -> sj_name_s);
														}
												}
										}
									else
										{
											char *uuid_s = GetUUIDAsString (job_p -> sj_id);

											if (uuid_s)
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to json response for json %s", uuid_s);
													FreeCopiedString (uuid_s);
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, "Failed to json response for json %s", job_p -> sj_name_s);
												}
										}
								}

						}		/* if (json_object_set_new (res_p, SERVICE_JOBS_S, jobs_array_p) == 0) */
					else
						{
							WipeJSON (jobs_array_p);
						}

				}		/* if (jobs_array_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to create json response array for service %s", GetServiceName (jobs_p -> sjs_service_p));
				}

			#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
			PrintJSONToLog (service_json_p, "\nservice json:\n", STM_LEVEL_FINE);
			#endif
		}		/* if (service_json_p) */


	return success_flag;
}




OperationStatus GetServiceJobStatus (ServiceJob *job_p)
{
	OperationStatus status;

	if (job_p -> sj_service_p -> se_get_status_fn)
		{
			job_p -> sj_status = job_p -> sj_service_p -> se_get_status_fn (job_p -> sj_service_p, job_p -> sj_id);
		}

	return job_p -> sj_status;
}



json_t *GetServiceJobStatusAsJSON (ServiceJob *job_p)
{
	json_t *json_p = NULL;
	char *uuid_s = GetUUIDAsString (job_p -> sj_id);

	/* Make sure that the job's status is up to date */
	GetServiceJobStatus (job_p);

	if (uuid_s)
		{
			json_error_t error;

			json_p = json_pack_ex (&error, 0, "{s:s,s:i}", SERVICE_UUID_S, uuid_s, SERVICE_STATUS_VALUE_S, job_p -> sj_status);

			if (json_p)
				{
					const char *service_name_s = GetServiceName (job_p -> sj_service_p);

					if (job_p -> sj_name_s)
						{
							if (json_object_set_new (json_p, JOB_NAME_S, json_string (job_p -> sj_name_s)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add job name %s to job status json", job_p -> sj_name_s);
								}
						}

					if (job_p -> sj_description_s)
						{
							if (json_object_set_new (json_p, JOB_DESCRIPTION_S, json_string (job_p -> sj_description_s)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add job description %s to job status json", job_p -> sj_description_s);
								}
						}

					if (service_name_s)
						{
							if (json_object_set_new (json_p, JOB_SERVICE_S, json_string (service_name_s)) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add service %s to job status json", service_name_s);
								}
						}
				}

			FreeUUIDString (uuid_s);
		}

	#if SERVICE_JOB_DEBUG >= STM_LEVEL_FINE
	if (json_p)
		{
			uuid_s = json_dumps (json_p, JSON_INDENT (2));

			if (uuid_s)
				{
					PrintLog (STM_LEVEL_FINE, "Job: %s\n", uuid_s);
					free (uuid_s);
				}
		}
	#endif

	return json_p;
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

								}
						}
					else
						{

						}
				}
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


