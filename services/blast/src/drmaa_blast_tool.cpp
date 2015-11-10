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
 * system_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include <new>

#include "drmaa_blast_tool.hpp"
#include "streams.h"



void DrmaaBlastTool :: SetCoresPerSearch (uint32 cores)
{
	dbt_drmaa_tool_p -> dt_num_cores = cores;
}


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


const char *DrmaaBlastTool :: GetResults ()
{
	return GetOutputData ();
}


OperationStatus DrmaaBlastTool :: Run ()
{
	if (RunDrmaaTool (dbt_drmaa_tool_p, dbt_async_flag))
		{
			bt_job_p -> sj_status = OS_STARTED;
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



bool DrmaaBlastTool :: SetOutputFilename (const char *filename_s)
{
	bool success_flag = false;

	if (ExternalBlastTool :: SetOutputFilename (filename_s))
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);

			if (buffer_p)
				{
					if (AppendStringsToByteBuffer (buffer_p, ":", filename_s, ".out", NULL))
						{
							success_flag = SetDrmaaToolOutputFilename (dbt_drmaa_tool_p, GetByteBufferData (buffer_p));
						}

					FreeByteBuffer (buffer_p);
				}
		}

	return success_flag;
}


bool DrmaaBlastTool :: SetQueue (const char *queue_s)
{
	bool success_flag = SetDrmaaToolQueueName (dbt_drmaa_tool_p, queue_s);

	return success_flag;
}
