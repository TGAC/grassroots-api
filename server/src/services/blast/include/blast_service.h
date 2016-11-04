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
#include "blast_app_parameters.h"
#include "blast_service_api.h"
#include "blast_service_job.h"
#include "parameter_set.h"
#include "temp_file.hpp"



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



/* forward class declarations */
class BlastToolFactory;
struct BlastServiceJob;

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


BLAST_SERVICE_LOCAL ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p, BlastAppParameters *app_params_p);

BLAST_SERVICE_LOCAL bool GetBlastServiceConfig (BlastServiceData *data_p);

BLAST_SERVICE_LOCAL BlastServiceData *AllocateBlastServiceData (Service *blast_service_p, DatabaseType database_type);

BLAST_SERVICE_LOCAL void FreeBlastServiceData (BlastServiceData *data_p);

BLAST_SERVICE_LOCAL bool CloseBlastService (Service *service_p);

BLAST_SERVICE_LOCAL ParameterSet *IsResourceForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p);

BLAST_SERVICE_LOCAL bool AddBaseBlastServiceParameters (Service *blast_service_p, ParameterSet *param_set_p, const DatabaseType db_type, bool (*add_additional_params_fn) (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p));

BLAST_SERVICE_LOCAL void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p);


BLAST_SERVICE_LOCAL bool DetermineBlastResult (Service *service_p, struct BlastServiceJob *job_p);

BLAST_SERVICE_LOCAL OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t service_id);

BLAST_SERVICE_LOCAL TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t job_id);

BLAST_SERVICE_LOCAL char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id, const uint32 output_format_code);

BLAST_SERVICE_LOCAL char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s, const uint32 output_format_code);

BLAST_SERVICE_LOCAL ServiceJobSet *GetPreviousJobResults (LinkedList *ids_p, BlastServiceData *blast_data_p, const uint32 output_format_code);

BLAST_SERVICE_LOCAL ServiceJobSet *CreateJobsForPreviousResults (ParameterSet *params_p, const char *ids_s, BlastServiceData *blast_data_p);

BLAST_SERVICE_LOCAL void PrepareBlastServiceJobs (const DatabaseInfo *db_p, const ParameterSet * const param_set_p, ServiceJobSet *jobs_p, BlastServiceData *data_p);


BLAST_SERVICE_LOCAL ServiceJob *BuildBlastServiceJob (struct Service *service_p, const json_t *service_job_json_p);

BLAST_SERVICE_LOCAL json_t *BuildBlastServiceJobJSON (Service *service_p, const ServiceJob *service_job_p);


BLAST_SERVICE_LOCAL void CustomiseBlastServiceJob (Service *service_p, ServiceJob *job_p);

BLAST_SERVICE_LOCAL LinkedList *GetValueFromBlastServiceJobOutput (Service *service_p, ServiceJob *job_p, const char * const input_s);

#ifdef __cplusplus
}
#endif



#endif		/* #ifndef BLAST_SERVICE_H */
