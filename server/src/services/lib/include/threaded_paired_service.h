/*
** Copyright 2014-2015 The Genome Analysis Centre
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
/*
 * threaded_paired_service.h
 *
 *  Created on: 20 Jul 2016
 *      Author: tyrrells
 */

#ifndef SRC_SERVICES_LIB_INCLUDE_THREADED_PAIRED_SERVICE_H_
#define SRC_SERVICES_LIB_INCLUDE_THREADED_PAIRED_SERVICE_H_

#include "paired_service.h"
#include "parameter_set.h"
#include "providers_state_table.h"
#include "grassroots_service_library.h"
#include "linked_list.h"


typedef struct RemoteServiceData
{
	const char * const rsd_service_name_s;
	ParameterSet *rsd_params_p;
	PairedService *rsd_paired_service_p;
	ProvidersStateTable *rsd_providers_p;
} RemoteServiceData;


typedef struct PThreadNode
{
	ListItem ptn_node;
	pthread_t *ptn_thread_p;
} PThreadNode;


#endif /* SRC_SERVICES_LIB_INCLUDE_THREADED_PAIRED_SERVICE_H_ */
