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
#ifndef PYTHON_MICRO_SERVICES_H
#define PYTHON_MICRO_SERVICES_H
  
  
#include "msParam.h"
#include "reGlobalsExtern.h"
/*#include "irods_ms_plugin.h"*/

#ifdef __cplusplus
extern "C" {
#endif	

/*************************************/
/******** FUNCTION PROTOTYPES ********/
/*************************************/
int ObjectRegisteredPythonCallback (msParam_t *user_p, msParam_t *resource_p, msParam_t *collection_p, msParam_t *data_object_p, ruleExecInfo_t *rei);

#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef PYTHON_MICRO_SERVICES_H */


