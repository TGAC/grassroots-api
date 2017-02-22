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
 * audit.h
 *
 *  Created on: 21 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SERVER_SRC_LIB_INCLUDE_AUDIT_H_
#define SERVER_SRC_LIB_INCLUDE_AUDIT_H_


#include "grassroots_service_manager_library.h"
#include "service_job.h"
#include "parameter_set.h"



#ifdef __cplusplus
extern "C"
{
#endif


GRASSROOTS_SERVICE_MANAGER_API bool LogServiceJob (ServiceJob *job_p);


GRASSROOTS_SERVICE_MANAGER_API bool LogParameterSet (ParameterSet *params_p, ServiceJob *job_p);


#ifdef __cplusplus
}
#endif


#endif /* SERVER_SRC_LIB_INCLUDE_AUDIT_H_ */
