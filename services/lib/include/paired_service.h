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
 * paired_service.h
 *
 *  Created on: 1 Feb 2016
 *      Author: billy
 */

#ifndef PAIRED_SERVICE_H_
#define PAIRED_SERVICE_H_



#include "jansson.h"
#include "uuid/uuid.h"
#include "parameter_set.h"


#include "linked_list.h"
#include "grassroots_service_library.h"


typedef struct PairedService
{
	uuid_t ps_extenal_server_id;
	char *ps_name_s;
	char *ps_uri_s;
	ParameterSet *ps_params_p;
} PairedService;


typedef struct PairedServiceNode
{
	ListItem psn_node;
	PairedService *psn_paired_service_p;
} PairedServiceNode;


#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_SERVICE_API PairedService *AllocatePairedService (const uuid_t id, const char *name_s, const char *uri_s, const json_t *op_p);


GRASSROOTS_SERVICE_API void FreePairedService (PairedService *paired_service_p);


GRASSROOTS_SERVICE_API PairedServiceNode *AllocatePairedServiceNode (PairedService *paired_service_p);


GRASSROOTS_SERVICE_API PairedServiceNode *AllocatePairedServiceNodeByParts (const uuid_t id, const char *name_s, const char *uri_s, const json_t *op_p);


GRASSROOTS_SERVICE_API void FreePairedServiceNode (ListItem *node_p);




#ifdef __cplusplus
}
#endif

#endif /* PAIRED_SERVICE_H_ */
