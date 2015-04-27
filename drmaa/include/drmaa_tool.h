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


#define MAX_LEN_JOBID 100


typedef struct DrmaaTool
{
	char *dt_program_name_s;
	LinkedList *dt_args_p;
	char *dt_queue_name_s;
	char *dt_working_directory_s;
	drmaa_job_template_t *dt_job_p;
	char dt_id_s [MAX_LEN_JOBID];
	char dt_id_out_s [MAX_LEN_JOBID];
	char dt_diagnosis_s [DRMAA_ERROR_STRING_BUFFER];
	int dt_stat;
} DrmaaTool;


#ifdef __cplusplus
extern "C"
{
#endif

WHEATIS_DRMAA_API DrmaaTool *AllocateDrmaaTool (const char *program_name_s);


WHEATIS_DRMAA_API void FreeDrmaaTool (DrmaaTool *tool_p);


WHEATIS_DRMAA_API bool SetDrmaaToolCurrentWorkingDirectory (DrmaaTool *tool_p, char *path_s);


WHEATIS_DRMAA_API bool SetDrmaaToolQueueName (DrmaaTool *tool_p, char *queue_name_s);


WHEATIS_DRMAA_API bool AddDrmaaToolArgument (DrmaaTool *tool_p, char *arg_s);


WHEATIS_DRMAA_API bool RunDrmaaToolSynchronously (DrmaaTool *tool_p);




#ifdef __cplusplus
}
#endif

#endif /* DRMAA_TOOL_H_ */
