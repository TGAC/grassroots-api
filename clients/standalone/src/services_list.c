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
