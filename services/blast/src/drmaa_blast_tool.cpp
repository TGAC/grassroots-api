/*
 * system_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include "drmaa_blast_tool.hpp"
#include "streams.h"


DrmaaBlastTool :: DrmaaBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s)
: ExternalBlastTool (job_p, name_s, working_directory_s)
{
	dbt_drmaa_tool_p = AllocateDrmaaTool ("/tgac/software/testing/blast/2.2.30/x86_64/bin/blastn");
	SetDrmaaToolQueueName(dbt_drmaa_tool_p, "-q webservices");
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
	if (RunDrmaaTool (dbt_drmaa_tool_p, false))
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

