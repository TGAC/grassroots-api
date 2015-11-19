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
 * threaded_blast_tool.cpp
 *
 *  Created on: 9 Nov 2015
 *      Author: billy
 */

#include "threaded_blast_tool.hpp"


static void *RunThreadedBlast (void *data_p);


ThreadedBlastTool :: ThreadedBlastTool (ServiceJob *service_job_p, const char *name_s, const char *working_directory_s, const char *blast_program_name_s)
	: SystemBlastTool (service_job_p, name_s, working_directory_s, blast_program_name_s)
{
	tbt_thread_p = 0;
	tbt_mutex_p = 0;
	tbt_thread_id = -1;
}


ThreadedBlastTool :: ~ThreadedBlastTool ()
{
	if (tbt_mutex_p)
		{
			int res = pthread_mutex_destroy (tbt_mutex_p);
		}

	if (tbt_thread_id != -1)
		{
			pthread_exit (NULL);
		}
}


OperationStatus ThreadedBlastTool :: Run ()
{
	bt_status = OS_IDLE;

	int id = pthread_create (tbt_thread_p, NULL, RunThreadedBlast, NULL);

	if (id == 0)
		{
			tbt_thread_id = id;
		}
	else
		{
			bt_status = OS_FAILED_TO_START;
		}

	return bt_status;
}


const char *ThreadedBlastTool :: GetResults ()
{
	return NULL;
}


OperationStatus ThreadedBlastTool :: GetStatus ()
{
	OperationStatus status = OS_ERROR;

	int res = pthread_mutex_lock (tbt_mutex_p);
	if (res)
		{
			/* an error has occurred */
	    perror ("pthread_mutex_lock");
	    pthread_exit (NULL);
		}

	status = bt_status;

	res = pthread_mutex_unlock (tbt_mutex_p);
	if (res)
		{
	    perror ("pthread_mutex_unlock");
	    pthread_exit (NULL);
		}

	return status;
}


static void *RunThreadedBlast (void *data_p)
{

	return NULL;
}
