#include "jobs_manager.h"

JobsManager *GetJobsManager (void)
{
	return NULL;
}


bool DestroyJobsManager (void *data_p)
{
	return true;
}


bool AddServiceJobToJobsManager (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p)
{
	return true;
}

ServiceJob *GetServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t job_key)
{
	return NULL;
}


ServiceJob *RemoveServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t job_key)
{
	return NULL;
}
