#include <string.h>

#include "drmaa_tool.h"
#include "streams.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "string_linked_list.h"


static bool SetValue (char **dest_ss, char *value_s);

static const char **CreateAndAddArgsArray (const DrmaaTool *tool_p);

static void FreeAndRemoveArgsArray (const DrmaaTool *tool_p, const char **args_ss);



DrmaaTool *AllocateDrmaaTool (const char *program_name_s)
{
	DrmaaTool *tool_p = (DrmaaTool *) AllocMemory (sizeof (DrmaaTool));

	if (tool_p)
		{
			if (drmaa_init (NULL, tool_p -> dt_diagnosis_s, sizeof (tool_p -> dt_diagnosis_s) - 1) == DRMAA_ERRNO_SUCCESS)
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
											tool_p -> dt_stat = -1;

											/* join output/error file */
											drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_JOIN_FILES, "y", NULL, 0);

											/* run jobs in user's home directory */
											drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_WD, DRMAA_PLACEHOLDER_HD, NULL, 0);

											/* the job to be run */
											drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_REMOTE_COMMAND, program_name_s, NULL, 0);

											/* path for output */
											drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_OUTPUT_PATH, ":" DRMAA_PLACEHOLDER_HD "/DRMAA_JOB", NULL, 0);

											return tool_p;
										}		/* if (tool_p -> dt_args_p) */

								}		/* if (tool_p -> dt_program_name_s) */

							drmaa_delete_job_template (tool_p -> dt_job_p, NULL, 0);
						}		/* if (drmaa_allocate_job_template (& (tool_p -> dt_job_p), NULL, 0) == DRMAA_ERRNO_SUCCESS) */


					if (drmaa_exit (tool_p -> dt_diagnosis_s, sizeof (tool_p -> dt_diagnosis_s) -1) != DRMAA_ERRNO_SUCCESS)
						{
							PrintErrors (STM_LEVEL_SEVERE, "drmaa_exit() failed: %s\n", tool_p -> dt_diagnosis_s);
						}

				}		/* if (drmaa_init (NULL, tool_p -> dt_diagnosis_s, sizeof (tool_p -> dt_diagnosis_s) - 1) == DRMAA_ERRNO_SUCCESS) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, "drmaa_init() failed: %s\n", tool_p -> dt_diagnosis_s);
				}

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

	if (drmaa_exit (tool_p -> dt_diagnosis_s, sizeof (tool_p -> dt_diagnosis_s) -1) != DRMAA_ERRNO_SUCCESS)
		{
			PrintErrors (STM_LEVEL_SEVERE, "drmaa_exit() failed: %s\n", tool_p -> dt_diagnosis_s);
		}

	FreeLinkedList (tool_p -> dt_args_p);
	FreeCopiedString (tool_p -> dt_program_name_s);

	if (tool_p -> dt_queue_name_s)
		{
			FreeCopiedString (tool_p -> dt_queue_name_s);
		}

	FreeMemory (tool_p);
}


bool SetDrmaaToolCurrentWorkingDirectory (DrmaaTool *tool_p, char *path_s)
{
	bool success_flag = false;

	if (SetValue (& (tool_p -> dt_working_directory_s), path_s))
		{
			drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_WD, tool_p -> dt_working_directory_s, NULL, 0);

			success_flag = true;
		}

	return success_flag;
}


bool SetDrmaaToolQueueName (DrmaaTool *tool_p, char *queue_name_s)
{
	bool success_flag = false;

	if (SetValue (& (tool_p -> dt_queue_name_s), queue_name_s))
		{
			drmaa_set_attribute (tool_p -> dt_job_p, DRMAA_NATIVE_SPECIFICATION, tool_p -> dt_queue_name_s, NULL, 0);

			success_flag = true;
		}

	return success_flag;
}


bool AddDrmaaToolArgument (DrmaaTool *tool_p, char *arg_s)
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



bool RunDrmaaToolSynchronously (DrmaaTool *tool_p)
{
	bool success_flag = false;

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
					result = drmaa_wait (tool_p -> dt_id_s, tool_p -> dt_id_out_s, sizeof (tool_p -> dt_id_out_s) - 1, & (tool_p -> dt_stat),
						DRMAA_TIMEOUT_WAIT_FOREVER, NULL, tool_p -> dt_diagnosis_s, sizeof (tool_p -> dt_diagnosis_s) - 1);

					success_flag = (result == DRMAA_ERRNO_SUCCESS) ? true : false;

					if (success_flag)
						{
							int exited;
							int exit_status;

							drmaa_wifexited (&exited, tool_p -> dt_stat, NULL, 0);

							if (exited)
								{
									drmaa_wexitstatus (&exit_status, tool_p -> dt_stat, NULL, 0);

									PrintLog (STM_LEVEL_INFO, "job <%s> finished with exit code %d\n", tool_p -> dt_id_s, exit_status);
								}
							else
								{
									int signal_status;

									drmaa_wifsignaled (&signal_status, tool_p -> dt_stat, NULL, 0);

									if (signal_status)
										{
											char termsig [DRMAA_SIGNAL_BUFFER+1];
											drmaa_wtermsig (termsig, DRMAA_SIGNAL_BUFFER, tool_p -> dt_stat, NULL, 0);
											PrintLog (STM_LEVEL_SEVERE, "job <%s> finished due to signal %s\n", tool_p -> dt_id_s, termsig);
										}
									else
										{
											PrintLog (STM_LEVEL_SEVERE, "job <%s> is aborted\n", tool_p -> dt_id_s);
										}
								}
						}
				}

			FreeAndRemoveArgsArray (tool_p, args_ss);
		}		/* if (args_ss) */

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


static bool SetValue (char **dest_ss, char *value_s)
{
	bool success_flag = false;
	char *new_value_s = CopyToNewString (value_s, 0, false);

	if (new_value_s)
		{
			if (*dest_ss)
				{
					FreeCopiedString (*dest_ss);
				}

			*dest_ss = new_value_s;
			success_flag = true;
		}

	return success_flag;

}

