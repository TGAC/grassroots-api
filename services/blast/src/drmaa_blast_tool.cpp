/*
 * system_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include "system_blast_tool.hpp"
#include "streams.h"


DrmaaBlastTool :: DrmaaBlastTool (ServiceJob *job_p, const char *name_s)
: ExternalBlastTool (job_p, name_s)
{
	dbt_drmaa_tool_p = AllocateDrmaaTool ("/tgac/software/testing/blast/2.2.30/x86_64/bin/blastn");
	ebt_arg_callback = &DrmaaBlastTool :: AddArgToDrmaaTool;
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
	const char *command_line_s = GetByteBufferData (ebt_buffer_p);
	int res;

	bt_job_p -> sj_status = OS_STARTED;
	res = system (command_line_s);

	bt_job_p -> sj_status = (res == 0) ? OS_SUCCEEDED : OS_FAILED;

	return bt_job_p -> sj_status;
}


bool DrmaaBlastTool :: AddArgToDrmaaTool (const char *arg_s)
{
	bool success_flag = AddDrmaaToolArgument (dbt_drmaa_tool_p, arg_s);

	return success_flag;
}

