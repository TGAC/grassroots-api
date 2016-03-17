/*
* Copyright 2014-2015 The Genome Analysis Centre
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

#include "drmaa.h"
#include "drmaa_tool.h"
#include "streams.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "string_linked_list.h"
#include "byte_buffer.h"
#include "filesystem_utils.h"

#ifdef _DEBUG
	#define DRMAA_TOOL_DEBUG (STM_LEVEL_FINEST)
#else
	#define DRMAA_TOOL_DEBUG (STM_LEVEL_NONE)
#endif


#ifdef LSF_DRMAA_ENABLED
	static const char S_QUEUE_KEY_S [] = "-q ";
#elif SLURM_DRMAA_ENABLED
	static const char S_QUEUE_KEY_S [] = "-p ";
#endif


/*
 * STATIC PROTOTYPES
 */

static const char **CreateAndAddArgsArray (const DrmaaTool *tool_p);

static void FreeAndRemoveArgsArray (const DrmaaTool *tool_p, const char **args_ss);

static bool BuildNativeSpecification (DrmaaTool *tool_p);

static bool SetDrmaaAttribute (DrmaaTool *tool_p, const char *name_s, const char *value_s);

static bool SetDrmaaVectorAttribute (DrmaaTool *tool_p, const char *name_s, const char **values_ss);

void FreeStringArray (char **values_ss);

static bool DeleteJobTemplate (DrmaaTool *tool_p);



/*
 * API FUNCTIONS
 */


bool InitDrmaa (void)
{
	bool success_flag = false;
	char error_diagnosis_s [DRMAA_ERROR_STRING_BUFFER];
	int res;

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "About to Init Drmaa");
	#endif

	res = drmaa_init (NULL, error_diagnosis_s, DRMAA_ERROR_STRING_BUFFER - 1);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			success_flag = true;
		}

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Init Drmaa %d res %d %s", success_flag, res, drmaa_diagnosis_s);
	#endif

	return success_flag;
}


bool ExitDrmaa (void)
{
	bool res_flag = true;
	char error_diagnosis_s [DRMAA_ERROR_STRING_BUFFER];
	int res;

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "About to Exit Drmaa");
	#endif

	res = drmaa_exit (error_diagnosis_s, DRMAA_ERROR_STRING_BUFFER - 1);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			res_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_exit() failed: %s\n", error_diagnosis_s);
		}

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Exit Drmaa %d res %d %s", res_flag, res, s_drmaa_diagnosis_s);
	#endif

	return res_flag;
}



DrmaaTool *AllocateDrmaaTool (const char *program_name_s, const uuid_t id)
{
	DrmaaTool *tool_p = (DrmaaTool *) AllocMemory (sizeof (DrmaaTool));

	if (tool_p)
		{
			if (InitDrmaaTool (tool_p, program_name_s, id))
				{
					return tool_p;
				}

			FreeMemory (tool_p);
		}

	return NULL;
}


bool InitDrmaaTool (DrmaaTool *tool_p, const char *program_name_s, const uuid_t id)
{
	tool_p -> dt_queue_name_s = NULL;
	tool_p -> dt_working_directory_s = NULL;
	tool_p -> dt_output_filename_s = NULL;

	tool_p -> dt_num_cores = 0;
	tool_p -> dt_host_name_s = NULL;
	tool_p -> dt_user_name_s = NULL;
	tool_p -> dt_email_addresses_ss = NULL;
	tool_p -> dt_mb_mem_usage = 0;
	tool_p -> dt_id_s = NULL;
	tool_p -> dt_id_out_s = NULL;
	tool_p -> dt_job_p = NULL;
	tool_p -> dt_args_p = NULL;


	tool_p -> dt_program_name_s = CopyToNewString (program_name_s, 0, false);

	if (tool_p -> dt_program_name_s)
		{
			if ((tool_p -> dt_id_s = GetUUIDAsString (id)) != NULL)
				{
					if ((tool_p -> dt_args_p = AllocateLinkedList (FreeStringListNode)) != NULL)
						{
							char error_s [DRMAA_ERROR_STRING_BUFFER] = { 0 };
							int err_code = drmaa_allocate_job_template (& (tool_p -> dt_job_p), error_s, DRMAA_ERROR_STRING_BUFFER);

							if (err_code == DRMAA_ERRNO_SUCCESS)
								{
									/* join output/error file */
									if (SetDrmaaAttribute (tool_p, DRMAA_JOIN_FILES, "y"))
										{
											/* run jobs in user's home directory */
											if (SetDrmaaAttribute (tool_p, DRMAA_WD, DRMAA_PLACEHOLDER_HD))
												{
													/* the job to be run */
													if (SetDrmaaAttribute (tool_p, DRMAA_REMOTE_COMMAND, program_name_s))
														{
															return true;
														}		/* if (SetDrmaaAttribute (tool_p, DRMAA_REMOTE_COMMAND, program_name_s)) */

												}		/* if (SetDrmaaAttribute (tool_p, DRMAA_WD, DRMAA_PLACEHOLDER_HD)) */

										}		/* if (SetDrmaaAttribute (tool_p, DRMAA_JOIN_FILES, "y")) */

								}		/* if (err_code == DRMAA_ERRNO_SUCCESS) */

							FreeLinkedList (tool_p -> dt_args_p);
							tool_p -> dt_args_p = NULL;
						}		/* if ((tool_p -> dt_args_p = AllocateLinkedList (FreeStringListNode)) != NULL) */

					FreeCopiedString (tool_p -> dt_id_s);
					tool_p -> dt_id_s = NULL;
				}		/* if ((tool_p -> dt_id_s = GetUUIDAsString (id)) != NULL) */

			FreeCopiedString (tool_p -> dt_program_name_s);
			tool_p -> dt_program_name_s = NULL;
		}		/* if (tool_p -> dt_program_name_s) */

	return false;
}


void ClearDrmaaTool (DrmaaTool *tool_p)
{
	FileInformation fi;

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "entering ClearDrmaaTool");
	#endif

	if (tool_p -> dt_job_p)
		{
			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting Job Template");
			#endif

			DeleteJobTemplate (tool_p);
		}

	if (tool_p -> dt_id_s)
		{
			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_id_s");
			#endif

			FreeCopiedString (tool_p -> dt_id_s);
		}

	if (tool_p -> dt_id_out_s)
		{
			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_id_out_s");
			#endif

			FreeCopiedString (tool_p -> dt_id_out_s);
		}

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_program_name_s");
	#endif
	FreeCopiedString (tool_p -> dt_program_name_s);

	if (tool_p -> dt_queue_name_s)
		{
			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_args_p");
			#endif

			FreeCopiedString (tool_p -> dt_queue_name_s);
		}

	if (tool_p -> dt_email_addresses_ss)
		{
			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_email_addresses_ss");
			#endif

			FreeStringArray (tool_p -> dt_email_addresses_ss);
		}

	if (tool_p -> dt_output_filename_s)
		{
			if (CalculateFileInformation (tool_p -> dt_output_filename_s, &fi))
				{
					/* If the stdout/stderr file is empty, then delete it */
					if (fi.fi_size == 0)
						{
							#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
							PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "removing file \"%s\"", tool_p -> dt_output_filename_s);
							#endif

							if (!RemoveFile (tool_p -> dt_output_filename_s))
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to delete file \"%s\"", tool_p -> dt_output_filename_s);
								}
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get file size for \"%s\"", tool_p -> dt_output_filename_s);
				}

			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_output_filename_s");
			#endif
			FreeCopiedString (tool_p -> dt_output_filename_s);
		}

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "exiting ClearDrmaaTool");
	#endif
}


void FreeDrmaaTool (DrmaaTool *tool_p)
{
	ClearDrmaaTool (tool_p);

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting tool_p");
	#endif
	FreeMemory (tool_p);
}


bool SetDrmaaToolEmailNotifications (DrmaaTool *tool_p, const char **email_addresses_ss)
{
	bool success_flag = false;

	if (email_addresses_ss)
		{
			char **copied_email_addresses_ss = NULL;
			size_t num_addresses = 0;
			const char **address_ss = email_addresses_ss;

			/* count the number of addresses */
			while (*address_ss)
				{
					++ address_ss;
					++ num_addresses;
				}

			copied_email_addresses_ss = (char **) AllocMemoryArray (num_addresses + 1, sizeof (char *));
			if (copied_email_addresses_ss)
				{
					size_t i = 0;
					bool loop_flag = i < num_addresses;
					char **copied_address_ss = copied_email_addresses_ss;

					address_ss = email_addresses_ss;

					success_flag = true;
					while (loop_flag)
						{
							char *copied_address_s = CopyToNewString (*address_ss, 0, false);

							if (copied_address_s)
								{
									*copied_address_ss = copied_address_s;

									++ i;
									++ copied_address_ss;
									++ address_ss;

									loop_flag = (*address_ss != NULL);
								}
							else
								{
									loop_flag = false;
									success_flag = false;
								}
						}

					if (success_flag)
						{
							if (tool_p -> dt_email_addresses_ss)
								{
									FreeStringArray (tool_p -> dt_email_addresses_ss);
								}

							tool_p -> dt_email_addresses_ss = copied_email_addresses_ss;

							#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINE
								{
									address_ss = (const char **) (tool_p -> dt_email_addresses_ss);
									size_t j = 0;

									while (*address_ss)
										{
											PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "adding email [%ld] = %s\n", j, *address_ss);
											++ address_ss;
											++ i;
										}
								}
							#endif

							success_flag = SetDrmaaVectorAttribute (tool_p, DRMAA_V_EMAIL, (const char **) (tool_p -> dt_email_addresses_ss));
						}
					else
						{
							size_t j;

							copied_address_ss = copied_email_addresses_ss;

							for (j = 0; j < i; ++ j, ++ copied_address_ss)
								{
									FreeCopiedString (*copied_address_ss);
								}

							FreeMemory (copied_email_addresses_ss);
						}

				}		/* if (copied_email_addresses_ss) */

		}
	else
		{
			if (tool_p -> dt_email_addresses_ss)
				{
					const char *addresses_s = NULL;

					FreeStringArray (tool_p -> dt_email_addresses_ss);
					tool_p -> dt_email_addresses_ss = NULL;
					success_flag = SetDrmaaVectorAttribute (tool_p, DRMAA_V_EMAIL, &addresses_s);
				}
			else
				{
					success_flag = true;
				}
		}

	return success_flag;
}


void FreeStringArray (char **values_ss)
{
	char **value_ss = values_ss;

	while (*value_ss)
		{
			FreeCopiedString (*value_ss);
			++ value_ss;
		}

	FreeMemory (values_ss);
}


bool SetDrmaaToolOutputFilename (DrmaaTool *tool_p, const char *output_name_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_output_filename_s), output_name_s))
		{
			success_flag = SetDrmaaAttribute (tool_p, DRMAA_OUTPUT_PATH, tool_p -> dt_output_filename_s);
		}

	return success_flag;
}


bool SetDrmaaToolCurrentWorkingDirectory (DrmaaTool *tool_p, const char *path_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_working_directory_s), path_s))
		{
			success_flag = SetDrmaaAttribute (tool_p, DRMAA_WD, tool_p -> dt_working_directory_s);
		}

	return success_flag;
}


bool SetDrmaaToolQueueName (DrmaaTool *tool_p, const char *queue_name_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_queue_name_s), queue_name_s))
		{
			success_flag = SetDrmaaAttribute (tool_p, DRMAA_NATIVE_SPECIFICATION, tool_p -> dt_queue_name_s);
		}

	return success_flag;
}


bool AddDrmaaToolArgument (DrmaaTool *tool_p, const char *arg_s)
{
	bool success_flag = false;
	StringListNode *node_p = AllocateStringListNode (arg_s, MF_DEEP_COPY);

	if (node_p)
		{
			LinkedListAddTail (tool_p -> dt_args_p, (ListItem * const) node_p);
			success_flag = true;
		}

	return success_flag;
}



OperationStatus GetDrmaaToolStatus (DrmaaTool *tool_p)
{
	OperationStatus status = OS_ERROR;
	char error_s [DRMAA_ERROR_STRING_BUFFER];
	int drmaa_status;
	int res = drmaa_job_ps (tool_p -> dt_id_s, &drmaa_status, error_s, DRMAA_ERROR_STRING_BUFFER);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			switch (drmaa_status)
				{
					case DRMAA_PS_QUEUED_ACTIVE:
					case DRMAA_PS_SYSTEM_ON_HOLD:
					case DRMAA_PS_USER_ON_HOLD:
					case DRMAA_PS_USER_SYSTEM_ON_HOLD:
						status = OS_PENDING;
						break;

					case DRMAA_PS_UNDETERMINED:
						status = OS_ERROR;
						break;

					case DRMAA_PS_RUNNING:
						status = OS_STARTED;
						break;

					case DRMAA_PS_FAILED:
						status = OS_FAILED;
						break;

					case DRMAA_PS_DONE:
						status = OS_SUCCEEDED;
						break;

					case DRMAA_PS_SYSTEM_SUSPENDED:
					case DRMAA_PS_USER_SUSPENDED:
					case DRMAA_PS_USER_SYSTEM_SUSPENDED:
						status = OS_PENDING;
						break;

					default:
						break;
				}

			PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa ps for %s: (%d = %d)", tool_p -> dt_id_s, drmaa_status, status);

		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get drmaa ps for %s: %d, error: %s", tool_p -> dt_id_s, res, error_s);
		}

	return status;
}


bool RunDrmaaTool (DrmaaTool *tool_p, const bool async_flag)
{
	bool success_flag = false;

	if (BuildNativeSpecification (tool_p))
		{
			const char **args_ss = CreateAndAddArgsArray (tool_p);

			if (args_ss)
				{
					char error_s [DRMAA_ERROR_STRING_BUFFER];

					/*run a job*/
					int result = drmaa_run_job (tool_p -> dt_id_s, sizeof (tool_p -> dt_id_s) - 1, tool_p -> dt_job_p, error_s, DRMAA_ERROR_STRING_BUFFER);

					/* Now the job has started we can delete its template */
					DeleteJobTemplate (tool_p);

					if (result == DRMAA_ERRNO_SUCCESS)
						{
							if (!async_flag)
								{
									int stat;

									result = drmaa_wait (tool_p -> dt_id_s, tool_p -> dt_id_out_s, sizeof (tool_p -> dt_id_out_s) - 1, &stat,
										DRMAA_TIMEOUT_WAIT_FOREVER, NULL, error_s, DRMAA_ERROR_STRING_BUFFER);

									if (result == DRMAA_ERRNO_SUCCESS)
										{
											int exited;
											int exit_status;

											success_flag = true;

											if (drmaa_wifexited (&exited, stat, error_s, DRMAA_ERROR_STRING_BUFFER) == 0)
												{
													PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "job <%s> may not have finished correctly", tool_p -> dt_id_s, exit_status);
												}

											if (exited)
												{
													drmaa_wexitstatus (&exit_status, stat, error_s, DRMAA_ERROR_STRING_BUFFER);

													PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "job <%s> finished with exit code %d\n", tool_p -> dt_id_s, exit_status);
												}
											else
												{
													int signal_status;

													drmaa_wifsignaled (&signal_status, stat, error_s, DRMAA_ERROR_STRING_BUFFER);

													if (signal_status)
														{
															char termsig [DRMAA_SIGNAL_BUFFER+1];
															drmaa_wtermsig (termsig, DRMAA_SIGNAL_BUFFER, stat, error_s, DRMAA_ERROR_STRING_BUFFER);

															PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "job <%s> finished due to signal %s\n", tool_p -> dt_id_s, termsig);
														}
													else
														{
															PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "job <%s> is aborted\n", tool_p -> dt_id_s);
														}
												}

										}		/* if (result == DRMAA_ERRNO_SUCCESS) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_wait failed with code %d, \"%s", result, error_s);
										}

								}		/* if (!async_flag) */

						}		/* if (result == DRMAA_ERRNO_SUCCESS) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_run_job failed with code %d, \"%s", result, error_s);
						}

					FreeAndRemoveArgsArray (tool_p, args_ss);
				}		/* if (args_ss) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to build args array");
				}

		}		/* if (BuildNativeSpecification (tool_p)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to build native specification");
		}

	return success_flag;
}


bool SetDrmaaToolCores (DrmaaTool *tool_p, uint32 num_cores)
{
	tool_p -> dt_num_cores = num_cores;
	return true;
}


bool SetDrmaaToolMemory (DrmaaTool *tool_p, uint32 mem)
{
	tool_p -> dt_mb_mem_usage = mem;

	return true;
}


bool SetDrmaaToolJobId (DrmaaTool *tool_p, const char *id_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_id_s), id_s))
		{
			success_flag = true;
		}

	return success_flag;
}


bool SetDrmaaToolJobOutId (DrmaaTool *tool_p, const char *id_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_id_out_s), id_s))
		{
			success_flag = true;
		}

	return success_flag;
}


bool SetDrmaaToolHostName (DrmaaTool *tool_p, const char *host_name_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_host_name_s), host_name_s))
		{
			success_flag = true;
		}

	return success_flag;
}


bool SetDrmaaToolJobName (DrmaaTool *tool_p, const char *job_name_s)
{
	bool success_flag = SetDrmaaAttribute (tool_p, DRMAA_JOB_NAME, job_name_s);

	return success_flag;
}



static const char **CreateAndAddArgsArray (const DrmaaTool *tool_p)
{
	const char **args_ss = (const char **) AllocMemory (((tool_p -> dt_args_p -> ll_size) + 1) * sizeof (const char *));

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Entering CreateAndAddArgsArray %16X args size " UINT32_FMT, args_ss, tool_p -> dt_args_p -> ll_size);
	#endif

	if (args_ss)
		{
			const char **arg_ss = args_ss;
			StringListNode *node_p = (StringListNode *) (tool_p -> dt_args_p -> ll_head_p);

			while (node_p)
				{
					*arg_ss = node_p -> sln_string_s;
		
					#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
					PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "set arg to \"%s\"", *arg_ss);
					#endif
		
					node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);			
					++ arg_ss;
				}

			/* Add the terminating NULL */
			*arg_ss = NULL;

			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINER
				{
					uint32 i;
					
					arg_ss = args_ss;

					for (i = 0; i < tool_p -> dt_args_p -> ll_size; ++ i, ++ arg_ss)
						{
							PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "arg [" UINT32_FMT "] = \"%s\"", i, *arg_ss);
						}				
				}
			#endif
		
			if (drmaa_set_vector_attribute (tool_p -> dt_job_p, DRMAA_V_ARGV, args_ss, NULL, 0) != DRMAA_ERRNO_SUCCESS)
				{
					PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set args");
				}
		}
	else
		{
			PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create args array");
		}

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Exiting CreateAndAddArgsArray");
	#endif


	return args_ss;
}


static void FreeAndRemoveArgsArray (const DrmaaTool *tool_p, const char **args_ss)
{
	FreeMemory (args_ss);
}



static bool BuildNativeSpecification (DrmaaTool *tool_p)
{
	bool success_flag = true;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			if (tool_p -> dt_queue_name_s)
				{
					success_flag = AppendStringsToByteBuffer (buffer_p, S_QUEUE_KEY_S, tool_p -> dt_queue_name_s, NULL);
				}

			/*
			if (tool_p -> dt_host_name_s)
				{
					success_flag = AppendStringsToByteBuffer (buffer_p, " -m ", tool_p -> dt_host_name_s, NULL);
				}
			*/

			if (success_flag)
				{
					if (tool_p -> dt_num_cores > 1)
						{
							char *temp_s = ConvertIntegerToString (tool_p -> dt_num_cores);

							if (temp_s)
								{
									success_flag = AppendStringsToByteBuffer (buffer_p, " -n ", temp_s, NULL);
									FreeCopiedString (temp_s);
								}
							else
								{
									success_flag = false;
								}
						}
				}

			if (success_flag)
				{
					if (tool_p -> dt_mb_mem_usage)
						{
							char *temp_s = ConvertIntegerToString (tool_p -> dt_mb_mem_usage);

							if (temp_s)
								{
									success_flag = AppendStringsToByteBuffer (buffer_p, " -R \"rusage[mem=", temp_s, "]\"", NULL);
									FreeCopiedString (temp_s);
								}
							else
								{
									success_flag = false;
								}
						}
				}


			if (success_flag)
				{
					const char *data_s = GetByteBufferData (buffer_p);

					char *spec_s = CopyToNewString (data_s, 0, false);

					if (spec_s)
						{
							success_flag = SetDrmaaAttribute (tool_p, DRMAA_NATIVE_SPECIFICATION, spec_s);
							FreeCopiedString (spec_s);
						}
					else
						{
							success_flag = false;
						}
				}


			FreeByteBuffer (buffer_p);
		}
	else
		{
			success_flag = false;
		}

	return success_flag;
}



json_t *ConvertDrmaaToolToJSON (const DrmaaTool * const tool_p)
{
	json_t *drmaa_json_p = json_object ();

	if (drmaa_json_p)
		{
			bool success_flag = false;

			if (json_object_set_new (drmaa_json_p, DRMAA_PROGRAM_NAME_S, json_string (tool_p -> dt_program_name_s)) == 0)
				{
					if (AddValidJSONString (drmaa_json_p, DRMAA_QUEUE_S, tool_p -> dt_queue_name_s))
						{
							if (AddValidJSONString (drmaa_json_p, DRMAA_WORKING_DIR_S, tool_p -> dt_working_directory_s))
								{
									if (AddValidJSONString (drmaa_json_p, DRMAA_OUTPUT_FILE_S, tool_p -> dt_output_filename_s))
										{
											if (json_object_set_new (drmaa_json_p, DRMAA_ID_S, json_string (tool_p -> dt_id_s)) == 0)
												{
													bool continue_flag = true;
													size_t l = strlen (tool_p -> dt_id_out_s);

													if (l > 0)
														{
															if (json_object_set_new (drmaa_json_p, DRMAA_OUT_ID_S, json_string (tool_p -> dt_id_out_s)) != 0)
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_id_out_s \"%s\" to drmaa tool json", tool_p -> dt_id_out_s ? tool_p -> dt_id_out_s : "");
																	continue_flag = false;
																}
														}		/* if (l > 0) */

													if (continue_flag)
														{
															if (json_object_set_new (drmaa_json_p, DRMAA_HOSTNAME_S, json_string (tool_p -> dt_host_name_s)) == 0)
																{
																	if (json_object_set_new (drmaa_json_p, DRMAA_USERNAME_S, json_string (tool_p -> dt_user_name_s)) == 0)
																		{
																			if (tool_p -> dt_num_cores > 0)
																				{
																					if (json_object_set_new (drmaa_json_p, DRMAA_NUM_CORES_S, json_integer (tool_p -> dt_num_cores)) != 0)
																						{
																							continue_flag = false;
																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_num_cores " UINT32_FMT " to drmaa tool json", tool_p -> dt_num_cores);
																						}

																				}		/* if (tool_p -> dt_num_cores > 0) */

																			if (continue_flag)
																				{
																					if (tool_p -> dt_mb_mem_usage > 0)
																						{
																							if (json_object_set_new (drmaa_json_p, DRMAA_MEM_USAGE_S, json_integer (tool_p -> dt_mb_mem_usage)) != 0)
																								{
																									continue_flag = false;
																									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_mb_mem_usage " UINT32_FMT " to drmaa tool json", tool_p -> dt_mb_mem_usage);
																								}
																						}		/* if (tool_p -> dt_mb_mem_usage > 0) */

																				}		/* if (continue_flag) */

																			if (continue_flag)
																				{
																					if (tool_p -> dt_email_addresses_ss)
																						{
																							continue_flag = AddStringArrayToJSON (drmaa_json_p, (const char ** const ) tool_p -> dt_email_addresses_ss, DRMAA_EMAILS_S);
																						}
																				}

																			if (continue_flag)
																				{
																					if (tool_p -> dt_args_p -> ll_size > 0)
																						{
																							success_flag = AddStringListToJSON (drmaa_json_p, tool_p -> dt_args_p, DRMAA_ARGS_S);
																						}
																				}		/* if (continue_flag) */

																		}		/* if (json_object_set_new (drmaa_json_p, DRMAA_USERNAME_S, json_string (tool_p -> dt_user_name_s)) == 0) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_user_name_s \"%s\" to drmaa tool json", tool_p -> dt_user_name_s);
																		}

																}		/* if (json_object_set_new (drmaa_json_p, DRMAA_HOSTNAME_S, json_string (tool_p -> dt_host_name_s)) == 0) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_host_name_s \"%s\" to drmaa tool json", tool_p -> dt_host_name_s);
																}

														}		/* if (continue_flag) */

												}		/* if (json_object_set_new (drmaa_json_p, DRMAA_ID_S, json_string (tool_p -> dt_id_s)) == 0) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_id_s \"%s\" to drmaa tool json", tool_p -> dt_id_s ? tool_p -> dt_id_s : "");
												}

										}		/* if (AddValidJSONString (drmaa_json_p, DRMAA_QUEUE_S, tool_p -> dt_output_filename_s)) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_output_filename_s \"%s\" to drmaa tool json", tool_p -> dt_output_filename_s ? tool_p -> dt_output_filename_s : "");
										}

								}		/* if (AddValidJSONString (drmaa_json_p, DRMAA_QUEUE_S, tool_p -> dt_working_directory_s)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_working_directory_s \"%s\" to drmaa tool json", tool_p -> dt_working_directory_s ? tool_p -> dt_working_directory_s : "");
								}

						}		/* if (AddValidJSONString (drmaa_json_p, DRMAA_QUEUE_S, tool_p -> dt_queue_name_s)) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add dt_queue_name_s \"%s\" to drmaa tool json", tool_p -> dt_queue_name_s ? tool_p -> dt_queue_name_s : "");
						}

				}		/* if (json_object_set_new (drmaa_json_p, DRMAA_NAME_S, json_string (tool_p -> dt_program_name_s)) == 0) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add programs name \"%s\" to drmaa tool json", tool_p -> dt_program_name_s);
				}


			if (!success_flag)
				{
					json_decref (drmaa_json_p);
					drmaa_json_p = NULL;
				}

		}		/* if (drmaa_json_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate drmaa tool json");
		}

	return drmaa_json_p;
}


//typedef struct DrmaaTool
//{
//	/** @privatesection */
//	LinkedList *dt_args_p;
//
//	/** Filename for where to store the stdout/stderr for the drmaa job */
//	char *dt_output_filename_s;
//	char dt_id_s [MAX_LEN_JOBID];
//	char dt_id_out_s [MAX_LEN_JOBID];
//	uint32 dt_num_cores;
//	uint32 dt_mb_mem_usage;
//	char *dt_user_name_s;
//	char **dt_email_addresses_ss;
//} DrmaaTool;

/*
	PREFIX const char *DRMAA_ID_S VAL("drmma_job_id");
	PREFIX const char *DRMAA_OUT_ID_S VAL("drmma_job_out_id");

	PREFIX const char *DRMAA_NUM_CORES VAL("num_cores");
	PREFIX const char *DRMAA_MEM_USAGE_S VAL("mem");
 */


static bool SetUpDrmaaToolValue (const json_t * const json_p, const char * const key_s, DrmaaTool * drmaa_p, bool (*assign_value_fn) (DrmaaTool *drmaa_p, const char *value_s))
{
	bool success_flag = false;
	const char *value_s = GetJSONString (json_p, key_s);

	if (value_s)
		{
			if (assign_value_fn (drmaa_p, value_s))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to assign \"%s\" from key \"%s\"", value_s, key_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get value for key \"%s\"", key_s);
		}

	return success_flag;
}


static char **GetEmailAddresses (const json_t * const json_p)
{
	char **emails_ss = NULL;
	json_t *array_p = json_object_get (json_p, DRMAA_EMAILS_S);

	if (array_p)
		{
			if (json_is_array (array_p))
				{
					if (json_array_size (array_p) > 0)
						{
							emails_ss = GetStringArrayFromJSON (array_p);

							if (!emails_ss)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to generate emails array from json");
								}
						}

				}		/* if (json_is_array (array_p)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "emails value is not an array, %d", json_typeof (array_p));
				}

		}		/* if (array_p) */

	return emails_ss;
}


static LinkedList *GetProgramArguments (const json_t * const json_p)
{
	LinkedList *args_p = NULL;
	json_t *array_p = json_object_get (json_p, DRMAA_ARGS_S);

	if (array_p)
		{
			if (json_is_array (array_p))
				{
					if (json_array_size (array_p) > 0)
						{
							args_p = GetStringListFromJSON (array_p);
						}		/* if (json_array_size (array_p) > 0) */
					else
						{
							PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "args value is an empty array");
						}
				}		/* if (json_is_array (array_p)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "args value is not an array, %d", json_typeof (array_p));
				}

		}		/* if (array_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Could not get args entry");
		}

	return args_p;
}


DrmaaTool *ConvertDrmaaToolFromJSON (const json_t * const json_p)
{
	DrmaaTool *drmaa_p = NULL;
	const char *program_name_s = GetJSONString (json_p, DRMAA_PROGRAM_NAME_S);
	const char *id_s = GetJSONString (json_p, DRMAA_ID_S);

	if (program_name_s && id_s)
		{
			drmaa_p = AllocateDrmaaTool (program_name_s, id_s);

			if (drmaa_p)
				{
					if (SetUpDrmaaToolValue (json_p, DRMAA_QUEUE_S, drmaa_p, SetDrmaaToolQueueName))
						{
							if (SetUpDrmaaToolValue (json_p, DRMAA_HOSTNAME_S, drmaa_p, SetDrmaaToolHostName))
								{
									if (SetUpDrmaaToolValue (json_p, DRMAA_JOB_NAME_S, drmaa_p, SetDrmaaToolJobName))
										{
											if (SetUpDrmaaToolValue (json_p, DRMAA_OUTPUT_FILE_S, drmaa_p, SetDrmaaToolOutputFilename))
												{
													if (SetUpDrmaaToolValue (json_p, DRMAA_WORKING_DIR_S, drmaa_p, SetDrmaaToolCurrentWorkingDirectory))
														{

															if (SetUpDrmaaToolValue (json_p, DRMAA_OUT_ID_S, drmaa_p, SetDrmaaToolJobOutId))
																{
																	drmaa_p -> dt_args_p = GetProgramArguments (json_p);

																	if (drmaa_p -> dt_args_p)
																		{
																			int i = 0;

																			if (GetJSONInteger (json_p, DRMAA_NUM_CORES_S, &i))
																				{
																					if (!SetDrmaaToolCores (drmaa_p, i))
																						{
																							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set num cores to %d", i);
																						}
																				}

																			if (GetJSONInteger (json_p, DRMAA_MEM_USAGE_S, &i))
																				{
																					if (!SetDrmaaToolMemory (drmaa_p, i))
																						{
																							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set memory usage to %d", i);
																						}
																				}

																			drmaa_p -> dt_email_addresses_ss = GetEmailAddresses (json_p);
																		}		/* if (drmaa_p -> dt_args_p) */

																}		/* if (SetUpDrmaaToolValue (json_p, DRMAA_OUT_ID_S, drmaa_p, SetDrmaaToolJobOutId)) */



														}		/* if (SetUpDrmaaToolValue (json_p, DRMAA_WORKING_DIR_S, drmaa_p, SetDrmaaToolCurrentWorkingDirectory)) */

												}		/* if (SetUpDrmaaToolValue (json_p, DRMAA_OUTPUT_FILE_S, drmaa_p, SetDrmaaToolOutputFilename)) */

										}		/* if (SetUpDrmaaToolValue (json_p, DRMAA_JOB_NAME_S, drmaa_p, SetDrmaaToolJobName)) */

								}		/* if (SetUpDrmaaToolValue (json_p, DRMAA_HOSTNAME_S, drmaa_p, SetDrmaaToolHostName)) */

						}		/* if (SetUpDrmaaToolValue (json_p, DRMAA_QUEUE_S, drmaa_p, SetDrmaaToolQueueName)) */

				}		/* if (drmaa_p) */

		}		/* if (program_name_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get program name from json");
		}
//									const char *host_s = GetJSONString (json_p, DRMAA_HOSTNAME_S);
//
//									if (host_s)
//										{
//											if (SetDrmaaToolHostName (drmaa_p, host_s))
//												{
//													const char *job_name_s = GetJSONString (json_p, DRMAA_JOB_NAME_S);
//
//													if (job_name_s)
//														{
//															if (SetDrmaaToolJobName (drmaa_p, job_name_s))
//																{
//																	const char *working_dir_s = GetJSONString (json_p, DRMAA_WORKING_DIR_S);
//
//																	if (working_dir_s)
//																		{
//																			if (SetDrmaaToolCurrentWorkingDirectory (drmaa_p, working_dir_s))
//																				{
//																					const char *output_file_s = GetJSONString (json_p, DRMAA_OUTPUT_FILE_S);
//
//																					if (output_file_s)
//																						{
//																							if (SetDrmaaToolOutputFilename ())
//
//																						}		/* if (output_file_s) */
//																					else
//																						{
//																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get output file from json");
//																						}
//
//																					uint32 i = 0;
//
//																					if (GetJSONInteger (json_p, DRMAA_NUM_CORES_S, &i))
//																						{
//																							if (!SetDrmaaToolCores (drmaa_p, i))
//																								{
//																									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set num cores to " UINT32_FMT, i);
//																								}
//																						}
//
//																					if (GetJSONInteger (json_p, DRMAA_MEM_USAGE_S, &i))
//																						{
//																							if (!SetDrmaaToolMemory (drmaa_p, i))
//																								{
//																									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set memory usage to " UINT32_FMT, i);
//																								}
//																						}
//
//																				}		/* if (SetDrmaaToolCurrentWorkingDirectory (drmaa_p, working_dir_s)) */
//																			else
//																				{
//																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set working dir to \"%s\"", working_dir_s);
//																				}
//
//																		}		/* if (working_dir_s)) */
//																	else
//																		{
//																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get working dir from json");
//																		}
//
//																}		/* if (SetDrmaaToolJobName (drmaa_p, job_name_s)) */
//															else
//																{
//																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set job name to \"%s\"", job_name_s);
//																}
//
//														}		/* if (job_name_s) */
//													else
//														{
//															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job name from json");
//														}
//
//												}		/* if (SetDrmaaToolHostName (drmaa_p, host_s)) */
//											else
//												{
//													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set host name to \"%s\"", host_s);
//												}
//
//										}		/* if (host_s) */
//									else
//										{
//											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get host from json");
//										}
//
//								}		/* if (SetDrmaaToolQueueName (drmaa_p, queue_name_s)) */
//							else
//								{
//									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set queue name to \"%s\"", queue_name_s);
//								}
//
//						}		/* if (queue_name_s) */
//					else
//						{
//							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get queue name from json");
//						}
//
//				}		/* if (drmaa_p) */
//
//
//
//					const char *user_s = GetJSONString (json_p, DRMAA_USERNAME_S);
//
//					if (user_s)
//						{
//							const char *host_s = GetJSONString (json_p, DRMAA_HOSTNAME_S);
//
//							if (host_s)
//								{
//									const char *working_dir_s = GetJSONString (json_p, DRMAA_WORKING_DIR_S);
//
//									if (working_dir_s)
//										{
//											const char *output_file_s = GetJSONString (json_p, DRMAA_OUTPUT_FILE_S);
//
//											if (output_file_s)
//												{
//													bool success_flag = true;
//													char *emails_ss = NULL;
//													json_t *array_p = json_object_get (json_p, DRMAA_EMAILS_S);
//
//													if (array_p)
//														{
//															if (json_is_array (array_p))
//																{
//																	if (json_array_size (array_p) > 0)
//																		{
//																			emails_ss = GetStringArrayFromJSON (array_p);
//
//																			if (!emails_ss)
//																				{
//																					success_flag = false;
//																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to generate emails array from json");
//																				}
//																		}
//
//																}		/* if (json_is_array (array_p)) */
//															else
//																{
//																	success_flag = false;
//																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "emails value is not an array, %d", json_typeof (array_p));
//																}
//
//														}		/* if (array_p) */
//
//													if (success_flag)
//														{
//															array_p = json_object_get (json_p, DRMAA_ARGS_S);
//
//															if (array_p)
//																{
//																	if (json_is_array (array_p))
//																		{
//																			if (json_array_size (array_p) > 0)
//																				{
//																					LinkedList *args_p = GetStringListFromJSON (array_p);
//
//																					if (args_p)
//																						{
//																							const char *id_s = GetJSONString (json_p, DRMAA_ID_S);
//
//																							if (id_s)
//																								{
//																									const char *id_out_s = GetJSONString (json_p, DRMAA_OUT_ID_S);
//
//																									if (id_out_s)
//																										{
//																											uint32 num_cores = 0;
//																											uint32 mem_usage = 0;
//
//																											GetJSONInteger (json_p, DRMAA_NUM_CORES_S, &num_cores);
//																											GetJSONInteger (json_p, DRMAA_MEM_USAGE_S, &mem_usage);
//
//																											drmaa_p = (DrmaaTool *) AllocMemory (sizeof (DrmaaTool));
//
//																											if (drmaa_p)
//																												{
//																													drmaa_p -> dt_args_p = args_p;
//
//																												}		/* if (drmaa_p) */
//
//																										}		/* if (id_out_s) */
//																									else
//																										{
//																											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get id out from json");
//																										}
//
//																								}		/* if (id_s) */
//																							else
//																								{
//																									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get id from json");
//																								}
//
//																						}
//																					else
//																						{
//																							success_flag = false;
//																							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to generate emails array from json");
//																						}
//																				}
//
//																		}		/* if (json_is_array (array_p)) */
//																	else
//																		{
//																			success_flag = false;
//																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "emails value is not an array, %d", json_typeof (array_p));
//																		}
//
//																}		/* if (array_p) */
//
//														}		/* if (success_flag) */
//
//												}		/* if (output_file_s) */
//											else
//												{
//													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get output file from json");
//												}
//
//										}		/* if (working_dir_s) */
//									else
//										{
//											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get working dir from json");
//										}
//
//								}		/* if (host_s) */
//							else
//								{
//									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get host from json");
//								}
//
//						}		/* if (user_s) */
//					else
//						{
//							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get user from json");
//						}
//




	return drmaa_p;
}



/***********************************/
/********* STATIC FUNCTIONS ********/
/***********************************/


static bool DeleteJobTemplate (DrmaaTool *tool_p)
{
	int err_code = DRMAA_ERRNO_SUCCESS;
	char error_s [DRMAA_ERROR_STRING_BUFFER];

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Entering DeleteJobTemplate job %0.16X", tool_p -> dt_job_p);
	#endif

	if (tool_p -> dt_job_p)
		{
			err_code = drmaa_delete_job_template (tool_p -> dt_job_p, error_s, DRMAA_ERROR_STRING_BUFFER);

			if (err_code != DRMAA_ERRNO_SUCCESS)
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_delete_job_template failed with error code %d \"%s\"", err_code, error_s);
				}

			tool_p -> dt_job_p = NULL;
		}

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Exiting DeleteJobTemplate");
	#endif

	return (err_code == DRMAA_ERRNO_SUCCESS);
}


static bool SetDrmaaAttribute (DrmaaTool *tool_p, const char *name_s, const char *value_s)
{
	char error_s [DRMAA_ERROR_STRING_BUFFER];
	bool success_flag = true;

	int res = drmaa_set_attribute (tool_p -> dt_job_p, name_s, value_s, error_s, DRMAA_ERROR_STRING_BUFFER);

	if (res != DRMAA_ERRNO_SUCCESS)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set %s for %s for %s, error %d, %s", name_s, value_s, tool_p -> dt_id_s, res, error_s);
			success_flag = false;
		}

	return success_flag;
}



static bool SetDrmaaVectorAttribute (DrmaaTool *tool_p, const char *name_s, const char **values_ss)
{
	char error_s [DRMAA_ERROR_STRING_BUFFER];
	bool success_flag = true;

	int res = drmaa_set_vector_attribute (tool_p -> dt_job_p, name_s, values_ss, error_s, DRMAA_ERROR_STRING_BUFFER);

	if (res != DRMAA_ERRNO_SUCCESS)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set %s beginning with %s for %s, error %d, %s", name_s, *values_ss, tool_p -> dt_id_s, res, error_s);
			success_flag = false;
		}

	return success_flag;
}


