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


typedef struct SamToolsServiceData
{
	ServiceData bsd_base_data;

} SamToolsServiceData;

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


static bool GetScaffoldData (SamToolsServiceData *data_p, const char * const filename_s, const char * const scaffold_name_s, ByteBuffer *buffer_p);


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
							
							* (services_p -> sa_services_pp) = service_p;

							return services_p;
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
 



static SamToolsServiceData *AllocateSamToolsServiceData (Service *service_p)
{
	SamToolsServiceData *data_p = (SamToolsServiceData *) AllocMemory (sizeof (SamToolsServiceData));

	if (data_p)
		{
			return data_p;
		}

	return NULL;
}


static void FreeSamToolsServiceData (SamToolsServiceData *data_p)
{
	FreeMemory (data_p);
}

 
static bool CloseSamToolsService (Service *service_p)
{
	FreeSamToolsServiceData ((SamToolsServiceData *) (service_p -> se_data_p));
	FreeService (service_p);

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
			Parameter *param_p = NULL;
			SharedType def;
			char *filename_s = CopyToNewString ("/tgac/references/internal/assembly/triticum_aestivum/TGAC/v1/Triticum_aestivum_CS42_TGACv1_all.fa", 0, false);

			if (filename_s)
				{
					def.st_string_value_s = filename_s;

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "index", "Fasta Index filename", "Fasta Index filename", TAG_SAMTOOLS_FILENAME, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
						{
							def.st_string_value_s = NULL;

							if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "Scaffold", "Scaffold name", "Scaffold NAME", TAG_SAMTOOLS_SCAFFOLD, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
								{
									return param_set_p;
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


static ServiceJobSet *RunSamToolsService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	SamToolsServiceData *data_p = (SamToolsServiceData *) (service_p -> se_data_p);
	service_p -> se_jobs_p = AllocateServiceJobSet (service_p, 1, CleanUpSamToolsJob);

	if (service_p -> se_jobs_p)
		{
			ServiceJob *job_p = service_p -> se_jobs_p -> sjs_jobs_p;
			Parameter *param_p = NULL;

			job_p -> sj_status = OS_FAILED_TO_START;

			param_p = GetParameterFromParameterSetByTag (param_set_p, TAG_SAMTOOLS_FILENAME);

			if (param_p)
				{
					const char *filename_s = param_p -> pa_current_value.st_string_value_s;

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
													job_p -> sj_status = OS_FAILED;

													if (GetScaffoldData (data_p, filename_s, scaffold_s, buffer_p))
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
																	PrintErrors (STM_LEVEL_SEVERE, "Failed to create job result sequence data for scaffold name %s from %s", scaffold_s, filename_s);
																}
														}

													FreeByteBuffer (buffer_p);

												}

										}		/* if (scaffold_s) */

								}

						}		/* if (filename_s) */

				}

		}		/* if (service_p -> se_jobs_p) */

	return service_p -> se_jobs_p;
}


static bool GetScaffoldData (SamToolsServiceData *data_p, const char * const filename_s, const char * const scaffold_name_s, ByteBuffer *buffer_p)
{
	bool success_flag = false;
	faidx_t *fai_p = fai_load (filename_s);

	if (fai_p)
		{
			if (AppendStringsToByteBuffer (buffer_p, ">", scaffold_name_s, "\n", NULL))
				{
					int seq_len;
					char *sequence_s = fai_fetch (fai_p, scaffold_name_s, &seq_len);

					if (sequence_s)
						{
							if (AppendToByteBuffer (buffer_p, sequence_s, (size_t) seq_len))
								{
									success_flag = true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, "Failed to add sequence data for scaffold name %s from %s", scaffold_name_s, filename_s);
								}

							free (sequence_s);
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, "Failed to fetch scaffold name %s from %s", scaffold_name_s, filename_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "Failed to add scaffold name %s to scaffold data", scaffold_name_s);
				}

			fai_destroy (fai_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "Failed to load fasta index %s", filename_s);
		}

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
