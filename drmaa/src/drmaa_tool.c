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


static const char **CreateAndAddArgsArray (const DrmaaTool *tool_p);

static void FreeAndRemoveArgsArray (const DrmaaTool *tool_p, const char **args_ss);

static bool BuildNativeSpecification (DrmaaTool *tool_p);

static bool SetDrmaaAttribute (DrmaaTool *tool_p, const char *name_s, const char *value_s);

static bool SetDrmaaVectorAttribute (DrmaaTool *tool_p, const char *name_s, const char **values_ss);

void FreeStringArray (char **values_ss);

DrmaaTool *AllocateDrmaaTool (const char *program_name_s)
{
	DrmaaTool *tool_p = (DrmaaTool *) AllocMemory (sizeof (DrmaaTool));

	if (tool_p)
		{
			if (drmaa_allocate_job_template (& (tool_p -> dt_job_p), NULL, 0) == DRMAA_ERRNO_SUCCESS)
				{
					tool_p -> dt_program_name_s = CopyToNewString (program_name_s, 0, false);

					if (tool_p -> dt_program_name_s)
						{
							tool_p -> dt_args_p = AllocateLinkedList (FreeStringListNode);

							if (tool_p -> dt_args_p)
								{
									memset (tool_p -> dt_id_s, 0, sizeof (tool_p -> dt_id_s));
									memset (tool_p -> dt_id_out_s, 0, sizeof (tool_p -> dt_id_out_s));

									tool_p -> dt_queue_name_s = NULL;
									tool_p -> dt_working_directory_s = NULL;
									tool_p -> dt_output_filename_s = NULL;

									tool_p -> dt_num_cores = 0;
									tool_p -> dt_host_name_s = NULL;
									tool_p -> dt_user_name_s = NULL;
									tool_p -> dt_email_addresses_ss = NULL;
									tool_p -> dt_mb_mem_usage = 0;

									/* join output/error file */
									if (SetDrmaaAttribute (tool_p, DRMAA_JOIN_FILES, "y"))
										{
											/* run jobs in user's home directory */
											if (SetDrmaaAttribute (tool_p, DRMAA_WD, DRMAA_PLACEHOLDER_HD))
												{
													/* the job to be run */
													if (SetDrmaaAttribute (tool_p, DRMAA_REMOTE_COMMAND, program_name_s))
														{
															/* path for output */
															if (SetDrmaaAttribute (tool_p, DRMAA_OUTPUT_PATH, ":/tgac/services/wheatis/out"))
																{
																	return tool_p;
																}

														}

												}

										}

								}		/* if (tool_p -> dt_args_p) */

						}		/* if (tool_p -> dt_program_name_s) */

					drmaa_delete_job_template (tool_p -> dt_job_p, NULL, 0);
				}		/* if (drmaa_allocate_job_template (& (tool_p -> dt_job_p), NULL, 0) == DRMAA_ERRNO_SUCCESS) */

			FreeMemory (tool_p);
		}

	return NULL;
}


void FreeDrmaaTool (DrmaaTool *tool_p)
{
	FileInformation fi;

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "entering FreeDrmaaTool");
	#endif

	if (tool_p -> dt_job_p)
		{
			#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_job_p");
			#endif

			drmaa_delete_job_template (tool_p -> dt_job_p, NULL, 0);
		}

	#if DRMAA_TOOL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "deleting dt_args_p");
	#endif
	FreeLinkedList (tool_p -> dt_args_p);


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
									const char **address_ss = (const char **) (tool_p -> dt_email_addresses_ss);
									size_t i = 0;

									while (*address_ss)
										{
											PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "adding email [%ld] = %s\n", i, *address_ss);
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
							char **copied_address_ss = copied_email_addresses_ss;

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
					/*run a job*/
					int result = drmaa_run_job (tool_p -> dt_id_s, sizeof (tool_p -> dt_id_s) - 1, tool_p -> dt_job_p, tool_p -> dt_diagnosis_s, sizeof (tool_p -> dt_diagnosis_s) - 1);

					if (result == DRMAA_ERRNO_SUCCESS)
						{
							success_flag = true;
						}

					/*
					drmaa_delete_job_template (tool_p -> dt_job_p, NULL, 0);
					tool_p -> dt_job_p = NULL;
					*/

					if (success_flag)
						{
							if (!async_flag)
								{
									int stat;

									result = drmaa_wait (tool_p -> dt_id_s, tool_p -> dt_id_out_s, sizeof (tool_p -> dt_id_out_s) - 1, &stat,
										DRMAA_TIMEOUT_WAIT_FOREVER, NULL, tool_p -> dt_diagnosis_s, sizeof (tool_p -> dt_diagnosis_s) - 1);

									success_flag = (result == DRMAA_ERRNO_SUCCESS) ? true : false;

									if (success_flag)
										{
											int exited;
											int exit_status;

											drmaa_wifexited (&exited, stat, NULL, 0);

											if (exited)
												{
													drmaa_wexitstatus (&exit_status, stat, NULL, 0);

													PrintLog (STM_LEVEL_INFO, __FILE__, __LINE__, "job <%s> finished with exit code %d\n", tool_p -> dt_id_s, exit_status);
												}
											else
												{
													int signal_status;

													drmaa_wifsignaled (&signal_status, stat, NULL, 0);

													if (signal_status)
														{
															char termsig [DRMAA_SIGNAL_BUFFER+1];
															drmaa_wtermsig (termsig, DRMAA_SIGNAL_BUFFER, stat, NULL, 0);
															PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "job <%s> finished due to signal %s\n", tool_p -> dt_id_s, termsig);
														}
													else
														{
															PrintLog (STM_LEVEL_SEVERE, __FILE__, __LINE__, "job <%s> is aborted\n", tool_p -> dt_id_s);
														}
												}
										}
								}		/* if (!async_flag) */
						}

					FreeAndRemoveArgsArray (tool_p, args_ss);
				}		/* if (args_ss) */

		}		/* if (BuildNativeSpecification (tool_p)) */

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
					success_flag = AppendStringsToByteBuffer (buffer_p, "-q ", tool_p -> dt_queue_name_s, NULL);
				}

			if (tool_p -> dt_host_name_s)
				{
					success_flag = AppendStringsToByteBuffer (buffer_p, " -m ", tool_p -> dt_host_name_s, NULL);
				}

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


static bool SetDrmaaAttribute (DrmaaTool *tool_p, const char *name_s, const char *value_s)
{
	#define ERROR_LENGTH (1023)
	char error_s [ERROR_LENGTH + 1];
	bool success_flag = true;

	int res = drmaa_set_attribute (tool_p -> dt_job_p, name_s, value_s, error_s, ERROR_LENGTH);

	if (res != DRMAA_ERRNO_SUCCESS)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set %s for %s for %s, error %s", name_s, value_s, tool_p -> dt_id_s, error_s);
			success_flag = false;
		}

	return success_flag;
}



static bool SetDrmaaVectorAttribute (DrmaaTool *tool_p, const char *name_s, const char **values_ss)
{
	#define ERROR_LENGTH (1023)
	char error_s [ERROR_LENGTH + 1];
	bool success_flag = true;

	int res = drmaa_set_vector_attribute (tool_p -> dt_job_p, name_s, values_ss, error_s, ERROR_LENGTH);

	if (res != DRMAA_ERRNO_SUCCESS)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set %s beginning with %s for %s, error %s", name_s, *values_ss, tool_p -> dt_id_s, error_s);
			success_flag = false;
		}

	return success_flag;
}

