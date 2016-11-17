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
 * blast_service_job.c
 *
 *  Created on: 21 Jan 2016
 *      Author: tyrrells
 */

#include <string.h>

#include "blast_service_job.h"
#include "byte_buffer.h"
#include "filesystem_utils.h"
#include "blast_service.h"
#include "string_utils.h"
#include "blast_tool_factory.hpp"
#include "blast_tool.hpp"
#include "blast_service_params.h"


/*
 * STATIC DECLARATIONS
 */

static const char * const BSJ_TOOL_S = "tool";
static const char * const BSJ_FACTORY_S = "factory";
static const char * const BSJ_JOB_S = "job";


static bool GetAndAddHitLocation (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_from_key_s, const char *hsp_to_key_s, const char *strand_key_s, const char *child_key_s);


static LinkedList *GetScaffoldsFromHit (const json_t *hit_p);

static bool MarkUpHit (const json_t *hit_p, json_t *mark_up_p, const char *database_s);

static json_t *GetInitialisedProcessedRequest (void);


static bool AddTerm (json_t *root_p, const char *key_s, const char *term_s);



static bool AddSequence (json_t *root_p, const char *key_s, const char *query_sequence_s);

static bool AddFaldoTerminus (json_t *parent_json_p, const char *child_key_s, const int32 position, const bool forward_strand_flag);


static bool AddIntScoreValue (json_t *parent_p, const char *key_s, int score_value);

static bool AddDoubleScoreValue (json_t *parent_p, const char *key_s, double64 score_value);


static bool GetAndAddDoubleScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s);

static bool GetAndAddIntScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s);

static bool GetAndAddSequenceValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *sequence_key_s);

static bool GetAndAddDatabaseDetails (json_t *marked_up_result_p, const char *database_s);


static bool AddHitDetails (json_t *marked_up_result_p, const json_t *blast_hit_p);


static bool AddHsp (json_t *marked_up_hit_p, const json_t *hsp_p);


/*
 * API DEFINITIONS
 */

BlastServiceJob *AllocateBlastServiceJob (Service *service_p, const char *job_name_s, const char *job_description_s, BlastServiceData *data_p)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) AllocMemory (sizeof (BlastServiceJob));

	if (blast_job_p)
		{
			BlastTool *tool_p = NULL;
			ServiceJob * const base_service_job_p = & (blast_job_p -> bsj_job);

			InitServiceJob (base_service_job_p, service_p, job_name_s, job_description_s, NULL, FreeBlastServiceJob);

			tool_p = CreateBlastToolFromFactory (data_p -> bsd_tool_factory_p, blast_job_p, job_name_s, data_p);

			if (tool_p)
				{
					blast_job_p -> bsj_tool_p = tool_p;

					return blast_job_p;
				}		/* if (tool_p) */

			ClearServiceJob (base_service_job_p);
			FreeMemory (blast_job_p);
		}		/* if (blast_job_p) */

	return NULL;
}


void FreeBlastServiceJob (ServiceJob *job_p)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) job_p;

	if (blast_job_p -> bsj_tool_p)
		{
			FreeBlastTool (blast_job_p -> bsj_tool_p);
		}

	ClearServiceJob (job_p);

	FreeMemory (blast_job_p);
}


char *GetPreviousJobFilename (const BlastServiceData *data_p, const char *job_id_s, const char *suffix_s)
{
	char *job_output_filename_s = NULL;

	if (data_p -> bsd_working_dir_s)
		{
			ByteBuffer *buffer_p = AllocateByteBuffer (1024);

			if (buffer_p)
				{
					char sep [2];

					*sep = GetFileSeparatorChar ();
					* (sep + 1) = '\0';

					if (AppendStringsToByteBuffer (buffer_p, data_p -> bsd_working_dir_s, sep, job_id_s, suffix_s, NULL))
						{
							job_output_filename_s = DetachByteBufferData (buffer_p);
						}		/* if (AppendStringsToByteBuffer (buffer_p, data_p -> bsd_working_dir_s, sep, job_id_s, NULL)) */
					else
						{
							FreeByteBuffer (buffer_p);
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't create full path to job file \"%s\"", job_id_s);
						}
				}
		}		/* if (data_p -> bsd_working_dir_s) */
	else
		{
			job_output_filename_s = CopyToNewString (job_id_s, 0, false);
		}

	return job_output_filename_s;
}


BlastServiceJob *GetBlastServiceJobFromJSON (const json_t *blast_job_json_p, BlastServiceData *config_p)
{
	json_t *job_json_p = json_object_get (blast_job_json_p, BSJ_JOB_S);

	if (job_json_p)
		{
			BlastServiceJob *blast_job_p = (BlastServiceJob *) AllocMemory (sizeof (BlastServiceJob));

			if (blast_job_p)
				{
					blast_job_p -> bsj_tool_p = NULL;

					if (InitServiceJobFromJSON (& (blast_job_p -> bsj_job), job_json_p))
						{
							const char *factory_name_s = GetJSONString (blast_job_json_p, BSJ_FACTORY_S);

							if (factory_name_s)
								{
									const char *config_factory_name_s = config_p -> bsd_tool_factory_p -> GetName ();

									if (strcmp (factory_name_s, config_factory_name_s) == 0)
										{
											const json_t *tool_json_p = json_object_get (blast_job_json_p, BSJ_TOOL_S);

											if (tool_json_p)
												{
													BlastTool *tool_p = config_p -> bsd_tool_factory_p -> CreateBlastTool (tool_json_p, blast_job_p, config_p);

													if (tool_p)
														{
															blast_job_p -> bsj_tool_p = tool_p;

															return blast_job_p;
														}		/* if (tool_p) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for BlastTool");
														}

												}		/* if (tool_json_p) */
											else
												{
													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_json_p, "Failed to get tool from json");
												}

										}		/* if (strcmp (factory_name_s, config_factory_name_s) == 0) */

								}		/* if (factory_name_s) */
							else
								{
									PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_json_p, "Failed to get factory from json");
								}

						}		/* if (InitServiceJobFromJSON (& (blast_job_p -> bsj_job), job_json_p)) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_json_p, "InitServiceJobFromJSON failed");
						}

					FreeBlastServiceJob ((ServiceJob *) blast_job_p);
				}		/* if (blast_job_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for BlastServiceJob");
				}

		}		/* if (job_json_p) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_json_p, "Failed to get job from json");
		}

	return NULL;
}


json_t *ConvertBlastServiceJobToJSON (BlastServiceJob *job_p)
{
	json_t *blast_job_json_p = json_object ();

	if (blast_job_json_p)
		{
			const char *service_name_s = GetServiceName (job_p -> bsj_job.sj_service_p);

			if (json_object_set_new (blast_job_json_p, JOB_SERVICE_S, json_string (service_name_s)) == 0)
				{
					json_t *base_job_json_p = GetServiceJobAsJSON (& (job_p -> bsj_job));

					if (base_job_json_p)
						{
							if (json_object_set_new (blast_job_json_p, BSJ_JOB_S, base_job_json_p) == 0)
								{
									const char *config_factory_name_s = job_p -> bsj_tool_p -> GetFactoryName ();

									if (config_factory_name_s)
										{
											if (json_object_set_new (blast_job_json_p, BSJ_FACTORY_S, json_string (config_factory_name_s)) == 0)
												{
													json_t *tool_json_p = job_p -> bsj_tool_p -> GetAsJSON ();

													if (tool_json_p)
														{
															if (json_object_set_new (blast_job_json_p, BSJ_TOOL_S, tool_json_p) == 0)
																{
																	return blast_job_json_p;
																}
															else
																{
																	json_decref (tool_json_p);
																	PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, tool_json_p, "Failed to add tool definition json");
																}

														}		/* if (tool_json_p) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to created tool definition");
														}

												}		/* if (json_object_set_new (blast_job_json_p, BSJ_FACTORY_S, json_string (config_factory_name_s)) == 0) */
											else
												{
													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, blast_job_json_p, "Failed to add factory definition json");
												}

										}		/* if (config_factory_name_s) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get factory name");
										}

								}		/* if (json_object_set_new (blast_job_json_p, BSJ_JOB_S, base_job_json_p) == 0) */
							else
								{
									json_decref (base_job_json_p);
								}

						}		/* if (base_job_json_p) */
					else
						{
							char uuid_s [UUID_STRING_BUFFER_SIZE];

							ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "GetServiceJobAsJSON failed for %s", uuid_s);
						}

				}		/* if (json_object_set_new (blast_job_json_p, JOB_SERVICE_S, json_string (service_name_s)) == 0) */
			else
				{
					char uuid_s [UUID_STRING_BUFFER_SIZE];

					ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, blast_job_json_p, "Could not set %s:%s", JOB_SERVICE_S, service_name_s);
				}

			json_decref (blast_job_json_p);
		}		/* if (blast_job_json_p) */
	else
		{
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json_object failed for %s", uuid_s);
		}

	return NULL;
}


ServiceJob *DeserialiseBlastServiceJob (unsigned char *data_p, void *config_p)
{
	BlastServiceJob *blast_job_p = NULL;
	json_error_t err;
	json_t *blast_job_json_p = json_loads ((char *) data_p, 0, &err);

	if (blast_job_json_p)
		{
			blast_job_p = GetBlastServiceJobFromJSON (blast_job_json_p, (BlastServiceData *) config_p);
			json_decref (blast_job_json_p);
		}		/* if (blast_job_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to json, err \"%s\" at line %d, column %d", data_p, err.text, err.line, err.column);
		}

	return (ServiceJob *) blast_job_p;
}


unsigned char *SerialiseBlastServiceJob (ServiceJob *base_job_p, unsigned int *value_length_p)
{
	unsigned char *result_p = NULL;
	BlastServiceJob *blast_job_p = (BlastServiceJob *) base_job_p;
	json_t *job_json_p = ConvertBlastServiceJobToJSON (blast_job_p);

	if (job_json_p)
		{
			char *job_s = json_dumps (job_json_p, JSON_INDENT (2));

			if (job_s)
				{
					/*
					 * include the terminating \0 to make sure
					 * the value as a valid c-style string
					 */
					*value_length_p = strlen (job_s) + 1;
					result_p = (unsigned char *) job_s;
				}		/* if (job_s) */
			else
				{
					char uuid_s [UUID_STRING_BUFFER_SIZE];

					ConvertUUIDToString (base_job_p -> sj_id, uuid_s);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "json_dumps failed for \"%s\"", uuid_s);
				}
		}		/* if (json_p) */


	return result_p;
}



bool AddErrorToBlastServiceJob (BlastServiceJob *job_p)
{
	bool success_flag = false;
	char *log_s = job_p -> bsj_tool_p -> GetLog ();

	if (log_s)
		{
			if (AddErrorToServiceJob (& (job_p -> bsj_job), ERROR_S, log_s))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add error \"%s\" to service job");
				}

			FreeCopiedString (log_s);
		}		/* if (log_s) */
	else
		{
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);

			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get log for %s", uuid_s);
		}

	return success_flag;
}


bool UpdateBlastServiceJob (ServiceJob *job_p)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) job_p;
	BlastTool *tool_p = blast_job_p -> bsj_tool_p;

	tool_p -> GetStatus ();

	return true;
}




json_t *MarkUpBlastResult (BlastServiceJob *job_p)
{
	json_t *results_array_p = json_array ();

	if (results_array_p)
		{
			bool success_flag = false;
			BlastServiceData *data_p = (BlastServiceData *) (job_p -> bsj_job.sj_service_p -> se_data_p);

			/*
			 * Get the result. Ideally we'd like to get this in a format that we can parse, so to begin with we'll use the single json format
			 * available in blast 2.3+
			 */
			char *raw_result_s = GetBlastResultByUUID (data_p, job_p -> bsj_job.sj_id, BOF_SINGLE_FILE_JSON_BLAST);

			if (raw_result_s)
				{
					json_error_t err;
					json_t *blast_output_p = json_loads (raw_result_s, 0, &err);

					if (blast_output_p)
						{
							/*
							 * We currently understand hits objects and database names
							 */
							json_t *root_p = json_object_get (blast_output_p, "BlastOutput2");

							if (root_p)
								{
									if (json_is_array (root_p))
										{
											size_t i;
											json_t *result_p;

											json_array_foreach (root_p, i, result_p)
											{
												json_t *report_p = json_object_get (result_p, "report");

												if (report_p)
													{
														json_t *hits_p = NULL;
														const char *database_s = NULL;

														json_t *db_p = GetCompoundJSONObject (report_p, "search_target.db");

														/* Get the database name */
														if (db_p)
															{
																if (json_is_string (db_p))
																	{
																		database_s = json_string_value (db_p);

																		if (database_s)
																			{
																				success_flag = true;
																			}		/* if (database_s) */
																	}
															}

														if (success_flag)
															{
																/* Get the hits */
																hits_p = GetCompoundJSONObject (report_p, "results.search.hits");

																if (hits_p)
																	{
																		if (json_is_array (hits_p))
																			{
																				size_t j = 0;
																				const size_t num_hits = json_array_size (hits_p);

																				while ((j < num_hits) && success_flag)
																					{
																						json_t *hit_p = json_array_get (hits_p, j);

																						json_t *output_p = GetInitialisedProcessedRequest ();

																						if (output_p)
																							{
																								if (GetAndAddDatabaseDetails (output_p, database_s))
																									{
																										if (MarkUpHit (hit_p, output_p, database_s))
																											{
																												if (json_array_append_new (results_array_p, output_p) == 0)
																													{
																														success_flag = true;
																														++ j;
																													}
																												else
																													{
																														success_flag = false;
																													}
																											}
																										else
																											{
																												success_flag = false;
																											}

																									}
																								else
																									{
																										success_flag = true;
																									}


																								if (!success_flag)
																									{
																										json_decref (output_p);
																									}

																							}		/* if (output_p) */

																					}		/* while ((j < num_hits) && success_flag) */

																			}		/* if (json_is_array (hits_p)) */

																	}		/* if (hits_p) */

															}		/* if (success_flag) */

													}		/* if (report_p) */

											}		/* json_array_foreach (root_p, i, output_p) */

										}		/* if (json_is_array (root_p)) */

								}		/* if (root_p) */

							json_decref (blast_output_p);
						}		/* if (blast_output_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to load \"%s\" as json, error at %d, %d", raw_result_s, err.line, err.column);
						}

					FreeCopiedString (raw_result_s);
				}		/* if (raw_result_s) */
			else
				{
					char uuid_s [UUID_STRING_BUFFER_SIZE];

					ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast results for id \"%s\" in %d format", uuid_s, BOF_SINGLE_FILE_JSON_BLAST);
				}

		}		/* if (results_array_p) */

	return results_array_p;
}


char *ProcessLinkedServicesForBlastServiceJobOutput (Service *service_p, ServiceJob *job_p, LinkedService *linked_service_p)
{
	BlastServiceJob *blast_job_p = (BlastServiceJob *) job_p;

	if (job_p -> sj_processed_results_p)
		{
			UserDetails *user_p = NULL;
			Resource *resource_p = NULL;
			Service *output_service_p = GetServiceByName (linked_service_p -> ls_output_service_s);

			if (output_service_p)
				{
					ParameterSet *output_params_p = GetServiceParameters (output_service_p, resource_p, user_p);

					if (output_params_p)
						{
							if (json_is_array (job_p -> sj_processed_results_p))
								{
									size_t i;
									json_t *result_p;

									json_array_foreach (job_p -> sj_processed_results_p, i, result_p)
									{
										bool continue_flag = false;
										json_t *linked_services_array_p = json_array ();

										if (linked_services_array_p)
											{
												if (json_object_set_new (result_p, "grassroots:services", linked_services_array_p) == 0)
													{
														/*
														 * Database
														 */
														MappedParameter *mapped_param_p = GetMappedParameterByInputParamName (linked_service_p, "database");
														const char *database_s = NULL;

														if (mapped_param_p)
															{
																database_s = GetDatabase (result_p);

																if (database_s)
																	{
																		Parameter *param_p = GetParameterFromParameterSetByName (output_params_p, mapped_param_p -> mp_output_param_s);

																		if (param_p)
																			{
																				if (SetParameterValueFromString (param_p, database_s))
																					{
																						continue_flag = true;
																					}
																			}		/* if (param_p) */
																	}

															}		/* if (mapped_param_p) */
															else
																{
																	continue_flag = true;
																}


														if (continue_flag)
															{
																mapped_param_p = GetMappedParameterByInputParamName (linked_service_p, "scaffold");

																if (mapped_param_p)
																	{
																		Parameter *param_p = GetParameterFromParameterSetByName (output_params_p, mapped_param_p -> mp_output_param_s);

																		if (param_p)
																			{
																				const json_t *scaffolds_p = GetScaffoldsForDatabaseHits (result_p, database_s);

																				if (scaffolds_p)
																					{
																						if (json_is_array (scaffolds_p))
																							{
																								size_t j;
																								json_t *scaffold_p;

																								json_array_foreach (scaffolds_p, j, scaffold_p)
																								{
																									if (json_is_string (scaffold_p))
																										{
																											const char *scaffold_s = json_string_value (scaffold_p);

																											if (SetParameterValueFromString (param_p, scaffold_s))
																												{
																													json_t *run_service_p = GetInterestedServiceJSON (linked_service_p -> ls_output_service_s, NULL, output_params_p);

																													if (run_service_p)
																														{
																															if (json_array_append_new (linked_services_array_p, run_service_p) == 0)
																																{

																																}
																															else
																																{
																																	json_decref (run_service_p);
																																}

																														}		/* if (run_service_p) */

																												}		/* if (SetParameterValueFromString (param_p, scaffold_s)) */

																										}		/* if (json_is_string (scaffold_p)) */

																								}		/* json_array_foreach (scaffolds_p, j, scaffold_p) */

																							}		/* if (json_is_array (scaffolds_p)) */
																						else
																							{

																							}

																					}		/* if (scaffolds_p) */

																			}		/* if (param_p) */

																	}		/* if (mapped_param_p) */

															}		/* if (contune_flag) */

													}
												else
													{
														json_decref (linked_services_array_p);
													}

											}		/* if (linked_services_array_p) */



									}		/* json_array_foreach (job_p -> sj_processed_results_p, i, result_p) */

								}		/* if (json_is_array (job_p -> sj_processed_results_p)) */

							FreeParameterSet (output_params_p);
						}		/* if (output_params_p) */

					FreeService (output_service_p);
				}		/* if (output_service_p) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING,__FILE__, __LINE__, "Failed to get service \"%s\"", linked_service_p -> ls_output_service_s);
				}
		}

	return NULL;
}



const char *GetDatabase (const json_t *result_p)
{
	const char *database_s = NULL;

	if (result_p)
		{
			database_s = GetJSONString (result_p, "database");
		}

	return database_s;
}


const json_t *GetScaffoldsForDatabaseHits (const json_t *result_p, const char * const database_s)
{
	const json_t *scaffolds_p = NULL;

	if (result_p)
		{
			scaffolds_p = json_object_get (result_p, "scaffolds");
		}

	return scaffolds_p;
}

/*
 * STATIC DEFINITIONS
 */


/*
{
"BlastOutput2": [
{
    "report": {
      "program": "blastp",
      "version": "BLASTP 2.5.0+",
      "reference": "Stephen F. Altschul, Thomas L. Madden, Alejandro A. Sch&auml;ffer, Jinghui Zhang, Zheng Zhang, Webb Miller, and David J. Lipman (1997), \"Gapped BLAST and PSI-BLAST: a new generation of protein database search programs\", Nucleic Acids Res. 25:3389-3402.",
      "search_target": {
        "db": "/home/billy/Applications/grassroots-0/grassroots/extras/blast/databases/Triticum_aestivum_CS42_TGACv1_scaffold.annotation.gff3.pep.fa"
      },
      "params": {
        "matrix": "BLOSUM62",
        "expect": 10,
        "gap_open": 11,
        "gap_extend": 1,
        "filter": "F",
        "cbs": 2
      },
      "results": {
        "search": {
          "query_id": "Query_1",
          "query_title": "unnamed protein product",
          "query_len": 58,
          "hits": [
            {
              "num": 1,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|0",
                  "accession": "0",
                  "title": "TRIAE_CS42_1AL_TGACv1_000001_AA0000010.1   gene=TRIAE_CS42_1AL_TGACv1_000001_AA0000010   biotype=protein_coding   confidence=Low"
                }
              ],
              "len": 153,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 107.842,
                  "score": 268,
                  "evalue": 2.92884e-31,
                  "identity": 58,
                  "positive": 58,
                  "query_from": 1,
                  "query_to": 58,
                  "hit_from": 1,
                  "hit_to": 60,
                  "align_len": 60,
                  "gaps": 2,
                  "qseq": "MASDPRRRRRRTSPATGSSTTTTGSS--ASDLPVPGHPNLASSLGAQRDGHGRPIAERRR",
                  "hseq": "MASDPRRRRRRTSPATGSSTTTTGSSCPASDLPVPGHPNLASSLGAQRDGHGRPIAERRR",
                  "midline": "MASDPRRRRRRTSPATGSSTTTTGSS  ASDLPVPGHPNLASSLGAQRDGHGRPIAERRR"
                }
              ]
            },
            {
              "num": 2,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|13485",
                  "accession": "13485",
                  "title": "TRIAE_CS42_1BL_TGACv1_031607_AA0117040.3   gene=TRIAE_CS42_1BL_TGACv1_031607_AA0117040   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 500,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 28.4906,
                  "score": 62,
                  "evalue": 1.46958,
                  "identity": 19,
                  "positive": 25,
                  "query_from": 4,
                  "query_to": 53,
                  "hit_from": 301,
                  "hit_to": 348,
                  "align_len": 50,
                  "gaps": 2,
                  "qseq": "DPRRRRRRTSPATGSSTTTTGSSASDLPVPGHPNLASSLGAQRDGHGRPI",
                  "hseq": "DERKIRIRPLPAGAADRTVATAAALDLPLES--GLASSYRGGVDSEGRPV",
                  "midline": "D R+ R R  PA  +  T   ++A DLP+     LASS     D  GRP+"
                }
              ]
            },
            {
              "num": 3,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|71187",
                  "accession": "71187",
                  "title": "TRIAE_CS42_3AL_TGACv1_193702_AA0618020.1   gene=TRIAE_CS42_3AL_TGACv1_193702_AA0618020   biotype=protein_coding   confidence=Low"
                }
              ],
              "len": 180,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 27.7202,
                  "score": 60,
                  "evalue": 2.35238,
                  "identity": 12,
                  "positive": 16,
                  "query_from": 36,
                  "query_to": 58,
                  "hit_from": 106,
                  "hit_to": 128,
                  "align_len": 23,
                  "gaps": 0,
                  "qseq": "PNLASSLGAQRDGHGRPIAERRR",
                  "hseq": "PSVSSQDETSRDGHGRPMPEHRR",
                  "midline": "P+++S     RDGHGRP+ E RR"
                }
              ]
            },
            {
              "num": 4,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|154126",
                  "accession": "154126",
                  "title": "TRIAE_CS42_5BL_TGACv1_405499_AA1328790.1   gene=TRIAE_CS42_5BL_TGACv1_405499_AA1328790   biotype=protein_coding   confidence=Low"
                }
              ],
              "len": 310,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 27.7202,
                  "score": 60,
                  "evalue": 2.66381,
                  "identity": 12,
                  "positive": 16,
                  "query_from": 36,
                  "query_to": 58,
                  "hit_from": 175,
                  "hit_to": 197,
                  "align_len": 23,
                  "gaps": 0,
                  "qseq": "PNLASSLGAQRDGHGRPIAERRR",
                  "hseq": "PSVSSQDVTSRDGHGRPMPEHRR",
                  "midline": "P+++S     RDGHGRP+ E RR"
                }
              ]
            },
            {
              "num": 5,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|73272",
                  "accession": "73272",
                  "title": "TRIAE_CS42_3AL_TGACv1_194560_AA0635830.2   gene=TRIAE_CS42_3AL_TGACv1_194560_AA0635830   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 696,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.9498,
                  "score": 58,
                  "evalue": 4.65126,
                  "identity": 18,
                  "positive": 24,
                  "query_from": 1,
                  "query_to": 52,
                  "hit_from": 112,
                  "hit_to": 157,
                  "align_len": 52,
                  "gaps": 6,
                  "qseq": "MASDPRRRRRRTSPATGSSTTTTGSSASDLPVPGHPNLASSLGAQRDGHGRP",
                  "hseq": "VLSDPAKR------ASFDSSNTAGCSVKQMAVGNCSNLASSSKTKRTDHCFP",
                  "midline": "+ SDP +R      A+  S+ T G S   + V    NLASS   +R  H  P"
                }
              ]
            },
            {
              "num": 6,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|54003",
                  "accession": "54003",
                  "title": "TRIAE_CS42_2BS_TGACv1_146627_AA0469650.6   gene=TRIAE_CS42_2BS_TGACv1_146627_AA0469650   biotype=protein_coding   confidence=Low"
                }
              ],
              "len": 1057,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.5646,
                  "score": 57,
                  "evalue": 5.89578,
                  "identity": 17,
                  "positive": 32,
                  "query_from": 3,
                  "query_to": 57,
                  "hit_from": 860,
                  "hit_to": 917,
                  "align_len": 59,
                  "gaps": 5,
                  "qseq": "SDPRRRRRRTSPATGSSTTTTGSSASDLPVPG----HPNLASSLGAQRDGHGRPIAERR",
                  "hseq": "SSPQQHAAKNSPSTSGSKSASTNPYSSMPMPSILGQQPNMAHSGGKQQS-HGPSLKQQQ",
                  "midline": "S P++   + SP+T  S + + +  S +P+P      PN+A S G Q+  HG  + +++"
                }
              ]
            },
            {
              "num": 7,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|54002",
                  "accession": "54002",
                  "title": "TRIAE_CS42_2BS_TGACv1_146627_AA0469650.5   gene=TRIAE_CS42_2BS_TGACv1_146627_AA0469650   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 1182,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.5646,
                  "score": 57,
                  "evalue": 7.20645,
                  "identity": 17,
                  "positive": 32,
                  "query_from": 3,
                  "query_to": 57,
                  "hit_from": 985,
                  "hit_to": 1042,
                  "align_len": 59,
                  "gaps": 5,
                  "qseq": "SDPRRRRRRTSPATGSSTTTTGSSASDLPVPG----HPNLASSLGAQRDGHGRPIAERR",
                  "hseq": "SSPQQHAAKNSPSTSGSKSASTNPYSSMPMPSILGQQPNMAHSGGKQQS-HGPSLKQQQ",
                  "midline": "S P++   + SP+T  S + + +  S +P+P      PN+A S G Q+  HG  + +++"
                }
              ]
            },
            {
              "num": 8,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|234446",
                  "accession": "234446",
                  "title": "TRIAE_CS42_7DS_TGACv1_622080_AA2032400.1   gene=TRIAE_CS42_7DS_TGACv1_622080_AA2032400   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 863,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 8.49026,
                  "identity": 16,
                  "positive": 20,
                  "query_from": 9,
                  "query_to": 46,
                  "hit_from": 38,
                  "hit_to": 74,
                  "align_len": 38,
                  "gaps": 1,
                  "qseq": "RRRTSPATGSSTTTTGSSASDLPVPGHPNLASSLGAQR",
                  "hseq": "RRRASPAAKGKSPATNARLQ-MPVAGQVELESSLGIKR",
                  "midline": "RRR SPA    +  T +    +PV G   L SSLG +R"
                }
              ]
            },
            {
              "num": 9,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|234447",
                  "accession": "234447",
                  "title": "TRIAE_CS42_7DS_TGACv1_622080_AA2032400.2   gene=TRIAE_CS42_7DS_TGACv1_622080_AA2032400   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 891,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 8.49552,
                  "identity": 16,
                  "positive": 20,
                  "query_from": 9,
                  "query_to": 46,
                  "hit_from": 38,
                  "hit_to": 74,
                  "align_len": 38,
                  "gaps": 1,
                  "qseq": "RRRTSPATGSSTTTTGSSASDLPVPGHPNLASSLGAQR",
                  "hseq": "RRRASPAAKGKSPATNARLQ-MPVAGQVELESSLGIKR",
                  "midline": "RRR SPA    +  T +    +PV G   L SSLG +R"
                }
              ]
            },
            {
              "num": 10,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|181553",
                  "accession": "181553",
                  "title": "TRIAE_CS42_6AS_TGACv1_486703_AA1564240.3   gene=TRIAE_CS42_6AS_TGACv1_486703_AA1564240   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 941,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 8.76199,
                  "identity": 13,
                  "positive": 18,
                  "query_from": 5,
                  "query_to": 36,
                  "hit_from": 879,
                  "hit_to": 913,
                  "align_len": 35,
                  "gaps": 3,
                  "qseq": "PRRRRRRTSPATGSSTTTT---GSSASDLPVPGHP",
                  "hseq": "PKNKERRSMSLPGSSTPTTPMSSHSSLDMERPARP",
                  "midline": "P+ + RR+    GSST TT     S+ D+  P  P"
                }
              ]
            },
            {
              "num": 11,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|181552",
                  "accession": "181552",
                  "title": "TRIAE_CS42_6AS_TGACv1_486703_AA1564240.2   gene=TRIAE_CS42_6AS_TGACv1_486703_AA1564240   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 923,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 9.0245,
                  "identity": 13,
                  "positive": 18,
                  "query_from": 5,
                  "query_to": 36,
                  "hit_from": 861,
                  "hit_to": 895,
                  "align_len": 35,
                  "gaps": 3,
                  "qseq": "PRRRRRRTSPATGSSTTTT---GSSASDLPVPGHP",
                  "hseq": "PKNKERRSMSLPGSSTPTTPMSSHSSLDMERPARP",
                  "midline": "P+ + RR+    GSST TT     S+ D+  P  P"
                }
              ]
            },
            {
              "num": 12,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|181551",
                  "accession": "181551",
                  "title": "TRIAE_CS42_6AS_TGACv1_486703_AA1564240.1   gene=TRIAE_CS42_6AS_TGACv1_486703_AA1564240   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 924,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 9.11499,
                  "identity": 13,
                  "positive": 18,
                  "query_from": 5,
                  "query_to": 36,
                  "hit_from": 879,
                  "hit_to": 913,
                  "align_len": 35,
                  "gaps": 3,
                  "qseq": "PRRRRRRTSPATGSSTTTT---GSSASDLPVPGHP",
                  "hseq": "PKNKERRSMSLPGSSTPTTPMSSHSSLDMERPARP",
                  "midline": "P+ + RR+    GSST TT     S+ D+  P  P"
                }
              ]
            },
            {
              "num": 13,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|181555",
                  "accession": "181555",
                  "title": "TRIAE_CS42_6AS_TGACv1_486703_AA1564240.5   gene=TRIAE_CS42_6AS_TGACv1_486703_AA1564240   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 774,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 9.35788,
                  "identity": 13,
                  "positive": 18,
                  "query_from": 5,
                  "query_to": 36,
                  "hit_from": 729,
                  "hit_to": 763,
                  "align_len": 35,
                  "gaps": 3,
                  "qseq": "PRRRRRRTSPATGSSTTTT---GSSASDLPVPGHP",
                  "hseq": "PKNKERRSMSLPGSSTPTTPMSSHSSLDMERPARP",
                  "midline": "P+ + RR+    GSST TT     S+ D+  P  P"
                }
              ]
            },
            {
              "num": 14,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|181554",
                  "accession": "181554",
                  "title": "TRIAE_CS42_6AS_TGACv1_486703_AA1564240.4   gene=TRIAE_CS42_6AS_TGACv1_486703_AA1564240   biotype=protein_coding   confidence=High"
                }
              ],
              "len": 906,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 9.38797,
                  "identity": 13,
                  "positive": 18,
                  "query_from": 5,
                  "query_to": 36,
                  "hit_from": 861,
                  "hit_to": 895,
                  "align_len": 35,
                  "gaps": 3,
                  "qseq": "PRRRRRRTSPATGSSTTTT---GSSASDLPVPGHP",
                  "hseq": "PKNKERRSMSLPGSSTPTTPMSSHSSLDMERPARP",
                  "midline": "P+ + RR+    GSST TT     S+ D+  P  P"
                }
              ]
            },
            {
              "num": 15,
              "description": [
                {
                  "id": "gnl|BL_ORD_ID|133310",
                  "accession": "133310",
                  "title": "TRIAE_CS42_4DL_TGACv1_345080_AA1151470.1   gene=TRIAE_CS42_4DL_TGACv1_345080_AA1151470   biotype=protein_coding   confidence=Low"
                }
              ],
              "len": 390,
              "hsps": [
                {
                  "num": 1,
                  "bit_score": 26.1794,
                  "score": 56,
                  "evalue": 9.62175,
                  "identity": 13,
                  "positive": 16,
                  "query_from": 26,
                  "query_to": 55,
                  "hit_from": 325,
                  "hit_to": 354,
                  "align_len": 30,
                  "gaps": 0,
                  "qseq": "SASDLPVPGHPNLASSLGAQRDGHGRPIAE",
                  "hseq": "NMDDAVVVGHPNKHSTLLAITDGCGEDLKE",
                  "midline": "+  D  V GHPN  S+L A  DG G  + E"
                }
              ]
            }
          ],
          "stat": {
            "db_num": 249547,
            "db_len": 96882341,
            "hsp_len": 31,
            "eff_space": 2406952368,
            "kappa": 0.041,
            "lambda": 0.267,
            "entropy": 0.14
          }
        }
      }
    }
  }
]
}


{
	"@context": {
		"reference_genome": {
			"@id": "http://www.sequenceontology.org/browser/current_svn/term/SO:0001505",
			"@type": "@id"
		}

		"scaffold": {
			"@id": "http://www.sequenceontology.org/browser/current_svn/term/SO:0000148",
			"@type": "@id"
		}

		"contig": {
      "@id": "http://www.sequenceontology.org/miso/current_svn/term/SO:0000149",
      "@type": "@id"
    },

    "quality_value": {
      "@id": "http://www.sequenceontology.org/miso/current_svn/term/SO:0001686",
      "@type": "@id"
    },

    "score": {
      "@id": "http://www.sequenceontology.org/miso/current_svn/term/SO:0001685",
      "@type": "@id"
    },

		"contained_by": {
			"@id": "http://www.sequenceontology.org/browser/current_svn/term/contained_by",
			"@type": "@id"
		},
    "faldo": "http://biohackathon.org/resource/faldo"
	}


	"database": {
		"@type": "reference_genome",
		"name": "/home/billy/Applications/grassroots-0/grassroots/extras/blast/databases/Triticum_aestivum_CS42_TGACv1_scaffold.annotation.gff3.cds.fa"
	}


	"query": {
		"sequence": {
			"@type": "contig",
			"value": "GATCAAGTTGCCCCGCCTCCGATCTACCCGTTCCCGGCCACCCCAACCTCGCCTCGTCATTGGGCGCGCA",
		}
	}

	"hit": {
		"sequence": {
			"@type": "contig",
			"value": "GATCAAGTTGCCCCGCCTCCGATCTACCCGTTCCCGGCCACCCCAACCTCGCCTCGTCATTGGGCGCGCA"
		}
		"bit_score": {
			"@type": "score",
			"value": 113.339
		},
		"score": {
			"@type": "score",
			"value": 140
		},
		"bit_score": {
			"@type": "score",
			"evalue": 6.2069e-25
		}

		"faldo:location": {
			"@type": "faldo:Region",
			"faldo:begin": {
				"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
				"faldo:position": "99"
			},
			"faldo:end": {
				"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
				"faldo:position": "168"
			}
			"contained_by": {
				"scaffold": {
					"title": {
						"@context": "http://schema.org/Thing",
						"name": "TRIAE_CS42_1AL_TGACv1_000001_AA0000020.1"
					}
				}
			}
		}
	}
}
 */


/*
 "query": {
	"sequence": {
		"@type": "contig",
		"value": "GATCAAGTTGCCCCGCCTCCGATCTACCCGTTCCCGGCCACCCCAACCTCGCCTCGTCATTGGGCGCGCA",
	}
 */




static bool AddSequence (json_t *root_p, const char *key_s, const char *query_sequence_s)
{
	bool success_flag = false;

	if (json_object_set_new (root_p, key_s, json_string (query_sequence_s)) == 0)
		{
			success_flag = true;
		}

	return success_flag;
}

/*
"hit": {
		"hit_sequence": "GATCAAGTTGCCCCGCCTCCGATCTACCCGTTCCCGGCCACCCCAACCTCGCCTCGTCATTGGGCGCGCA",
		"bit_score": 113.339,
		"score": 140,
		"evalue": 6.2069e-25,
		"faldo:location": {
			"@type": "faldo:Region",
			"faldo:begin": {
				"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
				"faldo:position": "99"
			},
			"faldo:end": {
				"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
				"faldo:position": "168"
			}
			"contained_by": {
				"scaffold": {
					"title": {
						"@context": "http://schema.org/Thing",
						"name": "TRIAE_CS42_1AL_TGACv1_000001_AA0000020.1"
					}
				}
			}
		}
 */
static bool AddHitDetails (json_t *marked_up_result_p, const json_t *blast_hit_p)
{
	bool success_flag = false;
	const json_t *hsps_p = json_object_get (blast_hit_p, "hsps");

	if (hsps_p)
		{
			json_t *marked_up_hsps_p = json_array ();

			if (marked_up_hsps_p)
				{
					if (json_object_set_new (marked_up_result_p, "hit_details", marked_up_hsps_p) == 0)
						{

							if (json_is_array (hsps_p))
								{
									size_t i;
									json_t *hsp_p;

									json_array_foreach (hsps_p, i, hsp_p)
									{
										json_t *marked_up_hit_p = json_object ();

										if (marked_up_hit_p)
											{
												if (AddHsp (marked_up_hit_p, hsp_p))
													{
														if (json_array_append_new (marked_up_hsps_p, marked_up_hit_p) == 0)
															{

															}
														else
															{


															}
													}

											}
									}

									success_flag = (json_array_size (hsps_p) == json_array_size (marked_up_hsps_p));
								}		/* if (json_is_array (hsps_p)) */


						}		/* if (json_object_set_new (marked_up_result_p, "hit_details", marked_up_hsps_p) == 0) */
					else
						{
							json_decref (marked_up_hsps_p);
						}

				}		/* if (marked_up_hsps_p) */

		}		/* if (hsps_p) */


	return success_flag;
}



static bool AddHsp (json_t *marked_up_hit_p, const json_t *hsp_p)
{
	if (GetAndAddDoubleScoreValue (marked_up_hit_p, hsp_p, "bit_score", "bit_score"))
		{
			if (GetAndAddDoubleScoreValue (marked_up_hit_p, hsp_p, "evalue", "evalue"))
				{
					if (GetAndAddIntScoreValue (marked_up_hit_p, hsp_p, "score", "score"))
						{
							if (GetAndAddSequenceValue (marked_up_hit_p, hsp_p, "hseq", "hit_sequence"))
								{
									if (GetAndAddHitLocation (marked_up_hit_p, hsp_p, "hit_from", "hit_to", "hit_strand", "hit_location"))
										{
											if (GetAndAddHitLocation (marked_up_hit_p, hsp_p, "query_from", "query_to", "query_strand", "query_location"))
												{
													return true;
												}		/* if (GetAndAddHitLocation (marked_up_hit_p, hsp_p, "query_from", "query_to", "query_strand", "query_location")) */

										}		/* if (GetAndAddHitLocation (marked_up_hit_p, hsp_p, "hit_from", "hit_to", "hit_strand", "hit_location")) */

								}		/* if (GetAndAddSequenceValue (marked_up_hit_p, hsps_p, "hseq", "sequence")) */

						}		/* if (GetAndAddIntScoreValue (marked_up_hit_p, hsps_p, "score", "score") */

				}		/* if (GetAndAddDoubleScoreValue (marked_up_hit_p, hsps_p, "evalue", "evalue")) */

		}		/* if (GetAndAddDoubleScoreValue (marked_up_hit_p, hsps_p, "bit_score", "bit_score")) */

	return false;
}


static bool GetAndAddHitLocation (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_from_key_s, const char *hsp_to_key_s, const char *strand_key_s, const char *child_key_s)
{
	bool success_flag = false;
	int32 from;

	if (GetJSONInteger (hsps_p, hsp_from_key_s, &from))
		{
			int32 to;

			if (GetJSONInteger (hsps_p, hsp_to_key_s, &to))
				{
					json_t *location_p = json_object ();


					/*
						"faldo:location": {
							"@type": "faldo:Region",
							"faldo:begin": {
								"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
								"faldo:position": "99"
							},
							"faldo:end": {
								"@type": [ "faldo:Position", "faldo:ExactPosition", "faldo:ForwardStrandPosition" ],
								"faldo:position": "168"
							}
						}
					 */

					if (location_p)
						{
							bool forward_strand_flag = true;
							const char *strand_s = GetJSONString (hsps_p, strand_key_s);

							if (strand_s)
								{
									if (strcmp (strand_s, "Plus") == 0)
										{
											forward_strand_flag = true;
										}
									else if (strcmp (strand_s, "Minus") == 0)
										{
											forward_strand_flag = false;
										}

									if (AddFaldoTerminus (location_p, "faldo:begin", from, forward_strand_flag))
										{
											if (AddFaldoTerminus (location_p, "faldo:end", to, forward_strand_flag))
												{
													if (json_object_set_new (location_p, "@type", json_string ("faldo:Region")) == 0)
														{
															if (json_object_set_new (marked_up_result_p, child_key_s, location_p) == 0)
																{
																	return true;
																}		/* if (json_object_set_new (marked_up_result_p, child_key_s, location_p) == 0) */

														}		/* if (json_object_set_new (location_p, "@type", json_string ("faldo:Region")) == 0) */

												}		/* if (AddFaldoTerminus (location_p, "faldo:end", to, forward_strand_flag)) */

										}		/* if (AddFaldoTerminus (location_p, "faldo:begin", from, forward_strand_flag)) */
								}

							json_decref (location_p);
						}		/* if (location_p) */

				}		/* if (GetJSONInteger (hsps_p, hsp_to_key_s, &to)) */

		}		/* if (GetJSONInteger (hsps_p, hsp_from_key_s, &from)) */

	return success_flag;
}


static bool AddFaldoTerminus (json_t *parent_json_p, const char *child_key_s, const int32 position, const bool forward_strand_flag)
{
	json_t *faldo_p = json_object ();

	if (faldo_p)
		{
			json_t *type_array_p = json_array ();

			if (type_array_p)
				{
					if (json_object_set_new (faldo_p, "@type", type_array_p) == 0)
						{
							if (json_array_append_new (type_array_p, json_string ("faldo:Position")) == 0)
								{
									if (json_array_append_new (type_array_p, json_string ("faldo:ExactPosition")) == 0)
										{
											const char *strand_s = forward_strand_flag ? "faldo:ForwardStrandPosition" : "faldo:ReverseStrandPosition";

											if (json_array_append_new (type_array_p, json_string (strand_s)) == 0)
												{
													if (json_object_set_new (faldo_p, "faldo:position", json_integer (position)) == 0)
														{
															if (json_object_set_new (parent_json_p, child_key_s, faldo_p) == 0)
																{
																	return true;
																}

														}		/* if (json_object_set_new (faldo_p, "faldo:position", json_integer (position)) */

												}		/* if (json_array_append_new (type_array_p, json_string (strand_s)) == 0) */

										}		/* if (json_array_append_new (type_array_p, json_string ("faldo:Position")) == 0) */

								}		/* if (json_array_append_new (type_array_p, json_string ("faldo:Position")) == 0) */


						}		/* if (json_object_set_new (faldo_p, "@type", type_array_p) == 0) */
					else
						{
							json_decref (type_array_p);
						}
				}		/* if (type_array_p) */

			json_decref (faldo_p);
		}		/* if (faldo_p) */

	return false;
}





static bool GetAndAddSequenceValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *sequence_key_s)
{
	bool success_flag = false;
	const char *sequence_value_s = GetJSONString (hsps_p, hsp_key_s);

	if (sequence_value_s)
		{
			if (AddSequence (marked_up_result_p, sequence_key_s, sequence_value_s))
				{
					success_flag = true;
				}
		}

	return success_flag;
}


static bool GetAndAddDoubleScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s)
{
	bool success_flag = false;
	double64 value;

	if (GetJSONReal (hsps_p, hsp_key_s, &value))
		{
			if (AddDoubleScoreValue (marked_up_result_p, marked_up_key_s, value))
				{
					success_flag = true;
				}
		}

	return success_flag;
}


static bool GetAndAddIntScoreValue (json_t *marked_up_result_p, const json_t *hsps_p, const char *hsp_key_s, const char *marked_up_key_s)
{
	bool success_flag = false;
	int32 value;

	if (GetJSONInteger (hsps_p, hsp_key_s, &value))
		{
			if (AddIntScoreValue (marked_up_result_p, marked_up_key_s, value))
				{
					success_flag = true;
				}
		}

	return success_flag;
}


static bool AddIntScoreValue (json_t *parent_p, const char *key_s, int score_value)
{
	bool success_flag = false;

	if (json_object_set_new (parent_p, key_s, json_integer (score_value)) == 0)
		{
			success_flag = true;
		}

	return success_flag;
}



static bool AddDoubleScoreValue (json_t *parent_p, const char *key_s, double64 score_value)
{
	bool success_flag = false;

	if (json_object_set_new (parent_p, key_s, json_real (score_value)) == 0)
		{
			success_flag = true;
		}

	return success_flag;
}


static bool GetAndAddDatabaseDetails (json_t *marked_up_result_p, const char *database_s)
{
	bool success_flag = false;

	if (json_object_set_new (marked_up_result_p, "database", json_string (database_s)) == 0)
		{
			success_flag = true;
		}

	return success_flag;
}



bool ProduceMarkedUpResult (BlastServiceJob *job_p, const json_t *blast_result_p)
{
	bool success_flag = false;
	json_t *mark_up_p = GetInitialisedProcessedRequest ();

	if (mark_up_p)
		{

		}		/* if (mark_up_p) */

	return success_flag;
}


static json_t *GetInitialisedProcessedRequest (void)
{
	/*
 "@context": {
    "database": {
      "@id": "http://www.sequenceontology.org/browser/current_svn/term/SO:0001505",
      "@type": "@id"
    }

    "scaffold": {
      "@id": "http://www.sequenceontology.org/browser/current_svn/term/SO:0000148",
      "@type": "@id"
    }

    "query_sequence": {
      "@id": "http://www.sequenceontology.org/miso/current_svn/term/SO:0000149",
      "@type": "@id"
    },

    "hit_sequence": {
      "@id": "http://www.sequenceontology.org/miso/current_svn/term/SO:0000149",
      "@type": "@id"
    },


    "bit_score": {
      "@id": "http://www.sequenceontology.org/miso/current_svn/term/SO:0001686",
      "@type": "@id"
    },

    "evalue": {
      "@id": "http://www.sequenceontology.org/miso/current_svn/term/SO:0001686",
      "@type": "@id"
    },

    "contained_by": {
      "@id": "http://www.sequenceontology.org/browser/current_svn/term/contained_by",
      "@type": "@id"
    },

    "faldo": "http://biohackathon.org/resource/faldo"
  }
	 */

	json_t *root_p = json_object ();

	if (root_p)
		{
			json_t *context_p = json_object ();

			if (context_p)
				{
					if (json_object_set_new (root_p, "@context", context_p) == 0)
						{
							if (AddTerm (context_p, "database", "http://www.sequenceontology.org/browser/current_svn/term/SO:0001505"))
								{
									if (AddTerm (context_p, "scaffold", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000148"))
										{
											if (AddTerm (context_p, "query_sequence", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000149"))
												{
													if (AddTerm (context_p, "hit_sequence", "http://www.sequenceontology.org/browser/current_svn/term/SO:0000149"))
														{
															if (AddTerm (context_p, "evalue", "http://www.sequenceontology.org/browser/current_svn/term/SO:0001686"))
																{
																	if (AddTerm (context_p, "bit_score", "http://www.sequenceontology.org/browser/current_svn/term/SO:0001685"))
																		{
																			if (AddTerm (context_p, "contained_by", "http://www.sequenceontology.org/browser/current_svn/term/contained_by"))
																				{
																					if (json_object_set_new (context_p, "faldo", json_string ("http://biohackathon.org/resource/faldo")) == 0)
																						{
																							return root_p;
																						}
																				}
																		}
																}
														}
												}
										}
								}

						}		/* if (json_object_set_new (root_p, "@context", context_p) == 0) */
					else
						{
							json_decref (context_p);
						}

				}		/* if (context_p) */

			json_decref (root_p);
		}		/* if (root_p) */

	return NULL;
}


static bool AddTerm (json_t *root_p, const char *key_s, const char *term_s)
{
	json_t *term_p = json_object ();

	if (term_p)
		{
			if (json_object_set_new (term_p, "@context", json_string (term_s)) == 0)
				{
					if (json_object_set_new (term_p, "@type", json_string ("@id")) == 0)
						{
							if (json_object_set_new (root_p, key_s, term_p) == 0)
								{
									return true;
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json object to root object with context \"%s\" with key \"%s", term_s, key_s);
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json object type for context \"%s\" with key \"%s", term_s, key_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add json object context \"%s\" with key \"%s", term_s, key_s);
				}

			json_decref (term_p);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create json object for context \"%s\" with key \"%s", term_s, key_s);
		}


	return false;
}



static bool GetAndAddScaffoldsFromHit (const json_t *hit_p, json_t *mark_up_p)
{
	bool success_flag = false;

	LinkedList *scaffolds_p = GetScaffoldsFromHit (hit_p);

	if (scaffolds_p)
		{
			json_t *scaffolds_array_p = json_array ();

			if (scaffolds_array_p)
				{
					if (json_object_set_new (mark_up_p, "scaffolds", scaffolds_array_p) == 0)
						{
							StringListNode *node_p = (StringListNode *) (scaffolds_p -> ll_head_p);

							success_flag = true;

							while (node_p && success_flag)
								{
									json_t *scaffold_p = json_object ();

									if (scaffold_p)
										{
											if (json_object_set_new (scaffold_p, "scaffold", json_string (node_p -> sln_string_s)) == 0)
												{
													if (json_array_append_new (scaffolds_array_p, scaffold_p) != 0)
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add scaffold object to scaffolds array for \"%s\"", node_p -> sln_string_s);
															success_flag = false;
														}
												}
											else
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add scaffold \"%s\" to scaffold object", node_p -> sln_string_s);
													success_flag = false;
												}

											if (!success_flag)
												{
													json_decref (scaffold_p);
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create JSON scaffold object");
											success_flag = false;
										}

									node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
								}		/* while (node_p) */

						}		/* if (json_object_set_new (mark_up_p, "scaffolds", scaffolds_array_p) == 0) */

				}		/* if (scaffolds_array_p) */

		}		/* if (scaffolds_p) */

	return success_flag;
}


static bool MarkUpHit (const json_t *hit_p, json_t *mark_up_p, const char *database_s)
{
	bool success_flag = true;

	if (GetAndAddDatabaseDetails (mark_up_p, database_s))
		{
			if (GetAndAddScaffoldsFromHit (hit_p, mark_up_p))
				{
					if (AddHitDetails (mark_up_p, hit_p))
						{
						}

				}		/* if (GetAndAddScaffoldsFromHit (hit_p, mark_up_p)) */
		}

	return success_flag;
}



static LinkedList *GetScaffoldsFromHit (const json_t *hit_p)
{
	LinkedList *scaffolds_p = AllocateStringLinkedList ();

	if (scaffolds_p)
		{
			json_t *description_p = json_object_get (hit_p, "description");

			if (description_p)
				{
					if (json_is_array (description_p))
						{
							size_t k;
							json_t *item_p;

							json_array_foreach (description_p, k, item_p)
							{
								json_t *data_p = json_object ();

								if (data_p)
									{
										const char *full_title_s = GetJSONString (item_p, "title");

										if (full_title_s)
											{
												/*
												 * There may be more on this line than just the scaffold name
												 * so lets get up until the first space or |
												 */
												const char *id_end_p = strpbrk (full_title_s, " |");
												const uint32 size  = id_end_p ? id_end_p - full_title_s : 0;
												char *scaffold_s = CopyToNewString (full_title_s, size, false);

												if (scaffold_s)
													{
														StringListNode *node_p = AllocateStringListNode (scaffold_s, MF_SHALLOW_COPY);

														if (node_p)
															{
																LinkedListAddTail (scaffolds_p, (ListItem *) node_p);
															}		/* if (node_p) */

													}		/* if (scaffold_s) */

											}		/* if (full_title_s) */

									}		/* if (data_p) */
							}
						}
				}

		}		/* if (scaffolds_p) */

	return scaffolds_p;
}



