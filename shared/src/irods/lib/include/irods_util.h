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
 * irods_util.h
 *
 *  Created on: 18 Feb 2016
 *      Author: tyrrells
 */

#ifndef IRODS_LIB_INCLUDE_IRODS_UTIL_H_
#define IRODS_LIB_INCLUDE_IRODS_UTIL_H_

#include "getRodsEnv.h"
#include "rcConnect.h"
#include "rodsPath.h"
#include "parseCommandLine.h"
#include "irods_util_library.h"

#ifdef __cplusplus
extern "C"
{
#endif


IRODS_UTIL_API int PutAndCheckForServices (rcComm_t **connection_pp, rodsEnv *env_p, rodsArguments_t *args_p, rodsPathInp_t *path_inp_p);


IRODS_UTIL_LOCAL int PrintRodsPath (FILE *out_f, const rodsPath_t * const rods_path_p, const char * const description_s);


IRODS_UTIL_LOCAL bool NotifyUser (userInfo_t *user_p, const char * const title_s, msParamArray_t *params_p);

#ifdef __cplusplus
}
#endif

#endif /* IRODS_LIB_INCLUDE_IRODS_UTIL_H_ */
