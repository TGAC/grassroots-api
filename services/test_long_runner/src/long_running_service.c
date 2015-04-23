#include <string.h>
#include <unistd.h>

#include "pthread.h"

#include "long_running_service.h"
#include "memory_allocations.h"
#include "string_utils.h"


static uint32 S_DEFAULT_DURATION = 30;

typedef struct ThreadData
{
	uint32 td_duration;
	OperationStatus td_status;
} ThreadData;

/*
 * STATIC DATATYPES
 */
typedef struct 
{
	ServiceData lsd_base_data;
	pthread_t lsd_thread;
	ThreadData lsd_thread_data;
	pthread_mutex_t lsd_mutex;
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

static json_t *GetLongRunningResultAsJSON (const char *result_s, const char *title_s);

static OperationStatus GetLongRunningServiceStatus (const Service *service_p, const uuid_t service_id);


static void *LongRunningFunction (void *params_p);

static void CleanupHandler (void *params_p);


static void CreateThread (pthread_t *thread_p, uint32 sleep_time);


static void InitThreadData (ThreadData *thread_data_p);


static void InitThreadData (ThreadData *thread_data_p, uint32 duration)
{
	thread_data_p -> td_duration = duration;
	thread_data_p -> td_status = OS_IDLE;
}


static void CleanupHandler (void *params_p)
{

}

static void CreateThread (pthread_t *thread_p, ThreadData *data_p)
{
	pthread_create (thread_p, NULL, LongRunningFunction, (void *) data_p);
}


static void *LongRunningFunction (void *params_p)
{
	ThreadData *data_p = (ThreadData *) params_p;

	pthread_cleanup_push (CleanupHandler, NULL);

	sleep (data_p -> td_duration);

	pthread_cleanup_pop (0);

	return NULL;
}


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
								NULL,
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

	pthread_mutex_init (& (data_p -> lsd_mutex), NULL);

	InitThreadData (& (data_p -> lsd_thread_data), S_DEFAULT_DURATION);

	return data_p;
}


static void FreeLongRunningServiceData (LongRunningServiceData *data_p)
{
	pthread_mutex_destroy (& (data_p -> lsd_mutex));

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

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, "duration", "Duration in seconds", "Duration in seconds", TAG_LONG_RUNNING_DURATION, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
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


static json_t *GetLongRunningResultsAsJSON (OperationStatus status)
{
	json_t *results_json_p = json_array ();

	if (results_json_p)
		{
			bool success_flag = false;
			json_t *result_json_p = json_integer (status);

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
	json_t *final_json_p;
	json_t *result_json_p = NULL;

	CreateThread (& (data_p -> lsd_thread), & (data_p -> lsd_thread_data));

	if (uuid_is_null (service_p -> se_id))
		{
			GenerateUUID (& (service_p -> se_id));
		}
	
	res = GetLongRunningServiceStatus (service_p, NULL);

	result_json_p = GetLongRunningResultsAsJSON (res);

	final_json_p = CreateServiceResponseAsJSON (service_p, res, result_json_p);
		
	return final_json_p;
}


static bool IsFileForLongRunningService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return true;
}


static OperationStatus GetLongRunningServiceStatus (const Service *service_p, const uuid_t service_id)
{
	OperationStatus status = OS_ERROR;
	LongRunningServiceData *data_p = (LongRunningServiceData *) (service_p -> se_data_p);

	pthread_mutex_lock (& (data_p -> lsd_mutex));
	status = data_p -> lsd_thread_data.td_status;
	pthread_mutex_unlock (& (data_p -> lsd_mutex));

	return status;
}

