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


typedef enum BLAST_SERVICE_LOCAL DatabaseType
{
	DT_NUCLEOTIDE,
	DT_PROTEIN,
	DT_NUM_TYPES
} DatabaseType;


typedef enum BLAST_SERVICE_LOCAL BlastServiceType
{
	BST_BLASTN,
	BST_BLASTP,
	BST_BLASTX,
	BST_NUM_TYPES
} BlastServiceType;

/**
 * A datatype describing the details of each database available
 * to search against.
 */
typedef struct BLAST_SERVICE_LOCAL DatabaseInfo
{
	/** The name of the database to display to the user. */
	const char *di_name_s;

	/** The filename for the database */
	const char *di_filename_s;

	/** The description of the database to display to the user. */
	const char *di_description_s;

	/**
	 * Sets whether the database defaults to being searched against
	 * or not.
	 */
	bool di_active_flag;

	/** The DatabaseType for this database. */
	DatabaseType di_type;

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



	DatabaseType bsd_type;

} BlastServiceData;



#ifdef ALLOCATE_BLAST_SERVICE_CONSTANTS
	#define BLAST_SERVICE_PREFIX BLAST_SERVICE_LOCAL
	#define BLAST_SERVICE_VAL(x)	= x
	#define BLAST_SERVICE_STRUCT_INITIALIZER(x) x
#else
	#define BLAST_SERVICE_PREFIX extern
	#define BLAST_SERVICE_VAL(x)
	 #define BLAST_SERVICE_STRUCT_INITIALIZER(x)
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
BLAST_SERVICE_PREFIX const char *BS_APP_NAME_S BLAST_SERVICE_VAL ("blast_app_type");


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
