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
#include <string.h>
#include <time.h>


#define ALLOCATE_POLYMARKER_TAGS (1)
#include "polymarker_service.h"

#include "memory_allocations.h"
#include "string_utils.h"
#include "jobs_manager.h"
#include "byte_buffer.h"
#include "polymarker_service_job.h"
#include "service_config.h"
#include "grassroots_config.h"
#include "provider.h"


#ifdef _DEBUG
	#define POLYMARKER_SERVICE_DEBUG	(STM_LEVEL_FINEST)
#else
	#define POLYMARKER_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


/*
 * bin/polymarker.rb --contigs ~/Applications/grassroots-0/grassroots/extras/blast/databases/Triticum_aestivum_CS42_TGACv1_all.fa --marker_list test/data/short_primer_design_test.csv --output ~/Desktop/polymarker_out
 */


static const char * const PS_BLASTDB_S = "Blast database";
static const char * const PS_FASTA_FILENAME_S = "Fasta";
static const char * const PS_DATABASE_GROUP_NAME_S = "Available contigs";

/*
 * STATIC PROTOTYPES
 */

static PolymarkerServiceData *AllocatePolymarkerServiceData (Service *service_p);

static void FreePolymarkerServiceData (PolymarkerServiceData *data_p);

static const char *GetPolymarkerServiceName (Service *service_p);

static const char *GetPolymarkerServiceDesciption (Service *service_p);

static ParameterSet *GetPolymarkerServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static void ReleasePolymarkerServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunPolymarkerService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static  ParameterSet *IsFileForPolymarkerService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool ClosePolymarkerService (Service *service_p);


static uint16 AddDatabaseParams (PolymarkerServiceData *data_p, ParameterSet *param_set_p);


static bool GetPolymarkerServiceConfig (PolymarkerServiceData *data_p);


static void CustomisePolymarkerServiceJob (Service * UNUSED_PARAM (service_p), ServiceJob *job_p);


static char *CreateGroupName (const char *server_s);

/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t * UNUSED_PARAM (config_p))
{
	Service *service_p = (Service *) AllocMemory (sizeof (Service));

	if (service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					PolymarkerServiceData *data_p = AllocatePolymarkerServiceData (service_p);
					
					if (data_p)
						{

							InitialiseService (service_p,
								GetPolymarkerServiceName,
								GetPolymarkerServiceDesciption,
								NULL,
								RunPolymarkerService,
								IsFileForPolymarkerService,
								GetPolymarkerServiceParameters,
								ReleasePolymarkerServiceParameters,
								ClosePolymarkerService,
								CustomisePolymarkerServiceJob,
								true,
								false,
								(ServiceData *) data_p);
							
							if (GetPolymarkerServiceConfig (data_p))
								{
									* (services_p -> sa_services_pp) = service_p;

									service_p -> se_deserialise_job_json_fn = GetPolymarkerServiceJobFromJSON;
									service_p -> se_serialise_job_json_fn = ConvertPolymarkerServiceJobToJSON;

									return services_p;
								}

						}

					FreeServicesArray (services_p);
				}

			FreeService (service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}


/*
 * STATIC FUNCTIONS 
 */
 

static bool GetPolymarkerServiceConfig (PolymarkerServiceData *data_p)
{
	bool success_flag = true;
	const json_t *polymarker_config_p = data_p -> psd_base_data.sd_config_p;

	if (polymarker_config_p)
		{
			json_t *index_files_p;
			const char *config_value_s = GetJSONString (polymarker_config_p, "tool");
			const char * const WORKING_DIRECTORY_KEY_S = "working_directory";

			if (config_value_s)
				{
					if (strcmp (config_value_s, "web") == 0)
						{
							data_p -> psd_tool_type = PTT_WEB;
						}
				}

			config_value_s = GetJSONString (polymarker_config_p, WORKING_DIRECTORY_KEY_S);
			if (config_value_s)
				{
					data_p -> psd_working_dir_s = config_value_s;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Polymarker Service requires a working directory specified by the %s", WORKING_DIRECTORY_KEY_S);
					success_flag = false;
				}




			index_files_p = json_object_get (polymarker_config_p, "index_files");

			if (index_files_p)
				{
					if (json_is_array (index_files_p))
						{
							size_t size = json_array_size (index_files_p);

							data_p -> psd_index_data_p = (IndexData *) AllocMemoryArray (sizeof (IndexData), size);

							if (data_p -> psd_index_data_p)
								{
									size_t i;
									json_t *index_file_p;

									json_array_foreach (index_files_p, i, index_file_p)
										{
											((data_p -> psd_index_data_p) + i) -> id_blast_db_name_s = GetJSONString (index_file_p, PS_BLASTDB_S);
											((data_p -> psd_index_data_p) + i) -> id_fasta_filename_s = GetJSONString (index_file_p, PS_FASTA_FILENAME_S);
										}

									data_p -> psd_index_data_size = size;

									success_flag = true;
								}

						}
					else
						{
							if (json_is_object (index_files_p))
								{
									data_p -> psd_index_data_p = (IndexData *) AllocMemoryArray (sizeof (IndexData), 1);

									if (data_p -> psd_index_data_p)
										{
											data_p -> psd_index_data_p -> id_blast_db_name_s = GetJSONString (index_files_p, PS_BLASTDB_S);
											data_p -> psd_index_data_p -> id_fasta_filename_s = GetJSONString (index_files_p, PS_FASTA_FILENAME_S);

											data_p -> psd_index_data_size = 1;

											success_flag = true;
										}
								}
						}

				}		/* if (index_files_p) */

		}		/* if (polymarker_config_p) */

	return success_flag;
}



static PolymarkerServiceData *AllocatePolymarkerServiceData (Service * UNUSED_PARAM (service_p))
{
	PolymarkerServiceData *data_p = (PolymarkerServiceData *) AllocMemory (sizeof (PolymarkerServiceData));

	data_p -> psd_tool_type = PTT_SYSTEM;

	return data_p;
}


static void FreePolymarkerServiceData (PolymarkerServiceData *data_p)
{
	FreeMemory (data_p);
}

 
static bool ClosePolymarkerService (Service *service_p)
{
	FreePolymarkerServiceData ((PolymarkerServiceData *) (service_p -> se_data_p));

	return true;
}
 
 
static const char *GetPolymarkerServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Polymarker service";
}


static const char *GetPolymarkerServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service using Polymarker";
}


static ParameterSet *GetPolymarkerServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Polymarker service parameters", "The parameters used for the Polymarker service");
	
	if (param_set_p)
		{
			PolymarkerServiceData *data_p = (PolymarkerServiceData *) (service_p -> se_data_p);
			Parameter *param_p = NULL;
			SharedType def;

			def.st_string_value_s = NULL;

			if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, PS_GENE_ID.npt_type, PS_GENE_ID.npt_name_s, "Gene ID", "An unique identifier for the assay", def, PL_ALL)) != NULL)
				{
					if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, PS_TARGET_CHROMOSOME.npt_type, PS_TARGET_CHROMOSOME.npt_name_s, "Target Chromosome", "The chromosome to use", def, PL_ALL)) != NULL)
						{
							if ((param_p = EasyCreateAndAddParameterToParameterSet (service_p -> se_data_p, param_set_p, NULL, PS_SEQUENCE.npt_type, PS_SEQUENCE.npt_name_s, "Sequence surrounding the polymorphisms", "The SNP must be marked in the format [A/T] for a varietal SNP with alternative bases, A or T",  def, PL_ALL)) != NULL)
								{
									if (AddDatabaseParams (data_p, param_set_p))
										{
											return param_set_p;
										}
								}
						}
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}


static void ReleasePolymarkerServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunPolymarkerService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	PolymarkerServiceData *data_p = (PolymarkerServiceData *) (service_p -> se_data_p);
	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Polymarker service job");


	return service_p -> se_jobs_p;
}


static  ParameterSet *IsFileForPolymarkerService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}


static void CustomisePolymarkerServiceJob (Service * UNUSED_PARAM (service_p), ServiceJob *job_p)
{
	job_p -> sj_update_fn = UpdatePolymarkerServiceJob;
	job_p -> sj_free_fn = FreePolymarkerServiceJob;
}



/*
 * The list of databases that can be searched
 */
static uint16 AddDatabaseParams (PolymarkerServiceData *data_p, ParameterSet *param_set_p)
{
	uint16 num_added_databases = 0;
	Parameter *param_p = NULL;
	SharedType def;

	if (data_p -> psd_index_data_size)
		{
			ParameterGroup *group_p = NULL;
			char *group_s = NULL;
			const json_t *provider_p = NULL;
			const char *group_to_use_s = NULL;

			provider_p = GetGlobalConfigValue (SERVER_PROVIDER_S);

			if (provider_p)
				{
					const char *provider_s = GetProviderName (provider_p);

					if (provider_s)
						{
							group_s = CreateGroupName (provider_s);
						}
				}

			group_to_use_s = group_s ? group_s : PS_DATABASE_GROUP_NAME_S;

			group_p = CreateAndAddParameterGroupToParameterSet (group_to_use_s, NULL, & (data_p -> psd_base_data), param_set_p);

			if (group_p)
				{
					const ServiceData *service_data_p = & (data_p -> psd_base_data);
					const IndexData *db_p = data_p -> psd_index_data_p;
					size_t i;


					for (i = data_p -> psd_index_data_size; i > 0; -- i, ++ db_p)
						{
							if (EasyCreateAndAddParameterToParameterSet (service_data_p, param_set_p, group_p, PT_BOOLEAN, db_p -> id_blast_db_name_s, db_p -> id_blast_db_name_s, db_p -> id_blast_db_name_s, def, PL_ALL))
								{
									++ num_added_databases;
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add database \"%s\"", db_p -> id_blast_db_name_s);
								}
						}

				}		/* if (db_p) */

			if (group_s)
				{
					FreeCopiedString (group_s);
				}

		}		/* if (num_group_params) */

	return num_added_databases;
}




static char *CreateGroupName (const char *server_s)
{
	char *group_name_s = ConcatenateVarargsStrings (PS_DATABASE_GROUP_NAME_S, " provided by ", server_s, NULL);

	if (group_name_s)
		{
			#if POLYMARKER_SERVICE_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Created group name \"%s\" for \"%s\" and \"%s\"", group_name_s, server_s);
			#endif
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create group name for \"%s\"", group_name_s);
		}

	return group_name_s;
}




