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
/*
 * service_job_set_iterator.c
 *
 *  Created on: 19 Jan 2016
 *      Author: tyrrells
 */

#include "memory_allocations.h"
#include "service_job_set_iterator.h"



ServiceJobSetIterator *AllocateServiceJobSetIterator (ServiceJobSet *set_p)
{
	ServiceJobSetIterator *iterator_p = (ServiceJobSetIterator *) AllocMemory (sizeof (ServiceJobSetIterator));

	if (iterator_p)
		{
			if (set_p)
				{
					InitServiceJobSetIterator (iterator_p, set_p);
				}
			else
				{
					iterator_p -> sjs_current_node_p = NULL;
					iterator_p -> sjsi_job_set_p = NULL;
				}
		}

	return iterator_p;
}


void InitServiceJobSetIterator (ServiceJobSetIterator *iterator_p, ServiceJobSet *set_p)
{
	iterator_p -> sjs_current_node_p = (ServiceJobNode *) (set_p -> sjs_jobs_p -> ll_head_p);
	iterator_p -> sjsi_job_set_p = set_p;
}


void FreeServiceJobSetIterator (ServiceJobSetIterator *iterator_p)
{
	FreeMemory (iterator_p);
}


ServiceJob *GetNextServiceJobFromServiceJobSetIterator (ServiceJobSetIterator *iterator_p)
{
	ServiceJob *job_p = NULL;

	if (iterator_p -> sjs_current_node_p)
		{
			job_p = iterator_p -> sjs_current_node_p -> sjn_job_p;
			iterator_p -> sjs_current_node_p = (ServiceJobNode *) (iterator_p -> sjs_current_node_p -> sjn_node.ln_next_p);
		}

	return job_p;
}


bool HasServiceJobSetIteratorFinished (const ServiceJobSetIterator *iterator_p)
{
	return (iterator_p -> sjs_current_node_p != NULL);
}
