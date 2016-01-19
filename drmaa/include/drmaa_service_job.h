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

#include "jansson.h"


typedef struct DrmaaServiceJob
{
	ServiceJob dsj_job;
	DrmaaTool *dsj_drmaa_p;
} DrmaaServiceJob;


#ifdef __cplusplus
extern "C" {
#endif


GRASSROOTS_DRMAA_API DrmaaServiceJob *AllocateDrmaaServiceJob (const char *drmaa_program_name_s, Service *service_p, const char *job_name_s);


GRASSROOTS_DRMAA_API void InitDrmaaServiceJob (DrmaaServiceJob *job_p);


GRASSROOTS_DRMAA_API ServiceJobSet *AllocateDrmaaServiceJobSet (void);


GRASSROOTS_DRMAA_API void FreeDrmaaServiceJobSet (ServiceJobSet *jobs_p);


GRASSROOTS_DRMAA_API json_t *GetDrmmaaServiceJobAsJSON (const DrmaaServiceJob * const job_p);


GRASSROOTS_DRMAA_API DrmaaServiceJob *GetDrmmaaServiceJobFromJSON (const json_t * const json_p);



#ifdef __cplusplus
}
#endif


#endif /* DRMAA_SERVICE_JOB_H_ */
