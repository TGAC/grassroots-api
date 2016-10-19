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

/*
 * blast_util.h
 *
 *  Created on: 19 Oct 2016
 *      Author: billy
 */

#ifndef SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_UTIL_H_
#define SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_UTIL_H_


#include "blast_service_api.h"
#include "byte_buffer.h"


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Add the argument to the command line arguments stored
 * in a ByteBuffer.
 *
 * @param buffer_p The ByteBuffer to store the value in.
 * @param arg_s The argument to add.
 * @return <code>true</code> if the argument was added
 * successfully, <code>false</code> otherwise.
 */
BLAST_SERVICE_LOCAL bool AddArg (const char *arg_s, ByteBuffer *buffer_p);



/**
 * Add a pair of arguments to the command line arguments
 * stored in a ByteBuffer.
 *
 * @param buffer_p The ByteBuffer to store the value in.
 * @param key_s The first argument to add.
 * @param value_s The second argument to add.
 * @return <code>true</code> if the arguments were added
 * successfully, <code>false</code> otherwise.
 */
BLAST_SERVICE_LOCAL bool AddArgsPair (const char *key_s, const char *value_s, ByteBuffer *buffer_p);


/**
 * Get the value of an integer-based Parameter and add it as key-value pair
 * to the command line arguments stored in a ByteBuffer.
 *
 * @param params_p The ParameterSet to get the Parameter from.
 * @param param_name_s The name of the desired Parameter.
 * @param key_s The key to use when creating the command line arguments.
 * @param buffer_p The ByteBuffer to store the value in.
 * @param unsigned_flag Is the parameter value unsigned or not?
 * @param required_flag If this is <code>true</code> then failure to find
 * the Parameter will cause this function to return <code>false</code>. If the
 * value is optional, then set this to <code>false</code>.
 * @return <code>true</code> if the arguments were added
 * successfully, <code>false</code> otherwise.
 */
BLAST_SERVICE_LOCAL bool AddArgsPairFromIntegerParameter (const ParameterSet *params_p, const char * const param_name_s, const char *key_s, ByteBuffer *buffer_p, const bool unsigned_flag, const bool required_flag);



#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_SERVICES_BLAST_INCLUDE_BLAST_UTIL_H_ */
