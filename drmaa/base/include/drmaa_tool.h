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
/**
 * drmaa_tool.h
 *
 *  Created on: 27 Apr 2015
 *      Author: billy
 *
 * @addtogroup Drmaa
 * @brief Routines for running server-based programs
 * @{
 */

#ifndef DRMAA_TOOL_H_
#define DRMAA_TOOL_H_

#include "drmaa_library.h"
#include "typedefs.h"
#include "linked_list.h"
#include "operation.h"
#include "json_util.h"
//#include "drmaa.h"


/* forward declaration */
struct drmaa_job_template_s;

/**
 * @struct DrmaaTool
 * @brief A Tool for running Drmaa jobs.
 *
 * Drmaa is a programmatic way of running jobs on grids, clusters or
 * cloud-based systems. It allows to specify things such as the job to
 * run, what resources it will use, whether to run asychronously, <i>etc.</i>
 */
typedef struct DrmaaTool
{
	/** @privatesection */
	char *dt_program_name_s;
	LinkedList *dt_args_p;
	char *dt_queue_name_s;
	char *dt_working_directory_s;

	struct drmaa_job_template_s *dt_job_p;
	char *dt_id_s;
	char *dt_id_out_s;


	/** Filename for where to store the stdout/stderr for the drmaa job */
	char *dt_output_filename_s;
	uint32 dt_num_cores;
	uint32 dt_mb_mem_usage;
	char *dt_host_name_s;
	char *dt_user_name_s;
	char **dt_email_addresses_ss;


	bool (*dt_run_fn) (struct DrmaaTool *tool_p, const bool async_flag);

	OperationStatus (*dt_get_status_fn) (struct DrmaaTool *tool_p);

} DrmaaTool;



#ifdef __cplusplus
extern "C"
{
#endif

/** @publicsection */


GRASSROOTS_DRMAA_API bool InitDrmaa (void);


GRASSROOTS_DRMAA_API bool ExitDrmaa (void);



/**
 * Allocate a DrmaaTool to run the given program.
 *
 * @param program_name_s The program that this DrmaaTool will run.
 * @return A newly-allocated DrmaaTool or <code>NULL</code> upon error.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API DrmaaTool *AllocateDrmaaTool (const char *program_name_s);


/**
 * Initialise a DrmaaTool to run the given program.
 *
 * @param tool_p The DrmaaTool to initialise.
 * @param program_name_s The program that this DrmaaTool will run.
 * @return <code>true</code> if the DrmaaTool was initialised successfully, <code>false</code> upon error.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool InitDrmaaTool (DrmaaTool *tool_p, const char *program_name_s);


/**
 * Free a DrmaaTool.
 *
 * The DrmaaTool will be cleared and then the memory for the tool will be freed.
 *
 * @param tool_p The DrmaaTool to free.
 * @memberof DrmaaTool
 * @see ClearDrmaaTool
 */
GRASSROOTS_DRMAA_API void FreeDrmaaTool (DrmaaTool *tool_p);


/**
 * Clear a DrmaaTool.
 *
 * @param tool_p The DrmaaTool to clear.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API void ClearDrmaaTool (DrmaaTool *tool_p);


/**
 * Set the current working directory for a DrmaaTool.
 *
 * @param tool_p The DrmaaTool to set the current working directory for.
 * @param path_s The new current working directory . The DrmaaTool will make a copy of this so the parameter
 * does not need to remain in memory after this call.
 * @return <code>true</code> if the current working directory  was set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolCurrentWorkingDirectory (DrmaaTool *tool_p, const char *path_s);

/**
 * Set the queue name that a DrmaaTool will set for run its program on.
 *
 * @param tool_p The DrmaaTool to set the queue name for.
 * @param queue_name_s The queue name to use. The DrmaaTool will make a copy of this so the parameter
 * does not need to remain in memory after this call.
 * @return <code>true</code> if the queue name was set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolQueueName (DrmaaTool *tool_p, const char *queue_name_s);


/**
 * Set the number of cores that a DrmaaTool will use when it runs.
 *
 * @param tool_p The DrmaaTool to set the number of cores for.
 * @param num_cores The number of cores that this DrmaaTool will use.
 * @return <code>true</code> if the number of cores was set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolCores (DrmaaTool *tool_p, uint32 num_cores);


/**
 * Set the amount of memory that a DrmaaTool will set for its program when it runs.
 *
 * @param tool_p The DrmaaTool to set the memory for.
 * @param mem The amount of memory in MB that this DrmaaTool will use.
 * @return <code>true</code> if the amount of memory was set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolMemory (DrmaaTool *tool_p, uint32 mem);


/**
 * Set the host that a DrmaaTool will set for run its program on.
 *
 * @param tool_p The DrmaaTool to set the hostname for.
 * @param host_name_s The hostname to use. The DrmaaTool will make a copy of this so the parameter
 * does not need to remain in memory after this call.
 * @return <code>true</code> if the hostname was set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolHostName (DrmaaTool *tool_p, const char *host_name_s);


GRASSROOTS_DRMAA_API bool SetDrmaaToolJobId (DrmaaTool *tool_p, const char *id_s);

GRASSROOTS_DRMAA_API bool SetDrmaaToolJobOutId (DrmaaTool *tool_p, const char *id_s);


/**
 * Set the program that a DrmaaTool will run.
 *
 * @param tool_p The DrmaaTool to set the job for.
 * @param job_name_s The program that this DrmaaTool will run. The DrmaaTool will make a copy of this so the parameter
 * does not need to remain in memory after this call.
 * @return <code>true</code> if the program was set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolJobName (DrmaaTool *tool_p, const char *job_name_s);


/**
 * Redirect the stdout/stderr streams from a DrmaaTool to a file.
 *
 * @param tool_p The DrmaaTool to redirect the streams for.
 * @param output_name_s The filename where the streams output will be written to. The DrmaaTool will make a copy of this so the parameter
 * does not need to remain in memory after this call.
 * @return <code>true</code> if the filename was set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolOutputFilename (DrmaaTool *tool_p, const char *output_name_s);


/**
 * Add an argument to the program that a DrmaaTool will run.
 *
 * @param tool_p The DrmaaTool to add the argument for.
 * @param arg_s The argument to add. The DrmaaTool will make a copy of this so the parameter
 * does not need to remain in memory after this call.
 * @return <code>true</code> if the argument was added successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool AddDrmaaToolArgument (DrmaaTool *tool_p, const char *arg_s);


/**
 * Run a DrmaaTool.
 *
 * @param tool_p The DrmaaTool to add the argument for.
 * @param async_flag If this is <code>true</code> then the method will return straight away and
 * the job will continue to run asynchrnously or in the background. If this is <code>false</code>
 * then this method will not return until the job has completed.
 * @return <code>true</code> if the job was started successfully, <code>false</code> otherwise. To get the
 * status of whether the job completed successfully, use <code>GetDrmaaToolStatus</code> @see GetDrmaaToolStatus.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool RunDrmaaTool (DrmaaTool *tool_p, const bool async_flag);


/**
 * Get the status of job for a DrmaaTool
 *
 * @param tool_p The DrmaaTool to get the job status for.
 * @return The current status of the job for this DrmaaTool.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API OperationStatus GetDrmaaToolStatus (DrmaaTool *tool_p);



/**
 * Set the email recipients for any job notifications for a DrmaaTool
 *
 * @param tool_p The DrmaaTool to get the job status for.
 * @param An array of email addresses with the final element being NULL.
 * @return <code>true</code> if the email addresses were set successfully, <code>false</code> otherwise.
 * @memberof DrmaaTool
 */
GRASSROOTS_DRMAA_API bool SetDrmaaToolEmailNotifications (DrmaaTool *tool_p, const char **email_addresses_ss);


GRASSROOTS_DRMAA_API json_t *ConvertDrmaaToolToJSON (const DrmaaTool * const tool_p);


GRASSROOTS_DRMAA_API DrmaaTool *ConvertDrmaaToolFromJSON (const json_t * const json_p);


GRASSROOTS_DRMAA_API unsigned char *SerialiseDrmaaTool (const DrmaaTool * const job_p, const size_t *size_p);


GRASSROOTS_DRMAA_API DrmaaTool *DeserialiseDrmaaTool (const unsigned char * const data_s);

#ifdef __cplusplus
}
#endif

/* @} */

#endif /* DRMAA_TOOL_H_ */