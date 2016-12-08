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
 * mapped_parameter.h
 *
 *  Created on: 7 Sep 2016
 *      Author: tyrrells
 */

#ifndef SERVER_SRC_SERVICES_LIB_INCLUDE_MAPPED_PARAMETER_H_
#define SERVER_SRC_SERVICES_LIB_INCLUDE_MAPPED_PARAMETER_H_

#include "jansson.h"

#include "grassroots_service_library.h"

#include "linked_list.h"

#include "parameter_set.h"



/* forward declarations */
struct ServiceJob;


/**
 * This structure is used to store a relationship between
 * a value from the output of running one service to use
 * as an input parameter value for another service.
 */
typedef struct MappedParameter
{
	/**
	 * This is the selector to get the value from the first service's
	 * output. This uses a dot notation to signify parent-child
	 * relationship.
	 */
	char *mp_input_param_s;

	/**
	 * This is name of the parameter in the second service.
	 */
	char *mp_output_param_s;


	/**
	 * Is this MappedParameter required for the LinkedService to run?
	 */
	bool mp_required_flag;


	/**
	 * Can there be multiple input values?
	 */
	bool mp_multiple_flag;

} MappedParameter;


/**
 * This datatype allows the storage of MappedParameters
 * on a LinkedList.
 *
 * @extends ListItem
 */
typedef struct MappedParameterNode
{
	/** The base node. */
	ListItem mpn_node;

	/** A pointer to the MappedParameter to store. */
	MappedParameter *mpn_mapped_param_p;
} MappedParameterNode;


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Allocate a MappedParameter for the given Parameter names.
 *
 * @param input_s The selector for the input Service's parameter. The new MappedParameter will make a deep copy of this value to store.
 * @param output_s The name of the output Service's parameter. The new MappedParameter will make a deep copy of this value to store.
 * @param required_flag <code>true</code> if this MappedParameter required or <code>false</code> if it is optional to run the LinkedService.
 * @param multi_flag <code>true</code> if this MappedParameter can have multiple input values or <code>false</code> if it just has a single value.
 * @return The newly-allocated MappedParameter or <code>NULL</code> upon error.
 * @memberof MappedParameter
 */
GRASSROOTS_SERVICE_API MappedParameter *AllocateMappedParameter (const char *input_s, const char *output_s, bool required_flag, bool multi_flag);


/**
 * Create a new MappedParameter from a JSON fragment.
 *
 * @param mapped_param_json_p The JSON fragment describing the MappedParameter.
 * @return The newly-allocated MappedParameter or <code>NULL</code> upon error.
 * @memberof LinkedService
 */
GRASSROOTS_SERVICE_API MappedParameter *CreateMappedParameterFromJSON (const json_t *mapped_param_json_p);


/**
 * Free a MappedParameter.
 *
 * @param mapped_param_p The MappedParameter to free.
 * @memberof MappedParameter
 */
GRASSROOTS_SERVICE_API void FreeMappedParameter (MappedParameter *mapped_param_p);


/**
 * Allocate a MappedParameterNode to store a MappedParameter on a LinkedList.
 *
 * @param mapped_param_p The MappedParameter to store.
 * @return The newly-allocated MappedParameterNode or <code>NULL</code> upon error.
 * @memberof MappedParameter
 */
GRASSROOTS_SERVICE_API MappedParameterNode *AllocateMappedParameterNode (MappedParameter *mapped_param_p);


/**
 * Free a MappedParameterNode and its associated MappedParameter.
 *
 * @param node_p The MappedParameterNode to free.
 * @memberof MappedParameter
 */
GRASSROOTS_SERVICE_API void FreeMappedParameterNode (ListItem *node_p);



#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_SERVICES_LIB_INCLUDE_MAPPED_PARAMETER_H_ */
