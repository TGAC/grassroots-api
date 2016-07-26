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
#include "blast_service.h"

class BlastTool;

/**
 * This datatype stores the ServiceJob and its associated BlastTool.
 *
 * @extends ServiceJob
 */
typedef struct BLAST_SERVICE_LOCAL BlastServiceJob
{
	/** The ServiceJob. */
	ServiceJob bsj_job;

	/** The BlastTool. */
	BlastTool *bsj_tool_p;
} BlastServiceJob;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Create a BlastServiceJob.
 *
 * @param service_p The Blast service.
 * @param job_name_s The name of the BlastServiceJob.
 * @param job_description_s The description of the BlastServiceJob. This can be <code>NULL</code>.
 * @param data_p The BlastServiceData.
 * @return The newly-allocated BlastServiceJob or <code>NULL</code> upon error.
 */
BLAST_SERVICE_LOCAL BlastServiceJob *AllocateBlastServiceJob (Service *service_p, const char *job_name_s, const char *job_description_s, BlastServiceData *data_p);


/**
 * Free the BlastServiceJob.
 *
 * @param job_p The BlastServiceJob to free.
 * @memberof BlastServiceJob
 */
BLAST_SERVICE_LOCAL void FreeBlastServiceJob (ServiceJob *job_p);


BLAST_SERVICE_LOCAL BlastServiceJob *CreateBlastServiceJobFromResultsJSON (const json_t *results_p, Service *service_p, const char *name_s, const char *description_s, OperationStatus status);


BLAST_SERVICE_LOCAL char *GetPreviousJobFilename (const BlastServiceData *data_p, const char *job_id_s, const char *suffix_s);


BLAST_SERVICE_LOCAL unsigned char *SerialiseBlastServiceJob (ServiceJob *base_job_p, unsigned int *value_length_p);


BLAST_SERVICE_LOCAL ServiceJob *DeserialiseBlastServiceJob (unsigned char *data_p, void *config_p);


BLAST_SERVICE_LOCAL BlastServiceJob *GetBlastServiceJobFromJSON (const json_t *blast_job_json_p, BlastServiceData *config_p);


BLAST_SERVICE_LOCAL json_t *ConvertBlastServiceJobToJSON (BlastServiceJob *job_p);


BLAST_SERVICE_LOCAL bool AddErrorToBlastServiceJob (BlastServiceJob *job_p);


BLAST_SERVICE_LOCAL bool UpdateBlastServiceJob (ServiceJob *job_p);



#ifdef __cplusplus
}
#endif


#endif /* BLAST_SERVICE_JOB_H_ */
