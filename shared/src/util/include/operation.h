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
#ifndef OPERATION_H
#define OPERATION_H

#include "grassroots_util_library.h"


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_OPERATION_CONSTANTS
	#define OPERATION_PREFIX GRASSROOTS_UTIL_API
	#define OPERATION_VAL(x)	= x
#else
	#define OPERATION_PREFIX extern
	#define OPERATION_VAL(x)
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */

/**
 * @brief The various Operations that a Server can perform.
 */
typedef enum Operation {
	OP_NONE = -1,               //!< OP_NONE
	
	/** Get list of all available services */
	OP_LIST_ALL_SERVICES,       //!< OP_LIST_ALL_SERVICES

	/** Get the SchemaVersion that the Server is using */
	OP_GET_SCHEMA_VERSION,
	
	/** Query services to see if they can work on file */
	OP_LIST_INTERESTED_SERVICES,//!< OP_LIST_INTERESTED_SERVICES
	
	/** Call any services that have a keyword parameter with a given keyword */
	OP_RUN_KEYWORD_SERVICES,    //!< OP_RUN_KEYWORD_SERVICES

	/** Get list of services matching the given names */
	OP_GET_NAMED_SERVICES,      //!< OP_GET_NAMED_SERVICES

	/** Get status of running services */
	OP_CHECK_SERVICE_STATUS,    //!< OP_CHECK_SERVICE_STATUS

	/** Get results from completed job */
	OP_GET_SERVICE_RESULTS,     //!< OP_GET_SERVICE_RESULTS

//	/** Get list of data objects and collections modified within a given time period */
//	OP_IRODS_MODIFIED_DATA,     //!< OP_IRODS_MODIFIED_DATA

	/** Tell the server that the jobs are no longer needed */
	OP_CLEAN_UP_JOBS,           //!< OP_CLEAN_UP_JOBS

	/**
	 * Get a requested Resource from the Server.
	 * @see Resource
	 */
	OP_GET_RESOURCE,						//!< OP_GET_RESOURCE

	OP_NUM_OPERATIONS           //!< OP_NUM_OPERATIONS
} Operation;



/**
 * @brief The current status of an Operation.
 * @details Each Operation has an OperationStatus detailing its current
 * state. Any value less than zero indicates an error with OS_LOWER_LIMIT
 * and OS_UPPER_LIMIT defining the exclusive boundary values.
 */
typedef enum OperationStatus
{
	OS_LOWER_LIMIT = -4,
	OS_FAILED = -3,
	OS_FAILED_TO_START = -2,
	OS_ERROR = -1,
	OS_IDLE = 0,
	OS_PENDING,
	OS_STARTED,
	OS_FINISHED,
	OS_PARTIALLY_SUCCEEDED,
	OS_SUCCEEDED,
	OS_CLEANED_UP,
	OS_UPPER_LIMIT,
	OS_NUM_STATUSES = OS_UPPER_LIMIT - OS_LOWER_LIMIT + 1
} OperationStatus;



#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Get the string-based representation of a given Operation.
 *
 * @param op The Operation to get the string-based representation of.
 * @return The string-based representation.
 */
GRASSROOTS_UTIL_API const char *GetOperationAsString (const Operation op);


/**
 * @brief Get the Operation for a given String.
 *
 * @param op_s The string to get the Operation for.
 * @return The Operation or OP_NONE if the string did not match a valid
 * operation string.
 */
GRASSROOTS_UTIL_API Operation GetOperationFromString (const char *op_s);


/**
 * @brief Get the string-based representation of a given OperationStatus
 *
 * @param status The OperationStatus to get the string-based representation of.
 * @return The string-based representation.
 */
GRASSROOTS_UTIL_API const char *GetOperationStatusAsString (const OperationStatus status);


/**
 * @brief Get the OperationStatus for a given String
 *
 * @param status_s The string to get the OperationStatus for.
 * @return The OperationStatus.
 */
GRASSROOTS_UTIL_API OperationStatus GetOperationStatusFromString (const char *status_s);


#ifdef __cplusplus
}
#endif





#endif		/* #ifndef OPERATION_H */		
