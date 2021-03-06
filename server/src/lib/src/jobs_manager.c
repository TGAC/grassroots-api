
/*
** Copyright 2014-2016 The Earlham Institute
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
/*
 * jobs_manager.c
 *
 *  Created on: 27 May 2015
 *      Author: tyrrells
 */

#include "jobs_manager.h"


static JobsManager *s_jobs_manager_p = NULL;


JobsManager *GetJobsManager (void)
{
	return s_jobs_manager_p;
}


void InitJobsManager (JobsManager *manager_p,
                      bool (*add_job_fn) (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p),
                      ServiceJob *(*get_job_fn)  (JobsManager *manager_p, const uuid_t key),
                      ServiceJob *(*remove_job_fn) (JobsManager *manager_p, const uuid_t key, bool get_job_flag),
											LinkedList *(*get_all_jobs_fn) (struct JobsManager *manager_p))
{
	manager_p -> jm_add_job_fn = add_job_fn;
	manager_p -> jm_get_job_fn = get_job_fn;
	manager_p -> jm_remove_job_fn = remove_job_fn;
	manager_p -> jm_get_all_jobs_fn = get_all_jobs_fn;

	s_jobs_manager_p = manager_p;
}


bool AddServiceJobToJobsManager (JobsManager *manager_p, uuid_t job_key, ServiceJob  *job_p)
{
	return (manager_p -> jm_add_job_fn (manager_p, job_key, job_p));
}


ServiceJob *GetServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t job_key)
{
	return (manager_p -> jm_get_job_fn (manager_p, job_key));
}



ServiceJob *RemoveServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t job_key, bool get_job_flag)
{
	return (manager_p -> jm_remove_job_fn (manager_p, job_key, get_job_flag));
}



ServiceJob *NullDeserialiser (unsigned char *data_p, void *config_p)
{
	return NULL;
}


LinkedList *GetAllServiceJobsFromJobsManager (struct JobsManager *manager_p)
{
	LinkedList *jobs_p = NULL;

	if (manager_p -> jm_get_all_jobs_fn)
		{
			jobs_p = manager_p -> jm_get_all_jobs_fn (manager_p);
		}


	return jobs_p;
}


