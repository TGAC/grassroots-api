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
/*
 * drmaa_service_job.c
 *
 *  Created on: 19 Jan 2016
 *      Author: tyrrells
 */

#include "drmaa_service_job.h"
#include "streams.h"


DrmaaServiceJob *AllocateDrmaaServiceJob (const char *drmaa_program_name_s, Service *service_p, const char *job_name_s)
{
	DrmaaServiceJob *job_p = NULL;
	DrmaaTool *drmaa_tool_p = AllocateDrmaaTool (drmaa_program_name_s);

	if (drmaa_tool_p)
		{
			job_p = (DrmaaServiceJob *) AllocMemory (sizeof (DrmaaServiceJob));

			if (job_p)
				{
					InitDrmaaServiceJob (job_p, service_p, job_name_s, NULL);
					job_p -> dsj_drmaa_p = drmaa_tool_p;
				}
			else
				{
					FreeDrmaaTool (drmaa_tool_p);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate drmaa service job");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate drmaa tool");
		}

	return job_p;
}


void InitDrmaaServiceJob (DrmaaServiceJob *job_p, Service *service_p, const char *job_name_s)
{
	InitServiceJob (& (job_p -> dsj_job), service_p, job_name_s, NULL, NULL, NULL, NULL);
}



json_t *GetDrmaaServiceJobAsJSON (const DrmaaServiceJob * const job_p)
{
	json_t *json_p = GetServiceJobAsJSON (& (job_p -> dsj_job));

	if (json_p)
		{

		}		/* if (json_p) */

	return json_p;
}


DrmaaServiceJob *GetDrmaaServiceJobFromJSON (const json_t * const json_p)
{

}
