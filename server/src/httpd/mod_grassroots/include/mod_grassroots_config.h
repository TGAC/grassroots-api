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
 * mod_grassroots_config.h
 *
 *  Created on: 15 May 2015
 *      Author: billy
 */

#ifndef MOD_GRASSROOTS_CONFIG_H_
#define MOD_GRASSROOTS_CONFIG_H_

#include "httpd.h"
#include "http_config.h"
#include "apr_global_mutex.h"
#include "ap_provider.h"
#include "ap_socache.h"

#include "jobs_manager.h"
#include "servers_pool.h"
#include "apr_global_storage.h"
#include "string_utils.h"


/**
 * This datatype is used by the Apache HTTPD server
 * to be the Grassroots JobsManager.
 *
 * @extends JobsManager
 *
 * @ingroup httpd_server
 */
typedef struct APRJobsManager
{
	/** The base JobsManager */
	JobsManager ajm_base_manager;

	/**
	 * The APRGlobalStorage implementation where the
	 * ServiceJobs will be stored.
	 */
	APRGlobalStorage *ajm_store_p;
} APRJobsManager;


/**
 * This datatype is used by the Apache HTTPD server
 * to be the Grassroots ServersManager.
 *
 * @extends ServersManager
 *
 *
 * @ingroup httpd_server
 */
typedef struct APRServersManager
{
	/** The base ServersManager */
	ServersManager asm_base_manager;

	/**
	 * The APRGlobalStorage implementation where the
	 * Server definitions will be stored.
	 */
	APRGlobalStorage *asm_store_p;

} APRServersManager;


/** @publicsection */

/**
 * @brief The configuration for the Grassroots module.
 *
 * @ingroup httpd_server
 */
typedef struct
{
	/** The path to the Grassroots installation */
	const char *wisc_root_path_s;

	/** The cache provider to use. */
	const char *wisc_provider_name_s;

	/** The server_rec that the module is running on. */
	server_rec *wisc_server_p;

	/** The JobsManager that the module is using. */
	APRJobsManager *wisc_jobs_manager_p;

	/** The ServersManager that the module is using. */
	APRServersManager *wisc_servers_manager_p;

} ModGrassrootsConfig;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Get the Grassroots module.
 *
 * @return the GrassrootsModule.
 *
 * @ingroup httpd_server
 */
const module *GetGrassrootsModule (void);



#ifdef __cplusplus
}
#endif



#endif /* MOD_GRASSROOTS_CONFIG_H_ */
