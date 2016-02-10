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
 * paired_blast_service.h
 *
 *  Created on: 9 Feb 2016
 *      Author: billy
 */

#ifndef SERVICES_BLAST_INCLUDE_PAIRED_BLAST_SERVICE_H_
#define SERVICES_BLAST_INCLUDE_PAIRED_BLAST_SERVICE_H_

#include "blast_service_api.h"
#include "service.h"
#include "paired_service.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
 *
 * @param service_p
 * @param internal_params_p
 * @param db_counter
 * @return
 */
BLAST_SERVICE_LOCAL bool AddPairedServiceParameters (Service *service_p, ParameterSet *internal_params_p, uint16 db_counter);


/**
 *
 * @param service_p
 * @param params_p
 * @return
 */
BLAST_SERVICE_LOCAL json_t *PrepareRemoteJobsForRunning (Service *service_p, ParameterSet *params_p, PairedService *paired_service_p);


/**
 *
 * @param service_p
 * @param jobs_p
 * @param params_p
 * @param paired_service_p
 * @return
 */
BLAST_SERVICE_LOCAL int32 RunRemoteBlastJobs (Service *service_p, ServiceJobSet *jobs_p, ParameterSet *params_p, PairedService *paired_service_p);


#ifdef __cplusplus
}
#endif





#endif /* SERVICES_BLAST_INCLUDE_PAIRED_BLAST_SERVICE_H_ */