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

/**
 * unix_process.c
 *
 *  Created on: 2 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#include <errno.h>
#include <spawn.h>
#include <sys/wait.h>

#include "process.h"
#include "streams.h"


int32 CreateProcess (const char * const program_s, char **args_ss, char **environment_ss)
{
	pid_t pid = -1;
	int res = posix_spawn (&pid, program_s, NULL, NULL, args_ss, environment_ss);

	if (res != 0)
		{

		}

	return (int32) pid;
}


bool TerminateProcess (int32 process_id)
{
	return false;
}


OperationStatus GetProcessStatus (int32 process_id)
{
	OperationStatus status = OS_ERROR;
	int state;
	pid_t pid = waitpid (process_id, &state, WNOHANG);

	if (pid == 0)
		{
			status = OS_STARTED;
		}
	else if (pid == -1)
		{
			switch (errno)
				{
					case ECHILD:
						PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Process " INT32_FMT " could not be found", process_id);
						break;

					case EINTR:
						PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "waitpid () was interrupted for process " INT32_FMT, process_id);
						break;

					case EINVAL:
						PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "The options value for waitpid () is invalid for process " INT32_FMT, process_id);
						break;
				}
		}
	else
		{
			if (WIFEXITED (state))
				{
					int process_exit_code = WEXITSTATUS (state);

					if (process_exit_code == 0)
						{
							status = OS_SUCCEEDED;
						}
					else
						{
							status = OS_ERROR;
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Process " INT32_FMT " exited with state " INT32_FMT, process_id, process_exit_code);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Process " INT32_FMT " exited unsuccessfully ", process_id);
				}
		}

	return status;
}

