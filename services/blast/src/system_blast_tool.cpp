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
#include "string_utils.h"


SystemBlastTool :: SystemBlastTool (ServiceJob *job_p, const char *name_s, const BlastServiceData *data_p, const char *blast_program_name_s)
: ExternalBlastTool (job_p, name_s, data_p, blast_program_name_s)
{
	if (!AddArg (blast_program_name_s))
		{
			throw std::bad_alloc ();
		}
}

SystemBlastTool :: ~SystemBlastTool ()
{

}


bool SystemBlastTool :: ParseParameters (ParameterSet *params_p)
{
	bool success_flag = false;

	if (ExternalBlastTool :: ParseParameters (params_p))
		{
			char *logfile_s = GetJobFilename (NULL, BS_LOG_SUFFIX_S);

			if (logfile_s)
				{
					if (AddArgsPair (">>", logfile_s))
						{
							if (AddArg ("2>&1"))
								{
									success_flag = true;
								}
						}

					FreeCopiedString (logfile_s);
				}

		}		/* if (ExternalBlastTool :: ParseParameters (params_p)) */

	return success_flag;
}


OperationStatus SystemBlastTool :: Run ()
{
	const char *command_line_s = GetByteBufferData (ebt_buffer_p);
	int res;

	bt_job_p -> sj_status = OS_STARTED;
	res = system (command_line_s);

	if (res == 0)
		{
			bt_job_p -> sj_status = OS_SUCCEEDED;
		}
	else
		{
			bt_job_p -> sj_status = OS_FAILED;
		}

	return bt_job_p -> sj_status;
}

