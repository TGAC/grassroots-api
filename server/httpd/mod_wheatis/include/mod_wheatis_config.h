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


typedef struct
{
	const char *wisc_root_path_s;
	server_rec *wisc_server_p;

	/** Our cross-thread/cross-process mutex */
	apr_global_mutex_t *wisc_mutex_p;
} WheatISConfig;


#endif /* MOD_WHEATIS_CONFIG_H_ */
