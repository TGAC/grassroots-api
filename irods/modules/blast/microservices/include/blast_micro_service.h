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
#ifndef BLAST_MICRO_SERVICE_H
#define BLAST_MICRO_SERVICE_H
  
  
#include "msParam.h"
#include "reGlobalsExtern.h"
/*#include "irods_ms_plugin.h"*/

#ifdef __cplusplus
extern "C" {
#endif	

/*************************************/
/******** FUNCTION PROTOTYPES ********/
/*************************************/
int BlastSequenceData (msParam_t *in1_p, msParam_t *out1_p, ruleExecInfo_t *rei);

#ifdef __cplusplus
}
#endif	


#endif		/* #ifndef BLAST_MICRO_SERVICE_H */


