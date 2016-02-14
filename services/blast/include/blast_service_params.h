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
 * blast_service_params.h
 *
 *  Created on: 13 Feb 2016
 *      Author: billy
 */

#ifndef SERVICES_BLAST_INCLUDE_BLAST_SERVICE_PARAMS_H_
#define SERVICES_BLAST_INCLUDE_BLAST_SERVICE_PARAMS_H_


#include "blast_service_api.h"
#include "blast_service.h"
#include "parameter_set.h"


#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_LOCAL bool AddQuerySequenceParams (ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL bool AddGeneralAlgorithmParams (ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL bool AddScoringParams (ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL uint16 AddDatabaseParams (BlastServiceData *data_p, ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL uint32 GetNumberOfDatabases (const BlastServiceData *data_p);


BLAST_SERVICE_LOCAL Parameter *SetUpPreviousJobUUIDParamater (ParameterSet *param_set_p);


#ifdef __cplusplus
}
#endif

#endif /* SERVICES_BLAST_INCLUDE_BLAST_SERVICE_PARAMS_H_ */
