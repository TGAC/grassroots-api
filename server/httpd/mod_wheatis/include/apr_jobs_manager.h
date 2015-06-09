/*
 * apr_jobs_manager.h
 *
 *  Created on: 19 May 2015
 *      Author: tyrrells
 */

#ifndef APR_JOBS_MANAGER_H_
#define APR_JOBS_MANAGER_H_

#include "httpd.h"

#include "mod_wheatis_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Get the JobsManager to use in the WheatIS module.
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


#ifdef __cplusplus
}
#endif

#endif /* APR_JOBS_MANAGER_H_ */
