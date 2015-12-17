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

typedef struct SamToolsServiceData
{
	ServiceData stsd_base_data;
	const char **stsd_index_files_ss;
	size_t stsd_num_index_files;
} SamToolsServiceData;


static const int ST_DEFAULT_LINE_BREAK_INDEX = 60;

static const char * const BLASTDB_S = "Blast";
static const char * const FASTA_FILENAME_S = "Fasta";


/*
 * STATIC PROTOTYPES
 */

static SamToolsServiceData *AllocateSamToolsServiceData (Service *service_p);

static void FreeSamToolsServiceData (SamToolsServiceData *data_p);

static const char *GetSamToolsServiceName (Service *service_p);

static const char *GetSamToolsServiceDesciption (Service *service_p);

static ParameterSet *GetSamToolsServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseSamToolsServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunSamToolsService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForSamToolsService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseSamToolsService (Service *service_p);

static json_t *GetSamToolsResultsAsJSON (Service *service_p, const uuid_t service_id);

static bool CleanUpSamToolsJob (ServiceJob *job_p);


static bool GetScaffoldData (SamToolsServiceData *data_p, const char * const filename_s, const char * const scaffold_name_s, int break_index, ByteBuffer *buffer_p);

static bool GetSamToolsServiceConfig (SamToolsServiceData *data_p);

static const char *GetIndexFilenameFromBlastDBName (const json_t * const data_p, const char * const blast_s);

/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *config_p)
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
								GetSamToolsResultsAsJSON,
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

							data_p -> stsd_index_files_ss = (const char **) AllocMemoryArray (sizeof (const char *), size);

							if (data_p -> stsd_index_files_ss)
								{
									size_t i;
									json_t *index_file_p;

									json_array_foreach (index_files_p, i, index_file_p)
										{
											* ((data_p -> stsd_index_files_ss) + i) = json_string_value (index_file_p);
										}

									data_p -> stsd_num_index_files = size;

									success_flag = true;
								}

						}
					else
						{
							if (json_is_string (index_files_p))
								{
									data_p -> stsd_index_files_ss = (const char **) AllocMemoryArray (sizeof (const char *), 1);

									if (data_p -> stsd_index_files_ss)
										{
											* (data_p -> stsd_index_files_ss) = json_string_value (index_files_p);
											data_p -> stsd_num_index_files = 1;

											success_flag = true;
										}
								}
						}

				}		/* if (index_files_p) */

		}		/* if (blast_config_p) */

	return success_flag;
}



static SamToolsServiceData *AllocateSamToolsServiceData (Service *service_p)
{
	SamToolsServiceData *data_p = (SamToolsServiceData *) AllocMemory (sizeof (SamToolsServiceData));

	if (data_p)
		{
			data_p -> stsd_index_files_ss = NULL;
			data_p -> stsd_num_index_files = 0;

			return data_p;
		}

	return NULL;
}


static void FreeSamToolsServiceData (SamToolsServiceData *data_p)
{
	if (data_p -> stsd_index_files_ss)
		{
			FreeMemory (data_p -> stsd_index_files_ss);
		}
	FreeMemory (data_p);
}

 
static bool CloseSamToolsService (Service *service_p)
{
	FreeSamToolsServiceData ((SamToolsServiceData *) (service_p -> se_data_p));

	return true;
}
 
 
static const char *GetSamToolsServiceName (Service *service_p)
{
	return "SamTools service";
}


static const char *GetSamToolsServiceDesciption (Service *service_p)
{
	return "A service using SamTools";
}


static ParameterSet *GetSamToolsServiceParameters (Service *service_p, Resource *resource_p, const json_t *config_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("SamTools service parameters", "The parameters used for the SamTools service");
	
	if (param_set_p)
		{
			SamToolsServiceData *data_p = (SamToolsServiceData *) (service_p -> se_data_p);
			Parameter *param_p = NULL;
			SharedType def;
			char *filename_s = NULL;

			if (data_p -> stsd_index_files_ss)
				{
					filename_s = (char *) (* (data_p -> stsd_index_files_ss));
				}

			if (filename_s)
				{
					def.st_string_value_s = filename_s;

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "Index", "Fasta Index filename", "Fasta Index filename", TAG_SAMTOOLS_FILENAME, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
						{
							def.st_string_value_s = NULL;

							if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "Blast database", "Blast database", "The path to the Blast database", TAG_SAMTOOLS_BLASTDB_FILENAME, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
								{
									def.st_string_value_s = NULL;

									if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "Scaffold", "Scaffold name", "The name of the scaffold to find", TAG_SAMTOOLS_SCAFFOLD, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
										{
											def.st_long_value = ST_DEFAULT_LINE_BREAK_INDEX;

											if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_SIGNED_INT, false, "Line break index", "Max Line Length", "If this is greater than 0, then add a newline after each block of this many letters", TAG_SAMTOOLS_SCAFFOLD_LINE_BREAK, NULL, def, NULL, NULL, PL_ADVANCED, NULL)) != NULL)
												{
													return param_set_p;
												}
										}
								}
						}

					FreeCopiedString (filename_s);
				}		/* if (filename_s) */

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */
		
	return NULL;
}


static void ReleaseSamToolsServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static json_t *GetSamToolsResultsAsJSON (Service *service_p, const uuid_t job_id)
{
	SamToolsServiceData *data_p = (SamToolsServiceData *) (service_p -> se_data_p);
	json_t *resource_json_p = NULL;

	return resource_json_p;
}


static const char *GetIndexFilenameFromBlastDBName (const json_t * const data_p, const char * const blast_s)
{
	const char *index_s = NULL;
	const json_t *value_p = json_object_get (data_p, BLASTDB_S);

	if (value_p)
		{
			if (json_is_string (value_p))
				{
					const char * value_s = json_string_value (value_p);

					if (strcmp (value_s, blast_s) == 0)
						{
							const json_t *fai_p = json_object_get (data_p, FASTA_FILENAME_S);

							if (fai_p)
								{
									if (json_is_string (fai_p))
										{
											index_s = json_string_value (fai_p);

											#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
											PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Matched index file \"%s\" from blast db \"%s\"", index_s, blast_s);
											#endif
										}
								}
						}
				}
		}

	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Index file \"%s\" for blast db \"%s\"", index_s ? index_s : "NULL", blast_s);
	#endif

	return index_s;
}



static ServiceJobSet *RunSamToolsService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	SamToolsServiceData *data_p = (SamToolsServiceData *) (service_p -> se_data_p);
	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1, CleanUpSamToolsJob);

	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: RunSamToolsService - enter");
	#endif

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = service_p -> se_jobs_p -> sjs_jobs_p;
			Parameter *param_p = NULL;
			const char *filename_s = NULL;

			job_p -> sj_status = OS_FAILED_TO_START;

			/* Try to get the fasta filename */
			param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_SAMTOOLS_FILENAME);
			if (param_p)
				{
					if (!IsStringEmpty (param_p -> pa_current_value.st_string_value_s))
						{
							filename_s = param_p -> pa_current_value.st_string_value_s;
						}
				}

			/* If the fasta filename is empty, try and get the blastdb name */
			if (!filename_s)
				{
					param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_SAMTOOLS_BLASTDB_FILENAME);
					if (param_p)
						{
							if (!IsStringEmpty (param_p -> pa_current_value.st_string_value_s))
								{
									const char *blast_db_s = param_p -> pa_current_value.st_string_value_s;

									if (blast_db_s)
										{
											const json_t *indexes_p = json_object_get (data_p -> stsd_base_data.sd_config_p, "index_files");

											if (indexes_p)
												{
													if (json_is_array (indexes_p))
														{
															size_t i;
															const size_t num_indexes = json_array_size (indexes_p);

															for (i = 0; i < num_indexes; ++ i)
																{
																	json_t *value_p = json_array_get (indexes_p, i);

																	filename_s = GetIndexFilenameFromBlastDBName (value_p, blast_db_s);

																	if (filename_s)
																		{
																			i = num_indexes;
																		}
																}
														}
													else
														{
															filename_s = GetIndexFilenameFromBlastDBName (indexes_p, blast_db_s);
														}
												}
										}
								}
						}
				}		/* if (!filename_s) */

			if (filename_s)
				{
					param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_SAMTOOLS_SCAFFOLD);

					if (param_p)
						{
							const char *scaffold_s = param_p -> pa_current_value.st_string_value_s;

							if (scaffold_s)
								{
									ByteBuffer *buffer_p = AllocateByteBuffer (16384);

									if (buffer_p)
										{
											int break_index = ST_DEFAULT_LINE_BREAK_INDEX;
											param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_SAMTOOLS_SCAFFOLD_LINE_BREAK);

											if (param_p)
												{
													break_index = param_p -> pa_current_value.st_long_value;
												}

											job_p -> sj_status = OS_FAILED;

											// temporarily don't pass break index
											break_index = 0;
											if (GetScaffoldData (data_p, filename_s, scaffold_s, break_index, buffer_p))
												{
													const char *sequence_s = GetByteBufferData (buffer_p);
													json_error_t error;
													json_t *res_p = json_pack_ex (&error, 0, "{s:s}", "scaffold", sequence_s);

													if (res_p)
														{
															job_p -> sj_status = OS_SUCCEEDED;
															job_p -> sj_result_p = res_p;
														}
													else
														{
															json_error_t error;
															job_p -> sj_errors_p = json_pack_ex (&error, 0, "[{s:s}]", "Create sequence error", sequence_s);
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create job result sequence data for scaffold name %s from %s", scaffold_s, filename_s);
														}
												}
											else
												{
													json_error_t error;
													job_p -> sj_errors_p = json_pack_ex (&error, 0, "[{s:s}]", "General error", "Failed to get scaffold data");
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
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s parameter", TAG_SAMTOOLS_SCAFFOLD);
						}

				}		/* if (filename_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get input filename");
				}

		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}


static bool GetScaffoldData (SamToolsServiceData *data_p, const char * const filename_s, const char * const scaffold_name_s, int break_index, ByteBuffer *buffer_p)
{
	bool success_flag = false;
	faidx_t *fai_p = NULL;

	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: GetScaffoldData - about to load %s", filename_s);
	#endif

	fai_p = fai_load (filename_s);

	#if SAMTOOLS_SERVICE_DEBUG >= STM_LEVEL_FINER
	PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "SamToolsService :: GetScaffoldData - loaded %s to " SIZET_FMT, filename_s, fai_p);
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


static bool IsFileForSamToolsService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	return true;
}




static bool CleanUpSamToolsJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;


	return cleaned_up_flag;
}
