/*
 * mod_wheatis_config.h
 *
 *  Created on: 15 May 2015
 *      Author: billy
 */

#ifndef MOD_WHEATIS_CONFIG_H_
#define MOD_WHEATIS_CONFIG_H_

#include "httpd.h"
#include "http_config.h"
#include "apr_global_mutex.h"
#include "ap_provider.h"
#include "ap_socache.h"

#include "jobs_manager.h"

/**
 * @brief A datatype to manage all of the ServiceJobs on a WheatIS system running on Apache httpd.
 *
 *
 * @see JobsManager
 */

typedef struct APRJobsManager
{
	/** The base JobsManager */
	JobsManager ajm_base_manager;

	/** @privatesection */
	apr_hash_t *ajm_running_jobs_p;

	/** Our cross-thread/cross-process mutex */
	apr_global_mutex_t *ajm_mutex_p;

	/** The pool to use for any temporary memory allocations */
	apr_pool_t *ajm_pool_p;

	char *ajm_mutex_lock_filename_s;

	server_rec *ajm_server_p;

	ap_socache_provider_t *ajm_socache_provider_p;
	ap_socache_instance_t *ajm_socache_instance_p;

} APRJobsManager;


/** @publicsection */

/**
 * @brief The configuration for the WheatIS module.
 */
typedef struct
{
	/** The path to the WheatIS installation */
	const char *wisc_root_path_s;

	/** The cache provider to use. */
	const char *wisc_provider_name_s;

	/** The server_rec that the module is running on. */
	server_rec *wisc_server_p;

	/** The JobsManager that the module is using. */
	APRJobsManager *wisc_jobs_manager_p;
} ModWheatISConfig;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Get the WheatIS module.
 *
 * @return the WheatISModule.
 */
const module *GetWheatISModule (void);



#ifdef __cplusplus
}
#endif



#endif /* MOD_WHEATIS_CONFIG_H_ */
