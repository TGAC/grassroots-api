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
 * process.h
 *
 *  Created on: 2 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SHARED_SRC_UTIL_INCLUDE_PROCESS_H_
#define SHARED_SRC_UTIL_INCLUDE_PROCESS_H_


#include "grassroots_util_library.h"
#include "typedefs.h"
#include "operation.h"


#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_UTIL_API	int32 CreateProcess (const char * const program_s, char **args_ss, char **environment_ss);

GRASSROOTS_UTIL_API	bool TerminateProcess (int32 process_id);

GRASSROOTS_UTIL_API	OperationStatus GetProcessStatus (int32 process_id);



#ifdef __cplusplus
}
#endif


#endif /* SHARED_SRC_UTIL_INCLUDE_PROCESS_H_ */
