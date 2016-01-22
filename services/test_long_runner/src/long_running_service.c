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
	time_t ti_duration;
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

static unsigned char *SerialiseTimedServiceJob (ServiceJob *job_p, unsigned int *value_length_p);


static ServiceJob *DeserialiseTimedServiceJob (unsigned char *data_p);


static void StartTimedServiceJob (TimedServiceJob *job_p);


static OperationStatus GetTimedServiceJobStatus (TimedServiceJob *job_p);


static void InitTimedServiceJob (TimedServiceJob *job_p);

static json_t *GetTimedTaskResult (TimedServiceJob *task_p);

static bool CleanUpLongRunningJob (ServiceJob *job_p);


static ServiceJobSet *GetServiceJobSet (Service *service_p, const uint32 num_jobs);


static TimedServiceJob *AllocateTimedServiceJob (Service *service_p, const char * const job_name_s, const char * const job_description_s, const time_t duration);


static void FreeTimedServiceJob (ServiceJob *job_p);


static json_t *GetTimedServiceJobAsJSON (TimedServiceJob *job_p);


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
	JobsManager *jobs_mananger_p = GetJobsManager ();
	TimedServiceJob *job_p = (TimedServiceJob *) GetServiceJobFromJobsManager (jobs_mananger_p, job_id, DeserialiseTimedServiceJob);
	json_t *results_array_p = NULL;

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
			char job_id_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_id, job_id_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job data for \"%s\"", job_id_s);
		}

	return results_array_p;
}


static ServiceJobSet *GetServiceJobSet (Service *service_p, const uint32 num_jobs)
{
	ServiceJobSet *jobs_p = AllocateServiceJobSet (service_p, FreeTimedServiceJob);

	if (jobs_p)
		{
			uint32 i = 0;
			bool loop_flag = (i < num_jobs);
			bool success_flag = true;

			while (loop_flag && success_flag)
				{
					TimedServiceJob *job_p = NULL;
					char job_name_s [256];
					char job_description_s [256];

					const int duration = 1 + (rand () % 60);

					sprintf (job_name_s, "job " INT32_FMT, i);
					sprintf (job_description_s, "duration " SIZET_FMT, (size_t) duration);

					job_p = AllocateTimedServiceJob (service_p, job_name_s, job_description_s, duration);

					if (job_p)
						{
							if (AddServiceJobToServiceJobSet (jobs_p, (ServiceJob *) job_p))
								{
									++ i;
									loop_flag = (i < num_jobs);
								}		/* if (AddServiceJobToServiceJobSet (jobs_p, (ServiceJob *) job_p)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add TimedServiceJob to ServiceJobSet");
									FreeTimedServiceJob ((ServiceJob *) job_p);
									success_flag = false;
								}

						}		/* if (job_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate TimedServiceJob");
							success_flag = false;
						}

				}		/* while (loop_flag && success_flag) */

			if (!success_flag)
				{
					FreeServiceJobSet (jobs_p);
					jobs_p = NULL;
				}

		}		/* if (jobs_p) */

	return jobs_p;
}


static ServiceJobSet *RunLongRunningService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	SharedType param_value;

	memset (&param_value, 0, sizeof (SharedType));

	if (GetParameterValueFromParameterSet (param_set_p, TAG_LONG_RUNNING_NUM_JOBS, &param_value, true))
		{
			const uint32 num_tasks = param_value.st_ulong_value;

			service_p -> se_jobs_p = GetServiceJobSet (service_p, num_tasks);

			if (service_p -> se_jobs_p)
				{
					ServiceJobSetIterator iterator;
					JobsManager *jobs_manager_p = GetJobsManager ();
					TimedServiceJob *job_p = NULL;
					uint32 i = 0;
					bool loop_flag;

					InitServiceJobSetIterator (&iterator, service_p -> se_jobs_p);
					job_p = (TimedServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);

					loop_flag = (job_p != NULL);

					while (loop_flag)
						{
							StartTimedServiceJob (job_p);

							job_p -> tsj_job.sj_status = GetTimedServiceJobStatus (job_p);

							if (!AddServiceJobToJobsManager (jobs_manager_p, job_p -> tsj_job.sj_id, (ServiceJob *) job_p, SerialiseTimedServiceJob))
								{
									char job_id_s [UUID_STRING_BUFFER_SIZE];

									ConvertUUIDToString (job_p -> tsj_job.sj_id, job_id_s);
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add job \"%s\" to JobsManager", job_id_s);
								}

							job_p = (TimedServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);

							if (job_p)
								{
									++ i;
								}
							else
								{
									loop_flag = false;
								}

						}		/* while (loop_flag) */

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
	JobsManager *jobs_mananger_p = GetJobsManager ();
	TimedServiceJob *job_p = (TimedServiceJob *) GetServiceJobFromJobsManager (jobs_mananger_p, job_id, DeserialiseTimedServiceJob);

	if (job_p)
		{
			status = GetTimedServiceJobStatus (job_p);
		}		/* if (job_p) */
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
	ti_p -> ti_duration = 0;
}


static void StartTimedServiceJob (TimedServiceJob *job_p)
{
	TimeInterval *ti_p = job_p -> tsj_interval_p;

	time (& (ti_p -> ti_start));
	ti_p -> ti_end = (ti_p -> ti_start) + (ti_p -> ti_duration);
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

//
//static bool CleanUpLongRunningJob (ServiceJob *job_p)
//{
//	bool cleaned_up_flag = true;
//	Service *service_p = job_p -> sj_service_p;
//
//	if (service_p -> se_jobs_p)
//		{
//			if (GetJobById (service_p -> se_jobs_p, job_p -> sj_id))
//				{
//					LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
//					TimedTask *task_p = data_p -> lsd_tasks_p;
//					uint32 i;
//
//					for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ task_p)
//						{
//							if (task_p -> tt_job_p == job_p)
//								{
//									OperationStatus status = GetCurrentTimedTaskStatus (task_p);
//
//									if (status != OS_STARTED)
//										{
//											InitTimedTask (task_p);
//											cleaned_up_flag = true;
//
//											job_p -> sj_status = OS_CLEANED_UP;
//
//											i = data_p -> lsd_num_tasks; 		/* force exit from loop */
//										}
//								}
//						}		/* for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ task_p) */
//
//				}		/* if (GetJobById (service_p -> se_jobs_p, job_p -> sj_id)) */
//
//		}		/* if (service_p -> se_jobs_p) */
//
//	return cleaned_up_flag;
//}


static TimedServiceJob *AllocateTimedServiceJob (Service *service_p, const char * const job_name_s, const char * const job_description_s, const time_t duration)
{
	TimedServiceJob *job_p = NULL;
	TimeInterval *interval_p = (TimeInterval *) AllocMemory (sizeof (TimeInterval));

	if (interval_p)
		{
			job_p = (TimedServiceJob *) AllocMemory (sizeof (TimedServiceJob));

			if (job_p)
				{
					interval_p -> ti_start = 0;
					interval_p -> ti_end = 0;
					interval_p -> ti_duration = duration;

					job_p -> tsj_interval_p = interval_p;

					InitServiceJob (& (job_p -> tsj_job), service_p, job_name_s, NULL);

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


static void FreeTimedServiceJob (ServiceJob *job_p)
{
	TimedServiceJob *timed_job_p = (TimedServiceJob *) job_p;

	FreeMemory (timed_job_p -> tsj_interval_p);

	ClearServiceJob (job_p);
	FreeMemory (timed_job_p);
}


static json_t *GetTimedServiceJobAsJSON (TimedServiceJob *job_p)
{
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




static ServiceJob *DeserialiseTimedServiceJob (unsigned char *data_p)
{
	TimedServiceJob *job_p = NULL;
	json_error_t err;
	json_t *job_json_p = NULL;

	#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "For job %s, got: \"%s\"", uuid_s, data_p);
	#endif

	job_json_p = json_loads ((char *) data_p, 0, &err);

	if (job_json_p)
		{
			job_p = GetTimedServiceJobFromJSON (job_json_p);

			if (!job_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetTimedServiceJobFromJSON failed for \"%s\"", data_p);
				}

		}		/* if (job_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to json, err \"%s\" at line %d, column %d", data_p, err.text, err.line, err.column);
		}

	return ((ServiceJob *) job_p);
}


static unsigned char *SerialiseTimedServiceJob (ServiceJob *base_job_p, unsigned int *value_length_p)
{
	TimedServiceJob *job_p = (TimedServiceJob *) base_job_p;
	unsigned char *value_p = NULL;
	json_t *job_json_p = GetTimedServiceJobAsJSON (job_p);

	if (job_json_p)
		{
			char *job_s = json_dumps (job_json_p, JSON_INDENT (2));

			if (job_s)
				{
					/*
					 * include the terminating \0 to make sure
					 * the value as a valid c-style string
					 */
					*value_length_p = strlen (job_s) + 1;
					value_p = (unsigned char *) job_s;
				}		/* if (job_s) */
			else
				{
					char uuid_s [UUID_STRING_BUFFER_SIZE];

					ConvertUUIDToString (job_p -> tsj_job.sj_id, uuid_s);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json_dumps failed for \"%s\"", uuid_s);
				}

		}		/* if (job_json_p) */
	else
		{
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_p -> tsj_job.sj_id, uuid_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetTimedServiceJobAsJSON failed for \"%s\"", uuid_s);
		}

	return value_p;
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
								}		/* if (GetJSONLong (json_p,  LRS_END_S, & (job_p -> tsj_interval_p -> ti_start))) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s from JSON", LRS_END_S);
								}

						}		/* if (GetJSONLong (json_p,  LRS_START_S, & (job_p -> tsj_interval_p -> ti_start))) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s from JSON", LRS_START_S);
						}
				}		/* if (InitServiceJobFromJSON (& (job_p -> tsj_job), json_p)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to init ServiceJob from JSON");
					PrintJSONToLog (json_p, "Init ServiceJob failure: ", STM_LEVEL_SEVERE, __FILE__, __LINE__);
				}

			FreeTimedServiceJob ((ServiceJob *) job_p);
		}		/* if (job_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate TimedServiceJob");
		}

	return NULL;
}


