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
#include <time.h>

#include "long_running_service.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "jobs_manager.h"
#include "service_job_set_iterator.h"


typedef struct TimeInterval
{
	time_t ti_start;
	time_t ti_end;
} TimeInterval;


typedef struct TimedServiceJob
{
	ServiceJob tsj_job;
	TimeInterval *tsj_interval_p;
} TimedServiceJob;



/*
 * STATIC DATATYPES
 */
typedef struct 
{
	ServiceData lsd_base_data;
} LongRunningServiceData;


static const char * const LRS_START_S = "start";
static const char * const LRS_END_S = "end";


/*
 * STATIC PROTOTYPES
 */

static LongRunningServiceData *AllocateLongRunningServiceData (Service *service_p);

static void FreeLongRunningServiceData (LongRunningServiceData *data_p);

static const char *GetLongRunningServiceName (Service *service_p);

static const char *GetLongRunningServiceDesciption (Service *service_p);

static ParameterSet *GetLongRunningServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseLongRunningServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunLongRunningService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForLongRunningService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseLongRunningService (Service *service_p);

static json_t *GetLongRunningResultsAsJSON (Service *service_p, const uuid_t service_id);

static OperationStatus GetLongRunningServiceStatus (Service *service_p, const uuid_t service_id);


static void StartTimedServiceJob (TimedServiceJob *job_p, size_t duration);


static OperationStatus GetTimedServiceJobStatus (TimedServiceJob *job_p);


static void InitTimedServiceJob (TimedServiceJob *job_p);

static json_t *GetTimedTaskResult (TimedServiceJob *task_p);

static bool CleanUpLongRunningJob (ServiceJob *job_p);


static TimedServiceJob *AllocateTimedServiceJob (const time_t start, const time_t end);


static void FreeTimedServiceJob (TimedServiceJob *job_p);


static json_t *GetTimedServiceJobAsJSON (const TimedServiceJob *job_p);


static TimedServiceJob *GetTimedServiceJobFromJSON (const json_t *json_p);




/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *config_p)
{
	Service *service_p = (Service *) AllocMemory (sizeof (Service));

	if (service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					ServiceData *data_p = (ServiceData *) AllocateLongRunningServiceData (service_p);
					
					if (data_p)
						{
							InitialiseService (service_p,
								GetLongRunningServiceName,
								GetLongRunningServiceDesciption,
								NULL,
								RunLongRunningService,
								IsFileForLongRunningService,
								GetLongRunningServiceParameters,
								ReleaseLongRunningServiceParameters,
								CloseLongRunningService,
								GetLongRunningResultsAsJSON,
								GetLongRunningServiceStatus,
								true,
								false,
								data_p);
							
							* (services_p -> sa_services_pp) = service_p;

							return services_p;
						}

					FreeServicesArray (services_p);
				}

			FreeService (service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


/*
 * STATIC FUNCTIONS 
 */
 



static LongRunningServiceData *AllocateLongRunningServiceData (Service *service_p)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) AllocMemory (sizeof (LongRunningServiceData));

	if (data_p)
		{
			return data_p;
		}

	return NULL;
}


static void FreeLongRunningServiceData (LongRunningServiceData *data_p)
{
	FreeMemory (data_p);
}

 
static bool CloseLongRunningService (Service *service_p)
{
	bool close_flag = true;
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	ServiceJobSetIterator iterator;
	TimedServiceJob *job_p = NULL;
	bool loop_flag = true;
	InitServiceJobSetIterator (&iterator, service_p -> se_jobs_p);

	job_p = (TimedServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);
	loop_flag = (job_p != NULL);

	while (loop_flag)
		{
			OperationStatus status = GetTimedServiceJobStatus (job_p);

			if (status == OS_PENDING || status == OS_STARTED)
				{
					close_flag = false;
					loop_flag = false;
				}
			else
				{
					job_p = (TimedServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);

					if (!job_p)
						{
							loop_flag = false;
						}
				}
		}

	if (close_flag)
		{
			FreeLongRunningServiceData (data_p);
		}

	return close_flag;
}
 
 
static const char *GetLongRunningServiceName (Service *service_p)
{
	return "Long Running service";
}


static const char *GetLongRunningServiceDesciption (Service *service_p)
{
	return "A service to test long-running asynchronous services";
}


static ParameterSet *GetLongRunningServiceParameters (Service *service_p, Resource *resource_p, const json_t *config_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("LongRunning service parameters", "The parameters used for the LongRunning service");
	
	if (param_set_p)
		{
			Parameter *param_p = NULL;
			SharedType def;

			def.st_ulong_value = 3;

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "jobs", "Number of jobs", "Number of jobs to run", TAG_LONG_RUNNING_NUM_JOBS, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					return param_set_p;
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}


static void ReleaseLongRunningServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static json_t *GetLongRunningResultsAsJSON (Service *service_p, const uuid_t job_id)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	JobsManager *jobs_mananger_p = GetJobsManager ();
	json_t *job_json_p = GetServiceJobFromJobsManager (jobs_mananger_p, job_id);
	json_t *results_array_p = NULL;

	if (job_json_p)
		{
			TimedServiceJob *job_p = GetTimedServiceJobFromJSON (job_json_p);

			if (job_p)
				{
					json_error_t error;
					json_t *result_p = json_pack_ex (&error, 0, "{s:i,s:i}", "start", job_p -> tsj_interval_p -> ti_start, "end", job_p -> tsj_interval_p -> ti_end);

					if (result_p)
						{
							json_t *resource_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, "Long Runner", result_p);

							if (resource_json_p)
								{
									json_decref (result_p);

									results_array_p = json_array ();

									if (results_array_p)
										{
											if (json_array_append_new (results_array_p, result_p) != 0)
												{
													json_decref (result_p);
													json_decref (results_array_p);
													results_array_p = NULL;
												}
										}

								}
						}

				}		/* if (job_p) */
			else
				{

				}

			json_decref (job_json_p);
		}		/* if (job_json_p) */
	else
		{
			char job_id_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_id, job_id_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job data for \"%s\"", job_id_s);
		}

	return results_array_p;
}


static ServiceJobSet *RunLongRunningService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	SharedType param_value;

	memset (&param_value, 0, sizeof (SharedType));

	if (GetParameterValueFromParameterSet (param_set_p, TAG_LONG_RUNNING_NUM_JOBS, &param_value, true))
		{
			const uint32 num_tasks = param_value.st_ulong_value;

			service_p -> se_jobs_p = AllocateServiceJobSet (service_p, num_tasks, NULL);

			if (service_p -> se_jobs_p)
				{
					ServiceJobSetIterator iterator;
					ServiceJob *job_p = NULL;
					uint32 i;

					InitServiceJobSetIterator (&iterator, service_p -> se_jobs_p);
					job_p = GetNextServiceJobFromServiceJobSetIterator (&iterator);


					for (i = 0; i < num_tasks; ++ i, ++ task_p)
						{
							/* get a randomish duration between 1 and 60 seconds */
							size_t duration = 1 + (rand () % 60);
							char buffer_s [256];

							task_p -> tt_job_p = job_p;
							StartTimedTask (task_p, duration);
							job_p -> sj_status = GetCurrentTimedTaskStatus (task_p);

							sprintf (buffer_s, "job " INT32_FMT, i);
							SetServiceJobName (job_p, buffer_s);

							sprintf (buffer_s, "start " SIZET_FMT " end " SIZET_FMT, task_p -> tt_start, task_p -> tt_end);
							SetServiceJobDescription (job_p, buffer_s);

							job_p = GetNextServiceJobFromServiceJobSetIterator (&iterator);

						}

				}		/* if (service_p -> se_jobs_p) */

		}		/* if (GetParameterValueFromParameterSet (param_set_p, TAG_LONG_RUNNING_NUM_JOBS, &param_value, true)) */


	return service_p -> se_jobs_p;
}


static bool IsFileForLongRunningService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return true;
}


static OperationStatus GetLongRunningServiceStatus (Service *service_p, const uuid_t job_id)
{
	OperationStatus status = OS_ERROR;
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);

	JobsManager *jobs_mananger_p = GetJobsManager ();
	json_t *job_json_p = GetServiceJobFromJobsManager (jobs_mananger_p, job_id);

	if (job_json_p)
		{
			TimedServiceJob *job_p = GetTimedServiceJobFromJSON (job_json_p);

			if (job_p)
				{
					status = GetTimedServiceJobStatus (job_p);
				}		/* if (job_p) */
			else
				{

				}

			json_decref (job_json_p);
		}		/* if (job_json_p) */
	else
		{
			char job_id_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_id, job_id_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job data for \"%s\"", job_id_s);
		}

	return status;
}


static void InitTimedServiceJob (TimedServiceJob *job_p)
{
	TimeInterval *ti_p = job_p -> tsj_interval_p;

	ti_p -> ti_start = 0;
	ti_p -> ti_end = 0;
}


static void StartTimedServiceJob (TimedServiceJob *job_p, size_t duration)
{
	TimeInterval *ti_p = job_p -> tsj_interval_p;

	time (& (ti_p -> ti_start));
	ti_p -> ti_end = (ti_p -> ti_start) + duration;
}



static OperationStatus GetTimedServiceJobStatus (TimedServiceJob *job_p)
{
	OperationStatus status = OS_IDLE;
	TimeInterval * const ti_p = job_p -> tsj_interval_p;

	if (ti_p -> ti_start != ti_p -> ti_end)
		{
			time_t t = time (NULL);

			if (t >= ti_p -> ti_start)
				{
					if (t <= ti_p -> ti_end)
						{
							status = OS_STARTED;
						}
					else
						{
							status = OS_SUCCEEDED;
						}
				}
			else
				{
					status = OS_ERROR;
				}
		}

	job_p -> tsj_job.sj_status = status;

	return status;
}


static json_t *GetTimedTaskResult (TimedServiceJob *task_p)
{
	json_t *result_p = json_pack ("{s:i,s:i}", "Started", task_p -> tsj_interval_p -> ti_start, "Ended", task_p -> tsj_interval_p -> ti_end);

	return result_p;
}


static bool CleanUpLongRunningJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;
	Service *service_p = job_p -> sj_service_p;

	if (service_p -> se_jobs_p)
		{
			if (GetJobById (service_p -> se_jobs_p, job_p -> sj_id))
				{
					LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
					TimedTask *task_p = data_p -> lsd_tasks_p;
					uint32 i;

					for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ task_p)
						{
							if (task_p -> tt_job_p == job_p)
								{
									OperationStatus status = GetCurrentTimedTaskStatus (task_p);

									if (status != OS_STARTED)
										{
											InitTimedTask (task_p);
											cleaned_up_flag = true;

											job_p -> sj_status = OS_CLEANED_UP;

											i = data_p -> lsd_num_tasks; 		/* force exit from loop */
										}
								}
						}		/* for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ task_p) */

				}		/* if (GetJobById (service_p -> se_jobs_p, job_p -> sj_id)) */

		}		/* if (service_p -> se_jobs_p) */

	return cleaned_up_flag;
}


static TimedServiceJob *AllocateTimedServiceJob (Service *service_p, const char * const job_name_s, const time_t start, const time_t end)
{
	TimedServiceJob *job_p = NULL;
	TimeInterval *interval_p = (TimeInterval *) AllocMemory (sizeof (TimeInterval));

	if (interval_p)
		{
			job_p = (TimedServiceJob *) AllocMemory (sizeof (TimedServiceJob));

			if (job_p)
				{
					interval_p -> ti_start = start;
					interval_p -> ti_end = end;

					job_p -> tsj_interval_p = interval_p;

					InitServiceJob (& (job_p -> tsj_job), service_p, job_name_s);

				}		/* if (job_p) */
			else
				{
					FreeMemory (interval_p);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate TimedServiceJob");
				}

		}		/* if (interval_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate TimeInterval");
		}

	return job_p;
}


static void FreeTimedServiceJob (TimedServiceJob *job_p)
{
	FreeMemory (job_p -> tsj_interval_p);

	ClearServiceJob (& (job_p -> tsj_job));
	FreeMemory (job_p);
}


static json_t *GetTimedServiceJobAsJSON (const TimedServiceJob *job_p)
{
	bool success_flag = false;
	json_t *json_p = GetServiceJobAsJSON (& (job_p -> tsj_job));

	if (json_p)
		{
			if (json_object_set_new (json_p, LRS_START_S, json_integer (job_p -> tsj_interval_p -> ti_start)) == 0)
				{
					if (json_object_set_new (json_p, LRS_END_S, json_integer (job_p -> tsj_interval_p -> ti_end)) == 0)
						{
							return json_p;
						}
				}

			json_decref (json_p);
		}

	return NULL;
}


static TimedServiceJob *GetTimedServiceJobFromJSON (const json_t *json_p)
{
	TimedServiceJob *job_p = (TimedServiceJob *) AllocMemory (sizeof (TimedServiceJob));

	if (job_p)
		{
			if (InitServiceJobFromJSON (& (job_p -> tsj_job), json_p))
				{
					if (GetJSONLong (json_p,  LRS_START_S, & (job_p -> tsj_interval_p -> ti_start)))
						{
							if (GetJSONLong (json_p,  LRS_END_S, & (job_p -> tsj_interval_p -> ti_end)))
								{
									return job_p;
								}		/* if (GetJSONInteger (json_p,  LRS_END_S, & (job_p -> tsj_interval_p -> ti_start))) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s from JSON", LRS_END_S);
								}

						}		/* if (GetJSONInteger (json_p,  LRS_START_S, & (job_p -> tsj_interval_p -> ti_start))) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s from JSON", LRS_START_S);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to init ServiceJob from JSON");
					PrintJSONToLog (json_p, "Init ServiceJob failure: ", STM_LEVEL_SEVERE, __FILE__, __LINE__);
				}

			FreeTimedServiceJob (job_p);
		}		/* if (job_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate TimedServiceJob");
		}

	return NULL;
}


