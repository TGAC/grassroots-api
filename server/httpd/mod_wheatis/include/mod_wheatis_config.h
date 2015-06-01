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


typedef struct APRJobsManager
{
	JobsManager ajm_base_manager;

	apr_hash_t *ajm_running_jobs_p;

	/** Our cross-thread/cross-process mutex */
	apr_global_mutex_t *ajm_mutex_p;

	apr_pool_t *ajm_pool_p;

	char *ajm_mutex_lock_filename_s;

	server_rec *ajm_server_p;

	ap_socache_provider_t *ajm_socache_provider_p;
	ap_socache_instance_t *ajm_socache_instance_p;

} APRJobsManager;

typedef struct
{
	const char *wisc_root_path_s;
	const char *wisc_provider_name_s;
	server_rec *wisc_server_p;
	APRJobsManager *wisc_jobs_manager_p;
} ModWheatISConfig;


#ifdef __cplusplus
extern "C"
{
#endif


const module *GetWheatISModule (void);


#ifdef __cplusplus
}
#endif



#endif /* MOD_WHEATIS_CONFIG_H_ */
