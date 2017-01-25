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


/**
 * This enumeration is for choosing the response format from the
 * call to Ensembl's web services.
 */
typedef enum
{
	SO_JSON,      //!< Get the response in JSON format.
	SO_FASTA,     //!< Get the response in fasta format.
	SO_SEQXML,    //!< Get the response in Seqxml format.
	SO_NUM_FORMATS//!< The number of available output formats.
} SequenceOutput;


/**
 * An enumeration specifying the different types of
 * sequence
 */
typedef enum
{
	/** The default value */
	ST_GENOMIC,

	/** This refers to the spliced transcript sequence without UTR */
	ST_CDS,

	/** This refers to the spliced transcript sequence with UTR */
	ST_CDNA,

	/** This refers to the protein */
	ST_PROTEIN,

	/** The number of available SequencTypes */
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
