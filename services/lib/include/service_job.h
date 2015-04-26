/*
 * service_job.h
 *
 *  Created on: 25 Apr 2015
 *      Author: billy
 */

#ifndef SERVICE_JOB_H_
#define SERVICE_JOB_H_


#include "wheatis_service_library.h"
#include "operation.h"
#include "jansson.h"
#include "uuid/uuid.h"

/* forward declaration */
struct Service;

/**
 * A datatype to represent a running task.
 */
typedef struct ServiceJob
{
	/** The Service that is running this job. */
	struct Service *sj_service_p;

	/** The unique identifier for this job. */
	uuid_t sj_id;

	/** Is the service currently in an open state? */
	enum OperationStatus sj_status;
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


WHEATIS_SERVICE_API void InitServiceJob (ServiceJob *job_p, struct Service *service_p);


WHEATIS_SERVICE_API ServiceJobSet *AllocateServiceJobSet (struct Service *service_p, size_t num_jobs);


WHEATIS_SERVICE_API void FreeServiceJobSet (ServiceJobSet *job_set_p);


WHEATIS_SERVICE_API ServiceJob *GetJobById (const ServiceJobSet *jobs_p, const uuid_t job_id);


WHEATIS_SERVICE_API json_t *GetServiceJobSetAsJSON (const ServiceJobSet *jobs_p);


WHEATIS_SERVICE_API json_t *GetServiceJobAsJSON (const ServiceJob *job_p);


WHEATIS_SERVICE_API json_t *GetServiceJobStatusAsJSON (const ServiceJob *job_p);



#ifdef __cplusplus
}
#endif



#endif /* SERVICE_JOB_H_ */
