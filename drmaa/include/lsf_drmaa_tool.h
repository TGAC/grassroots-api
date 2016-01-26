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

/*
 * lsf_drmaa_tool.h
 *
 *  Created on: 25 Jan 2016
 *      Author: billy
 */

#ifndef LSF_DRMAA_TOOL_H_
#define LSF_DRMAA_TOOL_H_


/* LSF includes */
#include "drmaa.h"


/* Grassroots includes" */
#include "drmaa_tool.h"




typedef struct LSFDrmaaTool
{
	DrmaaTool ldt_base_tool;
	drmaa_job_template_t *ldt_job_p;
	char *ldt_id_s;
	char *ldt_id_out_s;
} LSFDrmaaTool;



LSFDrmaaTool *AllocateLSFDrmaaTool (const char *program_name_s);


void FreeLSFDrmaaTool (LSFDrmaaTool *lsf_tool_p);


bool RunLSFDrmaaTool (struct DrmaaTool *tool_p, const bool async_flag);


OperationStatus GetLSFDrmaaToolStatus (struct DrmaaTool *tool_p);



#endif /* LSF_DRMAA_TOOL_H_ */
