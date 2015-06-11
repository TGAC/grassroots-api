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

/**
 * @brief A datatype for monitoring ServiceJobs.
 *
 * A JobsManager is used to store details of the ServiceJobs that are currently
 * running on a Server. ServiceJobs can be added, removed and searched for.
 */
typedef struct JobsManager
{
	/**
	 * @brief Add a ServiceJob to the JobsManager.
	 *
	 * @param manager_p The JobsManager to add the ServiceJob to.
	 * @param job_key The uuid_t for the ServiceJob to search for.
	 * @param job_p The ServiceJob to add.
	 * @return <code>true</code> if the ServiceJob was added successfully,
	 * <code>false</code> otherwise.
	 * @memberof JobsManager
	 * @see AddServiceJobToJobsManager
	 */
	bool (*jm_add_job_fn) (struct JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p);

	/**
	 * @brief Find a ServiceJob.
	 *
	 * Get a previously-added ServiceJob.
	 *
	 * @param manager_p The JobsManager to search on.
	 * @param key The uuid_t for the ServiceJob to search for.
	 * @return A pointer to the matching ServiceJobs or <code>NULL</code>
	 * if it could not be found.
	 * @memberof JobsManager
	 * @see GetServiceJobFromJobsManager
	 */
	ServiceJob *(*jm_get_job_fn)  (struct JobsManager *manager_p, const uuid_t key);


	/**
	 * @brief Remove a ServiceJob.
	 *
	 * Remove a previously-added ServiceJob from the JobsManager.
	 *
	 * @param manager_p The JobsManager to search on.
	 * @param key The uuid_t for the ServiceJob to search for.
	 * @return A pointer to the matching ServiceJobs or <code>NULL</code>
	 * if it could not be found.
	 * @memberof JobsManager
	 * @see RemoveServiceJobFromJobsManager
	 */
	ServiceJob *(*jm_remove_job_fn) (struct JobsManager *manager_p, const uuid_t key);

} JobsManager;


#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Get the current Server-wide JobsManager.
 *
 * @return The JobsManager.
 */
WHEATIS_SERVICE_MANAGER_API JobsManager *GetJobsManager (void);


/**
 * @brief Initialise a JobsManager.
 *
 * Set up the callback functions for a given JobsManager.
 *
 * @param manager_p The JobsManager to set up.
 * @param add_job_fn The callback function to set for jm_add_job_fn for the given JobsManager.
 * @param get_job_fn The callback function to set for jm_get_job_fn for the given JobsManager.
 * @param remove_job_fn The callback function to set for jm_remove_job_fn for the given JobsManager.
 */
WHEATIS_SERVICE_MANAGER_API void InitJobsManager (JobsManager *manager_p,
                      bool (*add_job_fn) (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p),
											ServiceJob *(*get_job_fn)  (JobsManager *manager_p, const uuid_t key),
											ServiceJob *(*remove_job_fn) (JobsManager *manager_p, const uuid_t key));


/**
 * @brief Add a ServiceJob to the JobsManager.
 *
 * This is simply a convenience wrapper around jm_add_job_fn
 * for the given JobsManager.
 *
 * @param manager_p The JobsManager to add the ServiceJob to.
 * @param job_key The uuid_t for the ServiceJob to search for.
 * @param job_p The ServiceJob to add.
 * @return <code>true</code> if the ServiceJob was added successfully,
 * <code>false</code> otherwise.
 * @memberof JobsManager
 * @see jm_add_job_fn
 */
WHEATIS_SERVICE_MANAGER_API bool AddServiceJobToJobsManager (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p);


/**
 * @brief Find a ServiceJob.
 *
 * Get a previously-added ServiceJob. This is simply a convenience
 * wrapper around jm_get_job_fn for the given JobsManager.
 *
 * @param manager_p The JobsManager to search on.
 * @param key The uuid_t for the ServiceJob to search for.
 * @return A pointer to the matching ServiceJobs or <code>NULL</code>
 * if it could not be found.
 * @memberof JobsManager
 * @see jm_get_job_fn
 */
WHEATIS_SERVICE_MANAGER_API ServiceJob *GetServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t key);


/**
 * @brief Remove a ServiceJob.
 *
 * Remove a previously-added ServiceJob from the JobsManager.
 * This is simply a convenience wrapper around jm_remove_job_fn
 * for the given JobsManager.
 *
 * @param manager_p The JobsManager to search on.
 * @param key The uuid_t for the ServiceJob to search for.
 * @return A pointer to the matching ServiceJobs or <code>NULL</code>
 * if it could not be found.
 * @memberof JobsManager
 * @see jm_remove_job_fn
 */
WHEATIS_SERVICE_MANAGER_API ServiceJob *RemoveServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t key);


#ifdef __cplusplus
}
#endif

#endif /* JOBS_MANAGER_H */
