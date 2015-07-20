/*
 * apr_servers_manager.h
 *
 *  Created on: 18 Jun 2015
 *      Author: billy
 */

#ifndef APR_SERVERS_MANAGER_H_
#define APR_SERVERS_MANAGER_H_

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
#ifdef ALLOCATE_APR_SERVERS_MANAGER_TAGS
	#define APR_SERVERS_MANAGER_PREFIX
	#define APR_SERVERS_MANAGER_VAL(x)	= x
#else
	#define APR_SERVERS_MANAGER_PREFIX extern
	#define APR_SERVERS_MANAGER_VAL(x)
#endif


APR_SERVERS_MANAGER_PREFIX const char *APR_SERVERS_MANAGER_CACHE_ID_S APR_SERVERS_MANAGER_VAL("wheatis-servers-socache");


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Get the ServersManager to use in the WheatIS module.
 *
 * @param server_p The httpd server that the module is running on.
 * @param pool_p The memory pool to use for any allocations that the APRServersManagre might need.
 * @param provider_name_s The cache provider to use.
 * @memberof APRServersManager
 * @return A newly-allocated APRServersManager or <code>NULL</code> upon error.
 */
APRServersManager *InitAPRServersManager (server_rec *server_p, apr_pool_t *pool_p, const char *provider_name_s);


/**
 * @brief Adjust an APRServersManager for when an httpd child process is started.
 *
 * When an httpd process is forked at startup, this routine sets up the necessary
 * configuration details.
 * @param server_p The httpd server that the module is running on.
 * @param pool_p The memory pool to use for any allocations that the APRServersManager might need.
 * @memberof APRServersManager
 * @return <code>true</code> if the APRServersManager is adjusted correctly,
 * <code>false</code> upon error.
 */
bool APRServersManagerChildInit (apr_pool_t *pool_p, server_rec *server_p);


/**
 * @brief Release any resources that an APRServersManager.
 *
 * This release any resources that an APRServersManager might have just prior to
 * the Apache httpd server shutting down.
 *
 * @param value_p A pointer to the APRServersManager to clean up.
 * @return <code>AP_SUCCESS</code> if successful, or an error code if there was a problem.
 * @memberof APRServersManager
 */
apr_status_t CleanUpAPRServersManager (void *value_p);



bool PostConfigAPRServersManager (APRServersManager *manager_p, apr_pool_t *config_pool_p, server_rec *server_p, const char *provider_name_s);


bool DestroyAPRServersManager (APRServersManager *manager_p);


bool APRServersManagerPreConfigure (APRServersManager *manager_p, apr_pool_t *config_pool_p);



#ifdef __cplusplus
}
#endif


#endif /* APR_SERVERS_MANAGER_H_ */
