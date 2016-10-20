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


/* Grassroots params */
BLAST_SERVICE_PREFIX NamedParameterType BS_INPUT_FILE BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("input_file", PT_FILE_TO_READ));
BLAST_SERVICE_PREFIX NamedParameterType BS_JOB_ID BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("job_id", PT_STRING));

/*
 * These become the -query_loc parameter with the value "<subrange_from>-<subrange_to>"
 */
BLAST_SERVICE_PREFIX NamedParameterType BS_SUBRANGE_FROM BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("subrange_from", PT_UNSIGNED_INT));
BLAST_SERVICE_PREFIX NamedParameterType BS_SUBRANGE_TO BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("subrange_to", PT_UNSIGNED_INT));



/* Blastn params */
BLAST_SERVICE_PREFIX NamedParameterType BS_TASK BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("task", PT_STRING));
BLAST_SERVICE_PREFIX NamedParameterType BS_INPUT_QUERY BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("query", PT_LARGE_STRING));
BLAST_SERVICE_PREFIX NamedParameterType BS_MAX_SEQUENCES BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("max_target_seqs", PT_UNSIGNED_INT));
BLAST_SERVICE_PREFIX NamedParameterType BS_EXPECT_THRESHOLD BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("evalue", PT_UNSIGNED_REAL));
BLAST_SERVICE_PREFIX NamedParameterType BS_OUTPUT_FORMAT BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("outfmt", PT_UNSIGNED_INT));
BLAST_SERVICE_PREFIX NamedParameterType BS_NUM_ALIGNMENTS BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("num_alignments", PT_UNSIGNED_INT));


typedef enum
{
	BOF_PAIRWISE,
	BOF_QUERY_ANCHORED_WITH_IDENTITIES,
	BOF_QUERY_ANCHORED_NO_IDENTITIES,
	BOF_FLAT_QUERY_ANCHORED_WITH_IDENTITIES,
	BOF_FLAT_QUERY_ANCHORED_NO_IDENTITIES,
	BOF_XML_BLAST,
	BOF_TABULAR,
	BOF_TABULAR_WITH_COMMENTS,
	BOF_TEXT_ASN1,
	BOF_BINARY_ASN1,
	BOF_CSV,
	BOF_BLAST_ASN1,
	BOF_JSON_SEQALIGN,
	BOF_MULTI_FILE_JSON_BLAST,
	BOF_MULTI_FILE_XML2_BLAST,
	BOF_SINGLE_FILE_JSON_BLAST,
	BOF_SINGLE_FILE_XML2_BLAST,
	BOF_SEQUENCE_ALIGNMENT,
	BOF_NUM_TYPES
} BlastOutputFormat;



typedef struct BlastTask
{
  const char *bt_name_s;
  const char *bt_description_s;
} BlastTask;


#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_LOCAL bool AddQuerySequenceParams (BlastServiceData *data_p, ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL bool AddGeneralAlgorithmParams (BlastServiceData *data_p, ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL bool AddScoringParams (BlastServiceData *data_p, ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL bool AddProgramSelectionParameters (BlastServiceData *blast_data_p, ParameterSet *param_set_p, const BlastTask *tasks_p, const size_t num_tasks);


BLAST_SERVICE_LOCAL bool AddBlastPParams (BlastServiceData *data_p, ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL bool AddBlastNParams (BlastServiceData *data_p, ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL bool AddBlastXParams (BlastServiceData *data_p, ParameterSet *param_set_p);


BLAST_SERVICE_LOCAL uint16 AddDatabaseParams (BlastServiceData *data_p, ParameterSet *param_set_p, const DatabaseType db_type);


BLAST_SERVICE_LOCAL uint32 GetNumberOfDatabases (const BlastServiceData *data_p, const DatabaseType dt);


BLAST_SERVICE_LOCAL Parameter *SetUpPreviousJobUUIDParamater (const BlastServiceData *service_data_p, ParameterSet *param_set_p, ParameterGroup *group_p);


BLAST_SERVICE_LOCAL Parameter *SetUpOutputFormatParamater (const BlastServiceData *service_data_p,ParameterSet *param_set_p, ParameterGroup *group_p);


BLAST_SERVICE_LOCAL char *CreateGroupName (const char *server_s);


#ifdef __cplusplus
}
#endif

#endif /* SERVICES_BLAST_INCLUDE_BLAST_SERVICE_PARAMS_H_ */
