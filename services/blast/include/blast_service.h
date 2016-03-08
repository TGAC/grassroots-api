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


#ifndef BLAST_SERVICE_H
#define BLAST_SERVICE_H


#include "blast_service_api.h"
#include "blast_formatter.h"


#define TAG_BLAST_INPUT_QUERY MAKE_TAG ('B', 'Q', 'U', 'Y')
#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'I', 'N', 'F')
#define TAG_BLAST_JOB_ID MAKE_TAG ('B', 'I', 'J', 'I')
#define TAG_BLAST_OUTPUT_FILE MAKE_TAG ('B', 'O', 'U', 'F')
#define TAG_BLAST_DATABASE_NAME MAKE_TAG ('B', 'D', 'B', 'N')
#define TAG_BLAST_MAX_SEQUENCES MAKE_TAG ('B', 'M', 'S', 'Q')
#define TAG_BLAST_SHORT_QUERIES MAKE_TAG ('B', 'S', 'H', 'Q')
#define TAG_BLAST_EXPECT_THRESHOLD MAKE_TAG ('B', 'E', 'X', 'T')
#define TAG_BLAST_WORD_SIZE MAKE_TAG ('B', 'W', 'D', 'S')
#define TAG_BLAST_MAX_RANGE_MATCHES MAKE_TAG ('B', 'M', 'R', 'G')
#define TAG_BLAST_SUBRANGE_FROM MAKE_TAG ('B', 'Q', 'F', 'R')
#define TAG_BLAST_SUBRANGE_TO MAKE_TAG ('B', 'Q', 'T', 'O')
#define TAG_BLAST_MATCH_SCORE MAKE_TAG ('B', 'M', 'T', 'C')
#define TAG_BLAST_MISMATCH_SCORE MAKE_TAG ('B', 'M', 'S', 'M')
#define TAG_BLAST_OUTPUT_FORMAT MAKE_TAG ('B', 'F', 'M', 'T')


/**
 * A datatype describing the details of each database available
 * to search against.
 */
typedef struct BLAST_SERVICE_LOCAL DatabaseInfo
{
	/** The name of the database to display to the user. */
	const char *di_name_s;

	/** The description of the database to display to the user. */
	const char *di_description_s;

	/**
	 * Sets whether the database defaults to being searched against
	 * or not.
	 */
	bool di_active_flag;
} DatabaseInfo;



/* forward class declaration */
class BlastToolFactory;

/**
 * The configuration data for the Blast Service.
 *
 * @extends ServiceData
 */
typedef struct BLAST_SERVICE_LOCAL BlastServiceData
{
	/** The base ServiceData. */
	ServiceData bsd_base_data;

	/**
	 * The directory where the Blast input, output and log files
	 * will be stored.
	 */
	const char *bsd_working_dir_s;

	/**
	 * The BlastFormatter used to convert the Blast output
	 * between the different output formats.
	 */
	BlastFormatter *bsd_formatter_p;

	/** A NULL-terminated array of the databases available to search */
	DatabaseInfo *bsd_databases_p;

	/** The BlastToolFactory used to generate each BlastTool that actually run the Blast jobs. */
	BlastToolFactory *bsd_tool_factory_p;

} BlastServiceData;



#ifdef ALLOCATE_BLAST_SERVICE_CONSTANTS
	#define BLAST_SERVICE_PREFIX BLAST_SERVICE_LOCAL
	#define BLAST_SERVICE_VAL(x)	= x
#else
	#define BLAST_SERVICE_PREFIX extern
	#define BLAST_SERVICE_VAL(x)
#endif


#define BS_DEFAULT_OUTPUT_FORMAT (11)

BLAST_SERVICE_PREFIX const char *BS_INPUT_SUFFIX_S BLAST_SERVICE_VAL (".input");
BLAST_SERVICE_PREFIX const char *BS_OUTPUT_SUFFIX_S BLAST_SERVICE_VAL (".output");
BLAST_SERVICE_PREFIX const char *BS_LOG_SUFFIX_S BLAST_SERVICE_VAL (".log");
//BLAST_SERVICE_PREFIX const uint32 BS_DEFAULT_OUTPUT_FORMAT BLAST_SERVICE_VAL (11);
BLAST_SERVICE_PREFIX const char *BS_DEFAULT_OUTPUT_FORMAT_S BLAST_SERVICE_VAL ("11");

BLAST_SERVICE_PREFIX const char *BS_DATABASE_GROUP_NAME_S BLAST_SERVICE_VAL ("Available Databases");

BLAST_SERVICE_PREFIX const char *BS_TOOL_TYPE_NAME_S BLAST_SERVICE_VAL ("blast_tool");
BLAST_SERVICE_PREFIX const char *BS_COMMAND_NAME_S BLAST_SERVICE_VAL ("blast_command");



#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


BLAST_SERVICE_API void ReleaseServices (ServicesArray *services_p);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef BLAST_SERVICE_H */
