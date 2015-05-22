/*
 * mod_wheatis_config.h
 *
 *  Created on: 15 May 2015
 *      Author: billy
 */

#ifndef MOD_WHEATIS_CONFIG_H_
#define MOD_WHEATIS_CONFIG_H_

#include "httpd.h"
#include "apr_global_mutex.h"


typedef struct APRJobsManager
{
	apr_hash_t *ajmc_running_jobs_p;

	/** Our cross-thread/cross-process mutex */
	apr_global_mutex_t *ajmc_mutex_p;

	apr_pool_t *ajmc_pool_p;

	char *ajmc_mutex_lock_filename_s;

} APRJobsManager;

typedef struct
{
	const char *wisc_root_path_s;
	server_rec *wisc_server_p;
	APRJobsManager *wisc_jobs_manager_p;
} ModWheatISConfig;





#endif /* MOD_WHEATIS_CONFIG_H_ */
