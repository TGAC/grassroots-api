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


#ifndef BLAST_SERVICE_H
#define BLAST_SERVICE_H


#include "blast_service_api.h"
#include "blast_formatter.h"
#include "blast_app_parameters.h"
#include "blast_service_api.h"

#include "parameter_set.h"
#include "temp_file.hpp"


/**
 * An enumeration for differentiating between
 * the different types of database that the
 * BLAST algorithms can be used with.
 */
typedef enum BLAST_SERVICE_LOCAL DatabaseType
{
	/** The database is a nucleotide sequence. */
	DT_NUCLEOTIDE,

	/** The database is a protein sequence. */
	DT_PROTEIN,

	/** The number of different database types. */
	DT_NUM_TYPES
} DatabaseType;


/**
 * An enumeration for differentiating between
 * the differentBLAST algorithms that can be
 * used.
 */
typedef enum BLAST_SERVICE_LOCAL BlastServiceType
{
	/** A search using the BlastN program. */
	BST_BLASTN,

	/** A search using the BlastP program. */
	BST_BLASTP,

	/** A search using the BlastX program. */
	BST_BLASTX,

	/** The number of different Blast service types. */
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


	/** Specifies whether the databases are nucelotide or protein databases. */
	DatabaseType bsd_type;

} BlastServiceData;




#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_BLAST_SERVICE_CONSTANTS
	#define BLAST_SERVICE_PREFIX BLAST_SERVICE_LOCAL
	#define BLAST_SERVICE_VAL(x)	= x
	#define BLAST_SERVICE_STRUCT_INITIALIZER(x) x
#else
	#define BLAST_SERVICE_PREFIX extern
	#define BLAST_SERVICE_VAL(x)
	 #define BLAST_SERVICE_STRUCT_INITIALIZER(x)
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */


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

/**
 * Get the Services available for running BLAST jobs.
 *
 * @param config_p The service configuration data.
 * @return The ServicesArray containing all of the BLAST Services or
 * <code>NULL</code> upon error.
 */
BLAST_SERVICE_API ServicesArray *GetServices (const json_t *config_p);


/**
 * Free the ServicesArray and its associated BLAST Services.
 *
 * @param services_p The ServicesArray to free.
 */
BLAST_SERVICE_API void ReleaseServices (ServicesArray *services_p);


BLAST_SERVICE_LOCAL ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p, BlastAppParameters *app_params_p);


/**
 * Check whether the required configuration details contain the
 * required information for a Blast Service to be used.
 *
 * @param data_p The BlastServcieData to check.
 * @return <code>true</code> if the configuration details are sufficient,
 * <code>false</code> otherwise.
 * @memberof BlastServiceData
 */
BLAST_SERVICE_LOCAL bool GetBlastServiceConfig (BlastServiceData *data_p);


/**
 * Allocate a BlastServiceData.
 *
 * @param blast_service_p The Blast Service that will own this BlastServiceData.
 * @param database_type The type of data that the given Blast Service can act upon.
 * @return The newly-created BlastServiceData or <code>NULL</code> upon error.
 * @memberof BlastServiceData
 */
BLAST_SERVICE_LOCAL BlastServiceData *AllocateBlastServiceData (Service *blast_service_p, DatabaseType database_type);


/**
 * Free a BlastServiceData.
 *
 * @param data_p The BlastServiceData to free.
 * @memberof BlastServiceData
 */
BLAST_SERVICE_LOCAL void FreeBlastServiceData (BlastServiceData *data_p);

BLAST_SERVICE_LOCAL bool CloseBlastService (Service *service_p);

BLAST_SERVICE_LOCAL ParameterSet *IsResourceForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p);

BLAST_SERVICE_LOCAL bool AddBaseBlastServiceParameters (Service *blast_service_p, ParameterSet *param_set_p, const DatabaseType db_type, bool (*add_additional_params_fn) (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p));

BLAST_SERVICE_LOCAL void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p);


BLAST_SERVICE_LOCAL bool DetermineBlastResult (Service *service_p, struct BlastServiceJob *job_p);

BLAST_SERVICE_LOCAL OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t service_id);

BLAST_SERVICE_LOCAL TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t job_id);


/**
 * Get the result of a previously ran BlastServiceJob in a given output format.
 *
 * @param data_p The BlastServiceData of the Blast Service that ran the job.
 * @param job_id The ServiceJob identifier to get the results for.
 * @param output_format_code The required output format code.
 * @return A newly-allocated string containing the results in the requested format
 * or <code>NULL</code> upon error.
 * @see GetBlastResultByUUIDString
 */
BLAST_SERVICE_LOCAL char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id, const uint32 output_format_code);

/**
 * Get the result of a previously ran BlastServiceJob in a given output format.
 *
 * @param data_p The BlastServiceData of the Blast Service that ran the job.
 * @param job_id_s The ServiceJob identifier, as a string, to get the results for.
 * @param output_format_code The required output format code.
 * @return A newly-allocated string containing the results in the requested format
 * or <code>NULL</code> upon error.
 * @see GetBlastResultByUUID
 */
BLAST_SERVICE_LOCAL char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s, const uint32 output_format_code);

BLAST_SERVICE_LOCAL ServiceJobSet *GetPreviousJobResults (LinkedList *ids_p, BlastServiceData *blast_data_p, const uint32 output_format_code);

BLAST_SERVICE_LOCAL ServiceJobSet *CreateJobsForPreviousResults (ParameterSet *params_p, const char *ids_s, BlastServiceData *blast_data_p);

BLAST_SERVICE_LOCAL void PrepareBlastServiceJobs (const DatabaseInfo *db_p, const ParameterSet * const param_set_p, ServiceJobSet *jobs_p, BlastServiceData *data_p);


BLAST_SERVICE_LOCAL ServiceJob *BuildBlastServiceJob (struct Service *service_p, const json_t *service_job_json_p);

BLAST_SERVICE_LOCAL json_t *BuildBlastServiceJobJSON (Service *service_p, const ServiceJob *service_job_p);


BLAST_SERVICE_LOCAL void CustomiseBlastServiceJob (Service *service_p, ServiceJob *job_p);


/**
 * For a given service-configured name, find the corresponding BLAST database filename.
 *
 * @param data_p The BlastServiceData with the service configuration details.
 * @param name_s The service-configured name.
 * @return The corresponding BLAST database filename or <code>NULL</code> if it could
 * not be found.
 */
BLAST_SERVICE_LOCAL const char *GetMatchingDatabaseFilename (BlastServiceData *data_p, const char *name_s);


/**
 * For a given BLAST database filename, find the corresponding service-configured name.
 *
 * @param data_p The BlastServiceData with the service configuration details.
 * @param name_s The BLAST database filename.
 * @return The corresponding service-configured name or <code>NULL</code> if it could
 * not be found.
 */
BLAST_SERVICE_LOCAL const char *GetMatchingDatabaseName (BlastServiceData *data_p, const char *filename_s);


#ifdef __cplusplus
}
#endif



#endif		/* #ifndef BLAST_SERVICE_H */
