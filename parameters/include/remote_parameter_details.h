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
 * remote_parameter_details.h
 *
 *  Created on: 6 Feb 2016
 *      Author: billy
 */

#ifndef REMOTE_PARAMETER_DETAILS_H_
#define REMOTE_PARAMETER_DETAILS_H_


#include "grassroots_params_library.h"
#include "tags.h"
#include "linked_list.h"
#include "jansson.h"


typedef struct RemoteParameterDetails
{
	/**
	 * If the Parameter belongs to a local Service, this will be
	 * NULL. If it is for an external Service, then this
	 * is the uri for the ExternalServer running that Service.
	 */
	char *rpd_server_uri_s;

	/**
	 * If the Parameter belongs to a local Service, this will be
	 * 0. If it is for an external Service, then this
	 * is the tag for the ExternalServer's Parameter running that Service.
	 */
	Tag rpd_tag;
} RemoteParameterDetails;


typedef struct RemoteParameterDetailsNode
{
	ListItem rpdn_node;
	RemoteParameterDetails *rpdn_details_p;
} RemoteParameterDetailsNode;


#ifdef __cplusplus
extern "C" {
#endif



GRASSROOTS_PARAMS_API	RemoteParameterDetails *AllocateRemoteParameterDetails (const char * const uri_s, Tag tag);


GRASSROOTS_PARAMS_API	void FreeRemoteParameterDetails (RemoteParameterDetails *details_p);


GRASSROOTS_PARAMS_API	RemoteParameterDetailsNode *AllocateRemoteParameterDetailsNode (RemoteParameterDetails *details_p);


GRASSROOTS_PARAMS_API	RemoteParameterDetailsNode *AllocateRemoteParameterDetailsNodeByParts (const char * const uri_s, Tag tag);


GRASSROOTS_PARAMS_API	void FreeRemoteParameterDetailsNode (ListItem *node_p);


GRASSROOTS_PARAMS_API	json_t *GetRemoteParameterDetailsAsJSON (const RemoteParameterDetails *details_p);


GRASSROOTS_PARAMS_API	RemoteParameterDetails *CreateRemoteParameterDetailsFromJSON (const json_t *json_p);



#ifdef __cplusplus
}
#endif



#endif /* REMOTE_PARAMETER_DETAILS_H_ */
