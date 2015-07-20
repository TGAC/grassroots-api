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
#ifdef ALLOCATE_APR_JOBS_MANAGER_TAGS
	#define APR_JOBS_MANAGER_PREFIX
	#define APR_JOBS_MANAGER_VAL(x)	= x
#else
	#define APR_JOBS_MANAGER_PREFIX extern
	#define APR_JOBS_MANAGER_VAL(x)
#endif


APR_JOBS_MANAGER_PREFIX const char *APR_JOBS_MANAGER_CACHE_ID_S APR_JOBS_MANAGER_VAL("wheatis-jobs-socache");


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



bool PostConfigAPRJobsManager (APRJobsManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s);

bool APRJobsManagerPreConfigure (APRJobsManager *manager_p, apr_pool_t *config_pool_p);

void APRServiceJobFinished (JobsManager *jobs_manager_p, uuid_t job_key);


bool DestroyAPRJobsManager (APRJobsManager *jobs_manager_p);


#ifdef __cplusplus
}
#endif

#endif /* APR_JOBS_MANAGER_H_ */
