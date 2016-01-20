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


static uint32 S_DEFAULT_DURATION = 30;



typedef struct
{
	time_t tt_start;
	time_t tt_end;
	ServiceJob *tt_job_p;
} TimedTask;

/*
 * STATIC DATATYPES
 */
typedef struct 
{
	ServiceData lsd_base_data;
	TimedTask *lsd_tasks_p;
	uint32 lsd_num_tasks;
} LongRunningServiceData;


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

static void InitTimedTask (TimedTask *task_p);

static void StartTimedTask (TimedTask *task_p, size_t duration);

static OperationStatus GetCurrentTimedTaskStatus (TimedTask *task_p);

static json_t *GetTimedTaskResult (TimedTask *task_p);


static bool CleanUpLongRunningJob (ServiceJob *job_p);


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
			data_p -> lsd_num_tasks = 3;

			if ((data_p -> lsd_tasks_p = (TimedTask *) AllocMemory (sizeof (TimedTask) * (data_p -> lsd_num_tasks))) != NULL)
				{
					uint32 i;

					for (i = 0; i < data_p -> lsd_num_tasks; ++ i)
						{
							InitTimedTask ((data_p -> lsd_tasks_p) + i);
						}

					return data_p;
				}

			FreeMemory (data_p);
		}

	return NULL;
}


static void FreeLongRunningServiceData (LongRunningServiceData *data_p)
{
	FreeMemory (data_p -> lsd_tasks_p);
	FreeMemory (data_p);
}

 
static bool CloseLongRunningService (Service *service_p)
{
	bool close_flag = true;
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	TimedTask *t_p = data_p -> lsd_tasks_p;
	uint32 i;

	for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ t_p)
		{
			OperationStatus status = GetCurrentTimedTaskStatus (t_p);

			if (status == OS_PENDING || status == OS_STARTED)
				{
					close_flag = false;
					i = data_p -> lsd_num_tasks; 		/* force exit from loop */
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

			def.st_ulong_value = S_DEFAULT_DURATION;

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "duration", "Duration in seconds", "Duration in seconds", TAG_LONG_RUNNING_DURATION, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
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
	json_t *results_array_p = NULL;
	TimedTask *task_p = NULL;
	TimedTask *t_p = data_p -> lsd_tasks_p;
	uint32 i;

	for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ t_p)
		{
			if (uuid_compare (t_p -> tt_job_p -> sj_id, job_id) == 0)
				{
					task_p = t_p;
					i = data_p -> lsd_num_tasks; 		/* force exit from loop */
				}
		}

	if (task_p)
		{
			json_error_t error;
			json_t *result_p = json_pack_ex (&error, 0, "{s:i,s:i}", "start", task_p -> tt_start, "end", task_p -> tt_end);

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
		}

	return results_array_p;
}


static ServiceJobSet *RunLongRunningService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, data_p -> lsd_num_tasks, CleanUpLongRunningJob);

	if (service_p -> se_jobs_p)
		{
			ServiceJobSetIterator iterator;
			ServiceJob *job_p = NULL;
			TimedTask *task_p = data_p -> lsd_tasks_p;
			uint32 i;

			InitServiceJobSetIterator (&iterator, service_p -> se_jobs_p);
			job_p = GetNextServiceJobFromServiceJobSetIterator (&iterator);


			for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ task_p)
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

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = GetJobById (service_p -> se_jobs_p, job_id);

			if (job_p)
				{
					TimedTask *task_p = data_p -> lsd_tasks_p;
					uint32 i;

					for (i = 0; i < data_p -> lsd_num_tasks; ++ i, ++ task_p)
						{
							if (task_p -> tt_job_p == job_p)
								{
									status = GetCurrentTimedTaskStatus (task_p);
									i = data_p -> lsd_num_tasks; 		/* force exit from loop */
								}
						}

					job_p -> sj_status = status;
				}
		}

	return status;
}


static void InitTimedTask (TimedTask *task_p)
{
	task_p -> tt_start = 0;
	task_p -> tt_end = 0;
	task_p -> tt_job_p = NULL;
}


static void StartTimedTask (TimedTask *task_p, size_t duration)
{
	time (& (task_p -> tt_start));
	task_p -> tt_end = task_p -> tt_start + duration;
}


static OperationStatus GetCurrentTimedTaskStatus (TimedTask *task_p)
{
	OperationStatus status = OS_IDLE;

	if (task_p -> tt_start != task_p -> tt_end)
		{
			time_t t = time (NULL);

			if (t >= task_p -> tt_start)
				{
					if (t <= task_p -> tt_end)
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

	return status;
}


static json_t *GetTimedTaskResult (TimedTask *task_p)
{
	json_t *result_p = json_pack ("{s:i,s:i}", "Started", task_p -> tt_start, "Ended", task_p -> tt_end);

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
