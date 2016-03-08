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
#include "grassroots_service_manager_library.h"
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
	 * @param job_p The json_t representing the ServiceJob to add.
	 * @param serialise_fn The function used to serialise the ServiceJob into the unsigned char array
	 * which is the value stored in this JobsManager. If this is <code>NULL</code> then
	 * SerialiseServiceJobToJSON is used by default.
	 * @return <code>true</code> if the ServiceJob was added successfully,
	 * <code>false</code> otherwise.
	 * @memberof JobsManager
	 * @see AddServiceJobToJobsManager
	 */
	bool (*jm_add_job_fn) (struct JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p, unsigned char *(*serialise_fn) (ServiceJob *job_p, uint32 *length_p));

	/**
	 * @brief Find a ServiceJob.
	 *
	 * Get a previously-added ServiceJob.
	 *
	 * @param manager_p The JobsManager to search on.
	 * @param key The uuid_t for the ServiceJob to search for.
	 * @param deserialise_fn The function used to deserialise the data stored in this JobsManager to create a ServiceJob.
	 * If this is <code>NULL</code> then DeserialiseServiceJobFromJSON is used by default.
	 * @return A json_t for the matching ServiceJob or <code>NULL</code>
	 * if it could not be found.
	 * @memberof JobsManager
	 * @see GetServiceJobFromJobsManager
	 */
	ServiceJob *(*jm_get_job_fn) (struct JobsManager *manager_p, const uuid_t key, ServiceJob *(*deserialise_fn) (unsigned char *data_p));


	/**
	 * @brief Remove a ServiceJob.
	 *
	 * Remove a previously-added ServiceJob from the JobsManager.
	 *
	 * @param manager_p The JobsManager to search on.
	 * @param key The uuid_t for the ServiceJob to search for.
	 * @param deserialise_fn The function used to deserialise the data stored in this JobsManager to create a ServiceJob.
	 * If this is <code>NULL</code> then DeserialiseServiceJobFromJSON is used by default.
	 * @return A json_t for the matching ServiceJob or <code>NULL</code>
	 * if it could not be found.
	 * @memberof JobsManager
	 * @see RemoveServiceJobFromJobsManager
	 */
	ServiceJob *(*jm_remove_job_fn) (struct JobsManager *manager_p, const uuid_t key, ServiceJob *(*deserialise_fn) (unsigned char *data_p));

} JobsManager;


#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Get the current Server-wide JobsManager.
 *
 * @return The JobsManager.
 */
GRASSROOTS_SERVICE_MANAGER_API JobsManager *GetJobsManager (void);


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
GRASSROOTS_SERVICE_MANAGER_API void InitJobsManager (JobsManager *manager_p,
                      bool (*add_job_fn) (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p, unsigned char *(*serialise_fn) (ServiceJob *job_p, uint32 *length_p)),
                      ServiceJob *(*get_job_fn)  (JobsManager *manager_p, const uuid_t key, ServiceJob *(*deserialise_fn) (unsigned char *data_p)),
                      ServiceJob *(*remove_job_fn) (JobsManager *manager_p, const uuid_t key, ServiceJob *(*deserialise_fn) (unsigned char *data_p)));


/**
 * @brief Add a ServiceJob to the JobsManager.
 *
 * This is simply a convenience wrapper around jm_add_job_fn
 * for the given JobsManager.
 *
 * @param manager_p The JobsManager to add the ServiceJob to.
 * @param job_key The uuid_t for the ServiceJob to search for.
 * @param job_p The ServiceJob to add.
 * @param serialise_fn The function used to serialise the ServiceJob into the unsigned char array
 * which is the value stored in this JobsManager. If this is <code>NULL</code> then
 * SerialiseServiceJobToJSON is used by default.
 * @return <code>true</code> if the ServiceJob was added successfully,
 * <code>false</code> otherwise.
 * @memberof JobsManager
 * @see jm_add_job_fn
 */
GRASSROOTS_SERVICE_MANAGER_API bool AddServiceJobToJobsManager (JobsManager *manager_p, uuid_t job_key, ServiceJob *job_p, unsigned char *(*serialise_fn) (ServiceJob *job_p, uint32 *length_p));


/**
 * @brief Find a ServiceJob.
 *
 * Get a previously-added ServiceJob. This is simply a convenience
 * wrapper around jm_get_job_fn for the given JobsManager.
 *
 * @param manager_p The JobsManager to search on.
 * @param key The uuid_t for the ServiceJob to search for.
 * @param deserialise_fn The function used to deserialise the data stored in this JobsManager to create a ServiceJob.
 * If this is <code>NULL</code> then DeserialiseServiceJobFromJSON is used by default.
 * @return A ServiceJob for the matching ServiceJob or <code>NULL</code>
 * if it could not be found.
 * @memberof JobsManager
 * @see jm_get_job_fn
 */
GRASSROOTS_SERVICE_MANAGER_API ServiceJob *GetServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t key, ServiceJob *(*deserialise_fn) (unsigned char *data_p));


/**
 * @brief Remove a ServiceJob.
 *
 * Remove a previously-added ServiceJob from the JobsManager.
 * This is simply a convenience wrapper around jm_remove_job_fn
 * for the given JobsManager.
 *
 * @param manager_p The JobsManager to search on.
 * @param key The uuid_t for the ServiceJob to search for.
 * @param deserialise_fn The function used to deserialise the data stored in this JobsManager to create a ServiceJob.
 * If this is <code>NULL</code> then DeserialiseServiceJobFromJSON is used by default.
 * @return A ServiceJob for the matching ServiceJob or <code>NULL</code>
 * if it could not be found.
 * @memberof JobsManager
 * @see jm_remove_job_fn
 */
GRASSROOTS_SERVICE_MANAGER_API ServiceJob *RemoveServiceJobFromJobsManager (JobsManager *manager_p, const uuid_t key, ServiceJob *(*deserialise_fn) (unsigned char *data_p));


#ifdef __cplusplus
}
#endif

#endif /* JOBS_MANAGER_H */
