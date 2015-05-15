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


/**
 * Create a HashTable where both the keys are strings and the values are services
 *
 * @param initital_capacity The initial number of HashBuckets for the HashTable.
 * @param load_percentage The percentage value for how full the HashTable should
 * be allowed to become before it is extended.
 * @return The HashTable or NULL is there was an error.
 */
bool InitJobsManager (void *data_p);


bool DestroyJobsManager (void *data_p);


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
