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
 * blast_service_job.h
 *
 *  Created on: 21 Jan 2016
 *      Author: tyrrells
 */

#ifndef BLAST_SERVICE_JOB_H_
#define BLAST_SERVICE_JOB_H_


#include "blast_service_api.h"
#include "service.h"
#include "service_job.h"
#include "blast_tool.hpp"

typedef struct BLAST_SERVICE_LOCAL BlastServiceJob
{
	ServiceJob bsj_job;
	BlastTool *bsj_tool_p;
} BlastServiceJob;


#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_LOCAL BlastServiceJob *AllocateBlastServiceJob (Service *service_p, const char *job_name_s, const char *job_description_s, const char * const working_directory_s, bool has_tool_flag);


BLAST_SERVICE_LOCAL void FreeBlastServiceJob (ServiceJob *job_p);


BLAST_SERVICE_LOCAL BlastServiceJob *CreateBlastServiceJobFromResultsJSON (const json_t *results_p, Service *service_p, const char *name_s, const char *description_s, OperationStatus status, const uuid_t *id_p);


#ifdef __cplusplus
}
#endif


#endif /* BLAST_SERVICE_JOB_H_ */
