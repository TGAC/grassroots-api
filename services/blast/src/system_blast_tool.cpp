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

#include "system_blast_tool.hpp"
#include "streams.h"


SystemBlastTool :: SystemBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s, const char *blast_program_name_s)
: ExternalBlastTool (job_p, name_s, working_directory_s, blast_program_name_s)
{

}

SystemBlastTool :: ~SystemBlastTool ()
{

}


bool SystemBlastTool :: ParseParameters (ParameterSet *params_p, const char *filename_s)
{
	bool success_flag = false;

	if (AddArg ("blastn "))
		{
			success_flag = ExternalBlastTool :: ParseParameters (params_p, filename_s);
		}

	return success_flag;
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




