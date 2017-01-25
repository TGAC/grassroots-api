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
 * threaded_paired_service.h
 *
 *  Created on: 9 Jan 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SERVER_SRC_SERVICES_LIB_INCLUDE_THREADED_PAIRED_SERVICE_H_
#define SERVER_SRC_SERVICES_LIB_INCLUDE_THREADED_PAIRED_SERVICE_H_

#include "paired_service.h"
#include "parameter_set.h"
#include "providers_state_table.h"
#include "grassroots_service_library.h"
#include "linked_list.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * A datatype to facilitate running PairedServices in parallel
 * rather than sequentially which leads to longer waiting times
 * for the caller who is running the Services.
 *
 * @memberof service_group
 */
typedef struct RemoteServiceData
{
	/** The name of the local Service */
	const char * const rsd_service_name_s;

	/** The ParameterSet that will be used to run the PairedService */
	ParameterSet *rsd_params_p;

	/** The PairedService that will be ran. */
	PairedService *rsd_paired_service_p;

	/** The details of all available ExternalServers. */
	ProvidersStateTable *rsd_providers_p;
} RemoteServiceData;


/**
 * Datatype for storing a pthread_t on a LinkedList.
 *
 * @extends ListItem
 * @memberof service_group
 */
typedef struct PThreadNode
{
	/** The base ListItem */
	ListItem ptn_node;

	/** Pointer to the pthread_t to store */
	pthread_t *ptn_thread_p;
} PThreadNode;



#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_SERVICES_LIB_INCLUDE_THREADED_PAIRED_SERVICE_H_ */
