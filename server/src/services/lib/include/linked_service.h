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
 * linked_service.h
 *
 *  Created on: 6 Sep 2016
 *      Author: tyrrells
 */

#ifndef SERVER_SRC_SERVICES_LIB_INCLUDE_LINKED_SERVICE_H_
#define SERVER_SRC_SERVICES_LIB_INCLUDE_LINKED_SERVICE_H_

#include "grassroots_service_library.h"

#include "jansson.h"

#include "linked_list.h"
#include "mapped_parameter.h"


/* forward declarations */
struct ServiceJob;


/**
 * This datatype stores the data needed to get the
 * required information from the output of one
 * Service to act as input for another.
 *
 * @ingroup services_group
 */
typedef struct LinkedService
{
	/**
	 * The name of the Service whose input will be generated
	 * from the output of the Service that owns this
	 * LinkedService object.
	 */
	char *ls_output_service_s;

	/**
	 * The list of MappedParameterNodes storing the information
	 * required to map values from the input Service's results
	 * to be input for the Service that owns this LinkedService
	 * object.
	 */
	LinkedList *ls_mapped_params_p;
} LinkedService;


/**
 * The datatype to allow the storage of LinkedServices
 * on LinkedLists.
 *
 * @extends ListItem
 * @ingroup services_group
 */
typedef struct LinkedServiceNode
{
	/** The base ListItem. */
	ListItem lsn_node;

	/** A pointer to the LinkedService. */
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
 * @param input_key_s The key for each result to generate a link for. This can be NULL if there
 * is only a single object. The LinkedService will make a deep copy
 * of this and store that value.
 * @return The newly-allocated LinkedService or <code>NULL</code> upon error.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API LinkedService *AllocateLinkedService (const char *input_service_s, const char *input_key_s);


/**
 * Create a new LinkedService from a JSON fragment.
 *
 * @param linked_service_json_p The JSON fragment describing the LinkedService.
 * @return The newly-allocated LinkedService or <code>NULL</code> upon error.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API LinkedService *CreateLinkedServiceFromJSON (const json_t *linked_service_json_p);



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


/**
 * Create and add a MappedParameter to a LinkedService.
 *
 * This calls AllocateMappedParameter and AddMappedParameterToLinkedService.
 *
 * @param linked_service_p The LinkedService to add the MappedParameter to.
 * @param input_s The selector for the input Service's parameter. The new MappedParameter will make a deep copy of this value to store.
 * @param output_s The name of the output Service's parameter. The new MappedParameter will make a deep copy of this value to store.
 * @param required_flag Is this MappedParameter required or is optional to run the LinkedService?
 * @param multi_flag <code>true</code> if this MappedParameter can have multiple input values or <code>false</code> if it just has a single value.
 * @return <code>true</code> if the MappedParameter was created and added successfully, <code>false</code> otherwise.
 * @memberof LinkedService
 * @see AllocateMappedParameter
 * @see AddMappedParameterToLinkedService
 */
GRASSROOTS_SERVICE_API bool CreateAndAddMappedParameterToLinkedService (LinkedService *linked_service_p, const char *input_s, const char *output_s, bool required_flag, bool multi_flag);


/**
 * Add a MappedParameter to a LinkedService.
 *
 * @param linked_service_p The LinkedService to add the MappedParameter to.
 * @param mapped_param_p The MappedParameter to add.
 * @return <code>true</code> if the MappedParameter was added successfully, <code>false</code> otherwise.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API bool AddMappedParameterToLinkedService (LinkedService *linked_service_p, struct MappedParameter *mapped_param_p);



/**
 * Get the MappedParameter with a given name from a LinkedService.
 *
 * @param linked_service_p The LinkedService to get the MappedParameter from.
 * @param name_s The name of the MappedParmeter to search for.
 * @return The matching MappedParameter or <code>NULL</code> if it copuld not be found.
 */
GRASSROOTS_SERVICE_API struct MappedParameter *GetMappedParameterByInputParamName (const LinkedService *linked_service_p, const char * const name_s);


/**
 * Get the JSON representation of a LinkedService.
 *
 * @param linked_service_p The LinkedService to serialise.
 * @return The JSON fragment.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API json_t *GetLinkedServiceAsJSON (LinkedService *linked_service_p);


/**
 * Run a LinkedService.
 *
 * @param linked_service_p The LinkedService to run.
 * @param job_p The ServiceJob that the results of running the LinkedService will be
 * added to.
 * @return <code>true</code> if the LinkedService was run successfully, <code>false</code> otherwise.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API bool ProcessLinkedService (LinkedService *linked_service_p, struct ServiceJob *job_p);



GRASSROOTS_SERVICE_API bool AddLinkedServiceToRequestJSON (json_t *request_p, LinkedService *linked_service_p, ParameterSet *output_params_p);


#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_SERVICES_LIB_INCLUDE_LINKED_SERVICE_H_ */
