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

So the Grassroots system has an interface for interacting with asynchronous jobs this is an  called *JobsManager* which deals with sharing persistent data between requests. For Httpd, this interface is implemented in the *APRJobsManager*. The JobsManager is essentially a Hash Table where persistent data can be stored where the keys are UUIDs for the data that you wish to store and the values are JSON representations of the data.

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

and these two fucntions are essentially wrapper functions that call the folllowing two functions:

~~~{.c}
static TimedServiceJob *GetTimedServiceJobFromJSON (const json_t *json_p);
static json_t *GetTimedServiceJobAsJSON (TimedServiceJob *job_p);
~~~


### Serialising the ServiceJob

In our example, this is done by ```GetTimedServiceJobAsJSON``` which is shown below:

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


### Deserialising the ServiceJob
