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
 * apr_jobs_manager.h
 *
 *  Created on: 19 May 2015
 *      Author: tyrrells
 */

#ifndef APR_JOBS_MANAGER_H_
#define APR_JOBS_MANAGER_H_

#include "httpd.h"

#include "mod_grassroots_config.h"
#include "apr_global_storage.h"



/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_JSON_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_RESOURCE_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * resource.c.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_APR_JOBS_MANAGER_TAGS
	#define APR_JOBS_MANAGER_PREFIX
	#define APR_JOBS_MANAGER_VAL(x)	= x
#else
	#define APR_JOBS_MANAGER_PREFIX extern
	#define APR_JOBS_MANAGER_VAL(x)
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */


/**
 * The identifier for the named shared object cache to use for sharing ServiceJobs
 * between different httpd processes and threads.
 *
 * @ingroup httpd_server
 */
APR_JOBS_MANAGER_PREFIX const char *APR_JOBS_MANAGER_CACHE_ID_S APR_JOBS_MANAGER_VAL("grassroots-jobs-socache");


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Get the JobsManager to use in the Grassroots module.
 *
 * @param server_p The httpd server that the module is running on.
 * @param pool_p The memory pool to use for any allocations that the APRJobsManagre might need.
 * @param provider_name_s The cache provider to use.
 * @memberof APRJobsManager
 * @return A newly-allocated APRJobsManager or <code>NULL</code> upon error.
 */
APRJobsManager *InitAPRJobsManager (server_rec *server_p, apr_pool_t *pool_p, const char *provider_name_s);


/**
 * @brief Adjust an APRJobsManager for when an httpd child process is started.
 *
 * When an httpd process is forked at startup, this routine sets up the necessary
 * configuration details.
 * @param server_p The httpd server that the module is running on.
 * @param pool_p The memory pool to use for any allocations that the APRJobsManager might need.
 * @memberof APRJobsManager
 * @return <code>true</code> if the APRJobsManager is adjusted correctly,
 * <code>false</code> upon error.
 */
bool APRJobsManagerChildInit (apr_pool_t *pool_p, server_rec *server_p);


/**
 * @brief Release any resources that an APRJobsManager.
 *
 * This release any resources that an APRJobsManager might have just prior to
 * the Apache httpd server shutting down.
 *
 * @param value_p A pointer to the APRJobsManager to clean up.
 * @return <code>AP_SUCCESS</code> if successful, or an error code if there was a problem.
 * @memberof APRJobsManager
 */
apr_status_t CleanUpAPRJobsManager (void *value_p);


/**
 * Configure an APRJobsManager in the Apache parent process before any child processes
 * are launched.
 *
 * @param manager_p The APRJobsManager to configure.
 * @param config_pool_p The memory pool available to use.
 * @param server_p The Apache server structure.
 * @param provider_name_s The name of sharded object cache provider to use.
 * @return <code>true</code> if successful or <code>false</code> if there was a problem.
 * @memberof APRJobsManager
 */
bool PostConfigAPRJobsManager (APRJobsManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s);


/**
 * Set up an APRJobsManager prior to the configuration details being used.
 *
 * @param manager_p The APRJobsManager to initialise.
 * @param config_pool_p A memory pool to use if needed.
 * @return <code>true</code> if the initialisation was successful or <code>false</code> if there was a problem.
 * @memberof APRJobsManager
 */
bool APRJobsManagerPreConfigure (APRJobsManager *manager_p, apr_pool_t *config_pool_p);

/**
 * Notify that a ServiceJob with a given UUID has finished and
 * can be removed from the given JobsManager.
 *
 * @param jobs_manager_p The JobsManager to remove the ServiceJob from.
 * @param job_key The UUID of the ServiceJob to be removed.
 *
 * @memberof APRJobsManager
 */
void APRServiceJobFinished (JobsManager *jobs_manager_p, uuid_t job_key);


/**
 * Free an APRJobsManager.
 *
 * @param jobs_manager_p The APRJobsManager to free.
 * @return<code>true</code> if the APRJobsManager was freed successfully, ,
 * <code>false</code> upon error.
 * @memberof APRJobsManager
 */
bool DestroyAPRJobsManager (APRJobsManager *jobs_manager_p);


#ifdef __cplusplus
}
#endif

#endif /* APR_JOBS_MANAGER_H_ */
