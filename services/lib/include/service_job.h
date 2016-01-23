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
 * service_job.h
 *
 *  Created on: 25 Apr 2015
 *      Author: billy
 */

#ifndef SERVICE_JOB_H_
#define SERVICE_JOB_H_

#include "typedefs.h"
#include "grassroots_service_library.h"
#include "operation.h"
#include "jansson.h"
#include "linked_list.h"
#include "memory_allocations.h"

#include "uuid/uuid.h"

/* forward declarations */
struct Service;
struct ServiceJob;


/**
 * @brief A datatype to represent a running task.
 *
 * Each Service can run one or more ServiceJobs.
 */
typedef struct ServiceJob
{
	/** The Service that is running this job. */
	struct Service *sj_service_p;

	/** The unique identifier for this job. */
	uuid_t sj_id;

	/** Is the service currently in an open state? */
	enum OperationStatus sj_status;

	/** The name of the ServiceJob */
	char *sj_name_s;

	/** The description of the ServiceJob */
	char *sj_description_s;

	/**
	 * @private
	 */
	json_t *sj_result_p;

	json_t *sj_metadata_p;

	json_t *sj_errors_p;

	OperationStatus (*sj_update_status_fn) (struct ServiceJob *job_p);

} ServiceJob;


/**
 * A datatype used to store a ServiceJob within
 * a ServceJobSet using a LinkedList.
 */
typedef struct ServiceJobNode
{
	/** The node used to hold this in a LinkedList. */
	ListItem sjn_node;

	/** The ServiceJob. */
	ServiceJob *sjn_job_p;

	void (*sjn_free_job_fn) (ServiceJob *job_p);

	/** The details of the ServiceJob ownsership */
	MEM_FLAG sjn_nob_mem;
} ServiceJobNode;


/**
 * A datatype to represent a collection of ServiceJobs.
 */
typedef struct ServiceJobSet
{
	/** The Service that is running these jobs. */
	struct Service *sjs_service_p;

	/** The ServiceJobs that are in use for the Service. */
	LinkedList *sjs_jobs_p;


	void (*sjs_free_job_fn) (ServiceJob *job_p);
} ServiceJobSet;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Free a ServiceJob.
 *
 * @param job_p The ServiceJob to free.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API void FreeServiceJob (ServiceJob *job_p);


/**
 * @brief Allocate a ServiceJob.
 * *
 * @param job_p The ServiceJob to initialise.
 * @param job_name_s The name to give to the ServiceJob.
 * @param job_description_s The description to give to the ServiceJob.
 * @param service_p The Service that is running the ServiceJob.
 * @return The newly-allocated ServiceJob or <code>NULL</code> upon error.
 * @memberof ServiceJob.
 */
ServiceJob *AllocateServiceJob (struct Service *service_p, const char *job_name_s, const char *job_description_s, OperationStatus (*update_status_fn) (struct ServiceJob *job_p));



/**
 * @brief Allocate a ServiceJob and add it to a ServiceJobSet.
 * *
 * @param job_set_p The ServiceJobSet to add the new ServiceJob to.
 * @param job_name_s The name to give to the ServiceJob.
 * @param job_description_s The description to give to the ServiceJob.
 * @param service_p The Service that is running the ServiceJob.
 * @return The newly-allocated ServiceJob or <code>NULL</code> upon error.
 * @memberof ServiceJob.
 */
ServiceJob *CreateAndAddServiceJobToServiceJobSet (ServiceJobSet *job_set_p, const char *job_name_s, const char *job_description_s, OperationStatus (*update_status_fn) (struct ServiceJob *job_p));



/**
 * @brief Initialise a ServiceJob.
 *
 * This will initialise a ServiceJob ready for use and will automatically get called for all
 * ServiceJobs in a ServiceJobSet when it is first created.
 *
 * @param job_p The ServiceJob to initialise.
 * @param job_name_s The name to give to the ServiceJob.
 * @param job_description_s The description to give to the ServiceJob.
 * @param service_p The Service that is running the ServiceJob.
 * @return <code>true</code> if the ServiceJob was initialised successfully, <code>false</code> otherwise
 * @memberof ServiceJob.
 */
GRASSROOTS_SERVICE_API bool InitServiceJob (ServiceJob *job_p, struct Service *service_p, const char *job_name_s, const char *job_description_s, OperationStatus (*update_status_fn) (struct ServiceJob *job_p));

/**
 * @brief Clear a Service Job ready for reuse.
 *
 * @param job_p The ServiceJob to clear.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API void ClearServiceJob (ServiceJob *job_p);


GRASSROOTS_SERVICE_API ServiceJobNode *FindServiceJobNodeInServiceJobSet (ServiceJobSet *job_set_p, ServiceJob *job_p);


/**
 * @brief Set the description of ServiceJob.
 *
 * @param job_p The ServiceJob to alter.
 * @param description_s The description to set. A deep copy will be made of this string so
 * this value does not need to stay in scope.
 * @return <code>true</code> if the name was set successfully, <code>false</code> otherwise.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API bool SetServiceJobDescription (ServiceJob *job_p, const char * const description_s);


/**
 * @brief Set the name of ServiceJob.
 *
 * @param job_p The ServiceJob to alter.
 * @param name_s The name to set. A deep copy will be made of this string so
 * this value does not need to stay in scope.
 * @return <code>true</code> if the name was set successfully, <code>false</code> otherwise.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API bool SetServiceJobName (ServiceJob *job_p, const char * const name_s);


/**
 * @brief Allocate a ServiceJobSet.
 *
 * @param service_p The Service to allocate the ServiceJobSet for.
 * @param num_jobs The number of ServiceJobs that this ServiceJobSet has space for.
 * @fn close_job_fn If a custom callback function is needed when the ServiceJobs
 * within this ServiceJobSet are allocated, it can be set here. If this is NULL, then
 * InitServiceJob will be used.
 * @return A newly-allocated ServiceJobSet or <code>NULL</code> upon error.
 * @memberof ServiceJobSet.
 * @see InitServiceJob
 */
GRASSROOTS_SERVICE_API ServiceJobSet *AllocateServiceJobSet (struct Service *service_p, void (*free_job_fn) (ServiceJob *job_p));



/**
 * @brief Allocate a ServiceJobSet and populate it with a single ServiceJob
 *
 * @param service_p The Service to allocate the ServiceJobSet for.
 * @param num_jobs The number of ServiceJobs that this ServiceJobSet has space for.
 * @fn close_job_fn If a custom callback function is needed when the ServiceJobs
 * within this ServiceJobSet are allocated, it can be set here. If this is NULL, then
 * InitServiceJob will be used.
 * @return A newly-allocated ServiceJobSet or <code>NULL</code> upon error.
 * @memberof ServiceJobSet.
 * @see InitServiceJob
 */
GRASSROOTS_SERVICE_API ServiceJobSet *AllocateSimpleServiceJobSet (struct Service *service_p, void (*free_job_fn) (ServiceJob *job_p), const char *job_name_s, const char *job_description_s);


/**
 * Free a ServiceJobSet.
 *
 * @param job_set_p The ServiceJobSet to free.
 * @memberof ServiceJobSet
 */
GRASSROOTS_SERVICE_API void FreeServiceJobSet (ServiceJobSet *job_set_p);


/**
 * Add a ServiceJob to a ServiceJobSet.
 *
 * @param job_set_p The ServiceJobSet to add to.
 * @param job_p The ServiceJob to add.
 * @return <code>true</code> if the ServiceJob was added to the ServiceJobSet successfully,
 * <code>false</code> otherwise.
 * @memberof ServiceJobSet
 */
GRASSROOTS_SERVICE_API bool AddServiceJobToServiceJobSet (ServiceJobSet *job_set_p, ServiceJob *job_p);


/**
 * Remove a ServiceJob from a ServiceJobSet.
 *
 * @param job_set_p The ServiceJobSet to remove the ServiceJob from.
 * @param job_p The ServiceJob to remove.
 * @return <code>true</code> if the ServiceJob was removed to the ServiceJobSet successfully,
 * <code>false</code> if could not as the ServiceJob is not a member of the ServiceJobSet
 * @memberof ServiceJobSet
 */
GRASSROOTS_SERVICE_API bool RemoveServiceJobToServiceJobSet (ServiceJobSet *job_set_p, ServiceJob *job_p);



/**
 * @brief Allocate a ServiceJobNode.
 *
 * @param job_p The ServiceJob to allocate a ServiceJobNode for.
 * @param mf The number of ServiceJobs that this ServiceJobSet has space for.
 * @return A newly-allocated ServiceJobNode or <code>NULL</code> upon error.
 * @memberof ServiceJobNode.
 */
GRASSROOTS_SERVICE_API ServiceJobNode *AllocateServiceJobNode (ServiceJob *job_p, MEM_FLAG mf, void (*free_job_fn) (ServiceJob *job_p));


/**
 * Free a ServiceJobNode.
 *
 * @param service_job_node_p The ServiceJobNode to free.
 * @memberof ServiceJobNode
 */
GRASSROOTS_SERVICE_API void FreeServiceJobNode (ListItem *service_job_node_p);


/**
 * Search a ServiceJobSet for ServiceJob.
 *
 * @param jobs_p The ServiceJobSet to search.
 * @param job_id The uuid_t for the ServiceJob to find.
 * @return A pointer to the matching ServiceJob or <code>NULL
 * </code> if it could not be found.
 * @memberof ServiceJobSet
 */
GRASSROOTS_SERVICE_API ServiceJob *GetJobById (const ServiceJobSet *jobs_p, const uuid_t job_id);


/**
 * Get the json representation of a ServiceJobSet.
 *
 * @param job_set_p The ServiceJobSet to get the representation of.
 * @return The json_t representing the ServiceJobSet or <code>NULL
 * </code> upon error.
 * @memberof ServiceJobSet
 */
GRASSROOTS_SERVICE_API json_t *GetServiceJobSetAsJSON (const ServiceJobSet *jobs_p);


/**
 * @brief Get a ServiceJob as JSON.
 *
 * @param job_p The ServiceJob to convert into JSON.
 * @return The json_t object representing the ServiceJob.
 * or <code>NULL</code> upon error.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API json_t *GetServiceJobAsJSON (ServiceJob * const job_p);


/**
 * @brief Get the Current OperationStatus of a ServiceJob as JSON.
 *
 * @param job_p The ServiceJob to query.
 * @return The current OperationStatus as a json_t object.
 * or <code>NULL</code> upon error.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API json_t *GetServiceJobStatusAsJSON (ServiceJob *job_p);




GRASSROOTS_SERVICE_API bool ProcessServiceJobSet (ServiceJobSet *jobs_p, json_t *res_p, bool *keep_service_p);


/**
 * @brief Create a ServiceJob from a json_t object.
 *
 * This will create a ServiceJob from a json_t object. It will
 * allocate a new ServiceJob and then call <code>SetServiceFromJSON</code>.
 *
 * @param json_p The json object reresenting a ServiceJob.
 * @return <code>true</code> if the ServiceJob was created successfully,
 * <code>false</code> otherwise.
 * @memberof ServiceJob
 * @see SetServiceJobFromJSON
 * @see
 */
GRASSROOTS_SERVICE_API ServiceJob *CreateServiceJobFromJSON (const json_t *json_p);


/**
 * @brief Get a ServiceJob from a json_t object.
 *
 * @param job_p The ServiceJob which will be filled in from the json data.
 * @param json_p The json object representing a ServiceJob.
 * @return <code>true</code> if the ServiceJob was created successfully,
 * <code>false</code> otherwise.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API bool InitServiceJobFromJSON (ServiceJob *job_p, const json_t *json_p);


/**
 * @brief Get the Current OperationStatus of a ServiceJob.
 *
 * @param job_p The ServiceJob to query.
 * @return The current OperationStatus.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API OperationStatus GetServiceJobStatus (ServiceJob *job_p);


/**
 * @brief Get the name of ServiceJob.
 *
 * @param job_p The ServiceJob to query.
 * @return The name of the ServiceJob
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API const char *GetServiceJobName (const ServiceJob *job_p);


/**
 * @brief Close a ServiceJob.
 *
 * This will call the Services close callback function.
 *
 * @param job_p The ServiceJob to close.
 * @return <code>true</code> if the ServiceJob was closed successfully,
 * <code>false</code> otherwise.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API bool CloseServiceJob (ServiceJob *job_p);


/**
 * @brief Check if any ServiceJobs within ServiceJobSet are still running
 *
 * @param jobs_p The ServiceJobSet to check.
 * @return <code>true</code> if there are still some ServiceJobs in the given set
 * that are still running, <code>false</code> otherwise.
 * @memberof ServiceJobSet
 */
GRASSROOTS_SERVICE_API bool AreAnyJobsLive (const ServiceJobSet *jobs_p);




GRASSROOTS_SERVICE_API ServiceJob *GetServiceJobFromServiceJobSet (const ServiceJobSet *jobs_p, const uint32 index);


GRASSROOTS_SERVICE_API uint32 GetServiceJobSetSize (const ServiceJobSet * const jobs_p);


/**
 * @brief Clear the results associated with a ServiceJob.
 *
 * This will call the Services close callback function.
 *
 * @param job_p The ServiceJob to close.
 * @param free_memory_flag If this is <code>true</code> then the ServiceJob's results will
 * be freed. If <code>false</code> then the results will just be set to NULL. This allows the
 * transfer of the results variable into another json_t object if needed for example.
 * @memberof ServiceJob
 */
GRASSROOTS_SERVICE_API void ClearServiceJobResults (ServiceJob *job_p, bool free_memory_flag);


GRASSROOTS_SERVICE_API char *SerialiseServiceJobToJSON (ServiceJob * const job_p);


GRASSROOTS_SERVICE_API ServiceJob *DeserialiseServiceJobFromJSON (char *raw_json_data_s);


#ifdef __cplusplus
}
#endif



#endif /* SERVICE_JOB_H_ */
