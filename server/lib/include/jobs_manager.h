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
#include "wheatis_service_manager_library.h"
#include "uuid/uuid.h"

/* forward declaration */
struct JobsManager;


typedef struct JobsManager
{
	bool (*jm_add_job_fn) (struct JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p);


	ServiceJob *(*jm_get_job_fn)  (struct JobsManager *manager_p, const uuid_t key);


	ServiceJob *(*jm_remove_job_fn) (struct JobsManager *manager_p, const uuid_t key);

} JobsManager;


#ifdef __cplusplus
	extern "C" {
#endif

WHEATIS_SERVICE_MANAGER_API JobsManager *GetJobsManager (void);


WHEATIS_SERVICE_MANAGER_API void InitJobsManager (JobsManager *manager_p,
                      bool (*add_job_fn) (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p),
											ServiceJob *(*get_job_fn)  (JobsManager *manager_p, const uuid_t key),
											ServiceJob *(*remove_job_fn) (JobsManager *manager_p, const uuid_t key));


WHEATIS_SERVICE_MANAGER_API bool AddServiceJobToJobsManager (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p);


WHEATIS_SERVICE_MANAGER_API ServiceJob *GetServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t key);


WHEATIS_SERVICE_MANAGER_API ServiceJob *RemoveServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t key);

/**
 *
 */
void ServiceJobFinished (JobsManager *manager_p, uuid_t job_key);


#ifdef __cplusplus
}
#endif

#endif /* JOBS_MANAGER_H */
