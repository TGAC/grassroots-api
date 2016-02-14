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
 * blast_service_job.c
 *
 *  Created on: 21 Jan 2016
 *      Author: tyrrells
 */

#include "blast_service_job.h"


BlastServiceJob *CreateBlastServiceJobFromResultsJSON (const json_t *results_p, Service *service_p, const char *name_s, const char *description_s, OperationStatus status)
{
	BlastServiceJob *job_p = (BlastServiceJob *) AllocMemory (sizeof (BlastServiceJob));

	if (job_p)
		{
			if (InitServiceJobFromResultsJSON (& (job_p -> bsj_job), results_p, service_p, name_s, description_s, status))
				{
					job_p -> bsj_tool_p = NULL;
					return job_p;
				}

			FreeMemory (job_p);
		}		/* if (job_p) */

	return NULL;
}


BlastServiceJob *AllocateBlastServiceJob (Service *service_p, const char *job_name_s, const char *job_description_s, const char * const working_directory_s, bool has_tool_flag)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) AllocMemory (sizeof (BlastServiceJob));

	if (blast_job_p)
		{
			bool success_flag = true;
			BlastTool *tool_p = NULL;
			ServiceJob * const base_service_job_p = & (blast_job_p -> bsj_job);

			InitServiceJob (base_service_job_p, service_p, job_name_s, job_description_s, NULL);

			if (has_tool_flag)
				{
					tool_p = CreateBlastTool (base_service_job_p, job_name_s, working_directory_s);


					if (tool_p)
						{
							blast_job_p -> bsj_tool_p = tool_p;
						}		/* if (tool_p) */
					else
						{
							success_flag = false;
						}
				}
			else
				{
					blast_job_p -> bsj_tool_p = NULL;
				}

			if (success_flag)
				{
					return blast_job_p;
				}

			ClearServiceJob (base_service_job_p);
			FreeMemory (blast_job_p);
		}		/* if (blast_job_p) */

	return NULL;
}


void FreeBlastServiceJob (ServiceJob *job_p)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) job_p;

	if (blast_job_p -> bsj_tool_p)
		{
			FreeBlastTool (blast_job_p -> bsj_tool_p);
		}

	ClearServiceJob (job_p);

	FreeMemory (blast_job_p);
}
