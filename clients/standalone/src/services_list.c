#include "jobs_manager.h"

bool InitJobsManager (void *data_p)
{
	return true;
}


bool DestroyJobsManager (void *data_p)
{
	return true;
}


bool AddServiceJobToJobsManager (uuid_t job_key, ServiceJob *job_p)
{
	return true;
}

ServiceJob *GetServiceJobFromJobsManager (const uuid_t job_key)
{
	return NULL;
}


ServiceJob *RemoveServiceJobFromJobsManager (const uuid_t job_key)
{
	return NULL;
}
