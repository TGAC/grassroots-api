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

/* General Blast params */
BLAST_SERVICE_PREFIX NamedParameterType BS_TASK BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("task", PT_STRING));
BLAST_SERVICE_PREFIX NamedParameterType BS_INPUT_QUERY BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("query", PT_LARGE_STRING));
BLAST_SERVICE_PREFIX NamedParameterType BS_MAX_SEQUENCES BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("max_target_seqs", PT_UNSIGNED_INT));
BLAST_SERVICE_PREFIX NamedParameterType BS_EXPECT_THRESHOLD BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("evalue", PT_UNSIGNED_REAL));
BLAST_SERVICE_PREFIX NamedParameterType BS_OUTPUT_FORMAT BLAST_SERVICE_VAL (SET_NAMED_PARAMETER_TYPE_TAGS ("outfmt", PT_UNSIGNED_INT));


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
	BOF_GRASSROOTS,
	BOF_NUM_TYPES
} BlastOutputFormat;


/**
 * This datatype is used for describing the algorithms that some
 * of the BLAST tools allow the user to choose to optimise the
 * search procedure.
 */
typedef struct BlastTask
{
	/**
	 * The name of the algorithm that will be passed to the BlastTool
	 * using the -task parameter.
	 */
  const char *bt_name_s;

  /**
   * A user-friendly descritpion of the algorithm.
   */
  const char *bt_description_s;
} BlastTask;


/**
 * A callback function used to amend a given ParameterSet.
 *
 * @param data_p The configuration data for the Blast Service.
 * @param param_set_p The ParameterSet that the callback function's Parameters will be added to.
 * @param group_p The optional ParameterGroup to add the generated Parameter to. This can be <code>NULL</code>.
 * @return <code>true</code> if the callback function's parameters were added successfully, <code>
 * false</code> otherwise.
 */
typedef bool (*AddAdditionalParamsFn) (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Add the common query sequence parameters for a Blast Service.
 *
 * @param data_p The configuration data for the Blast Service.
 * @param param_set_p The ParameterSet that the query sequence parameters will be added to.
 * @param callback_fn If the Blast Service wants to add extra Parameters to the query sequence group
 * of Parameters, it can do so via this callback_fn. This can be <code>NULL</code>
 * @return <code>true</code> if the query sequence parameters were added successfully, <code>
 * false</code> otherwise.
 */
BLAST_SERVICE_LOCAL bool AddQuerySequenceParams (BlastServiceData *data_p, ParameterSet *param_set_p, AddAdditionalParamsFn callback_fn	);


/**
 * Add the common general algorithm parameters for a Blast Service.
 *
 * @param data_p The configuration data for the Blast Service.
 * @param param_set_p The ParameterSet that the general algorithm parameters will be added to.
 * @param callback_fn If the Blast Service wants to add extra Parameters to the general algorithm group
 * of Parameters, it can do so via this callback_fn. This can be <code>NULL</code>
 * @return <code>true</code> if the general algorithm parameters were added successfully, <code>
 * false</code> otherwise.
 */
BLAST_SERVICE_LOCAL bool AddGeneralAlgorithmParams (BlastServiceData *data_p, ParameterSet *param_set_p, AddAdditionalParamsFn callback_fn);


/**
 * Add the program selection parameters for a Blast Service.
 *
 * @param blast_data_p The configuration data for the Blast Service.
 * @param param_set_p The ParameterSet that the general algorithm parameters will be added to.
 * @param tasks_p An optional array of BlastTasks that be can be chosen to alter the search.
 * @param num_tasks The number of BlastTasks in the array pointed to by tasks_p.
 * @return <code>true</code> if the general algorithm parameters were added successfully, <code>
 * false</code> otherwise.
 */
BLAST_SERVICE_LOCAL bool AddProgramSelectionParameters (BlastServiceData *blast_data_p, ParameterSet *param_set_p, const BlastTask *tasks_p, const size_t num_tasks);


/**
 * Add the database parameters for a Blast Service.
 *
 * @param data_p The configuration data for the Blast Service.
 * @param param_set_p The ParameterSet that the general algorithm parameters will be added to.
 * @param db_type The type of databases to add.
 * @return The number of database parameters added.
 */
BLAST_SERVICE_LOCAL uint16 AddDatabaseParams (BlastServiceData *data_p, ParameterSet *param_set_p, const DatabaseType db_type);


/**
 * Get the number of databases of a given type that this BlastService has.
 *
 * @param data_p The configuration data for the BlastService to check.
 * @param dt The type of database to check.
 * @return The number of available databases of the given type.
 */
BLAST_SERVICE_LOCAL uint32 GetNumberOfDatabases (const BlastServiceData *data_p, const DatabaseType dt);



/**
 * Create the Parameter for specifying the UUIDs for any previous Blast searches.
 *
 * @param data_p The configuration data for the Blast Service.
 * @param param_set_p The ParameterSet that the UUID Parameter will be added to.
 * @param group_p The optional ParameterGroup to add the generated Parameter to. This can be <code>NULL</code>.
 * @return The UUID Parameter or <code>NULL</code> upon error.
 */
BLAST_SERVICE_LOCAL Parameter *SetUpPreviousJobUUIDParamater (const BlastServiceData *service_data_p, ParameterSet *param_set_p, ParameterGroup *group_p);


/**
 * Create the Parameter for specifying the output format from a Blast search.
 *
 * @param service_data_p The configuration data for the Blast Service.
 * @param param_set_p The ParameterSet that the output format Parameter will be added to.
 * @param group_p The optional ParameterGroup to add the generated Parameter to. This can be <code>NULL</code>.
 * @return The output format Parameter or <code>NULL</code> upon error.
 */
BLAST_SERVICE_LOCAL Parameter *SetUpOutputFormatParamater (const BlastServiceData *service_data_p, ParameterSet *param_set_p, ParameterGroup *group_p);


/**
 * Create the group name to use for available databases from a given named Server.
 *
 * @param server_s The name of the Server.
 * @return The newly-allocated group name that will need to be freed with
 * FreeCopiedString to avoid a memory leak or <code>NULL</code> upon error.
 */
BLAST_SERVICE_LOCAL char *CreateGroupName (const char *server_s);


#ifdef __cplusplus
}
#endif

#endif /* SERVICES_BLAST_INCLUDE_BLAST_SERVICE_PARAMS_H_ */
