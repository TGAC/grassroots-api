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
 * parameter_type.h
 *
 *  Created on: 14 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SHARED_SRC_PARAMETERS_INCLUDE_PARAMETER_TYPE_H_
#define SHARED_SRC_PARAMETERS_INCLUDE_PARAMETER_TYPE_H_


/**
 * The different types that a Parameter can take.
 * @ingroup parameters_group
 */
typedef enum ParameterType
{
	/** A boolean parameter */
	PT_BOOLEAN,

	/** A 32-bit integer */
	PT_SIGNED_INT,

	/** A non-negative 32-bit integer */
	PT_UNSIGNED_INT,


	/** A non-positive 32-bit integer */
	PT_NEGATIVE_INT,


	/** A real number */
	PT_SIGNED_REAL,

	/** An unsigned real number */
	PT_UNSIGNED_REAL,

	/** A c-style string */
	PT_STRING,

	/** An output filename string */
	PT_FILE_TO_WRITE,

	/** An input filename string*/
	PT_FILE_TO_READ,

	/** A directory string */
	PT_DIRECTORY,

	/** A single 1-byte character */
	PT_CHAR,

	/**
	 * A sensitive c-style string that shouldn't
	 * be displayed explicitly to the user.
	 */
	PT_PASSWORD,

	/**
	 * A value that a Service can use without any other parameters
	 * being set to produce results.
	 */
	PT_KEYWORD,


	/**
	 * A potentially large c-style string. This is used by clients to determine
	 * the appropriate editor to show to the user. E.g. a multi-line text bos
	 * as opposed to a single-line text box for a PT_STRING.
	 */
	PT_LARGE_STRING,

	/**
	 * A JSON fragment.
	 */
	PT_JSON,

	/**
	 * A c-style string of tabular data.
	 */
	PT_TABLE,

	/** The number of possible ParameterType values. */
	PT_NUM_TYPES
} ParameterType;





#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif


#endif /* SHARED_SRC_PARAMETERS_INCLUDE_PARAMETER_TYPE_H_ */
