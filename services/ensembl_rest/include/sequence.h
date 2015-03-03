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


ENSEMBL_REST_SERVICE_LOCAL bool RunSequenceSearch (ParameterSet *params_p, json_t *res_p, CurlTool *curl_tool_p);


ENSEMBL_REST_SERVICE_LOCAL bool AddSequenceParameters (ParameterSet *param_set_p);


ENSEMBL_REST_SERVICE_LOCAL const char **GetSequenceFormatNames (void);


ENSEMBL_REST_SERVICE_LOCAL const char **GetSequenceContentTypes (void);


ENSEMBL_REST_SERVICE_LOCAL const char **GetSequenceSequenceTypes (void);



#ifdef __cplusplus
}
#endif


#endif /* SEQUENCE_H_ */
