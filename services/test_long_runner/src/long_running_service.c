#include <string.h>
#include <time.h>

#include "long_running_service.h"
#include "memory_allocations.h"
#include "string_utils.h"


static uint32 S_DEFAULT_DURATION = 30;

/*
 * STATIC DATATYPES
 */
typedef struct 
{
	ServiceData lsd_base_data;
	time_t lsd_start_time;
	time_t lsd_end_time;
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

	data_p -> lsd_start_time = 0;
	data_p -> lsd_end_time = 0;

	return data_p;
}


static void FreeLongRunningServiceData (LongRunningServiceData *data_p)
{
	FreeMemory (data_p);
}

 
static bool CloseLongRunningService (Service *service_p)
{
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);

	FreeLongRunningServiceData (data_p);

	return true;
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
			json_t *result_json_p = json_pack ("{s:s,s:i,s:i}", "test key", "test value", "status", service_p -> se_status, "answer to the meaning of life, the universe and everything", 42);

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

	OperationStatus res = OS_FAILED_TO_START;
	json_t *final_json_p = NULL;
	json_t *result_json_p = NULL;
	json_error_t error;

	/* simulate a task that takes 1 minute */
	time (& (data_p -> lsd_start_time));
	data_p -> lsd_end_time = data_p -> lsd_start_time + 60;

	res = GetLongRunningServiceStatus (service_p, NULL);
	result_json_p = json_pack_ex (&error, 0, "{s:i}", SERVICE_STATUS_S, res);

	final_json_p = CreateServiceResponseAsJSON (service_p, res, result_json_p);

	return final_json_p;
}


static bool IsFileForLongRunningService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return true;
}


static OperationStatus GetLongRunningServiceStatus (Service *service_p, const uuid_t service_id)
{
	OperationStatus status = OS_IDLE;
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);

	if (data_p -> lsd_start_time != data_p -> lsd_end_time)
		{
			time_t t = time (NULL);

			if (t >= data_p -> lsd_start_time)
				{
					if (t <= data_p -> lsd_end_time)
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

	if (status != service_p -> se_status)
		{
			service_p -> se_status = status;
		}

	return status;
}

