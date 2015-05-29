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


APRJobsManager *InitAPRJobsManager (server_rec *server_p, apr_pool_t *pool_p, const char *provider_name_s);


#ifdef __cplusplus
}
#endif

#endif /* APR_JOBS_MANAGER_H_ */
