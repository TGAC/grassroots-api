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


/**
 * A datatype describing a ServiceJob that is run on a remote
 * Grassroots Server.
 *
 * @extends ServiceJob
 */
typedef struct RemoteServiceJob
{
	/** The base ServiceJob. */
	ServiceJob rsj_job;

	/** The URI for the ExternalServer that is running the RemoteServiceJob. */
	char *rsj_uri_s;

	/** The name of the Service that will run the RemoteServiceJob. */
	char *rsj_service_name_s;

	/** The UUID of the RemoteServiceJob on this local Server. */
	uuid_t rsj_job_id;
} RemoteServiceJob;



#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate a RemoteServiceJob.
 *
 * @param service_p The local Service will connect to the ExternalServer that will run this RemoteServiceJob.
 * @param job_name_s The name of the RemoteServiceJob.
 * @param job_description_s An optional description to give the RemoteServiceJob. This can be <code>NULL</code>.
 * @param remote_service_s The name of the Service on the ExternalServer that will run the RemoteServiceJob.
 * @param remote_uri_s  The URI for the ExternalServer that is running the RemoteServiceJob.
 * @param remote_job_id The UUID of the RemoteServiceJob on this local Server.
 * @return The new RemoteServiceJob or <code>NULL</code> upon error.
 * @memberof RemoteServiceJob
 */
GRASSROOTS_SERVICE_API RemoteServiceJob *AllocateRemoteServiceJob (struct Service *service_p, const char *job_name_s, const char *job_description_s, const char *remote_service_s, const char *remote_uri_s, uuid_t remote_job_id);


/**
 * Free the RemoteServiceJob.
 *
 * @param job_p The RemoteServiceJob to free.
 * @memberof RemoteServiceJob
 */
GRASSROOTS_SERVICE_API void FreeRemoteServiceJob (ServiceJob *job_p);


/**
 * Create a RemoteServiceJob from the JSON fragment returned from the ExternalServer
 * that ran the job.
 *
 * @param results_p The JSON fragment returned by the ExternalServer.
 * @param service_p The Service on the local Server.
 * @param name_s The name to give the RemoteServiceJob.
 * @param description_s An optional description to give the RemoteServiceJob. This can be <code>NULL</code>.
 * @param status The OperationStatus of the RemoteServiceJob.
 * @return The newly-created RemoteServiceJob or <code>NULL</code> upon error.
 * @memberof RemoteServiceJob
 */
GRASSROOTS_SERVICE_API RemoteServiceJob *CreateRemoteServiceJobFromResultsJSON (const json_t *results_p, struct Service *service_p, const char *name_s, const char *description_s, OperationStatus status);


#ifdef __cplusplus
}
#endif


#endif /* SERVICES_LIB_INCLUDE_REMOTE_SERVICE_JOB_H_ */
