/*
 * jobs_manager.c
 *
 *  Created on: 27 May 2015
 *      Author: tyrrells
 */

#include "jobs_manager.h"


void InitJobsManager (JobsManager *manager_p,
                      bool (*add_job_fn) (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p),
											ServiceJob *(*get_job_fn)  (JobsManager *manager_p, const uuid_t key),
											ServiceJob *(*remove_job_fn) (JobsManager *manager_p, const uuid_t key))
{
	manager_p -> jm_add_job_fn = add_job_fn;
	manager_p -> jm_get_job_fn = get_job_fn;
	manager_p -> jm_remove_job_fn = remove_job_fn;
}


bool AddServiceJobToJobsManager (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p)
{
	return (manager_p -> jm_add_job_fn (manager_p, job_key, job_p));
}


ServiceJob *GetServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t job_key)
{
	return (manager_p -> jm_get_job_fn (manager_p, job_key));
}


ServiceJob *RemoveServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t job_key)
{
	return (manager_p -> jm_remove_job_fn (manager_p, job_key));
}

