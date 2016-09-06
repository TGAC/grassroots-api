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


typedef struct MappedParameter
{
	char *mp_input_param_s;

	char *mp_output_param_s;
} MappedParameter;


typedef struct MappedParameterNode
{
	ListItem mpn_node;

	MappedParameter *mpn_mapped_param_p;
} MappedParameterNode;


typedef struct LinkedService
{
	char *ls_input_service_s;
	LinkedList *ls_mapped_params_p;

} LinkedService;


#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_SERVICE_API MappedParameter *AllocateMappedParameter (const char *input_s, const char *output_s);


GRASSROOTS_SERVICE_API void FreeMappedParameter (MappedParameter *mapped_param_p);


GRASSROOTS_SERVICE_API MappedParameterNode *AllocateMappedParameterNode (MappedParameter *mapped_param_p);


GRASSROOTS_SERVICE_API void FreeMappedParameterNode (ListItem *node_p);


GRASSROOTS_SERVICE_API bool AddMappedParameter (LinkedService *linked_service_p, const char *input_s, const char *output_s);



#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_SERVICES_LIB_INCLUDE_LINKED_SERVICE_H_ */
