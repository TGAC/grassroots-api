/*
 * system_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include "system_blast_tool.hpp"
#include "streams.h"


SystemBlastTool :: SystemBlastTool (ServiceJob *job_p)
: ExternalBlastTool (job_p)
{

}

SystemBlastTool :: ~SystemBlastTool ()
{

}


bool SystemBlastTool :: ParseParameters (ParameterSet *params_p)
{
	bool success_flag = false;

	if (AppendStringToByteBuffer (ebt_buffer_p, "blastn "))
		{
			/* TESTING REMOVE THIS WHEN DATABASE SELECTOR IS ACTIVE */
			if (AppendStringToByteBuffer (ebt_buffer_p, "-db testdb "))
				{
					success_flag = ExternalBlastTool :: ParseParameters (params_p);
				}
		}

	return success_flag;
}


const char *SystemBlastTool :: GetResults ()
{
	return GetOutputData ();
}


OperationStatus SystemBlastTool :: Run ()
{
	const char *command_line_s = GetByteBufferData (ebt_buffer_p);
	int res;

	bt_job_p -> sj_status = OS_STARTED;
	res = system (command_line_s);

	bt_job_p -> sj_status = (res == 0) ? OS_SUCCEEDED : OS_FAILED;

	return bt_job_p -> sj_status;
}



