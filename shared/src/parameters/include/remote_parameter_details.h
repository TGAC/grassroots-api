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
 * remote_parameter_details.h
 *
 *  Created on: 6 Feb 2016
 *      Author: billy
 */

#ifndef REMOTE_PARAMETER_DETAILS_H_
#define REMOTE_PARAMETER_DETAILS_H_


#include "grassroots_params_library.h"
#include "linked_list.h"
#include "jansson.h"

/**
 * This datatype is used to denote that a Parameter is
 * for use on a PairedService.
 *
 * @ingroup parameters_group
 */
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
	 * <code>NULL</code>. If it is for an external Service, then this
	 * is the name for the ExternalServer's Parameter running that Service.
	 */
	char *rpd_name_s;

} RemoteParameterDetails;


/**
 * A ListItem for storing RemoteParameterDetails
 * on a LinkedList.
 *
 * @extends ListItem
 * @ingroup parameters_group
 */
typedef struct RemoteParameterDetailsNode
{
	/** The base node. */
	ListItem rpdn_node;

	/** The RemoteParameterDetails. */
	RemoteParameterDetails *rpdn_details_p;
} RemoteParameterDetailsNode;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Allocate a RemoteParameterDetails.
 *
 * @param uri_s The URI of the ExternalServer that runs the PairedService that this RemoteParameter belongs to.
 * @param param_s The name for this Parameter on the PairedService.
 * @memberof RemoteParameterDetails
 * @return The new RemoteParameterDetails or <code>NULL</code> upon error.
 */
GRASSROOTS_PARAMS_API	RemoteParameterDetails *AllocateRemoteParameterDetails (const char * const uri_s, const char * const param_s);


/**
 * Free a RemoteParameterDetails.
 *
 * @param details_p The RemoteParameterDetails to free.
 * @memberof RemoteParameterDetails
 */
GRASSROOTS_PARAMS_API	void FreeRemoteParameterDetails (RemoteParameterDetails *details_p);


/**
 * Allocate a RemoteParameterDetailsNode.
 *
 * @param details_p The RemoteParameterDetails to store in the new RemoteParameterDetailsNode.
 * @memberof RemoteParameterDetailsNode
 * @return The new RemoteParameterDetailsNode or <code>NULL</code> upon error.
 */
GRASSROOTS_PARAMS_API	RemoteParameterDetailsNode *AllocateRemoteParameterDetailsNode (RemoteParameterDetails *details_p);


/**
 * Allocate a new RemoteParameterDetailsNode and a new RemoteParameterDetails to store in it.
 *
 * @param uri_s The URI of the ExternalServer that runs the PairedService that this RemoteParameter belongs to.
 * @param param_s The name for this Parameter on the PairedService.
 * @memberof RemoteParameterDetailsNode
 * @return The new RemoteParameterDetailsNode or <code>NULL</code> upon error.
 */
GRASSROOTS_PARAMS_API	RemoteParameterDetailsNode *AllocateRemoteParameterDetailsNodeByParts (const char * const uri_s,  const char * const param_s);


/**
 * Free a RemoteParameterDetailsNode.
 *
 * @param node_p The RemoteParameterDetailsNode to free.
 * @memberof RemoteParameterDetailsNode
 */
GRASSROOTS_PARAMS_API	void FreeRemoteParameterDetailsNode (ListItem *node_p);


/**
 * Get the JSON fragment representing a RemoteParameterDetails.
 *
 * @param details_p The RemoteParameterDetails to get the JSON fragment for.
 * @return The JSON fragment or <code>NULL</code> upon error.
 * @memberof RemoteParameterDetails
 * @see CreateRemoteParameterDetailsFromJSON
 */
GRASSROOTS_PARAMS_API	json_t *GetRemoteParameterDetailsAsJSON (const RemoteParameterDetails *details_p);


/**
 * Create a RemoteParameterDetails from a JSON fragment representation.
 *
 * @param json_p The JSON fragment representing the RemoteParameterDetails to get.
 * @return The RemoteParameterDetails or <code>NULL</code> upon error.
 * @memberof RemoteParameterDetails
 * @see GetRemoteParameterDetailsAsJSON
 */
GRASSROOTS_PARAMS_API	RemoteParameterDetails *CreateRemoteParameterDetailsFromJSON (const json_t *json_p);


#ifdef __cplusplus
}
#endif



#endif /* REMOTE_PARAMETER_DETAILS_H_ */
