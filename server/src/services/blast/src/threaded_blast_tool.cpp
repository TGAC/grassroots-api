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
#include "streams.h"
#include "blast_service_job.h"


static void *RunThreadedBlast (void *data_p);


ThreadedBlastTool :: ThreadedBlastTool (BlastServiceJob *service_job_p, const char *name_s, const char *factory_s, const BlastServiceData *data_p, const char *blast_program_name_s)
	: SystemBlastTool (service_job_p, name_s, factory_s, data_p, blast_program_name_s)
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

			if (res != 0)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "pthread_mutex_destroy failed with %d", res);
				}
		}

	if (tbt_thread_id != -1)
		{
			pthread_exit (NULL);
		}
}


OperationStatus ThreadedBlastTool :: Run ()
{
	OperationStatus status;

	SetServiceJobStatus (& (bt_job_p -> bsj_job), OS_IDLE);

	int id = pthread_create (tbt_thread_p, NULL, RunThreadedBlast, NULL);

	if (id == 0)
		{
			tbt_thread_id = id;
			status = OS_PENDING;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "pthread_create failed with %d", id);
			status = OS_FAILED_TO_START;
		}


	SetServiceJobStatus (& (bt_job_p -> bsj_job), status);

	return status;
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

	status = GetServiceJobStatus (& (bt_job_p -> bsj_job));

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
