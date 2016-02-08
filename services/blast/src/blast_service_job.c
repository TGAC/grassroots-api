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




BlastServiceJob *AllocateBlastServiceJob (Service *service_p, const char *job_name_s, const char *job_description_s, const char * const working_directory_s)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) AllocMemory (sizeof (BlastServiceJob));

	if (blast_job_p)
		{
			ServiceJob * const base_service_job_p = & (blast_job_p -> bsj_job);
			BlastTool *tool_p = NULL;

			InitServiceJob (base_service_job_p, service_p, job_name_s, job_description_s, NULL);

			tool_p = CreateBlastTool (base_service_job_p, job_name_s, working_directory_s);

			if (tool_p)
				{
					blast_job_p -> bsj_tool_p = tool_p;

					return blast_job_p;
				}		/* if (tool_p) */

			ClearServiceJob (base_service_job_p);
			FreeMemory (blast_job_p);
		}		/* if (blast_job_p) */

	return NULL;
}


void FreeBlastServiceJob (ServiceJob *job_p)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) job_p;

	FreeBlastTool (blast_job_p -> bsj_tool_p);
	ClearServiceJob (job_p);

	FreeMemory (blast_job_p);
}
