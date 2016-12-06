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
#include "blast_service.h"
#include "providers_state_table.h"


#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_LOCAL bool AddPairedServiceParameters (Service *service_p, ParameterSet *internal_params_p, uint16 db_counter);


BLAST_SERVICE_LOCAL int32 RunRemoteBlastJobs (Service *service_p, ServiceJobSet *jobs_p, ParameterSet *params_p, PairedService *paired_service_p, ProvidersStateTable *providers_p);


/**
 * Try to get the results of a previously run BlastServiceJob on a remote server.
 *
 * @param local_job_id_s The uuid as a c-style string on the local Server.
 * @param output_format_code The output format to convert the result to.
 * @param blast_data_p The configuration data for the local BlastService.
 * @return The previous result or <code>NULL</code> upon error.
 */
BLAST_SERVICE_LOCAL char *GetPreviousRemoteBlastServiceJob (const char *local_job_id_s, const uint32 output_format_code, const BlastServiceData *blast_data_p);


#ifdef __cplusplus
}
#endif





#endif /* SERVICES_BLAST_INCLUDE_PAIRED_BLAST_SERVICE_H_ */
