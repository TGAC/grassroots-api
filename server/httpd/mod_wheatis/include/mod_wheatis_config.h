/*
 * mod_wheatis_config.h
 *
 *  Created on: 15 May 2015
 *      Author: billy
 */

#ifndef MOD_WHEATIS_CONFIG_H_
#define MOD_WHEATIS_CONFIG_H_

#include "apr_thread_mutex.h"


typedef struct
{
	const char *wisc_root_path_s;
	apr_thread_mutex_t *wisc_mutex_p;
	apr_pool_t *wisc_pool_p;
	server_rec *wisc_server_p;
} WheatISConfig;


#endif /* MOD_WHEATIS_CONFIG_H_ */
