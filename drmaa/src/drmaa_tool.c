#include <string.h>

#include "drmaa_tool.h"
#include "streams.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "string_linked_list.h"
#include "byte_buffer.h"


static const char **CreateAndAddArgsArray (const DrmaaTool *tool_p);

static void FreeAndRemoveArgsArray (const DrmaaTool *tool_p, const char **args_ss);

static bool BuildNativeSpecification (const DrmaaTool *tool_p);



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
									tool_p -> dt_mb_mem_usage = 0;

									/* join output/error file */
									drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_JOIN_FILES, "y", NULL, 0);

									/* run jobs in user's home directory */
									drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_WD, DRMAA_PLACEHOLDER_HD, NULL, 0);

									/* the job to be run */
									drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_REMOTE_COMMAND, program_name_s, NULL, 0);

									/* path for output */
									drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_OUTPUT_PATH, ":/tgac/services/wheatis/out", NULL, 0);

									return tool_p;
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
	if (tool_p -> dt_job_p)
		{
			drmaa_delete_job_template (tool_p -> dt_job_p, NULL, 0);
		}

	FreeLinkedList (tool_p -> dt_args_p);
	FreeCopiedString (tool_p -> dt_program_name_s);

	if (tool_p -> dt_queue_name_s)
		{
			FreeCopiedString (tool_p -> dt_queue_name_s);
		}

	FreeMemory (tool_p);
}



bool SetDrmaaToolOutputFilename (DrmaaTool *tool_p, const char *output_name_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_working_directory_s), output_name_s))
		{
			drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_OUTPUT_PATH, tool_p -> dt_working_directory_s, NULL, 0);

			success_flag = true;
		}

	return success_flag;
}


bool SetDrmaaToolCurrentWorkingDirectory (DrmaaTool *tool_p, const char *path_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_working_directory_s), path_s))
		{
			drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_WD, tool_p -> dt_working_directory_s, NULL, 0);

			success_flag = true;
		}

	return success_flag;
}


bool SetDrmaaToolQueueName (DrmaaTool *tool_p, const char *queue_name_s)
{
	bool success_flag = false;

	if (ReplaceStringValue (& (tool_p -> dt_queue_name_s), queue_name_s))
		{
			drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_NATIVE_SPECIFICATION, tool_p -> dt_queue_name_s, NULL, 0);

			success_flag = true;
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
						status = OS_STARTED;
						break;

					default:
						break;
				}
		}
	else
		{

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

					drmaa_delete_job_template (tool_p -> dt_job_p, NULL, 0);
					tool_p -> dt_job_p = NULL;

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
											int stat;

											drmaa_wifexited (&exited, stat, NULL, 0);

											if (exited)
												{
													drmaa_wexitstatus (&exit_status, stat, NULL, 0);

													PrintLog (STM_LEVEL_INFO, "job <%s> finished with exit code %d\n", tool_p -> dt_id_s, exit_status);
												}
											else
												{
													int signal_status;

													drmaa_wifsignaled (&signal_status, stat, NULL, 0);

													if (signal_status)
														{
															char termsig [DRMAA_SIGNAL_BUFFER+1];
															drmaa_wtermsig (termsig, DRMAA_SIGNAL_BUFFER, stat, NULL, 0);
															PrintLog (STM_LEVEL_SEVERE, "job <%s> finished due to signal %s\n", tool_p -> dt_id_s, termsig);
														}
													else
														{
															PrintLog (STM_LEVEL_SEVERE, "job <%s> is aborted\n", tool_p -> dt_id_s);
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
	bool success_flag = (drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_JOB_NAME, job_name_s, NULL, 0) == DRMAA_ERRNO_SUCCESS);

	return success_flag;
}



static const char **CreateAndAddArgsArray (const DrmaaTool *tool_p)
{
	const char **args_ss = (const char **) AllocMemory (((tool_p -> dt_args_p -> ll_size) + 1) * sizeof (const char *));

	if (args_ss)
		{
			const char **arg_ss = args_ss;
			StringListNode *node_p = (StringListNode *) (tool_p -> dt_args_p -> ll_head_p);

			while (node_p)
				{
					*arg_ss = node_p -> sln_string_s;
					node_p = (StringListNode *) (node_p -> sln_node.ln_next_p);
					++ arg_ss;
				}

			/* Add the terminating NULL */
			*arg_ss = NULL;
		}

	if (args_ss)
		{
			drmaa_set_vector_attribute (tool_p -> dt_job_p, DRMAA_V_ARGV, args_ss, NULL, 0);
		}

	return args_ss;
}


static void FreeAndRemoveArgsArray (const DrmaaTool *tool_p, const char **args_ss)
{
	FreeMemory (args_ss);
}



static bool BuildNativeSpecification (const DrmaaTool *tool_p)
{
	bool success_flag = true;
	char *value_s = NULL;
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
							success_flag = (drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_NATIVE_SPECIFICATION, spec_s, NULL, 0) == DRMAA_ERRNO_SUCCESS);
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

