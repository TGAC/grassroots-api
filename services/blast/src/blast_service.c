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

#define ALLOCATE_BLAST_SERVICE_CONSTANTS (1)
#include "blast_service.h"
#include "memory_allocations.h"


#include "string_utils.h"
#include "grassroots_config.h"
#include "temp_file.hpp"
#include "json_tools.h"


/*
 * STATIC PROTOTYPES
 */

static bool GetBlastServiceConfig (BlastServiceData *data_p);

static BlastServiceData *AllocateBlastServiceData (Service *blast_service_p);

static void FreeBlastServiceData (BlastServiceData *data_p);

static const char *GetBlastServiceName (Service *service_p);

static const char *GetBlastServiceDesciption (Service *service_p);

static ParameterSet *GetBlastServiceParameters (Service *service_p, Resource *resource_p, const json_t *json_p);

static void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p);

static bool IsFileForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseBlastService (Service *service_p);

static bool AddQuerySequenceParams (ParameterSet *param_set_p);

static bool AddGeneralAlgorithmParams (ParameterSet *param_set_p);

static bool AddScoringParams (ParameterSet *param_set_p);

static bool AddDatabaseParams (BlastServiceData *data_p, ParameterSet *param_set_p);

static json_t *GetBlastResultAsJSON (Service *service_p, const uuid_t service_id);

static OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t service_id);

static TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t job_id);


static uint32 GetNumberOfDatabases (const BlastServiceData *data_p);

static bool CleanUpBlastJob (ServiceJob *job_p);

static BlastTool *GetBlastToolForId (Service *service_p, const uuid_t service_id);

static char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id);

static char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s);

static ServiceJobSet *GetPreviousJobResults (const char *job_id_s, BlastServiceData *blast_data_p);


/*
 * API FUNCTIONS
 */
ServicesArray *GetServices (const json_t *config_p)
{
	Service *blast_service_p = (Service *) AllocMemory (sizeof (Service));

	if (blast_service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					ServiceData *data_p = (ServiceData *) AllocateBlastServiceData (blast_service_p);
					
					if (data_p)
						{
							InitialiseService (blast_service_p,
								GetBlastServiceName,
								GetBlastServiceDesciption,
								NULL,
								RunBlastService,
								IsFileForBlastService,
								GetBlastServiceParameters,
								ReleaseBlastServiceParameters,
								CloseBlastService,
								GetBlastResultAsJSON,
								GetBlastServiceStatus,
								true,
								false,
								data_p);

							if (GetBlastServiceConfig ((BlastServiceData *) data_p))
								{
									* (services_p -> sa_services_pp) = blast_service_p;

									return services_p;
								}

						}

					FreeServicesArray (services_p);
				}

			FreeService (blast_service_p);
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


static bool GetBlastServiceConfig (BlastServiceData *data_p)
{
	bool success_flag = false;
	const json_t *blast_config_p = data_p -> bsd_base_data.sd_config_p;

	if (blast_config_p)
		{
			json_t *value_p = json_object_get (blast_config_p, "working_directory");

			if (value_p)
				{
					if (json_is_string (value_p))
						{
							data_p -> bsd_working_dir_s = json_string_value (value_p);
							success_flag = true;
						}
				}

			if (success_flag)
				{
					value_p = json_object_get (blast_config_p, "databases");

					success_flag = false;

					if (value_p)
						{
							if (json_is_array (value_p))
								{
									size_t i = json_array_size (value_p);
									DatabaseInfo *databases_p = (DatabaseInfo *) AllocMemoryArray (i + 1, sizeof (DatabaseInfo));

									if (databases_p)
										{
											json_t *db_json_p;
											DatabaseInfo *db_p = databases_p;

											json_array_foreach (value_p, i, db_json_p)
												{
													json_t *name_p = json_object_get (db_json_p, "name");

													if (name_p && (json_is_string (name_p)))
														{
															json_t *description_p = json_object_get (db_json_p, "description");

															if (description_p && (json_is_string (description_p)))
																{
																	db_p -> di_name_s = json_string_value (name_p);
																	db_p -> di_description_s = json_string_value (description_p);

																	success_flag = true;
																	++ db_p;
																}		/* if (description_p) */

														}		/* if (name_p) */

												}		/* json_array_foreach (value_p, i, db_json_p) */

											if (success_flag)
												{
													data_p -> bsd_databases_p = databases_p;
												}
											else
												{
													FreeMemory (databases_p);
												}

										}		/* if (databases_p) */

								if (success_flag)
									{
										value_p = json_object_get (blast_config_p, "blast_tool");

										if (value_p)
											{
												if (json_is_string (value_p))
													{
														BlastTool :: SetBlastToolType (json_string_value (value_p));
													}
											}
									}

								}		/* if (json_is_array (value_p)) */

						}		/* if (value_p) */

				}		/* if (success_flag) */

		}		/* if (blast_config_p) */

	return success_flag;
}


static BlastServiceData *AllocateBlastServiceData (Service *blast_service_p)
{
	BlastServiceData *data_p = (BlastServiceData *) AllocMemory (sizeof (BlastServiceData));

	if (data_p)
		{
			data_p -> bsd_blast_tools_p = CreateBlastToolSet (blast_service_p);

			if (data_p -> bsd_blast_tools_p)
				{
					data_p -> bsd_working_dir_s = NULL;
					data_p -> bsd_databases_p = NULL;

					return data_p;
				}

			FreeMemory (data_p);
		}

	return NULL;
}


static void FreeBlastServiceData (BlastServiceData *data_p)
{
	FreeBlastToolSet (data_p -> bsd_blast_tools_p);
	FreeMemory (data_p);
}

 
static bool CloseBlastService (Service *service_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

	FreeBlastServiceData (blast_data_p);

	return true;
}
 
 
static const char *GetBlastServiceName (Service *service_p)
{
	return "Blast service";
}


static const char *GetBlastServiceDesciption (Service *service_p)
{
	return "A service to run the Blast program";
}


static uint32 GetNumberOfDatabases (const BlastServiceData *data_p)
{
	uint32 i = 0;
	const DatabaseInfo *db_p = data_p -> bsd_databases_p;

	if (db_p)
		{
			while (db_p -> di_name_s)
				{
					++ i;
					++ db_p;
				}
		}

	return i;
}


/*
 * The list of databases that can be searched
 */
static bool AddDatabaseParams (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = GetNumberOfDatabases (data_p);

	if (num_group_params)
		{
			Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
			Parameter **grouped_param_pp = grouped_params_pp;
			const DatabaseInfo *db_p = data_p -> bsd_databases_p;

			if (db_p)
				{
					uint8 a = 0;
					uint8 b = 0;

					def.st_boolean_value = true;
					success_flag = true;

					while ((db_p -> di_name_s) && success_flag)
						{
							/* try and get the local name of the database */
							const char *local_name_s = strrchr (db_p -> di_name_s, GetFileSeparatorChar ());
							uint32 tag = MAKE_TAG ('B', 'D', a, b);

							if (local_name_s)
								{
									++ local_name_s;
								}

							if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_BOOLEAN, false, db_p -> di_name_s, db_p -> di_description_s, db_p -> di_name_s, tag, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ALL, NULL)) != NULL)
								{
									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									if (b == 0xFF)
										{
											b = 0;
											++ a;
										}
									else
										{
											++ b;
										}

									++ db_p;
								}
							else
								{
									success_flag = false;
								}

						}		/* while (db_p && success_flag) */

				}		/* if (db_p) */



			if (success_flag)
				{
					const char * const group_name_s = "Available Databases";

					if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
							FreeMemory (grouped_params_pp);
						}
				}

		}		/* if (num_group_params) */

	return success_flag;
}


static bool AddQuerySequenceParams (ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 6;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;

	def.st_string_value_s = NULL;

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_READ, false, "input", "Input", "The input file to read", TAG_BLAST_INPUT_FILE, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
		{
			def.st_string_value_s = NULL;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "job_id", "Job ID", "The UUID for a Blast job that has been run previously", TAG_BLAST_JOB_ID, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					def.st_string_value_s = NULL;

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_FILE_TO_WRITE, false, "output", "Output", "The output file to write", TAG_BLAST_OUTPUT_FILE, NULL, def, NULL, NULL, PL_ALL, NULL)) != NULL)
						{
							def.st_string_value_s = NULL;

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}

							if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_STRING, false, "query_sequence", "Query Sequence(s)", "Query sequence(s) to be used for a BLAST search should be pasted in the 'Search' text area. "
								"It accepts a number of different types of input and automatically determines the format or the input."
								" To allow this feature there are certain conventions required with regard to the input of identifiers (e.g., accessions or gi's)", TAG_BLAST_INPUT_QUERY, NULL, def, NULL, NULL, PL_ALL, NULL))  != NULL)
								{
									const char *subrange_s = "Coordinates for a subrange of the query sequence. The BLAST search will apply only to the residues in the range. Valid sequence coordinates are from 1 to the sequence length. Set either From or To to 0 to ignore the range. The range includes the residue at the To coordinate.";

									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									def.st_ulong_value = 0;

									if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "from", "From", subrange_s, TAG_BLAST_SUBRANGE_FROM, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
										{
											def.st_ulong_value = 0;

											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;
													++ grouped_param_pp;
												}

											if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "to", "To", subrange_s, TAG_BLAST_SUBRANGE_TO, NULL, def, NULL, NULL, PL_INTERMEDIATE | PL_ADVANCED, NULL)) != NULL)
												{
													const char * const group_name_s = "Query Sequence Parameters";

													if (grouped_param_pp)
														{
															*grouped_param_pp = param_p;
															++ grouped_param_pp;
														}

													if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
															FreeMemory (grouped_params_pp);
														}

													success_flag = true;
												}
										}
								}
						}
				}
		}

	return success_flag;
}


static bool AddGeneralAlgorithmParams (ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 6;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;
	uint8 level = PL_INTERMEDIATE | PL_ALL;

	def.st_ulong_value = 5;

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "max_target_sequences", "Max target sequences", "Select the maximum number of aligned sequences to display (the actual number of alignments may be greater than this)." , TAG_BLAST_MAX_SEQUENCES, NULL, def, NULL, NULL, level, NULL)) != NULL)
		{
			def.st_boolean_value = true;

			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}


			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_BOOLEAN, false, "short_queries", "Short queries", "Automatically adjust parameters for short input sequences", TAG_BLAST_SHORT_QUERIES, NULL, def, NULL, NULL, level, NULL)) != NULL)
				{
					def.st_ulong_value = 10;

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "expect_threshold", "Expect threshold", "Expected number of chance matches in a random model" , TAG_BLAST_EXPECT_THRESHOLD, NULL, def, NULL, NULL, level, NULL)) != NULL)
						{
							def.st_ulong_value = 28;

							if (grouped_param_pp)
								{
									*grouped_param_pp = param_p;
									++ grouped_param_pp;
								}

							if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "word_size", "Word size", "Expected number of chance matches in a random model", TAG_BLAST_WORD_SIZE, NULL, def, NULL, NULL, level, NULL)) != NULL)
								{
									def.st_ulong_value = 0;

									if (grouped_param_pp)
										{
											*grouped_param_pp = param_p;
											++ grouped_param_pp;
										}

									if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "max_matches_in_a_query_range", "Max matches in a query range", "Limit the number of matches to a query range. This option is useful if many strong matches to one part of a query may prevent BLAST from presenting weaker matches to another part of the query", TAG_BLAST_MAX_RANGE_MATCHES, NULL, def, NULL, NULL, level, NULL)) != NULL)
										{
											/* default to xml */
											def.st_ulong_value = 5;

											if (grouped_param_pp)
												{
													*grouped_param_pp = param_p;
													++ grouped_param_pp;
												}

											if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_UNSIGNED_INT, false, "output_format", "Output format", "The output format for the results", TAG_BLAST_OUTPUT_FORMAT, NULL, def, NULL, NULL, level, NULL)) != NULL)
												{
													const char * const group_name_s = "General Algorithm Parameters";

													if (grouped_param_pp)
														{
															*grouped_param_pp = param_p;
															++ grouped_param_pp;
														}

													if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
														{
															PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
															FreeMemory (grouped_params_pp);
														}

													success_flag = true;
												}
										}
								}
						}
				}
		}

	return success_flag;
}


static bool AddScoringParams (ParameterSet *param_set_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	size_t num_group_params = 2;
	Parameter **grouped_params_pp = (Parameter **) AllocMemoryArray (num_group_params, sizeof (Parameter *));
	Parameter **grouped_param_pp = grouped_params_pp;
	const ParameterLevel level = PL_INTERMEDIATE | PL_ADVANCED;

	def.st_long_value = 2;

	if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_SIGNED_INT, false, "match", "Match", "Reward for a nucleotide match.", TAG_BLAST_MATCH_SCORE, NULL, def, NULL, NULL, level, NULL)) != NULL)
		{
			if (grouped_param_pp)
				{
					*grouped_param_pp = param_p;
					++ grouped_param_pp;
				}

			def.st_long_value = -3;

			if ((param_p = CreateAndAddParameterToParameterSet (param_set_p, PT_SIGNED_INT, false, "mismatch", "Mismatch", "Penalty for a nucleotide mismatch.", TAG_BLAST_MISMATCH_SCORE, NULL, def, NULL, NULL, level, NULL)) != NULL)
				{
					const char * const group_name_s = "Scoring Parameters";

					if (grouped_param_pp)
						{
							*grouped_param_pp = param_p;
							++ grouped_param_pp;
						}

					if (!AddParameterGroupToParameterSet (param_set_p, group_name_s, grouped_params_pp, num_group_params))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add %s grouping", group_name_s);
							FreeMemory (grouped_params_pp);
						}

					success_flag = true;
				}
		}


	return success_flag;
}


static ParameterSet *GetBlastServiceParameters (Service *service_p, Resource *resource_p, const json_t *config_p)
{
	ParameterSet *param_set_p = AllocateParameterSet ("Blast service parameters", "The parameters used for the Blast service");
	
	if (param_set_p)
		{
			BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

			if (AddQuerySequenceParams (param_set_p))
				{
					if (AddGeneralAlgorithmParams (param_set_p))
						{
							if (AddScoringParams (param_set_p))
								{
									if (AddDatabaseParams (blast_data_p, param_set_p))
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


static void ReleaseBlastServiceParameters (Service *service_p, ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static json_t *GetBlastResultAsJSON (Service *service_p, const uuid_t job_id)
{
	json_t *blast_result_json_p = NULL;
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	BlastTool *tool_p = blast_data_p -> bsd_blast_tools_p -> GetBlastTool (job_id);

	if (tool_p)
		{
			OperationStatus status = tool_p -> GetStatus ();
			const char * const name_s = tool_p -> GetName ();

			if (status == OS_SUCCEEDED)
				{
					const char *result_s = tool_p -> GetResults ();

					if (result_s)
						{
							json_t *result_json_p = json_string (result_s);

							if (result_json_p)
								{
									blast_result_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, name_s, result_json_p);

									if (!blast_result_json_p)
										{
											WipeJSON (result_json_p);
										}
								}

							tool_p -> ClearResults ();
						}
				}
			else
				{
					json_error_t error;
					blast_result_json_p = json_pack_ex (&error, 0, "{s:i}", SERVICE_STATUS_S, status);

					if (blast_result_json_p)
						{
							char *uuid_s = GetUUIDAsString (job_id);

							if (uuid_s)
								{
									if (json_object_set_new (blast_result_json_p, SERVICE_UUID_S, json_string (uuid_s)) != 0)
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add service id %s to blast result json", uuid_s);
										}

									FreeUUIDString (uuid_s);
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create idsdtring for blast result json", uuid_s);
								}
						}
				}

		}		/* if (tool_p) */


	return blast_result_json_p;
}


static TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t job_id)
{
	TempFile *input_file_p = NULL;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	/* Input query */
	if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_INPUT_QUERY, &value, true))
		{
			char *sequence_s = value.st_string_value_s;

			if (!IsStringEmpty (sequence_s))
				{
					char *uuid_s = GetUUIDAsString (job_id);

					if (uuid_s)
						{
							char *buffer_s = GetTempFilenameBuffer (working_directory_s, uuid_s, ".input");

							if (buffer_s)
								{
									input_file_p = TempFile :: GetTempFile (buffer_s, false);

									if (input_file_p)
										{
											bool success_flag = input_file_p -> Print (sequence_s);

											input_file_p -> Close ();

											if (!success_flag)
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast service failed to write to temp file \"%s\" for query \"%s\"", input_file_p -> GetFilename (), sequence_s);
													input_file_p = NULL;
												}
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast service failed to open temp file for query \"%s\"", sequence_s);
										}
								}		/* if (buffer_p) */
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast service failed to allocate temp file buffer for query \"%s\"", sequence_s);
								}

							FreeCopiedString (uuid_s);
						}		/* if (uuid_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid as string");
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast input query is empty");
				}
		}

	return input_file_p;
}


static ServiceJobSet *GetPreviousJobResults (const char *job_id_s, BlastServiceData *blast_data_p)
{
	ServiceJobSet *jobs_p = AllocateServiceJobSet (blast_data_p -> bsd_base_data.sd_service_p, 1, NULL);

	if (jobs_p)
		{
			uuid_t job_id;
			char *error_s = NULL;

			if (uuid_parse (job_id_s, job_id) == 0)
				{
					char *result_s = GetBlastResultByUUIDString (blast_data_p, job_id_s);

					if (result_s)
						{
							json_t *result_json_p = json_string (result_s);

							if (result_json_p)
								{
									json_t *blast_result_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, job_id_s, result_json_p);

									if (blast_result_json_p)
										{
											jobs_p -> sjs_jobs_p -> sj_result_p = blast_result_json_p;
										}
									else
										{
											error_s = ConcatenateVarargsStrings ("Failed to get full blast result as json \"%s\"", job_id_s);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get full blast result as json \"%s\"", job_id_s);

											WipeJSON (result_json_p);
										}
								}
							else
								{
									error_s = ConcatenateVarargsStrings ("Failed to get blast result as json \"%s\"", job_id_s);
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result as json \"%s\"", job_id_s);
								}

							FreeCopiedString (result_s);
						}		/* if (result_s) */
					else
						{
							error_s = ConcatenateVarargsStrings ("Failed to get blast result for \"%s\"", job_id_s);
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result for \"%s\"", job_id_s);
						}
				}		/* if (uuid_parse (param_value.st_string_value_s, job_id) == 0) */
			else
				{
					error_s = ConcatenateVarargsStrings ("Failed to convert \"%s\" to a valid uuid", job_id_s);
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to a valid uuid", job_id_s);
				}


			if (error_s)
				{
					json_error_t err;
					jobs_p -> sjs_jobs_p -> sj_errors_p = json_pack_ex (&err, 0, "{s:s}", JOB_ERRORS_S, error_s);

					if (! (jobs_p -> sjs_jobs_p -> sj_errors_p))
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create json error string for \"%s\"", job_id_s);
						}

					FreeCopiedString (error_s);
				}
		}

	return jobs_p;
}


static ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	SharedType param_value;

	memset (&param_value, 0, sizeof (SharedType));

	/* Are we retrieving a previously run job? */
	if ((GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_JOB_ID, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s)))
		{
			service_p -> se_jobs_p = GetPreviousJobResults (param_value.st_string_value_s, blast_data_p);

			if (!service_p -> se_jobs_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get ServiceJobSet for previously run blast job \"%s\"", param_value.st_string_value_s);
				}

		}		/* if ((GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_JOB_ID, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s))) */
	else
		{
			/* count how many jobs we a running */
			size_t num_jobs = 0;
			const DatabaseInfo *db_p = blast_data_p -> bsd_databases_p;
			bool all_flag = false;

			/* count the number of databases to search */
			if (db_p)
				{
					while (db_p -> di_name_s)
						{
							if (all_flag)
								{
									++ num_jobs;
								}
							else
								{
									Parameter *param_p = GetParameterFromParameterSetByName (param_set_p, db_p -> di_name_s);

									if (param_p)
										{
											if (param_p -> pa_current_value.st_boolean_value)
												{
													++ num_jobs;
												}
										}
								}

							++ db_p;
						}		/* while (db_p) */
				}

			if (num_jobs > 0)
				{
					service_p -> se_jobs_p = AllocateServiceJobSet (service_p, num_jobs, CleanUpBlastJob);

					if (service_p -> se_jobs_p)
						{
							ServiceJob *job_p = service_p -> se_jobs_p -> sjs_jobs_p;
							TempFile *input_p = GetInputTempFile (param_set_p, blast_data_p -> bsd_working_dir_s, job_p -> sj_id);
							const char *input_filename_s = NULL;

							if (input_p)
								{
									input_filename_s = input_p -> GetFilename ();
								}
							else
								{
									memset (&param_value, 0, sizeof (SharedType));

									/* try to get the input file */
									if (GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_INPUT_FILE, &param_value, true))
										{
											input_filename_s = param_value.st_string_value_s;
										}
								}

							if (input_filename_s)
								{
									size_t num_jobs_ran = 0;
									db_p = blast_data_p -> bsd_databases_p;

									while ((db_p -> di_name_s) && (num_jobs_ran < num_jobs))
										{
											bool run_flag = false;

											if (all_flag)
												{
													run_flag = true;
												}
											else
												{
													Parameter *param_p = GetParameterFromParameterSetByName (param_set_p, db_p -> di_name_s);

													if (param_p)
														{
															if (param_p -> pa_current_value.st_boolean_value)
																{
																	run_flag = true;
																}
														}
												}

											if (run_flag)
												{
													const char *db_name_s = NULL;
													const char *description_s = NULL;

													if (all_flag)
														{
															db_name_s = db_p -> di_name_s;
															description_s = db_p -> di_description_s;
														}
													else
														{
															Parameter *param_p = GetParameterFromParameterSetByName (param_set_p, db_p -> di_name_s);

															if (param_p)
																{
																	db_name_s = param_p -> pa_name_s;

																	if (param_p -> pa_description_s)
																		{
																			description_s = param_p -> pa_description_s;
																		}
																	else
																		{
																			description_s = db_p -> di_description_s;
																		}
																}
														}

													if (db_name_s)
														{
															BlastTool *tool_p = blast_data_p -> bsd_blast_tools_p -> GetNewBlastTool (job_p, db_name_s, blast_data_p -> bsd_working_dir_s);

															job_p -> sj_status = OS_FAILED_TO_START;

															if (tool_p)
																{
																	if (description_s)
																		{
																			SetServiceJobDescription (job_p, description_s);
																		}

																	if (tool_p -> ParseParameters (param_set_p, input_filename_s))
																		{
																			if (RunBlast (tool_p))
																				{
																					job_p -> sj_status = tool_p -> GetStatus ();
																					++ num_jobs_ran;
																					++ job_p;
																				}
																		}
																}
														}
												}		/* if (run_flag) */

											++ db_p;
										}		/* while (db_p && (num_jobs_ran < num_jobs)) */


								}		/* if (filename_s) */

							if (input_p)
								{
									TempFile :: DeleteTempFile (input_p);
								}

						}		/* if (service_p -> se_jobs_p) */

				}		/* if (num_jobs > 0) */

		}		/* if ((GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_JOB_ID, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s))) else */


	return service_p -> se_jobs_p;
}


static bool IsFileForBlastService (Service *service_p, Resource *resource_p, Handler *handler_p)
{
	bool interested_flag = false;
	const char *filename_s = resource_p -> re_value_s;
	
	
	/*
	 * @TODO
	 * We could check if the file is on a remote filesystem and if so
	 * make a full or partial local copy for analysis.
	 */
	
	/* 
	 * We can check on file extension and also the content of the file
	 * to determine if we want to blast this file.
	 */
	if (filename_s)
		{
			const char *extension_s = strstr (filename_s, ".");
			
			if (extension_s)
				{
					/* move past the . */
					++ extension_s;
					
					/* check that the file doesn't end with the . */
					if (*extension_s != '\0')
						{
							if (strcmp (extension_s, "fa") == 0)
								{
									interested_flag = true;
								}
								
						}		/* if (*extension_s != '\0') */
					
				}		/* if (extension_s) */
				
		}		/* if (filename_s) */
	
	
	return interested_flag;	
}



static BlastTool *GetBlastToolForId (Service *service_p, const uuid_t service_id)
{
	BlastTool *tool_p = NULL;
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);

	if (blast_data_p)
		{
			if (blast_data_p -> bsd_blast_tools_p)
				{
					tool_p = blast_data_p -> bsd_blast_tools_p -> GetBlastTool (service_id);
				}
		}

	return tool_p;
}


static OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t service_id)
{
	OperationStatus status = OS_ERROR;
	BlastTool *tool_p = GetBlastToolForId (service_p, service_id);

	if (tool_p)
		{
			status = tool_p -> GetStatus ();
		}

	return status;
}



static bool CleanUpBlastJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;
	BlastTool *tool_p = GetBlastToolForId (job_p -> sj_service_p, job_p -> sj_id);


	return cleaned_up_flag;
}


static char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id)
{
	char *result_s = NULL;
	char *job_id_s = GetUUIDAsString (job_id);

	if (job_id_s)
		{
			result_s = GetBlastResultByUUIDString (data_p, job_id_s);
			FreeCopiedString (job_id_s);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get job id as string");
		}

	return result_s;
}


static char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s)
{
	char *result_s = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			const char *job_output_filename_s = NULL;

			if (data_p -> bsd_working_dir_s)
				{
					char sep [2];

					*sep = GetFileSeparatorChar ();
					* (sep + 1) = '\0';

					if (AppendStringsToByteBuffer (buffer_p, data_p -> bsd_working_dir_s, sep, job_id_s, BS_OUTPUT_SUFFIX_S, NULL))
						{
							job_output_filename_s = GetByteBufferData (buffer_p);
						}		/* if (AppendStringsToByteBuffer (buffer_p, data_p -> bsd_working_dir_s, sep, job_id_s, NULL)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't create full path to job file \"%s\"", job_id_s);
						}
				}		/* if (data_p -> bsd_working_dir_s) */
			else
				{
					job_output_filename_s = job_id_s;
				}

			if (job_output_filename_s)
				{
					FILE *job_f = fopen (job_output_filename_s, "r");

					if (job_f)
						{
							result_s = GetFileContentsAsString (job_f);

							if (!result_s)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't get content of job file \"%s\"", job_output_filename_s);
								}

							if (fclose (job_f) != 0)
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Couldn't close job file \"%s\"", job_output_filename_s);
								}
						}		/* if (job_f) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't open job file \"%s\"", job_output_filename_s);
						}
				}		/* if (job_output_filename_s) */

			FreeByteBuffer (buffer_p);
		}		/* if (buffer_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Couldn't allocate ByteBuffer");
		}

	return result_s;
}


