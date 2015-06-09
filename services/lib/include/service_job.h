/*
 * service_job.h
 *
 *  Created on: 25 Apr 2015
 *      Author: billy
 */

#ifndef SERVICE_JOB_H_
#define SERVICE_JOB_H_

#include "typedefs.h"
#include "wheatis_service_library.h"
#include "operation.h"
#include "jansson.h"

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
	 * @brief Callback function for closing the ServiceJob
	 *
	 * If a ServiceJob needs a custom routine to release resources,
	 * this callback function can be set.
	 */
	bool (*sj_close_fn) (struct ServiceJob *job_p);
} ServiceJob;



/**
 * A datatype to represent a collection of ServiceJobs.
 */
typedef struct ServiceJobSet
{
	/** The Service that is running these jobs. */
	struct Service *sjs_service_p;

	/** The ServiceJobs that are in use for the Service. */
	ServiceJob *sjs_jobs_p;

	/** The number of ServiceJobs in this set. */
	size_t sjs_num_jobs;
} ServiceJobSet;



#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Initialise a ServiceJob.
 *
 * This will initialise a ServiceJob ready for use and will automatically get called for all
 * ServiceJobs in a ServiceJobSet when it is first created.
 *
 * @param job_p The ServiceJob to initialise.
 * @param name_s The name to give to the ServiceJob.
 * @param service_p The Service that is running the ServiceJob.
 * @fn close_fn An optional callback function that the ServiceJob will call when it is
 * freed. This is used if you need any custom routines to clean up a ServiceJob.
 * @memberof ServiceJob.
 */
WHEATIS_SERVICE_API void InitServiceJob (ServiceJob *job_p, struct Service *service_p, const char *name_s, bool (*close_fn) (ServiceJob *job_p));


/**
 * @brief Clear a Service Job ready for reuse.
 *
 * @param job_p The ServiceJob to clear.
 * @memberof ServiceJob
 */
WHEATIS_SERVICE_API void ClearServiceJob (ServiceJob *job_p);


/**
 * @brief Set the descrition of ServiceJob.
 *
 * @param job_p The ServiceJob to alter.
 * @param description_s The description to set. A deep copy will be made of this string so
 * this value does not need to stay in scope.
 * @return <code>true</code> if the name was set successfully, <code>false</code> otherwise.
 * @memberof ServiceJob
 */
WHEATIS_SERVICE_API bool SetServiceJobDescription (ServiceJob *job_p, const char * const description_s);


/**
 * @brief Set the name of ServiceJob.
 *
 * @param job_p The ServiceJob to alter.
 * @param name_s The name to set. A deep copy will be made of this string so
 * this value does not need to stay in scope.
 * @return <code>true</code> if the name was set successfully, <code>false</code> otherwise.
 * @memberof ServiceJob
 */
WHEATIS_SERVICE_API bool SetServiceJobName (ServiceJob *job_p, const char * const name_s);


/**
 * @brief Allocate a ServiceJobSet.
 *
 * @param service_p The Service to allocate the ServiceJobSet for.
 * @param num_jobs The number of ServiceJobs that this ServiceJobSet has space for.
 * @fn close_job_fn If a custom callback function is needed when the ServiceJobs
 * within this ServiceJobSet are freed, it can be set here.
 * @return A newly-allocated ServiceJobSet or <code>NULL</code> upon error.
 * @memberof ServiceJobSet.
 */
WHEATIS_SERVICE_API ServiceJobSet *AllocateServiceJobSet (struct Service *service_p, size_t num_jobs, bool (*close_job_fn) (ServiceJob *job_p));


/**
 * Free a ServiceJobSet.
 *
 * @param job_set_p The ServiceJobSet to free.
 * @memberof ServiceJobSet
 */
WHEATIS_SERVICE_API void FreeServiceJobSet (ServiceJobSet *job_set_p);


/**
 * Search a ServiceJobSet for ServiceJob.
 *
 * @param jobs_p The ServiceJobSet to search.
 * @param job_id The uuid_t for the ServiceJob to find.
 * @return A pointer to the matching ServiceJob or <code>NULL
 * </code> if it could not be found.
 * @memberof ServiceJobSet
 */
WHEATIS_SERVICE_API ServiceJob *GetJobById (const ServiceJobSet *jobs_p, const uuid_t job_id);


/**
 * Get the json representation of a ServiceJobSet.
 *
 * @param job_set_p The ServiceJobSet to get the representation of.
 * @return The json_t representing the ServiceJobSet or <code>NULL
 * </code> upon error.
 * @memberof ServiceJobSet
 */
WHEATIS_SERVICE_API json_t *GetServiceJobSetAsJSON (const ServiceJobSet *jobs_p);


/**
 * @brief Get a ServiceJob as JSON.
 *
 * @param job_p The ServiceJob to convert into JSON.
 * @return The json_t object representing the ServiceJob.
 * or <code>NULL</code> upon error.
 * @memberof ServiceJob
 */WHEATIS_SERVICE_API json_t *GetServiceJobAsJSON (const ServiceJob *job_p);


/**
 * @brief Get the Current OperationStatus of a ServiceJob as JSON.
 *
 * @param job_p The ServiceJob to query.
 * @return The current OperationStatus as a json_t object.
 * or <code>NULL</code> upon error.
 * @memberof ServiceJob
 */
WHEATIS_SERVICE_API json_t *GetServiceJobStatusAsJSON (ServiceJob *job_p);


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
 */
WHEATIS_SERVICE_API ServiceJob *CreateServiceJobFromJSON (const json_t *json_p);


/**
 * @brief Get a ServiceJob from a json_t object.
 *
 * @param job_p The ServiceJob which will be filled in from the json data.
 * @param json_p The json object representing a ServiceJob.
 * @return <code>true</code> if the ServiceJob was created successfully,
 * <code>false</code> otherwise.
 * @memberof ServiceJob
 */
WHEATIS_SERVICE_API bool SetServiceJobFromJSON (ServiceJob *job_p, const json_t *json_p);


/**
 * @brief Get the Current OperationStatus of a ServiceJob.
 *
 * @param job_p The ServiceJob to query.
 * @return The current OperationStatus.
 * @memberof ServiceJob
 */
WHEATIS_SERVICE_API OperationStatus GetServiceJobStatus (ServiceJob *job_p);


/**
 * @brief Get the name of ServiceJob.
 *
 * @param job_p The ServiceJob to query.
 * @return The name of the ServiceJob
 * @memberof ServiceJob
 */
WHEATIS_SERVICE_API const char *GetServiceJobName (const ServiceJob *job_p);


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
WHEATIS_SERVICE_API bool CloseServiceJob (ServiceJob *job_p);


/**
 * @brief Check if any ServiceJobs within ServiceJobSet are still running
 *
 * @param jobs_p The ServiceJobSet to check.
 * @return <code>true</code> if there are still some ServiceJobs in the given set
 * that are still running, <code>false</code> otherwise.
 * @memberof ServiceJobSet
 */
WHEATIS_SERVICE_API bool AreAnyJobsLive (const ServiceJobSet *jobs_p);


#ifdef __cplusplus
}
#endif



#endif /* SERVICE_JOB_H_ */
