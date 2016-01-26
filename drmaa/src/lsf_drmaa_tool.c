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
 * lsf_drmaa_tool.c
 *
 *  Created on: 26 Jan 2016
 *      Author: billy
 */

#include "lsf_drmaa_tool.h"

#include "streams.h"



LSFDrmaaTool *AllocateLSFDrmaaTool (const char *program_name_s)
{
	LSFDrmaaTool *lsf_tool_p = (LSFDrmaaTool *) AllocMemory (sizeof (LSFDrmaaTool));

	if (lsf_tool_p)
		{
			if (InitDrmaaTool (& (lsf_tool_p -> ldt_base_tool), program_name_s, RunLSFDrmaaTool, GetLSFDrmaaToolStatus))
				{
					char error_s [DRMAA_ERROR_STRING_BUFFER];
					int err_code = drmaa_allocate_job_template (& (lsf_tool_p -> ldt_job_p), error_s, DRMAA_ERROR_STRING_BUFFER);

					if (err_code == DRMAA_ERRNO_SUCCESS)
						{
							lsf_tool_p -> ldt_id_out_s = NULL;
							lsf_tool_p -> ldt_id_s = NULL;

							return lsf_tool_p;
						}

					ClearDrmaaTool (& (lsf_tool_p -> ldt_base_tool));
				}		/* if (InitDrmaaTool (& (lsf_tool_p -> ldt_base_tool), program_name_s, RunLSFDrmaaTool, GetLSFDrmaaToolStatus)) */

			FreeMemory (lsf_tool_p);
		}		/* if (lsf_tool_p) */

	return NULL;
}


void FreeLSFDrmaaTool (LSFDrmaaTool *lsf_tool_p)
{
	if (lsf_tool_p -> ldt_job_p)
		{

		}

	ClearDrmaaTool (& (lsf_tool_p -> ldt_base_tool));

	FreeMemory (lsf_tool_p);
}


OperationStatus GetLSFDrmaaToolStatus (DrmaaTool *tool_p)
{
	LSFDrmaaTool *lsf_tool_p = (LSFDrmaaTool *) tool_p;
	OperationStatus status = OS_ERROR;
	char error_s [DRMAA_ERROR_STRING_BUFFER];
	int drmaa_status;
	int res = drmaa_job_ps (lsf_tool_p -> ldt_id_s, &drmaa_status, error_s, DRMAA_ERROR_STRING_BUFFER);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			switch (drmaa_status)
				{
					case DRMAA_PS_QUEUED_ACTIVE:
					case DRMAA_PS_SYSTEM_ON_HOLD:
					case DRMAA_PS_USER_ON_HOLD:
					case DRMAA_PS_USER_SYSTEM_ON_HOLD:
						status = OS_PENDING;
						break;

					case DRMAA_PS_UNDETERMINED:
						status = OS_ERROR;
						break;

					case DRMAA_PS_RUNNING:
						status = OS_STARTED;
						break;

					case DRMAA_PS_FAILED:
						status = OS_FAILED;
						break;

					case DRMAA_PS_DONE:
						status = OS_SUCCEEDED;
						break;

					case DRMAA_PS_SYSTEM_SUSPENDED:
					case DRMAA_PS_USER_SUSPENDED:
					case DRMAA_PS_USER_SYSTEM_SUSPENDED:
						status = OS_PENDING;
						break;

					default:
						break;
				}

			PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa ps for %s: (%d = %d)", lsf_tool_p -> ldt_id_s, drmaa_status, status);

		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get drmaa ps for %s: %d, error: %s", lsf_tool_p -> ldt_id_s, res, error_s);
		}

	return status;
}




/***********************************/
/********* STATIC FJNCTIONS ********/
/***********************************/


static bool DeleteJobTemplate (LSFDrmaaTool *tool_p)
{
	int err_code = DRMAA_ERRNO_SUCCESS;
	char error_s [DRMAA_ERROR_STRING_BUFFER];

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Entering DeleteJobTemplate job %0.16X", tool_p -> dt_job_p);
	#endif

	if (tool_p -> ldt_job_p)
		{
			err_code = drmaa_delete_job_template (tool_p -> ldt_job_p, error_s, DRMAA_ERROR_STRING_BUFFER);

			if (err_code != DRMAA_ERRNO_SUCCESS)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_delete_job_template failed with error code %d \"%s\"", err_code, error_s);
				}

			tool_p -> ldt_job_p = NULL;
		}

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Exiting DeleteJobTemplate");
	#endif

	return (err_code == DRMAA_ERRNO_SUCCESS);
}

