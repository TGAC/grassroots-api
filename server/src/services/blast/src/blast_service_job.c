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




static const char * const BSJ_TOOL_S = "tool";
static const char * const BSJ_FACTORY_S = "factory";
static const char * const BSJ_JOB_S = "job";



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
	OperationStatus status = tool_p -> GetStatus ();

	return true;
}




json_t *GetValueFromBlastServiceJobOutput (struct Service *service_p, struct ServiceJob *job_p, LinkedService *linked_service_p)
{
	json_t *results_p = NULL;
	char *raw_result_s = NULL;

	/*
	 * Get the result. Ideally we'd like to get this in a format that we can parse, so to begin with we'll use the single json format
	 * available in blast 2.3+
	 */
	raw_result_s = GetBlastResultByUUID ((BlastServiceData *) (service_p -> se_data_p), job_p -> sj_id, BOF_SINGLE_FILE_JSON_BLAST);

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
									json_t *output_p;

									json_array_foreach (root_p, i, output_p)
										{
											json_t *report_p = json_object_get (output_p, "report");

											if (report_p)
												{
													const char *database_s = NULL;
													MappedParameterNode *mapped_param_node_p = (MappedParameterNode *) (linked_service_p -> ls_mapped_params_p -> ll_head_p);


													/*
													 * Now we've got to the report we can see what values
													 * the LinkedService is after.
													 */
													MappedParameterNode *mapped_param_node_p = (MappedParameterNode *) (linked_service_p -> ls_mapped_params_p -> ll_head_p);

													while (mapped_param_node_p)
														{
															MappedParameter *mapped_param_p = mapped_param_node_p -> mpn_mapped_param_p;

															if (strcmp (mapped_param_p -> mp_input_param_s, "database") == 0)
																{
																	json_t *db_p = GetCompoundJSONObject (report_p, "search_target.db");

																	if (db_p)
																		{
																			if (json_is_string (db_p))
																				{
																					database_s = json_string_value (db_p);
																				}
																		}

																}		/* if (strcmp (mapped_param_p -> mp_input_param_s, "database") == 0) */
															else if (strcmp (mapped_param_p -> mp_input_param_s, "scaffold") == 0)
																{
																	json_t *hits_p = GetCompoundJSONObject (report_p, "results.search.hits");

																	if (hits_p)
																		{
																			if (json_is_array (hits_p))
																				{
																					size_t j;
																					json_t *hit_p;

																					json_array_foreach (hits_p, j, hit_p)
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
																													const char *full_title_s = GetJSONString (item_p, "title_p");

																													if (full_title_s)
																														{
																															/*
																															 * There may be more on this line than just the scaffold name
																															 * so lets get up until the first space or |
																															 */

																															const char *id_end_p = strpbrk (full_title_s, " |");

																															if (id_end_p)
																																{

																																}
																															else
																																{
																																	/* just get the whole string */
																																}

																														}
																												}

																										}
																								}

																						}

																				}		/* if (json_is_array (hits_p)) */

																		}		/* if (hits_p) */

																}		/* else if (strcmp (mapped_param_p -> mp_input_param_s, "scaffold") == 0) */
															else
																{
																	PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Unknown MappedParameter input key %s", mapped_param_p -> mp_input_param_s);
																}
															mapped_param_node_p = (MappedParameterNode *) (mapped_param_node_p -> mpn_node.ln_next_p);
														}		/* while (mapped_param_node_p) */

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

			ConvertUUIDToString (job_p -> sj_id, uuid_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast results for id \"%s\" in %d format", uuid_s, BOF_SINGLE_FILE_JSON_BLAST);
		}

	return results_p;
}

