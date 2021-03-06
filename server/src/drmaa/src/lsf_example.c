/*
** Copyright 2014-2016 The Earlham Institute
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
/**************************************************************
 * A sample for job submission, in this program a job "sleep 20"
 * will submit to LSF cluster and wait until it finished.
 **************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "drmaa.h"

#include "typedefs.h"
#include "streams.h"

#define MAX_LEN_JOBID 100

static int
RunJob (const char *command_s, const char * const inline_query_s,
				const char * const db_name_s, const char * const output_path_s, const char *job_output_path_s);

int main(int argc, char *argv[])
{
	int ret = 10;
	char *job_output_path_s = NULL;
	char *command_s = NULL;
	char *query_filename_s = "/tgac/services/wheatis/query.txt";
	char *db_name_s = "/tgac/public/databases/blast/triticum_aestivum/IWGSC/v2/IWGSCv2.0.00";
	char *output_path_s = ":/tgac/services/wheatis/DRMAA_JOB";


	switch (argc)
	{
		case 3:
			job_output_path_s = argv [2];

		/* deliberate drop through */
		case 2:
			command_s = argv [1];
			ret = RunJob (argv[1], query_filename_s, db_name_s, output_path_s, job_output_path_s);
		}

	return ret;
}


static int RunJob (const char *command_s, const char * const query_filename_s,
				const char * const db_name_s, const char * const output_path_s, const char *job_output_path_s)
{
	char buffer [1024];
	char *raw_query_s = NULL;
	char diagnosis[DRMAA_ERROR_STRING_BUFFER];
	int drmaa_errno = 0;
	char job_id[MAX_LEN_JOBID];
	const char * job_argv[] =
		{ "-db", NULL, "-query", NULL, "-out", job_output_path_s, NULL };
	drmaa_job_template_t *jt = NULL;
	char *job_name = NULL;
	const char* queue_s = "-q webservices";
	char job_id_out[MAX_LEN_JOBID];
	int stat;
	int exited, exit_status, signaled;

	job_argv[1] = db_name_s;
	job_argv[3] = query_filename_s;
	job_argv[5] = job_output_path_s;

	memset (job_id, 0, sizeof(job_id));
	memset (job_id_out, 0, sizeof(job_id_out));
	if (drmaa_init (NULL, diagnosis, sizeof(diagnosis) - 1) != DRMAA_ERRNO_SUCCESS)
		{
			fprintf (stderr, "drmaa_init() failed: %s\n", diagnosis);
			return 1;
		}

	if (drmaa_allocate_job_template (&jt, NULL, 0) != DRMAA_ERRNO_SUCCESS)
		{
			fprintf (stderr, "drmaa_allocate_job_template() failed: %s\n", diagnosis);
			return 1;
		}

	if (job_name)
		drmaa_set_attribute (jt, DRMAA_JOB_NAME, job_name, NULL, 0);

	/* run in users home directory */
	drmaa_set_attribute (
			jt,
			DRMAA_WD,
			"/tgac/workarea/group-si/blast-2.2.30/x86_64/bin/" /* DRMAA_PLACEHOLDER_HD */,
			NULL, 0);

	/* the job to be run */
	if (raw_query_s != NULL)
		{
			sprintf (buffer, "echo -e \"%s\" | %s\0", raw_query_s, command_s);
			drmaa_set_attribute (jt, DRMAA_REMOTE_COMMAND, buffer, NULL, 0);
		}
	else
		{
			drmaa_set_attribute (jt, DRMAA_REMOTE_COMMAND, command_s, NULL, 0);
		}

	/* the job's arguments */
	drmaa_set_vector_attribute (jt, DRMAA_V_ARGV, job_argv, NULL, 0);

	/* join output/error file */
	drmaa_set_attribute (jt, DRMAA_JOIN_FILES, "y", NULL, 0);

	/* path for output */
	drmaa_set_attribute (jt, DRMAA_OUTPUT_PATH,
											 output_path_s /* ":"DRMAA_PLACEHOLDER_HD"/DRMAA_JOB" */,
											 NULL, 0);

	/* the job's native specification */
	drmaa_set_attribute (jt, DRMAA_NATIVE_SPECIFICATION, queue_s, NULL, 0);

	/*run a job*/
	drmaa_errno = drmaa_run_job (job_id, sizeof(job_id) - 1, jt, diagnosis,
															 sizeof(diagnosis) - 1);

	if (drmaa_errno != DRMAA_ERRNO_SUCCESS)
		{
			fprintf (stderr, "drmaa_run_job() failed: %s\n", diagnosis);
			return 1;
		}

	drmaa_delete_job_template (jt, NULL, 0);

	printf ("Waiting for job<%s> finish ...\n", job_id);

	/*wait job finished*/
	drmaa_errno = drmaa_wait (job_id, job_id_out, sizeof(job_id_out) - 1, &stat,
	DRMAA_TIMEOUT_WAIT_FOREVER,
														NULL, diagnosis, sizeof(diagnosis) - 1);

	if (drmaa_errno != DRMAA_ERRNO_SUCCESS)
		{
			fprintf (stderr, "drmaa_wait(%s) failed: %s\n", job_id, diagnosis);
			return 1;
		}
	printf ("job stat is %d\n", stat);

	drmaa_wifexited (&exited, stat, NULL, 0);

	if (exited)
		{
			drmaa_wexitstatus (&exit_status, stat, NULL, 0);
			printf ("job<%s> finished with exit code %d\n", job_id, exit_status);
		}
	else
		{
			drmaa_wifsignaled (&signaled, stat, NULL, 0);

			if (signaled)
				{
					char termsig[DRMAA_SIGNAL_BUFFER + 1];
					drmaa_wtermsig (termsig, DRMAA_SIGNAL_BUFFER, stat, NULL, 0);
					printf ("job<%s> finished due to signal %s\n", job_id, termsig);
				}
			else
				{
					printf ("job<%s> is aborted\n", job_id);
				}
		}

	if (drmaa_exit (diagnosis, sizeof(diagnosis) - 1) != DRMAA_ERRNO_SUCCESS)
		{
			fprintf (stderr, "drmaa_exit() failed: %s\n", diagnosis);
			return 1;
		}

	return 0;
}

