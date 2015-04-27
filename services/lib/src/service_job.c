#include "service_job.h"
#include "service.h"

#include "string_utils.h"



#ifdef _DEBUG
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_FINE)
#else
	#define SERVICE_JOB_DEBUG	(STM_LEVEL_NONE)
#endif

void InitServiceJob (ServiceJob *job_p, Service *service_p)
{
	job_p -> sj_service_p = service_p;
	uuid_generate (job_p -> sj_id);
	job_p -> sj_status = OS_IDLE;


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


ServiceJobSet *AllocateServiceJobSet (Service *service_p, const size_t num_jobs)
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
							InitServiceJob (jobs_p + i, service_p);
						}

					return job_set_p;
				}

			FreeMemory (jobs_p);
		}

	return NULL;
}


void FreeServiceJobSet (ServiceJobSet *job_set_p)
{
	FreeMemory (job_set_p -> sjs_jobs_p);
	FreeMemory (job_set_p);
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


json_t *GetServiceJobAsJSON (const ServiceJob *job_p)
{
	json_t *json_p = GetServiceResults (job_p -> sj_service_p, job_p -> sj_id);

	return json_p;
}


json_t *GetServiceJobStatusAsJSON (const ServiceJob *job_p)
{
	json_t *json_p = NULL;
	char *uuid_s = GetUUIDAsString (job_p);

	if (uuid_s)
		{
			json_error_t error;

			json_p = json_pack_ex (&error, 0, "{s:s,s:i}", SERVICE_UUID_S, uuid_s, SERVICE_STATUS_S, job_p -> sj_status);

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

