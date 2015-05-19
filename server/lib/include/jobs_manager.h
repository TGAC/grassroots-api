/*
 * running_services_table.h
 *
 *  Created on: 17 Apr 2015
 *      Author: tyrrells
 */

#ifndef JOBS_MANAGER_H
#define JOBS_MANAGER_H

#include "hash_table.h"
#include "service.h"
#include "service_job.h"

#include "uuid/uuid.h"

#ifdef __cplusplus
	extern "C" {
#endif



bool AddServiceJobToJobsManager (uuid_t job_key, ServiceJob *job_p);


ServiceJob *GetServiceJobFromJobsManager (const uuid_t key);


ServiceJob *RemoveServiceJobFromJobsManager (const uuid_t key);



/**
 *
 */
void ServiceJobFinished (uuid_t job_key);






#ifdef __cplusplus
}
#endif

#endif /* JOBS_MANAGER_H */
