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

#include "service_job_set_iterator.h"
#include "string_utils.h"
#include "grassroots_config.h"
#include "temp_file.hpp"
#include "json_tools.h"
#include "blast_formatter.h"
#include "blast_service_job.h"
#include "paired_blast_service.h"
#include "blast_service_params.h"

#include "servers_pool.h"
#include "remote_parameter_details.h"

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


static json_t *GetBlastResultAsJSON (Service *service_p, const uuid_t service_id);

static OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t service_id);

static TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t job_id);


static bool CleanUpBlastJob (ServiceJob *job_p);

static BlastTool *GetBlastToolForId (Service *service_p, const uuid_t service_id);

static char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id, const uint32 output_format_code);

static char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s, const uint32 output_format_code);

static ServiceJobSet *GetPreviousJobResults (LinkedList *ids_p, BlastServiceData *blast_data_p, const uint32 output_format_code);

static ServiceJobSet *CreateJobsForPreviousResults (ParameterSet *params_p, const char *ids_s, BlastServiceData *blast_data_p);

static void PrepareBlastServiceJobs (const DatabaseInfo *db_p, const bool all_flag, const ParameterSet * const param_set_p, ServiceJobSet *jobs_p, const char *working_directory_s);



/***************************************/




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
																 true,
																 data_p);

							if (GetBlastServiceConfig ((BlastServiceData *) data_p))
								{
									if (strcmp (BlastTool :: bt_tool_type_s, "drmaa") == 0)
										{
											blast_service_p -> se_synchronous_flag = false;
										}

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

			if (success_flag)
				{
					const char *formatter_type_s = GetJSONString (blast_config_p, "blast_formatter");

					if (formatter_type_s)
						{
							if (strcmp (formatter_type_s, "system") == 0)
								{
									const json_t *formatter_config_p = json_object_get (blast_config_p, "system_formatter_config");

									data_p -> bsd_formatter_p = SystemBlastFormatter :: Create (formatter_config_p);
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Unknown BlastFormatter type \"%s\"", formatter_type_s);
								}
						}
				}

		}		/* if (blast_config_p) */

	return success_flag;
}


static BlastServiceData *AllocateBlastServiceData (Service *blast_service_p)
{
	BlastServiceData *data_p = (BlastServiceData *) AllocMemory (sizeof (BlastServiceData));

	if (data_p)
		{
			data_p -> bsd_working_dir_s = NULL;
			data_p -> bsd_databases_p = NULL;
			data_p -> bsd_formatter_p = NULL;
		}

	return data_p;
}


static void FreeBlastServiceData (BlastServiceData *data_p)
{
	if (data_p -> bsd_databases_p)
		{
			FreeMemory (data_p -> bsd_databases_p);
		}

	if (data_p -> bsd_formatter_p)
		{
			delete (data_p -> bsd_formatter_p);
		}

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
									uint16 num_dbs = AddDatabaseParams (blast_data_p, param_set_p);

									num_dbs = AddPairedServiceParameters (service_p, param_set_p, num_dbs);

									if (num_dbs > 0)
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


static json_t *GetBlastResult (Service *service_p, BlastServiceJob *job_p)
{
	json_t *results_p = json_array ();

	if (results_p)
		{
			BlastTool *tool_p = job_p -> bsj_tool_p;
			OperationStatus status = tool_p -> GetStatus ();
			const char * const name_s = tool_p -> GetName ();
			BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (job_p -> bsj_job.sj_id, uuid_s);

			if (status == OS_SUCCEEDED)
				{
					char *result_s = tool_p -> GetResults (blast_data_p -> bsd_formatter_p);

					if (result_s)
						{
							json_t *result_json_p = json_string (result_s);

							if (result_json_p)
								{
									json_t *blast_result_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, uuid_s, result_json_p);

									if (blast_result_json_p)
										{
											if (json_array_append_new (results_p, blast_result_json_p) == 0)
												{
													ClearServiceJobResults (& (job_p -> bsj_job), true);
													job_p -> bsj_job.sj_result_p = results_p;
												}
											else
												{
													json_decref (blast_result_json_p);
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to append blast result for \"%s\"", uuid_s);
												}

										}		/* if (blast_result_json_p) */

									json_decref (result_json_p);
								}		/* if (result_json_p) */

							FreeCopiedString (result_s);
						}		/* if (result_s) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result for \"%s\"", uuid_s);
						}

				}		/* if (status == OS_SUCCEEDED) */
			else
				{
					json_error_t error;
					json_t *status_json_p = json_pack_ex (&error, 0, "{s:s,s:i}", SERVICE_UUID_S, uuid_s, SERVICE_STATUS_S, status);

					if (status_json_p)
						{
							if (json_array_append (results_p, status_json_p) != 0)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add status json result %s to json results array", uuid_s);
									json_decref (status_json_p);
								}		/* if (json_array_append (results_p, status_json_p) != 0) */
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result json for \"%s\", error at line %d col %d, text \"%s\"", uuid_s, error.line, error.column, error.text);
						}
				}

		}		/* if (results_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate json array to get blast results");
		}

	return results_p;
}


static json_t *GetBlastResultAsJSON (Service *service_p, const uuid_t job_id)
{
	json_t *results_p = NULL;
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	ConvertUUIDToString (job_id, uuid_s);

	if (service_p -> se_jobs_p)
		{
			BlastServiceJob *job_p = (BlastServiceJob *) GetServiceJobFromServiceJobSetById (service_p -> se_jobs_p, job_id);

			if (job_p)
				{
					results_p = GetBlastResult (service_p, job_p);
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get BlastServiceJob \"%s\"");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "No ServiceJobSet");
		}

	return results_p;
}


static TempFile *GetInputTempFile (const ParameterSet *params_p, const char *working_directory_s, const uuid_t id)
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
					input_file_p = TempFile :: GetTempFile (working_directory_s, id, BS_INPUT_SUFFIX_S);

					if (input_file_p)
						{
							bool success_flag = true;

							if (!input_file_p -> IsOpen ())
								{
									success_flag = input_file_p -> Open ("w");
								}

							if (success_flag)
								{
									success_flag = input_file_p -> Print (sequence_s);
									input_file_p -> Close ();
								}

							if (!success_flag)
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast service failed to write to temp file \"%s\" for query \"%s\"", input_file_p -> GetFilename (), sequence_s);
									delete input_file_p;
									input_file_p = NULL;
								}
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast service failed to open temp file for query \"%s\"", sequence_s);
						}



				}		/* if (!IsStringEmpty (sequence_s)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid as string");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Blast input query is empty");
		}

	return input_file_p;
}


static ServiceJobSet *GetPreviousJobResults (LinkedList *ids_p, BlastServiceData *blast_data_p, const uint32 output_format_code)
{
	char *error_s = NULL;
	Service *service_p = blast_data_p -> bsd_base_data.sd_service_p;
	ServiceJobSet *jobs_p = AllocateServiceJobSet (service_p);

	if (jobs_p)
		{
			ServiceJob *job_p = CreateAndAddServiceJobToServiceJobSet (jobs_p, "Previous Blast Results", NULL, NULL, NULL);

			if (job_p)
				{
					uuid_t job_id;
					StringListNode *node_p = (StringListNode *) (ids_p -> ll_head_p);
					uint32 num_successful_jobs = 0;
					json_t *results_p = json_array ();

					if (results_p)
						{
							while (node_p)
								{
									const char * const job_id_s = node_p -> sln_string_s;

									if (uuid_parse (job_id_s, job_id) == 0)
										{
											char *result_s = GetBlastResultByUUIDString (blast_data_p, job_id_s, output_format_code);
											job_p -> sj_status = OS_FAILED;

											if (result_s)
												{
													json_t *result_json_p = json_string (result_s);

													if (result_json_p)
														{
															json_t *blast_result_json_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, job_id_s, result_json_p);

															if (blast_result_json_p)
																{
																	if (json_array_append_new (results_p, blast_result_json_p) == 0)
																		{
																			++ num_successful_jobs;
																		}
																	else
																		{
																			error_s = ConcatenateVarargsStrings ("Failed to add blast result \"", job_id_s, "\" to json results array", NULL);
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add blast result \"%s\" to json results array", job_id_s);
																		}
																}
															else
																{
																	error_s = ConcatenateVarargsStrings ("Failed to get full blast result as json \"", job_id_s, "\"", NULL);
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get full blast result as json \"%s\"", job_id_s);
																}

															json_decref (result_json_p);
														}
													else
														{
															error_s = ConcatenateVarargsStrings ("Failed to get blast result as json \"", job_id_s, "\"", NULL);
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result as json \"%s\"", job_id_s);
														}

													FreeCopiedString (result_s);
												}		/* if (result_s) */
											else
												{
													error_s = ConcatenateVarargsStrings ("Failed to get blast result for \"", job_id_s, "\"", NULL);
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast result for \"%s\"", job_id_s);
												}
										}		/* if (uuid_parse (param_value.st_string_value_s, job_id) == 0) */
									else
										{
											error_s = ConcatenateVarargsStrings ("Failed to convert \"", job_id_s, "\" to a valid uuid", NULL);
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to a valid uuid", job_id_s);
										}

									if (error_s)
										{
											json_error_t err;
											job_p -> sj_errors_p = json_pack_ex (&err, 0, "{s:s}", JOB_ERRORS_S, error_s);

											if (! (job_p -> sj_errors_p))
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create json error string for \"%s\"", job_id_s);
												}

											FreeCopiedString (error_s);
										}		/* if (error_s) */


									node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
								}		/* while (node_p) */

							#if BLAST_SERVICE_DEBUG >= STM_LEVEL_FINE
							PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Num input jobs " UINT32_FMT " num successful json results " UINT32_FMT, ids_p -> ll_size, num_successful_jobs);
							#endif

							if (num_successful_jobs == ids_p -> ll_size)
								{
									job_p -> sj_status = OS_SUCCEEDED;
								}
							else
								{
									job_p -> sj_status = OS_PARTIALLY_SUCCEEDED;
								}

							job_p -> sj_result_p = results_p;


						}		/* if (results_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate json results arrays");
						}

				}		/* if (job_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate service job");
				}

		}		/* if (jobs_p) */

	return jobs_p;
}


static LinkedList *GetUUIDSList (const char *ids_s)
{
	LinkedList *ids_p = AllocateLinkedList (FreeStringListNode);

	if (ids_p)
		{
			bool loop_flag = true;
			const char *start_p = ids_s;
			const char *end_p = NULL;

			while (loop_flag)
				{
					/* scroll to the start of the token */
					while (isspace (*start_p))
						{
							++ start_p;
						}

					if (*start_p != '\0')
						{
							end_p = start_p;

							/* scroll to the end of the token */
							while ((isalnum (*end_p)) || (*end_p == '-'))
								{
									++ end_p;
								}

							/* Is the region the corrrect size for a uuid? */
							if (end_p - start_p == UUID_STRING_BUFFER_SIZE - 1)
								{
									StringListNode *node_p = NULL;
									char buffer_s [UUID_STRING_BUFFER_SIZE];

									memcpy (buffer_s, start_p, UUID_STRING_BUFFER_SIZE - 1);
									* (buffer_s + (UUID_STRING_BUFFER_SIZE - 1)) = '\0';

									node_p = AllocateStringListNode (buffer_s, MF_DEEP_COPY);

									if (node_p)
										{
											LinkedListAddTail (ids_p, (ListItem *) node_p);
										}
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add uuid \"%s\" to list", buffer_s);
										}

								}		/* if (end_p - start_p == UUID_STRING_BUFFER_SIZE - 1) */
							else
								{
									char *c_p =  (char *) end_p;
									char c = *c_p;

									*c_p = '\0';
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "token \"%s\" is " SIZET_FMT " bytes long, not %d", start_p, end_p - start_p, UUID_STRING_BUFFER_SIZE - 1);
									*c_p = c;
								}

							if (*end_p != '\0')
								{
									start_p = end_p + 1;
								}
							else
								{
									loop_flag = false;
								}

						}		/* if (*start_p != '\0') */
					else
						{
							loop_flag = false;
						}

				}		/* while (loop_flag) */

			if (ids_p -> ll_size == 0)
				{
					FreeLinkedList (ids_p);
					ids_p = NULL;
				}		/* if (ids_p -> ll_size == 0) */

		}		/* if (ids_p) */

	return ids_p;
}


static void PrepareBlastServiceJobs (const DatabaseInfo *db_p, const bool all_flag, const ParameterSet * const param_set_p, ServiceJobSet *jobs_p, const char *working_directory_s)
{
	/* count the number of databases to search */
	if (db_p)
		{
			while (db_p -> di_name_s)
				{
					const char *job_name_s = NULL;
					const char *job_description_s = NULL;

					if (all_flag)
						{
							job_name_s = db_p -> di_name_s;
							job_description_s = db_p -> di_description_s;
						}
					else
						{
							Parameter *param_p = GetParameterFromParameterSetByName (param_set_p, db_p -> di_name_s);

							if (param_p)
								{
									if (param_p -> pa_current_value.st_boolean_value)
										{
											job_name_s = db_p -> di_name_s;
											job_description_s = db_p -> di_description_s;
										}
								}
						}

					if (job_name_s)
						{
							BlastServiceJob *job_p = AllocateBlastServiceJob (jobs_p -> sjs_service_p, job_name_s, job_description_s, working_directory_s);

							if (job_p)
								{
									if (!AddServiceJobToServiceJobSet (jobs_p, (ServiceJob *) job_p))
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add ServiceJob to the ServiceJobSet for \"%s\"", job_name_s);
											FreeBlastServiceJob (& (job_p -> bsj_job));
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create ServiceJob for \"%s\"", job_name_s);
								}

						}		/* if (job_name_s) */

					++ db_p;
				}		/* while (db_p) */

		}		/* if (db_p) */

}


static ServiceJobSet *CreateJobsForPreviousResults (ParameterSet *params_p, const char *ids_s, BlastServiceData *blast_data_p)
{
	LinkedList *ids_p = GetUUIDSList (ids_s);
	ServiceJobSet *jobs_p = NULL;

	if (ids_p)
		{
			SharedType param_value;
			uint32 output_format_code = BS_DEFAULT_OUTPUT_FORMAT;

			memset (&param_value, 0, sizeof (SharedType));

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_OUTPUT_FORMAT, &param_value, true))
				{
					output_format_code = param_value.st_ulong_value;
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Couldn't get requested output format code, using " UINT32_FMT " instead", output_format_code);
				}

			jobs_p = GetPreviousJobResults (ids_p, blast_data_p, output_format_code);

			if (!jobs_p)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get ServiceJobSet for previously run blast job \"%s\"", param_value.st_string_value_s);
				}

			FreeLinkedList (ids_p);
		}		/* if (ids_p) */
	else
		{
			jobs_p = AllocateServiceJobSet (blast_data_p -> bsd_base_data.sd_service_p);

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to parse \"%s\" to get uuids", ids_s);

			if (jobs_p)
				{
					ServiceJob *job_p = CreateAndAddServiceJobToServiceJobSet (jobs_p, ids_s, "Failed UUID lookup", NULL, NULL);

					if (job_p)
						{
							json_error_t json_err;

							char *errors_s = ConcatenateVarargsStrings ("Failed to parse \"", ids_s, "\" to get uuids", NULL);

							if (errors_s)
								{
									job_p -> sj_errors_p = json_pack_ex (&json_err, 0, "{s:s}", ERROR_S, errors_s);
									FreeCopiedString (errors_s);
								}
							else
								{
									job_p -> sj_errors_p = json_pack_ex (&json_err, 0, "{s:s}", ERROR_S, "Failed to parse uuids");
								}

						}		/* if (job_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add failed job for \"%s\" uuids", ids_s);
							FreeServiceJobSet (jobs_p);
							jobs_p = NULL;
						}

				}		/* if (service_p -> se_jobs_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate job set");
				}
		}

	return jobs_p;
}


static ServiceJobSet *RunBlastService (Service *service_p, ParameterSet *param_set_p, json_t *credentials_p)
{
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	SharedType param_value;

	memset (&param_value, 0, sizeof (SharedType));

	/* Are we retrieving previously run jobs? */
	if ((GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_JOB_ID, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s)))
		{
			service_p -> se_jobs_p  = CreateJobsForPreviousResults (param_set_p, param_value.st_string_value_s, blast_data_p);
		}		/* if ((GetParameterValueFromParameterSet (param_set_p, TAG_BLAST_JOB_ID, &param_value, true)) && (!IsStringEmpty (param_value.st_string_value_s))) */
	else
		{
			service_p -> se_jobs_p = AllocateServiceJobSet (service_p);

			if (service_p -> se_jobs_p)
				{
					/* Get all of the selected databases and create a BlastServiceJob for each one */
					PrepareBlastServiceJobs (blast_data_p -> bsd_databases_p, false, param_set_p, service_p -> se_jobs_p, blast_data_p -> bsd_working_dir_s);

					if (GetServiceJobSetSize (service_p -> se_jobs_p) > 0)
						{
							/*
							 * Get the absolute path and filename stem e.g.
							 *
							 *  file_stem_s = /usr/foo/bar/job_id
							 *
							 *  which will be used to build the input, output and other associated filenames e.g.
							 *
							 *  input file = file_stem_s + ".input"
							 *  output_file = file_stem_s + ".output"
							 *
							 *  As each job will have the same input file name it using the first job's id
							 *
							 */
							ServiceJobSetIterator iterator;
							BlastServiceJob *job_p = NULL;

							InitServiceJobSetIterator (&iterator, service_p -> se_jobs_p);
							job_p = (BlastServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);

							if (job_p)
								{
									TempFile *input_p = GetInputTempFile (param_set_p, blast_data_p -> bsd_working_dir_s, job_p -> bsj_job.sj_id);

									if (input_p)
										{
											const char *input_filename_s = input_p -> GetFilename ();

											if (input_filename_s)
												{
													size_t num_jobs_ran = 0;
													bool loop_flag = true;

													while (loop_flag)
														{
															BlastTool *tool_p = job_p -> bsj_tool_p;

															job_p -> bsj_job.sj_status = OS_FAILED_TO_START;

															if (tool_p)
																{
																	if (tool_p -> SetInputFilename (input_filename_s))
																		{
																			char *output_filename_s = NULL;

																			if (tool_p -> SetUpOutputFile ())
																				{
																					if (tool_p -> ParseParameters (param_set_p))
																						{
																							if (RunBlast (tool_p))
																								{
																									job_p -> bsj_job.sj_status = tool_p -> GetStatus ();

																									if ((job_p -> bsj_job.sj_status == OS_SUCCEEDED) ||
																											(job_p -> bsj_job.sj_status == OS_PARTIALLY_SUCCEEDED))
																										{
																											GetBlastResult (service_p, job_p);
																										}
																								}
																							else
																								{
																									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to run blast tool \"%s\"", job_p -> bsj_job.sj_name_s);
																								}

																						}		/* if (tool_p -> ParseParameters (param_set_p, input_filename_s)) */
																					else
																						{
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to parse parameters for blast tool \"%s\"", job_p -> bsj_job.sj_name_s);
																						}

																				}		/* if (tool_p -> SetOutputFilename (output_filename_s)) */
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set output filename for blast tool \"%s\" to \"%s\"", job_p -> bsj_job.sj_name_s, output_filename_s);
																				}

																		}		/* if (tool_p -> SetInputFilename (input_filename_s)) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set input filename for blast tool \"%s\" to \"%s\"", job_p -> bsj_job.sj_name_s, input_filename_s);
																		}

																}		/* if (tool_p) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get blast tool for \"%s\"", job_p -> bsj_job.sj_name_s);
																}

															job_p = (BlastServiceJob *) GetNextServiceJobFromServiceJobSetIterator (&iterator);
															loop_flag = (job_p != NULL);

														}		/* while (loop_flag) */

												}		/* if (input_filename_s) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get input filename for blast tool \"%s\"", job_p -> bsj_job.sj_name_s);
												}

											delete input_p;
										}		/* if (input_p) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create input temp file for blast tool \"%s\" in \"%s\"", job_p -> bsj_job.sj_name_s, blast_data_p -> bsd_working_dir_s);
										}

								}		/* if (job_p) */

						}		/* if (GetServiceJobSetSize (service_p -> se_jobs_p) > 0) */

					/* Are there any remote jobs to run? */
					if (service_p -> se_paired_services.ll_size > 0)
						{
							PairedServiceNode *node_p = (PairedServiceNode *) (service_p -> se_paired_services.ll_head_p);

							while (node_p)
								{
									PairedService *paired_service_p = node_p -> psn_paired_service_p;
									int32 res = RunRemoteBlastJobs (service_p, service_p -> se_jobs_p, param_set_p, paired_service_p);

									if (res >= 0)
										{
#if BLAST_SERVICE_DEBUG >= STM_LEVEL_FINER
											PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Got " UINT32_FMT " results from \"%s\" at \"%s\"", res, paired_service_p -> ps_name_s, paired_service_p -> ps_server_uri_s);
#endif
										}		/* if (res < 0) */
									else
										{
											PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Got " UINT32_FMT " error from \"%s\" at \"%s\"", res, paired_service_p -> ps_name_s, paired_service_p -> ps_server_uri_s);
										}

									node_p = (PairedServiceNode *) (node_p -> psn_node.ln_next_p);
								}		/* while (node_p) */

						}		/* if (service_p -> se_paired_services.ll_size > 0) */


					if (GetServiceJobSetSize (service_p -> se_jobs_p) == 0)
						{
							PrintErrors (STM_LEVEL_INFO, __FILE__, __LINE__, "No jobs specified");
						}

				}		/* if (service_p -> se_jobs_p) */

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


static OperationStatus GetBlastServiceStatus (Service *service_p, const uuid_t job_id)
{
	OperationStatus status = OS_ERROR;
	BlastServiceData *blast_data_p = (BlastServiceData *) (service_p -> se_data_p);
	BlastServiceJob *job_p = NULL;
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	ConvertUUIDToString (job_id, uuid_s);

	if (service_p -> se_jobs_p)
		{
			job_p = (BlastServiceJob *) GetServiceJobFromServiceJobSetById (service_p -> se_jobs_p, job_id);

			if (job_p)
				{
					status = job_p -> bsj_tool_p -> GetStatus ();
				}
		}

	return status;
}



static bool CleanUpBlastJob (ServiceJob *job_p)
{
	bool cleaned_up_flag = true;
	BlastServiceJob *blast_job_p = (BlastServiceJob *) job_p;

	return cleaned_up_flag;
}


static char *GetBlastResultByUUID (const BlastServiceData *data_p, const uuid_t job_id, const uint32 output_format_code)
{
	char job_id_s [UUID_STRING_BUFFER_SIZE];
	char *result_s = NULL;

	ConvertUUIDToString (job_id, job_id_s);

	result_s = GetBlastResultByUUIDString (data_p, job_id_s, output_format_code);

	return result_s;
}




static char *GetBlastResultByUUIDString (const BlastServiceData *data_p, const char *job_id_s, const uint32 output_format_code)
{
	char *result_s = NULL;
	char *job_output_filename_s = GetPreviousJobFilename (data_p, job_id_s, BS_OUTPUT_SUFFIX_S);

	if (job_output_filename_s)
		{
			/* Does the file already exist? */
			char *converted_filename_s = BlastFormatter :: GetConvertedOutputFilename (job_output_filename_s, output_format_code, NULL);

			if (converted_filename_s)
				{
					FILE *job_f = fopen (converted_filename_s, "r");

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

					FreeCopiedString (converted_filename_s);
				}		/* if (converted_filename_s) */

			if (!result_s)
				{
					/*
					 * We haven't got the output in the desired output format so we need to run the formatter.
					 */

					/* Is it a local job? */
					if (IsPathValid (job_output_filename_s))
						{
							if (data_p -> bsd_formatter_p)
								{
									result_s = data_p -> bsd_formatter_p -> GetConvertedOutput (job_output_filename_s, output_format_code);
								}		/* if (data_p -> bsd_formatter_p) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "No formatter specified");
								}

						}		/* if (IsPathValid (job_output_filename_s)) */

				}		/* if (!result_s) */

			FreeCopiedString (job_output_filename_s);
		}		/* if (job_output_filename_s) */


	if (!result_s)
		{
			/* Is it a remote job? */
			result_s = GetPreviousRemoteBlastServiceJob (job_id_s, output_format_code, data_p);
		}

	return result_s;
}

