#include <string.h>
#include <time.h>

#include "long_running_service.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "running_services_table.h"

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
	TimedTask lsd_task_0;
	TimedTask lsd_task_1;
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

	InitTimedTask (& (data_p -> lsd_task_0));
	InitTimedTask (& (data_p -> lsd_task_1));

	return data_p;
}


static void FreeLongRunningServiceData (LongRunningServiceData *data_p)
{
	FreeMemory (data_p);
}

 
static bool CloseLongRunningService (Service *service_p)
{
	bool close_flag = false;
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	OperationStatus status = GetCurrentTimedTaskStatus (& (data_p -> lsd_task_0));

	if (status == OS_SUCCEEDED || status == OS_IDLE || status == OS_FAILED)
		{
			OperationStatus status = GetCurrentTimedTaskStatus (& (data_p -> lsd_task_1));

			if (status == OS_SUCCEEDED || status == OS_IDLE || status == OS_FAILED)
				{
					FreeLongRunningServiceData (data_p);
					FreeService (service_p);

					close_flag = true;
				}
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

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "duration", "Duration in seconds", "Duration in seconds", TAG_LONG_RUNNING_DURATION, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
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
	json_t *resource_json_p = NULL;

	TimedTask *task_p = NULL;

	if (uuid_compare (data_p -> lsd_task_0.tt_job_p -> sj_id, job_id) == 0)
		{
			task_p = & (data_p -> lsd_task_0);
		}
	else if (uuid_compare (data_p -> lsd_task_1.tt_job_p -> sj_id, job_id) == 0)
		{
			task_p = & (data_p -> lsd_task_1);
		}

	if (task_p)
		{
			json_error_t error;
			json_t *result_p = json_pack_ex (&error, 0, "{s:i}", SERVICE_STATUS_S, task_p -> tt_job_p -> sj_status);
			resource_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, "Long Runner", result_p);
		}

	return resource_json_p;
}


static ServiceJobSet *RunLongRunningService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	const size_t NUM_TASKS = 2;

	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, NUM_TASKS);

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = service_p -> se_jobs_p -> sjs_jobs_p;

			/* simulate a task that takes 1 minute */
			data_p -> lsd_task_0.tt_job_p = job_p;
			StartTimedTask (& (data_p -> lsd_task_0), 60);
			job_p -> sj_status = GetCurrentTimedTaskStatus (& (data_p -> lsd_task_0));
			if (job_p -> sj_status == OS_STARTED)
				{
					AddServiceJobToStatusTable (job_p -> sj_id, job_p);
				}

			++ job_p;

			/* simulate a task that takes 3 minutes */
			data_p -> lsd_task_1.tt_job_p = job_p;
			StartTimedTask (& (data_p -> lsd_task_1), 180);
			job_p -> sj_status = GetCurrentTimedTaskStatus (& (data_p -> lsd_task_1));
			if (job_p -> sj_status == OS_STARTED)
				{
					AddServiceJobToStatusTable (job_p -> sj_id, job_p);
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
					status = job_p -> sj_status;
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
