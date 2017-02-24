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
 * parameter_option.h
 *
 *  Created on: 14 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SHARED_SRC_PARAMETERS_INCLUDE_PARAMETER_OPTION_H_
#define SHARED_SRC_PARAMETERS_INCLUDE_PARAMETER_OPTION_H_


#include "grassroots_params_library.h"
#include "shared_type.h"


/**
 * A ParameterOption is used when a Parameter
 * wants to constrain the possible values to one from
 * a fixed set.
 *
 * @ingroup parameters_group
 */
typedef struct ParameterOption
{
	/** The user-friendly description for this value */
	char *po_description_s;

	/** The internal value for this option */
	SharedType po_value;

	/** The ParameterType for this option */
	ParameterType po_type;

} ParameterOption;



/**
 * A datatype for storing ParameterMultiOptions on
 * a LinkedList
 *
 * @see ParameterOption
 * @extends ListItem
 * @ingroup parameters_group
 */
typedef struct ParameterOptionNode
{
	/** The base ListItem. */
	ListItem pon_node;

	/** The ParameterOption to store. */
	ParameterOption *pon_option_p;
} ParameterOptionNode;



#ifdef __cplusplus
	extern "C" {
#endif


GRASSROOTS_PARAMS_API ParameterOption *AllocateParameterOption (SharedType value, const char * const description_s, const ParameterType param_type);


GRASSROOTS_PARAMS_API void FreeParameterOption (ParameterOption *option_p);


GRASSROOTS_PARAMS_API LinkedList *CreateProgramOptionsList (void);


GRASSROOTS_PARAMS_API ParameterOptionNode *AllocateParameterOptionNode (ParameterOption *option_p);


GRASSROOTS_PARAMS_API void FreeParameterOptionNode (ListItem *item_p);


GRASSROOTS_PARAMS_API bool CreateAndAddParameterOption (LinkedList *options_p, SharedType value, const char * const description_s, const ParameterType param_type);


#ifdef __cplusplus
}
#endif


#endif /* SHARED_SRC_PARAMETERS_INCLUDE_PARAMETER_OPTION_H_ */
