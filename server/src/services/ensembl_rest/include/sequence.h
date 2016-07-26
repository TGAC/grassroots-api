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
 * sequence.h
 *
 *  Created on: 3 Mar 2015
 *      Author: tyrrells
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "ensembl_rest_service_library.h"
#include "parameter_set.h"
#include "curl_tools.h"
#include "service.h"



typedef enum
{
	SO_JSON,
	SO_FASTA,
	SO_SEQXML,
	SO_NUM_FORMATS
} SequenceOutput;


typedef enum
{
	ST_GENOMIC,
	ST_CDS,
	ST_CDNA,
	ST_PROTEIN,
	ST_NUM_TYPES
} SequenceType;




#ifdef __cplusplus
extern "C"
{
#endif


ENSEMBL_REST_SERVICE_LOCAL json_t *RunSequenceSearch (ParameterSet *params_p, CurlTool *curl_tool_p);


ENSEMBL_REST_SERVICE_LOCAL bool AddSequenceParameters (ServiceData *data_p, ParameterSet *param_set_p);


ENSEMBL_REST_SERVICE_LOCAL const char **GetSequenceFormatNames (void);


ENSEMBL_REST_SERVICE_LOCAL const char **GetSequenceContentTypes (void);


ENSEMBL_REST_SERVICE_LOCAL const char **GetSequenceSequenceTypes (void);



#ifdef __cplusplus
}
#endif


#endif /* SEQUENCE_H_ */
