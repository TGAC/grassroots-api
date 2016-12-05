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

#include "providers_state_table.h"


/**
 * A datatype for describing a remote Service
 * that can be used in conjuction with a local
 * Service.
 */
typedef struct PairedService
{
	/** The UUID for the ExternalServer that runs the PairedService. */
	uuid_t ps_extenal_server_id;

	/** The name of the PairedService. */
	char *ps_name_s;

	/** The name of the ExternalServer. */
	char *ps_server_name_s;

	/** The URI of the ExternalServer's Grassroots service. */
	char *ps_server_uri_s;

	/** The JSON fragment detailing the Provider of this PairedService. */
	json_t *ps_provider_p;

	/** The ParameterSet for this PairedService. */
	ParameterSet *ps_params_p;
} PairedService;


/**
 * A datatype for storing PairedServices on
 * a LinkedList.
 *
 * @extends ListItem
 */
typedef struct PairedServiceNode
{
	/** The base list node. */
	ListItem psn_node;

	/** The PairedService. */
	PairedService *psn_paired_service_p;
} PairedServiceNode;


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Allocate a PairedService.
 *
 * @param id The UUID of the ExternalServer that has the PairedService.
 * @param name_s The name of the PairedService.
 * @param server_uri_s The URI for the ExternalServer's Grassroots access.
 * @param server_name_s The name of the ExternalServer.
 * @param op_p The JSON fragment for the Service.
 * @param provider_p The JSON fragment from the server configuration file detailing the Provider of this PairedService.
 * @return The new PairedService or <code>NULL</code> upon error.
 * @memberof PairedService
 */
GRASSROOTS_SERVICE_API PairedService *AllocatePairedService (const uuid_t id, const char *name_s, const char *server_uri_s, const char *server_name_s, const json_t *op_p, const json_t *provider_p);


/**
 * Free a PairedService.
 *
 * @param paired_service_p The PairedService to free.
 * @memberof PairedService
 */
GRASSROOTS_SERVICE_API void FreePairedService (PairedService *paired_service_p);


/**
 * Allocate a PairedServiceNode.
 *
 * @param paired_service_p The PairedService to store in the new PairedServiceNode.
 * @return The new PairedServiceNode or <code>NULL</code> upon error.
 * @memberof PairedServiceNode
 */
GRASSROOTS_SERVICE_API PairedServiceNode *AllocatePairedServiceNode (PairedService *paired_service_p);


/**
 * Allocate a PairedService and attach it to a PairedServiceNode.
 *
 * @param id The UUID of the ExternalServer that has the PairedService.
 * @param name_s The name of the PairedService.
 * @param server_uri_s The URI for the ExternalServer's Grassroots access.
 * @param server_name_s The name of the ExternalServer.
 * @param op_p The JSON fragment for the Service.
 * @param provider_p The JSON fragment representing the Provider.
 * @ref server_configuration.md
 * @return The new PairedServiceNode or <code>NULL</code> upon error.
 * @memberof PairedServiceNode
 * @see AllocatePairedService
 */
GRASSROOTS_SERVICE_API PairedServiceNode *AllocatePairedServiceNodeByParts (const uuid_t id, const char *name_s, const char *server_uri_s, const char *server_name_s, const json_t *op_p, const json_t *provider_p);


/**
 * Free a PairedServiceNode.
 *
 * @param node_p The PairedServiceNode to free.
 * @memberof PairedServiceNode
 */
GRASSROOTS_SERVICE_API void FreePairedServiceNode (ListItem *node_p);


/**
 * Call the PairedService and get its JSON response.
 *
 * @param service_name_s The name of the PairedServce to call.
 * @param params_p The ParameterSet to send to the PairedService.
 * @param paired_service_uri_s The URI of the ExternalServer to send the request to.
 * @return The JSON fragment of the results of the PairedService or <code>NULL</code> upon error.
 */
GRASSROOTS_SERVICE_API json_t *MakeRemotePairedServiceCall (const char * const service_name_s, ParameterSet *params_p, const char * const paired_service_uri_s, ProvidersStateTable *providers_p);



#ifdef __cplusplus
}
#endif

#endif /* PAIRED_SERVICE_H_ */
