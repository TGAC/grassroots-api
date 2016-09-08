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
/*
 * linked_service.h
 *
 *  Created on: 6 Sep 2016
 *      Author: tyrrells
 */

#ifndef SERVER_SRC_SERVICES_LIB_INCLUDE_LINKED_SERVICE_H_
#define SERVER_SRC_SERVICES_LIB_INCLUDE_LINKED_SERVICE_H_

#include "grassroots_service_library.h"

#include "linked_list.h"
#include "mapped_parameter.h"


typedef struct LinkedService
{
	char *ls_input_service_s;

	LinkedList *ls_mapped_params_p;
} LinkedService;


typedef struct LinkedServiceNode
{
	ListItem lsn_node;

	LinkedService *lsn_linked_service_p;
} LinkedServiceNode;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate a LinkedService.
 *
 * @param input_service_s The name of the input service. The LinkedService will make a deep copy
 * of this and store that value.
 * @return The newly-allocated LinkedService or <code>NULL</code> upon error.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API LinkedService *AllocateLinkedService (const char *input_service_s);


/**
 * Free a LinkedService.
 *
 * @param linked_service_p The LinkedService to free.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API void FreeLinkedService (LinkedService *linked_service_p);


/**
 * Allocate a LinkedServiceNode to store a LinkedService on a LinkedList.
 *
 * @param linked_service_p The LinkedService to store in this LinkedServiceNode.
 * @return The newly-allocated LinkedServiceNode or <code>NULL</code> upon error.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API LinkedServiceNode *AllocateLinkedServiceNode (LinkedService *linked_service_p);


/**
 * Free a LinkedServiceNode and its associated LinkedService.
 *
 * @param node_p The LinkedServiceNode to free.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API void FreeLinkedServiceNode (ListItem *node_p);


GRASSROOTS_SERVICE_API bool CreateAndAddMappedParameterToLinkedService (LinkedService *linked_service_p, const char *input_s, const char *output_s);


GRASSROOTS_SERVICE_API bool AddMappedParameterToLinkedService (LinkedService *linked_service_p, MappedParameter *mapped_param_p);


#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_SERVICES_LIB_INCLUDE_LINKED_SERVICE_H_ */
