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
 * service_job_set_iterator.h
 *
 *  Created on: 19 Jan 2016
 *      Author: tyrrells
 */

#ifndef SERVICE_JOB_SET_ITERATOR_H_
#define SERVICE_JOB_SET_ITERATOR_H_

#include "grassroots_service_library.h"
#include "service_job.h"


/**
 * A datatype allowing easy traversal over all of
 * the ServiceJobs within a ServiceJobSet.
 */
typedef struct ServiceJobSetIterator
{
	/** The ServiceJobSet to traverse. */
	ServiceJobSet *sjsi_job_set_p;

	/** The current position in the entries of the ServiceJobSet. */
	ServiceJobNode *sjs_current_node_p;
} ServiceJobSetIterator;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate a ServiceJobSetIterator
 *
 * @param set_p The ServiceJobSet to iterate over
 * @return  The ServiceJobSetIterator or <code>NULL</code> upon error.
 * @memberof ServiceJobSetIterator
 */
GRASSROOTS_SERVICE_API ServiceJobSetIterator *AllocateServiceJobSetIterator (ServiceJobSet *set_p);


/**
 * Initialise a ServiceJobSetIterator to point to the first entry in a given ServiceJobSet.
 *
 * @param iterator_p The ServiceJobSetIterator to initialise.
 * @param set_p The ServiceJobSet to iterate over.
 * @memberof ServiceJobSetIterator
 */
GRASSROOTS_SERVICE_API void InitServiceJobSetIterator (ServiceJobSetIterator *iterator_p, ServiceJobSet *set_p);

/**
 * Free a ServiceJobSetIterator
 *
 * @param iterator_p The ServiceJobSetIterator to free.
 * @memberof ServiceJobSetIterator
 */
GRASSROOTS_SERVICE_API void FreeServiceJobSetIterator (ServiceJobSetIterator *iterator_p);


/**
 * Get the next ServiceJob from the Iterator.
 *
 * @param iterator_p The ServiceJobSetIterator to get the ServiceJob from.
 * @return The next ServiceJob or <code>NULL</code> if all of the ServiceJobs
 * in the ServiceJobSet have been read.
 * @memberof ServiceJobSetIterator.
 */
GRASSROOTS_SERVICE_API ServiceJob *GetNextServiceJobFromServiceJobSetIterator (ServiceJobSetIterator *iterator_p);


/**
 * Has a ServiceJobSetIterator parsed all of the entries in a ServiceJobSet.
 *
 * @param iterator_p The ServiceJobSetIterator to check.
 * @return <code>true</code> if the end of the ServiceJobSet has been reached, <code>false</code>
 * otherwise.
 * @memberof ServiceJobSetIterator
 */
GRASSROOTS_SERVICE_API bool HasServiceJobSetIteratorFinished (const ServiceJobSetIterator *iterator_p);


#ifdef __cplusplus
	}
#endif


#endif /* SERVICE_JOB_SET_ITERATOR_H_ */
