# Asynchronous Services

Services can run in two different ways: 
 * **Synchronous** where the Service does not return until it has completed.  
 * **Asynchronous** where the Service starts its work and returns immediately. The job then continues and the Client can send a message to the Service to see if it has completed successfully or not.

By default, Services run in synchronous mode. 


## Asynchronous Example

Below is a snippet of the definition for a Service that runs asynchronously.

~~~{.json}
{
	"description": "A service to run the Blast program",
	"services": "Blast service",
	"operations": {
		"synchronous": false,
		"summary": "A service to run the Blast program",
  		"nickname": "Blast service",
  		"parameter_set": {
  		}
  		...
  	}
}
~~~

The **synchronous** key determines how the Service runs. If it is ```false``` then the Service runs asynchronously, if it is set to ```true``` or is missing then the Service runs synchronously. 


## Developing an Asynchronous Service

## Job lifetime

When a Service runs a job synchronously, all of the resources that it requires can be released as it sends the response back. With asynchronous services, this becomes more difficult as any required resources need to be accessible between different requests. Given that Httpd can run run as a multi-threaded and/or multi-process system, any data that needs to be persistent, *i.e.* stored between requests, has to be adaptable to any Httpd runtime configuration.

So the Grassroots system has an interface for interacting with asynchronous jobs this is an  called *JobsManager* which deals with sharing persistent data between requests. For Httpd, this interface is implemented in the *APRJobsManager*. The JobsManager is essentially a Hash Table where persistent data can be stored where the keys are UUIDs for the data that you wish to store and the values are JSON representations of the data. All of the needed data to recreate the ServiceJob must be stored in this JSON value.

When writing a Service that you want to be able to run jobs asynchronously, you need to specify two callback functions within the Service:

~~~{.c}

struct ServiceJob *(*se_deserialise_job_json_fn) (struct Service *service_p, const json_t *service_job_json_p);

json_t *(*se_serialise_job_json_fn) (struct Service *service_p, const struct ServiceJob *service_job_p);

~~~

These two functions are used to convert between a ServiceJob and its JSON serialisation. To illustrate how this works, let's go through an example Service that uses this functionality. Included within the Grassroots distribution is an example Service called *Long Running service* and its source at server/src/services/test_long_runner/src/long_running_service.c. This Service mimics the running of asynchronous jobs and how to store and retrieve them from the JobsManager.

Since we are going to store the data representing the asynchronous tasks in the JobsManager, we need to specify the callback functions that we will use to convert our ServiceJobs to and from their JSON
representations. In the ```GetServices ``` function there is the following piece of code that specifies these functions for the Service.

~~~{.c}
service_p -> se_deserialise_job_json_fn = BuildTimedServiceJob;
service_p -> se_serialise_job_json_fn = BuildTimedServiceJobJSON;
~~~

and these two functions are essentially wrapper functions that call the following two functions:

~~~{.c}
static TimedServiceJob *GetTimedServiceJobFromJSON (const json_t *json_p);
static json_t *GetTimedServiceJobAsJSON (TimedServiceJob *job_p);
~~~

For the Long Running service, the required information for a task is stored in a child class of *ServiceJob* called *TimedServiceJob*. The code snippet showing its structure is shown below.

~~~{.c}
/**
 * This datatype stores the start and end times to mimic a real job.
 */
typedef struct TimeInterval
{
	/** The start time of the job. */
	time_t ti_start;

	/** The finish time of the job. */
	time_t ti_end;

	/**
	 * The duration of the job, simply ti_end - ti_start.
	 */
	time_t ti_duration;
} TimeInterval;


/**
 * This is the subclassed ServiceJob that is used to store the information
 * for the mimicked jobs that this Service runs.
 */
typedef struct TimedServiceJob
{
	/** The base ServiceJob */
	ServiceJob tsj_job;

	/**
	 * A pointer to the TimeInterval that is used to mimic the running of a real task.
	 */
	TimeInterval *tsj_interval_p;

	/** Has the TimedServiceJob been added to the JobsManager yet? */
	bool tsj_added_flag;
} TimedServiceJob;
~~~
 
*TimedServiceJob* has a pointer to a *TimeInterval* structure which is where the values used to determine the status of the mimicked task is. So these values need to be preserved between the separate client requests about a given task. This makes it an ideal candidate to store within the *JobsManager* and the details of hos this is done are shown in the next section.


### Serialising the ServiceJob

As mentioned above, in our example, the underlying function that serialises a *TimedServiceJob* is ```GetTimedServiceJobAsJSON``` which is shown below:

~~~{.c}
static json_t *GetTimedServiceJobAsJSON (TimedServiceJob *job_p)
{
	/*
	 * Get the JSON for the ServiceJob base class.
	 */
	json_t *json_p = GetServiceJobAsJSON (& (job_p -> tsj_job));

	if (json_p)
		{
			/*
			 * Now we add our extra data which is the start and end time of the TimeInterval
			 * for the given TimedServiceJob.
			 */
			if (json_object_set_new (json_p, LRS_START_S, json_integer (job_p -> tsj_interval_p -> ti_start)) == 0)
				{
					if (json_object_set_new (json_p, LRS_END_S, json_integer (job_p -> tsj_interval_p -> ti_end)) == 0)
						{
							return json_p;
						}		/* if (json_object_set_new (json_p, LRS_END_S, json_integer (job_p -> tsj_interval_p -> ti_end)) == 0) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, json_p, "Failed to add %s " SIZET_FMT " to json", LRS_END_S, job_p -> tsj_interval_p -> ti_end);
						}

				}		/* if (json_object_set_new (json_p, LRS_START_S, json_integer (job_p -> tsj_interval_p -> ti_start)) == 0) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, json_p, "Failed to add %s " SIZET_FMT " to json", LRS_END_S, job_p -> tsj_interval_p -> ti_end);
				}

			json_decref (json_p);
		}		/* if (json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create JSON for TimedServiceJob");
		}

	return NULL;
}
~~~

This code starts by calling the ```GetServiceJobAsJSON``` which gets the JSON representation for the standard *ServiceJob* structure. The next stage is to add the information for the associated *TimeInterval* structure pointed to by the *tsj_interval_p* member variable since it is this that allows us to check the status of the mimicked task. 
The values we need to be able to recreate all of our needed information are *ti_start* and *ti_end* and so these are added to the JSON fragment using the ```LRS_START_S``` and ```LRS_END_S``` constants respectively.


### Deserialising the ServiceJob

The task of creating the *TimedServiceJob* from a JSON fragment is basically the reverse of the procedure in the previous section. 
This is done by the ```GetTimedServiceJobFromJSON``` function shown below

~~~{.c}
static TimedServiceJob *GetTimedServiceJobFromJSON (const json_t *json_p)
{
	/* allocate the memory for the TimedServiceJob */
	TimedServiceJob *job_p = (TimedServiceJob *) AllocMemory (sizeof (TimedServiceJob));

	if (job_p)
		{
			/* allocate the memory for the TimeInterval */
			job_p -> tsj_interval_p = (TimeInterval *) AllocMemory (sizeof (TimeInterval));

			if (job_p -> tsj_interval_p)
				{
					/* initialise the base ServiceJob from the JSON fragment */
					if (InitServiceJobFromJSON (& (job_p -> tsj_job), json_p))
						{
							/*
							 * We now need to get the start and end times for the TimeInterval
							 * from the JSON.
							 */

							if (GetJSONLong (json_p, LRS_START_S, & (job_p -> tsj_interval_p -> ti_start)))
								{
									if (GetJSONLong (json_p, LRS_END_S, & (job_p -> tsj_interval_p -> ti_end)))
										{
											bool b;

											if (GetJSONBoolean (json_p, LRS_ADDED_FLAG_S, &b))
												{
													job_p -> tsj_added_flag = b;
												}
											else
												{
													job_p -> tsj_added_flag = false;
												}

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
							PrintJSONToLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, json_p, "Init ServiceJob failure: ");
						}

				}		/* if (job_p -> tsj_interval_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate TimeInterval");
				}

			FreeTimedServiceJob ((ServiceJob *) job_p);
		}		/* if (job_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate TimedServiceJob");
		}

	return NULL;
}
~~~

The code starts by allocating the required memory for the *TimedServiceJob* and its associated *TimeInterval* structure. 
If this succeeds, then the information for the *ServiceJob* parent object is filled in. Finally if this is successful, the start and end values required for our *TimeInterval* structure are extracted from the JSON fragment as well.
If this all works, then the reconstructed *TimedServiceJob* is returned and is ready to be used.  