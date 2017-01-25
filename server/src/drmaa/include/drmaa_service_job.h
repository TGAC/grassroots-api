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

/**
 * @file
 * @brief
 */
/*
 * drmaa_service_job.h
 *
 *  Created on: 19 Jan 2016
 *      Author: tyrrells
 */

#ifndef DRMAA_SERVICE_JOB_H_
#define DRMAA_SERVICE_JOB_H_

#include "drmaa_library.h"
#include "drmaa_tool.h"
#include "service_job.h"
#include "service.h"
#include "jansson.h"


/**
 * A ServiceJob that also has the data for
 * running as Drmaa-based task.
 *
 * @extends ServiceJob
 * @ingroup drmaa_group
 */
typedef struct DrmaaServiceJob
{
	/** The base ServiceJob. */
	ServiceJob dsj_job;

	/** The associated DrmaaTool. */
	DrmaaTool *dsj_drmaa_p;
} DrmaaServiceJob;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Allocate a DrmaaServiceJob.
 *
 * @param drmaa_program_name_s The command line tool that will be run using DRMAA.
 * @param service_p The Service that will run the ServiceJob.
 * @param job_name_s The name to give to the job.
 * @return The DrmaaServiceJob or <code>DrmaaServiceJob</code>.
 * @memberof DrmaaServiceJob
 */
GRASSROOTS_DRMAA_API DrmaaServiceJob *AllocateDrmaaServiceJob (const char *drmaa_program_name_s, Service *service_p, const char *job_name_s);


/**
 * Initialise a DrmaaServiceJob.
 *
 * @param job_p The DrmaaServiceJob to initialise.
 * @param service_p The Service that will run the ServiceJob.
 * @param job_name_s The name to give to the job.
 * @memberof DrmaaServiceJob
 */
GRASSROOTS_DRMAA_API void InitDrmaaServiceJob (DrmaaServiceJob *job_p, Service *service_p, const char *job_name_s);


/**
 * Allocate a ServiceJobSet to store DrmaaServiceJobs.
 *
 * @return The ServiceJobSet or <code>NULL</code> upon error.
 */
GRASSROOTS_DRMAA_API ServiceJobSet *AllocateDrmaaServiceJobSet (void);


/**
 * Free a DrmaaServiceJobSet.
 *
 * @param jobs_p The ServiceJobSet to free.
 */
GRASSROOTS_DRMAA_API void FreeDrmaaServiceJobSet (ServiceJobSet *jobs_p);


/**
 * Serialise the DrmaaServiceJob into a JSON fragment.
 *
 * @param job_p The DrmaaServiceJob.
 * @return The JSON fragment or <code>NULL</code> upon error.
 * @memberof DrmaaServiceJob
 * @see GetDrmmaaServiceJobFromJSON
 */
GRASSROOTS_DRMAA_API json_t *GetDrmmaaServiceJobAsJSON (const DrmaaServiceJob * const job_p);


/**
 * Deserialise a DrmaaServiceJob from a JSON fragment.
 *
 * @param json_p The JSON fragment.
 * @return The DrmaaServiceJob or <code>NULL</code> upon error.
 * @memberof DrmaaServiceJob
 * @see GetDrmmaaServiceJobAsJSON
 */
GRASSROOTS_DRMAA_API DrmaaServiceJob *GetDrmmaaServiceJobFromJSON (const json_t * const json_p);



#ifdef __cplusplus
}
#endif


#endif /* DRMAA_SERVICE_JOB_H_ */
