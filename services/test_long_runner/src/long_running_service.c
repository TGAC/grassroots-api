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
	uuid_t tt_id;
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

static json_t *RunLongRunningService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForLongRunningService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseLongRunningService (Service *service_p);

static json_t *GetLongRunningResultsAsJSON (Service *service_p, const uuid_t service_id);

static OperationStatus GetLongRunningServiceStatus (Service *service_p, const uuid_t service_id);

static void InitTimedTask (TimedTask *task_p);

static void StartTimedTask (TimedTask *task_p, size_t duration);

static OperationStatus GetTimedTaskStatus (TimedTask *task_p);

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
	OperationStatus status = GetTimedTaskStatus (& (data_p -> lsd_task_0));

	if (status == OS_SUCCEEDED || status == OS_IDLE || status == OS_FAILED)
		{
			OperationStatus status = GetTimedTaskStatus (& (data_p -> lsd_task_1));

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


static json_t *GetLongRunningResultsAsJSON (Service *service_p, const uuid_t service_id)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	json_t *results_json_p = json_array ();

	if (results_json_p)
		{
			bool success_flag = false;
			json_t *result_json_p = NULL;

			if (uuid_compare (service_id, data_p -> lsd_task_0.tt_id) == 0)
				{
					result_json_p = GetTimedTaskResult (& (data_p -> lsd_task_0));
				}
			else if (uuid_compare (service_id, data_p -> lsd_task_1.tt_id) == 0)
				{
					result_json_p = GetTimedTaskResult (& (data_p -> lsd_task_1));
				}
			else
				{
					char *id_s = GetUUIDAsString (service_id);

					if (id_s)
						{
							result_json_p = json_pack ("{s:s,s:s}", SERVICES_ID_S, id_s, ERROR_S, "No matching id");
							FreeUUIDString (id_s);
						}
					else
						{
							result_json_p = json_pack ("{s:s}", ERROR_S, "No matching id");
						}
				}

			if (result_json_p)
				{
					json_t *resource_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, "Long Runner", result_json_p);

					if (resource_json_p)
						{
							success_flag = (json_array_append_new (results_json_p, resource_json_p) == 0);
						}
					else
						{
							json_object_clear (result_json_p);
							json_decref (result_json_p);
						}
				}

			if (!success_flag)
				{
					json_object_clear (results_json_p);
					json_decref (results_json_p);
					results_json_p = NULL;
				}
		}

	return results_json_p;
}


static json_t *RunLongRunningService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);

	OperationStatus res_0 = OS_FAILED_TO_START;
	OperationStatus res_1 = OS_FAILED_TO_START;
	json_t *task_0_json_p = NULL;
	json_t *task_1_json_p = NULL;
	json_t *result_0_json_p = NULL;
	json_t *result_1_json_p = NULL;
	json_t *final_json_p = NULL;
	json_error_t error;
	bool success_flag = false;

	/* simulate a task that takes 1 minute */
	StartTimedTask (& (data_p -> lsd_task_0), 60);
	res_0 = GetTimedTaskStatus (& (data_p -> lsd_task_0));
	if (res_0 == OS_STARTED)
		{
			AddServiceToStatusTable (data_p -> lsd_task_0.tt_id, service_p);
		}

	/* simulate a task that takes 3 minutes */
	StartTimedTask (& (data_p -> lsd_task_1), 180);
	res_1 = GetTimedTaskStatus (& (data_p -> lsd_task_1));
	if (res_0 == OS_STARTED)
		{
			AddServiceToStatusTable (data_p -> lsd_task_1.tt_id, service_p);
		}

	task_0_json_p = json_pack_ex (&error, 0, "{s:i}", SERVICE_STATUS_S, res_0);

	if (task_0_json_p)
		{
			task_1_json_p = json_pack_ex (&error, 0, "{s:i}", SERVICE_STATUS_S, res_1);

			if (task_1_json_p)
				{
					result_0_json_p = CreateServiceResponseAsJSON (service_p, res_0, task_0_json_p, data_p -> lsd_task_0.tt_id);

					if (result_0_json_p)
						{
							task_0_json_p = NULL;

							result_1_json_p = CreateServiceResponseAsJSON (service_p, res_1, task_1_json_p, data_p -> lsd_task_1.tt_id);

							if (result_1_json_p)
								{
									final_json_p = json_pack ("[o,o]", result_0_json_p, result_1_json_p);

									if (final_json_p)
										{
											result_0_json_p = NULL;
											result_1_json_p = NULL;

											success_flag = true;
										}
									else
										{
											json_object_clear (result_1_json_p);
											json_decref (result_1_json_p);
										}
								}

							if ((!success_flag) && result_0_json_p)
								{
									json_object_clear (result_0_json_p);
									json_decref (result_0_json_p);
								}
						}

					if ((!success_flag) && task_1_json_p)
						{
							json_object_clear (task_1_json_p);
							json_decref (task_1_json_p);
						}
				}

			if ((!success_flag) && task_0_json_p)
				{
					json_object_clear (task_0_json_p);
					json_decref (task_0_json_p);
				}
		}



	return final_json_p;
}


static bool IsFileForLongRunningService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return true;
}


static OperationStatus GetLongRunningServiceStatus (Service *service_p, const uuid_t service_id)
{
	OperationStatus status = OS_ERROR;
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);
	OperationStatus status_0 = GetTimedTaskStatus (& (data_p -> lsd_task_0));
	OperationStatus status_1 = GetTimedTaskStatus (& (data_p -> lsd_task_1));

	if (service_id)
		{
			if (uuid_compare (service_id, data_p -> lsd_task_0.tt_id) == 0)
				{
					status = status_0;
				}
			else if (uuid_compare (service_id, data_p -> lsd_task_1.tt_id) == 0)
				{
					status = status_1;
				}
		}
	else
		{
			if (status_0 == OS_STARTED || status_1 == OS_STARTED)
				{
					status = OS_STARTED;
				}
			else
				{
					status = (status_0 > status_1) ? status_0 : status_1;
				}
		}

	return status;
}


static void InitTimedTask (TimedTask *task_p)
{
	task_p -> tt_start = 0;
	task_p -> tt_end = 0;
	uuid_clear (task_p -> tt_id);
}


static void StartTimedTask (TimedTask *task_p, size_t duration)
{
	if (uuid_is_null (task_p -> tt_id))
		{
			uuid_generate (task_p -> tt_id);
		}

	time (& (task_p -> tt_start));
	task_p -> tt_end = task_p -> tt_start + duration;
}


static OperationStatus GetTimedTaskStatus (TimedTask *task_p)
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
