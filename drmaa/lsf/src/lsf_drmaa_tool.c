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


/***********************************/

static bool DeleteJobTemplate (LSFDrmaaTool *tool_p);


/***********************************/


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
			DeleteJobTemplate (lsf_tool_p);
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


bool RunLSFDrmaaTool (DrmaaTool *tool_p, const bool async_flag)
{
	LSFDrmaaTool *lsf_tool_p = (LSFDrmaaTool *) tool_p;
	bool success_flag = false;

	if (BuildNativeSpecification (tool_p))
		{
			const char **args_ss = CreateAndAddArgsArray (tool_p);

			if (args_ss)
				{
					char error_s [DRMAA_ERROR_STRING_BUFFER];

					/*run a job*/
					int result = drmaa_run_job (lsf_tool_p -> ldt_id_s, sizeof (lsf_tool_p -> ldt_id_s) - 1, lsf_tool_p -> ldt_job_p, error_s, DRMAA_ERROR_STRING_BUFFER);

					/* Now the job has started we can delete its template */
					DeleteJobTemplate (lsf_tool_p -> ldt_job_p);

					if (result == DRMAA_ERRNO_SUCCESS)
						{
							if (!async_flag)
								{
									int stat;

									result = drmaa_wait (lsf_tool_p -> ldt_id_s, lsf_tool_p -> ldt_id_out_s, sizeof (lsf_tool_p -> ldt_id_out_s) - 1, &stat,
										DRMAA_TIMEOUT_WAIT_FOREVER, NULL, error_s, DRMAA_ERROR_STRING_BUFFER);

									if (result == DRMAA_ERRNO_SUCCESS)
										{
											int exited;
											int exit_status;

											success_flag = true;

											if (drmaa_wifexited (&exited, stat, error_s, DRMAA_ERROR_STRING_BUFFER) == 0)
												{
													PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "job <%s> may not have finished correctly", lsf_tool_p -> ldt_id_s, exit_status);
												}

											if (exited)
												{
													drmaa_wexitstatus (&exit_status, stat, error_s, DRMAA_ERROR_STRING_BUFFER);

													PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "job <%s> finished with exit code %d\n", lsf_tool_p -> ldt_id_s, exit_status);
												}
											else
												{
													int signal_status;

													drmaa_wifsignaled (&signal_status, stat, error_s, DRMAA_ERROR_STRING_BUFFER);

													if (signal_status)
														{
															char termsig [DRMAA_SIGNAL_BUFFER+1];
															drmaa_wtermsig (termsig, DRMAA_SIGNAL_BUFFER, stat, error_s, DRMAA_ERROR_STRING_BUFFER);

															PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "job <%s> finished due to signal %s\n", lsf_tool_p -> ldt_id_s, termsig);
														}
													else
														{
															PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "job <%s> is aborted\n", lsf_tool_p -> ldt_id_s);
														}
												}

										}		/* if (result == DRMAA_ERRNO_SUCCESS) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_wait failed with code %d, \"%s", result, error_s);
										}

								}		/* if (!async_flag) */

						}		/* if (result == DRMAA_ERRNO_SUCCESS) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_run_job failed with code %d, \"%s", result, error_s);
						}

					FreeAndRemoveArgsArray (tool_p, args_ss);
				}		/* if (args_ss) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to build args array");
				}

		}		/* if (BuildNativeSpecification (tool_p)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to build native specification");
		}

	return success_flag;
}


/***********************************/
/********* STATIC FUNCTIONS ********/
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

