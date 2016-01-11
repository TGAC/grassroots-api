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
 * drmaa_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include <new>

#include "drmaa_blast_tool.hpp"
#include "streams.h"
#include "string_utils.h"

#ifdef _DEBUG
	#define DRMAA_BLAST_TOOL_DEBUG	(STM_LEVEL_FINE)
#else
	#define DRMAA_BLAST_TOOL_DEBUG (STM_LEVEL_NONE)
#endif



DrmaaBlastTool :: DrmaaBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s, const char *blast_program_name_s, bool async_flag)
: ExternalBlastTool (job_p, name_s, working_directory_s, blast_program_name_s)
{
	dbt_drmaa_tool_p = AllocateDrmaaTool (blast_program_name_s);

	if (!dbt_drmaa_tool_p)
		{
			throw std :: bad_alloc ();
		}

	dbt_async_flag = async_flag;
	SetDrmaaToolQueueName (dbt_drmaa_tool_p, "webservices");
	SetDrmaaToolJobName (dbt_drmaa_tool_p, name_s);
}


DrmaaBlastTool :: ~DrmaaBlastTool ()
{
	FreeDrmaaTool (dbt_drmaa_tool_p);
}


void DrmaaBlastTool :: SetCoresPerSearch (uint32 cores)
{
	dbt_drmaa_tool_p -> dt_num_cores = cores;
}


bool DrmaaBlastTool :: SetEmailNotifications (const char **email_addresses_ss)
{
	return SetDrmaaToolEmailNotifications (dbt_drmaa_tool_p, email_addresses_ss);
}


OperationStatus DrmaaBlastTool :: Run ()
{
	if (RunDrmaaTool (dbt_drmaa_tool_p, dbt_async_flag))
		{
			#if DRMAA_BLAST_TOOL_DEBUG >= STM_LEVEL_FINE
				{
					char *uuid_s = GetUUIDAsString (bt_job_p -> sj_id);

					if (uuid_s)
						{
							PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Added drmma blast tool %s with job id %s", uuid_s, dbt_drmaa_tool_p -> dt_id_s);
							FreeCopiedString (uuid_s);
						}
					else
						{
							PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Added drmma blast tool with job id %s", dbt_drmaa_tool_p -> dt_id_s);
						}
				}
			#endif

			bt_job_p -> sj_status = GetStatus ();
		}
	else
		{
			bt_job_p -> sj_status = OS_FAILED;
		}

	return bt_job_p -> sj_status;
}


bool DrmaaBlastTool :: AddArg (const char *arg_s)
{
	bool success_flag = AddDrmaaToolArgument (dbt_drmaa_tool_p, arg_s);

	return success_flag;
}


OperationStatus DrmaaBlastTool :: GetStatus ()
{
	bt_status = GetDrmaaToolStatus (dbt_drmaa_tool_p);

	return bt_status;
}



bool DrmaaBlastTool :: SetUpOutputFile ()
{
	bool success_flag = false;

	if (ExternalBlastTool :: SetUpOutputFile ())
		{
			char *job_id_s = GetUUIDAsString (bt_job_p -> sj_id);

			if (job_id_s)
				{
					char *logfile_stem_s = NULL;
					bool alloc_logfile_stm_flag = false;

					if (ebt_working_directory_s)
						{
							logfile_stem_s = MakeFilename (ebt_working_directory_s, job_id_s);
							alloc_logfile_stm_flag = true;
						}
					else
						{
							logfile_stem_s = job_id_s;
						}

					if (logfile_stem_s)
						{
							char *logfile_s = ConcatenateVarargsStrings (":", logfile_stem_s, ".log", NULL);

							if (logfile_s)
								{
									if (SetDrmaaToolOutputFilename (dbt_drmaa_tool_p, logfile_s))
										{
											success_flag = true;
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set drmaa logfile name to \"%s\"", logfile_stem_s);
										}

									FreeCopiedString (logfile_s);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get logfile name for \"%s\" with stem \"%s\"", job_id_s, logfile_stem_s);
								}

							if (alloc_logfile_stm_flag)
								{
									FreeCopiedString (logfile_stem_s);
								}

						}		/* if (logfile_stem_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get logile stem for \"%s\"", job_id_s);
						}

					FreeUUIDString (job_id_s);
				}		/* if (job_id_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid string for %s", bt_job_p -> sj_name_s);
				}

		}		/* if (ExternalBlastTool :: SetOutputFile ()) */

	return success_flag;
}
