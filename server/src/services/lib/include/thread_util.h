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
 * @file
 * @brief
 */
/*
 * thread_util.h
 *
 *  Created on: 18 Apr 2015
 *      Author: billy
 */

#ifndef THREAD_UTIL_H_
#define THREAD_UTIL_H_

#include <pthread.h>

#include "grassroots_service_manager_library.h"


GRASSROOTS_SERVICE_MANAGER_API pthread_t *CreateRunningThread (void *run_fn_p);


GRASSROOTS_SERVICE_MANAGER_LOCAL void ThreadFinished (void);


#endif /* THREAD_UTIL_H_ */
