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
#include <string.h>
#include <time.h>

#include "samtools_service.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "jobs_manager.h"
#include "byte_buffer.h"

#include "htslib/faidx.h"



#ifdef _DEBUG
	#define SAMTOOLS_SERVICE_DEBUG	(STM_LEVEL_FINEST)
#else
	#define SAMTOOLS_SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif


typedef struct IndexData
{
	const char *id_blast_db_name_s;
	const char *id_fasta_filename_s;
} IndexData;

typedef struct SamToolsServiceData
{
	ServiceData stsd_base_data;
	IndexData *stsd_index_data_p;
	size_t stsd_index_data_size;
} SamToolsServiceData;


static const int ST_DEFAULT_LINE_BREAK_INDEX = 60;

static const char * const BLASTDB_S = "Blast database";
static const char * const FASTA_FILENAME_S = "Fasta";



static NamedParameterType SS_INPUT_FILENAME = { "Index filename", PT_STRING };
static NamedParameterType SS_BLASTDB_FILENAME = { "Blast database", PT_STRING };
static NamedParameterType SS_SCAFFOLD = { "Scaffold", PT_STRING };
static NamedParameterType SS_SCAFFOLLD_LINE_BREAK = { "Scaffold line break index", PT_SIGNED_INT };




/*
 * STATIC PROTOTYPES
 */

static SamToolsServiceData *AllocateSamToolsServiceData (Service *service_p);

static void FreeSamToolsServiceData (SamToolsServiceData *data_p);

static const char *GetSamToolsServiceName (Service *service_p);

static const char *GetSamToolsServiceDesciption (Service *service_p);

static ParameterSet *GetSamToolsServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static void ReleaseSamToolsServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunSamToolsService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static  ParameterSet *IsFileForSamToolsService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseSamToolsService (Service *service_p);


static bool GetScaffoldData (const char * const filename_s, const char * const scaffold_name_s, int break_index, ByteBuffer *buffer_p);

static bool GetSamToolsServiceConfig (SamToolsServiceData *data_p);



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
					ServiceData *data_p = (ServiceData *) AllocateSamToolsServiceData (service_p);
					
					if (data_p)
						{
							SamToolsServiceData *sam_data_p = (SamToolsServiceData *) data_p;

							InitialiseService (service_p,
								GetSamToolsServiceName,
								GetSamToolsServiceDesciption,
								NULL,
								RunSamToolsService,
								IsFileForSamToolsService,
								GetSamToolsServiceParameters,
								ReleaseSamToolsServiceParameters,
								CloseSamToolsService,
								NULL,
								true,
								true,
								data_p);
							
							if (GetSamToolsServiceConfig (sam_data_p))
								{
									* (services_p -> sa_services_pp) = service_p;

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
 

static bool GetSamToolsServiceConfig (SamToolsServiceData *data_p)
{
	bool success_flag = false;
	const json_t *sam_tools_config_p = data_p -> stsd_base_data.sd_config_p;

	if (sam_tools_config_p)
		{
			json_t *index_files_p = json_object_get (sam_tools_config_p, "index_files");

			if (index_files_p)
				{
					if (json_is_array (index_files_p))
						{
							size_t size = json_array_size (index_files_p);

							data_p -> stsd_index_data_p = (IndexData *) AllocMemoryArray (sizeof (IndexData), size);

							if (data_p -> stsd_index_data_p)
								{
									size_t i;
									json_t *index_file_p;

									json_array_foreach (index_files_p, i, index_file_p)
										{
											((data_p -> stsd_index_data_p) + i) -> id_blast_db_name_s = GetJSONString (index_file_p, BLASTDB_S);
											((data_p -> stsd_index_data_p) + i) -> id_fasta_filename_s = GetJSONString (index_file_p, FASTA_FILENAME_S);
										}

									data_p -> stsd_index_data_size = size;

									success_flag = true;
								}

						}
					else
						{
							if (json_is_object (index_files_p))
								{
									data_p -> stsd_index_data_p = (IndexData *) AllocMemoryArray (sizeof (IndexData), 1);

									if (data_p -> stsd_index_data_p)
										{
											data_p -> stsd_index_data_p -> id_blast_db_name_s = GetJSONString (index_files_p, BLASTDB_S);
											data_p -> stsd_index_data_p -> id_fasta_filename_s = GetJSONString (index_files_p, FASTA_FILENAME_S);

											data_p -> stsd_index_data_size = 1;

											success_flag = true;
										}
								}
						}

				}		/* if (index_files_p) */

		}		/* if (blast_config_p) */

	return success_flag;
}



static SamToolsServiceData *AllocateSamToolsServiceData (Service * UNUSED_PARAM (service_p))
{
	SamToolsServiceData *data_p = (SamToolsServiceData *) AllocMemory (sizeof (SamToolsServiceData));

	if (data_p)
		{
			data_p -> stsd_index_data_p = NULL;
			data_p -> stsd_index_data_size = 0;

			return data_p;
		}

	return NULL;
}


static void FreeSamToolsServiceData (SamToolsServiceData *data_p)
{
	if (data_p -> stsd_index_data_p)
		{
			FreeMemory (data_p -> stsd_index_data_p);
		}
	FreeMemory (data_p);
}

 
static bool CloseSamToolsService (Service *service_p)
{
	FreeSamToolsServiceData ((SamToolsServiceData *) (service_p -> se_data_p));

	return true;
}
 
 
static const char *GetSamToolsServiceName (Service * UNUSED_PARAM (service_p))
{
	return "SamTools service";
}


static const char *GetSamToolsServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service using SamTools";
}


static ParameterSet *GetSamToolsServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("SamTools service parameters", "The parameters used for the SamTools service");
	
	if (param_set_p)
		{
			SamToolsServiceData *data_p = (SamToolsServiceData *) (service_p -> se_data_p);
			Parameter *param_p = NULL;
			SharedType def;
			const char *filename_s = NULL;

			if (data_p -> stsd_index_data_p)
				{
					if (data_p -> stsd_index_data_p -> id_fasta_filename_s)
						{
							filename_s = data_p -> stsd_index_data_p -> id_fasta_filename_s;
						}
					else
						{
							filename_s = data_p -> stsd_index_data_p -> id_blast_db_name_s;
						}
				}


			if (filename_s)
				{
					def.st_string_value_s = (char *) filename_s;

					if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> stsd_base_data), param_set_p, NULL, SS_INPUT_FILENAME.npt_type, false, SS_INPUT_FILENAME.npt_name_s, "Fasta Index filename", "Fasta Index filename", NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
						{
							def.st_string_value_s = NULL;

							if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> stsd_base_data), param_set_p, NULL, SS_BLASTDB_FILENAME.npt_type, false, SS_BLASTDB_FILENAME.npt_name_s, "Blast database", "The path to the Blast database", NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
								{
									def.st_string_value_s = NULL;

									if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> stsd_base_data), param_set_p, NULL, SS_SCAFFOLD.npt_type, false, SS_SCAFFOLD.npt_name_s, "Scaffold name", "The name of the scaffold to find", NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
										{
											def.st_long_value = ST_DEFAULT_LINE_BREAK_INDEX;

											if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> stsd_base_data), param_set_p, NULL, SS_SCAFFOLLD_LINE_BREAK.npt_type, false, SS_SCAFFOLLD_LINE_BREAK.npt_name_s, "Max Line Length", "If this is greater than 0, then add a newline after each block of this many letters", NULL, def, NULL, NULL, PL_ADVANCED, NULL)) != NULL)
												{
													return param_set_p;
												}
										}
								}
						}
				}		/* if (filename_s) */

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}


static void ReleaseSamToolsServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunSamToolsService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	SamToolsServiceData *data_p = (SamToolsServiceData *) (service_p -> se_data_p);
	service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Samtools service job");

	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: RunSamToolsService - enter");
	#endif

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);
			Parameter *param_p = NULL;
			const char *filename_s = NULL;

			job_p -> sj_status = OS_FAILED_TO_START;

			/* Try to get the fasta filename */
			param_p = GetParameterFromParameterSetByName (param_set_p, SS_INPUT_FILENAME.npt_name_s);
			if (param_p)
				{
					if (!IsStringEmpty (param_p -> pa_current_value.st_string_value_s))
						{
							filename_s = param_p -> pa_current_value.st_string_value_s;
						}
					else
						{
							PrintLog (STM_LEVEL_WARNING, __FILE__, __LINE__, "param \"%s\" has an empty value", param_p -> pa_name_s);
						}
				}
			else
				{
					#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
					PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Could not find \"%s\" parameter, trying \"%s\" instead", SS_INPUT_FILENAME.npt_name_s, SS_BLASTDB_FILENAME.npt_name_s);
					#endif
				}

			/* If the fasta filename is empty, try and get the blastdb name */
			if (!filename_s)
				{
					param_p = GetParameterFromParameterSetByName (param_set_p, SS_BLASTDB_FILENAME.npt_name_s);

					if (param_p)
						{
							const char *blast_db_s = param_p -> pa_current_value.st_string_value_s;

							if (!IsStringEmpty (blast_db_s))
								{
									#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
									PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Looking to match database \"%s\"", blast_db_s);
									#endif

									if (data_p -> stsd_index_data_p)
										{
											IndexData *index_data_p = data_p -> stsd_index_data_p;
											size_t i = data_p ->  stsd_index_data_size;

											#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
											PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Checking against " SIZET_FMT " databases", i);
											#endif

											while (i > 0)
												{
													if (index_data_p -> id_blast_db_name_s)
														{
															#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
															PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Checking against database \"%s\"", index_data_p -> id_blast_db_name_s);
															#endif

															if (strcmp (index_data_p -> id_blast_db_name_s, blast_db_s) == 0)
																{
																	filename_s = index_data_p -> id_fasta_filename_s;

																	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
																	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "matched \"%s\" and using filename \"%s\"", blast_db_s, filename_s);
																	#endif
																	i = 0;
																}
															else
																{
																	++ index_data_p;
																	-- i;
																}
														}
													else
														{
															++ index_data_p;
															-- i;
														}
												}

											#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
											PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Finished checking against databases, filename_s = \"%s\"", filename_s ? filename_s : "NULL");
											#endif

										}		/* if (data_p -> stsd_index_data_p) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "No databases available");
										}

								}		/* if (!IsStringEmpty (blast_db_s)) */
							else
								{
									PrintLog (STM_LEVEL_WARNING, __FILE__, __LINE__, "param \"%s\" has an empty value", param_p -> pa_name_s);
								}

						}		/* if (param_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Could not find param \"%s\"", BLASTDB_S);
						}

				}		/* if (!filename_s) */

			if (filename_s)
				{
					param_p = GetParameterFromParameterSetByName (param_set_p, SS_SCAFFOLD.npt_name_s);

					if (param_p)
						{
							const char *scaffold_s = param_p -> pa_current_value.st_string_value_s;

							if (scaffold_s)
								{
									ByteBuffer *buffer_p = AllocateByteBuffer (16384);

									if (buffer_p)
										{
											int break_index = ST_DEFAULT_LINE_BREAK_INDEX;
											param_p = GetParameterFromParameterSetByName (param_set_p, SS_SCAFFOLLD_LINE_BREAK.npt_name_s);

											if (param_p)
												{
													break_index = param_p -> pa_current_value.st_long_value;
												}

											job_p -> sj_status = OS_FAILED;

											// temporarily don't pass break index
											break_index = 0;
											if (GetScaffoldData (filename_s, scaffold_s, break_index, buffer_p))
												{
													json_t *result_p = NULL;
													const char *sequence_s = GetByteBufferData (buffer_p);

													if (sequence_s)
														{
															json_t *sequence_p = json_string (sequence_s);

															if (sequence_p)
																{
																	result_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, scaffold_s, sequence_p);

																	if (!result_p)
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get json result for %s", sequence_s);
																		}

																	json_decref (sequence_p);
																}
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create json sequence from %s", sequence_s);
																}
														}		/* if (sequence_s) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get sequence from buffer for %s from %s", scaffold_s, filename_s);
														}


													if (result_p)
														{
															if (AddResultToServiceJob (job_p, result_p))
																{
																	job_p -> sj_status = OS_SUCCEEDED;
																}
															else
																{
																	char uuid_s [UUID_STRING_BUFFER_SIZE];

																	json_decref (result_p);
																	AddErrorToServiceJob (job_p, ERROR_S, "Failed to add result");

																	ConvertUUIDToString (job_p -> sj_id, uuid_s);
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add result for %s", uuid_s);
																}
														}
													else
														{
															if (!AddErrorToServiceJob (job_p, "Create sequence error", scaffold_s))
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create job result sequence data for scaffold name %s from %s", scaffold_s, filename_s);
																}
														}
												}
											else
												{
													if (!AddErrorToServiceJob (job_p, "Get scaffold error", "Failed to get scaffold data"))
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add error to job");
														}
												}

											FreeByteBuffer (buffer_p);

										}		/* if (buffer_p) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate byte buffer to store scaffold data");
										}

								}		/* if (scaffold_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get scaffold");
								}

						}		/* if (param_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s parameter", SS_SCAFFOLD.npt_name_s);
						}

				}		/* if (filename_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get input filename");
				}

		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}


static bool GetScaffoldData (const char * const filename_s, const char * const scaffold_name_s, int break_index, ByteBuffer *buffer_p)
{
	bool success_flag = false;
	faidx_t *fai_p = NULL;

	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: GetScaffoldData - about to load %s", filename_s);
	#endif

	fai_p = fai_load (filename_s);

	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: GetScaffoldData - loaded %s to " SIZET_FMT, filename_s, (size_t) fai_p);
	#endif


	if (fai_p)
		{
			if (AppendStringsToByteBuffer (buffer_p, ">", scaffold_name_s, "\n", NULL))
				{
					int seq_len;
					char *sequence_s = fai_fetch (fai_p, scaffold_name_s, &seq_len);

					#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
					PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: GetScaffoldData - fetched %s with length %d", scaffold_name_s, seq_len);
					#endif

					if (sequence_s)
						{
							#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
							PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: GetScaffoldData - breaking at %d", break_index);
							#endif

							if (break_index > 0)
								{
									int i = 0;
									int block_size = break_index;
									char *current_p = sequence_s;
									bool loop_flag = true;

									success_flag = true;

									while (loop_flag && success_flag)
										{
											if (AppendToByteBuffer (buffer_p, current_p, block_size))
												{
													if (AppendToByteBuffer (buffer_p, "\n", 1))
														{
															if (i + break_index < seq_len)
																{
																	i += break_index;
																	current_p += break_index;
																}
															else
																{
																	loop_flag = false;
																}
														}
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add new line to scaffold data %s", sequence_s);
															success_flag = false;
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to split scaffold data %s with new lines", sequence_s);
													success_flag = false;
												}
										}

									if (success_flag)
										{
											if (seq_len > i)
												{
													success_flag = false;

													if (AppendToByteBuffer (buffer_p, current_p, seq_len - i))
														{
															if (AppendToByteBuffer (buffer_p, "\n", 1))
																{
																	success_flag = true;
																}
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add new line to scaffold data %s", sequence_s);
																	success_flag = false;
																}
														}
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to split scaffold data %s with new lines", sequence_s);
															success_flag = false;
														}
												}
										}

								}
							else
								{
									if (AppendToByteBuffer (buffer_p, sequence_s, (size_t) seq_len))
										{
											success_flag = true;
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add sequence data for scaffold name %s from %s", scaffold_name_s, filename_s);
										}
								}

							free (sequence_s);
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to fetch scaffold name %s from %s", scaffold_name_s, filename_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add scaffold name %s to scaffold data", scaffold_name_s);
				}

			fai_destroy (fai_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load fasta index %s", filename_s);
		}


	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINE
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "SamToolsService :: GetScaffoldData - returning %d:\n%s\n", success_flag, GetByteBufferData (buffer_p));
	#endif


	return success_flag;
}


static  ParameterSet *IsFileForSamToolsService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}

