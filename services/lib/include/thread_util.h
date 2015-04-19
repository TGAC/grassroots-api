/*
 * thread_util.h
 *
 *  Created on: 18 Apr 2015
 *      Author: billy
 */

#ifndef THREAD_UTIL_H_
#define THREAD_UTIL_H_

#include <pthread.h>

#include "wheatis_service_manager_library.h"


WHEATIS_SERVICE_MANAGER_API pthread_t *CreateRunningThread (void *run_fn_p);


WHEATIS_SERVICE_MANAGER_LOCAL void ThreadFinished (void);


#endif /* THREAD_UTIL_H_ */
