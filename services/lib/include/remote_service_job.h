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
 * remote_service_job.h
 *
 *  Created on: 14 Feb 2016
 *      Author: billy
 */

#ifndef SERVICES_LIB_INCLUDE_REMOTE_SERVICE_JOB_H_
#define SERVICES_LIB_INCLUDE_REMOTE_SERVICE_JOB_H_

#include "jansson.h"

#include "service_job.h"
#include "grassroots_service_library.h"


typedef struct RemoteServiceJob
{
	ServiceJob rsj_job;

	char *rsj_uri_s;

	char *rsj_service_name_s;

	uuid_t rsj_job_id;
} RemoteServiceJob;



#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_SERVICE_API RemoteServiceJob *AllocateRemoteServiceJob (struct Service *service_p, const char *job_name_s, const char *job_description_s, const char * const working_directory_s, const char *remote_service_s, const char *remote_uri_s, uuid_t remote_job_id);


GRASSROOTS_SERVICE_API void FreeRemoteServiceJob (ServiceJob *job_p);


GRASSROOTS_SERVICE_API RemoteServiceJob *CreateRemoteServiceJobFromResultsJSON (const json_t *results_p, struct Service *service_p, const char *name_s, const char *description_s, OperationStatus status);


#ifdef __cplusplus
}
#endif


#endif /* SERVICES_LIB_INCLUDE_REMOTE_SERVICE_JOB_H_ */
