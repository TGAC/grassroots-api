/*
 * drmaa_tool.h
 *
 *  Created on: 27 Apr 2015
 *      Author: billy
 */

#ifndef DRMAA_TOOL_H_
#define DRMAA_TOOL_H_

#include "drmaa_library.h"
#include "drmaa.h"
#include "typedefs.h"
#include "linked_list.h"
#include "operation.h"


#define MAX_LEN_JOBID 100


typedef struct DrmaaTool
{
	char *dt_program_name_s;
	LinkedList *dt_args_p;
	char *dt_queue_name_s;
	char *dt_working_directory_s;
	char *dt_output_filename_s;
	drmaa_job_template_t *dt_job_p;
	char dt_id_s [MAX_LEN_JOBID];
	char dt_id_out_s [MAX_LEN_JOBID];
	char dt_diagnosis_s [DRMAA_ERROR_STRING_BUFFER];
	uint32 dt_num_cores;
	uint32 dt_mb_mem_usage;
	char *dt_host_name_s;
	char *dt_user_name_s;
} DrmaaTool;


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_DRMAA_API DrmaaTool *AllocateDrmaaTool (const char *program_name_s);


WHEATIS_DRMAA_API void FreeDrmaaTool (DrmaaTool *tool_p);


WHEATIS_DRMAA_API bool SetDrmaaToolCurrentWorkingDirectory (DrmaaTool *tool_p, const char *path_s);


WHEATIS_DRMAA_API bool SetDrmaaToolQueueName (DrmaaTool *tool_p, const char *queue_name_s);


WHEATIS_DRMAA_API bool SetDrmaaToolCores (DrmaaTool *tool_p, uint32 num_cores);


WHEATIS_DRMAA_API bool SetDrmaaToolMemory (DrmaaTool *tool_p, uint32 mem);


WHEATIS_DRMAA_API bool SetDrmaaToolHostName (DrmaaTool *tool_p, const char *host_name_s);


WHEATIS_DRMAA_API bool SetDrmaaToolJobName (DrmaaTool *tool_p, const char *job_name_s);


/** Redirect the stdout/stderr streams to a file */
WHEATIS_DRMAA_API bool SetDrmaaToolOutputFilename (DrmaaTool *tool_p, const char *output_name_s);


WHEATIS_DRMAA_API bool AddDrmaaToolArgument (DrmaaTool *tool_p, const char *arg_s);


WHEATIS_DRMAA_API bool RunDrmaaTool (DrmaaTool *tool_p, const bool async_flag);


WHEATIS_DRMAA_API OperationStatus GetDrmaaToolStatus (DrmaaTool *tool_p);



#ifdef __cplusplus
}
#endif

#endif /* DRMAA_TOOL_H_ */
