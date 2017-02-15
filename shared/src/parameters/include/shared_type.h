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
 * shared_type.h
 *
 *  Created on: 14 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SHARED_SRC_PARAMETERS_INCLUDE_SHARED_TYPE_H_
#define SHARED_SRC_PARAMETERS_INCLUDE_SHARED_TYPE_H_

#include "grassroots_params_library.h"

#include "data_resource.h"

#include "linked_list.h"

#include "parameter_type.h"



/**
 * A datatype to store values for a Parameter.
 * Use the ParameterType to access the correct
 * value.
 *
 * @ingroup parameters_group
 */
typedef union SharedType
{
	/** A Boolean value */
	bool st_boolean_value;

	/** A signed integer */
	int32 st_long_value;

	/** An unsigned integer */
	uint32 st_ulong_value;

	/** A real number */
	double64 st_data_value;

	/** A c-style string */
	char *st_string_value_s;

	/** A single character */
	char st_char_value;

	/** A Resource */
	Resource *st_resource_value_p;

	/** A LinkedList of multiple values */
	LinkedList *st_multiple_values_p;

	/** A JSON fragment */
	json_t *st_json_p;
} SharedType;


/**
 * A datatype allowing a SharedType to be stored
 * on a LinkedList.
 *
 * @extends ListItem
 * @ingroup parameters_group
 */
typedef struct SharedTypeNode
{
	/** The basic ListItem */
	ListItem stn_node;

	/** The SharedType value */
	SharedType stn_value;
} SharedTypeNode;



#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Clear the value of a SharedType.
 *
 * @param st_p The SharedType to clear.
 * @param pt The ParameterType for this SharedType
 * @memberof SharedType
 */
GRASSROOTS_PARAMS_API void ClearSharedType (SharedType *st_p, const ParameterType pt);


/**
 * Make a deep copy of the value of one SharedType to another.
 *
 * @param src The SharedType to copy the value from.
 * @param dest_p The SharedType to copy the value to.
 * @param pt The ParameterType for this SharedType
 * @return <code>true</code> if the value was copied successfully, <code>false</code> otherwise.
 * @memberof SharedType
 */
GRASSROOTS_PARAMS_API bool CopySharedType (const SharedType src, SharedType *dest_p, const ParameterType pt);


#ifdef __cplusplus
}
#endif


#endif /* SHARED_SRC_PARAMETERS_INCLUDE_SHARED_TYPE_H_ */
