/*
 * sequence.h
 *
 *  Created on: 3 Mar 2015
 *      Author: tyrrells
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "wensembl_rest_service_library.h"

typedef enum
{
	SO_JSON,
	SO_FASTA,
	SO_SEQXML,
	SO_NUM_FORMATS
} SequenceOutput;


const char *SEQ_FORMAT_NAMES_PP [SO_NUM_FORMATS] =
{
	".json",
	".fasta",
	".seqxml",
};


const char *SEQ_FORMAT_CONTENT_TYPES_PP [SO_NUM_FORMATS] =
{
	"application/json",
	"text/x-fasta",
	"text/x-seqxml+xml"
};



#ifdef __cplusplus
extern "C"
{
#endif


ENSEMBL_REST_SERVICE_LOCAL json_t *GetSequencesById (const char * const id_s);



#ifdef __cplusplus
}
#endif


#endif /* SEQUENCE_H_ */
