/*
 * sequence.c
 *
 *  Created on: 3 Mar 2015
 *      Author: tyrrells
 */

#include "sequence.h"




static const char *S_SEQ_FORMAT_NAMES_PP [SO_NUM_FORMATS] =
{
	".json",
	".fasta",
	".seqxml",
};


static const char *S_SEQ_FORMAT_CONTENT_TYPES_PP [SO_NUM_FORMATS] =
{
	"application/json",
	"text/x-fasta",
	"text/x-seqxml+xml"
};


static const char *S_SEQ_SEQUENCE_TYPES_PP [ST_NUM_TYPES] =
{
	"genomic",
	"cds",
	"cdna",
	"protein",
};



json_t *GetSequencesById (const char * const id_s, const SequenceOutput output_format, const SequenceType seq_type)
{
	json_t *res_p = NULL;

	return res_p;
}


bool AddSequenceParameters (ParameterSet *param_set_p)
{
	bool success_flag = false;


	return success_flag;
}



const char **GetSequenceFormatNames (void)
{
	return S_SEQ_FORMAT_NAMES_PP;
}


const char **GetSequenceContentTypes (void)
{
	return S_SEQ_FORMAT_CONTENT_TYPES_PP;
}


const char **GetSequenceSequenceTypes (void)
{
	return S_SEQ_SEQUENCE_TYPES_PP;
}
