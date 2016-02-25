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
#include <cstdlib>
#include <cstring>

#include <syslog.h>

#include "blast_tool.hpp"
#include "blast_service.h"

#include "io_utils.h"

#include "byte_buffer.h"
#include "string_utils.h"



void FreeBlastTool (BlastTool *tool_p)
{
	delete tool_p;
}


OperationStatus RunBlast (BlastTool *tool_p)
{
	OperationStatus status = OS_IDLE;

	tool_p -> PreRun  ();
	status = tool_p -> Run ();
	tool_p -> PostRun  ();

	return status;
}


OperationStatus GetBlastStatus (BlastTool *tool_p)
{
	return (tool_p -> GetStatus ());
}


/******************************/



BlastTool :: BlastTool (ServiceJob *service_job_p, const char *name_s, const BlastServiceData *data_p)
{
	bt_status = OS_IDLE;
	bt_job_p = service_job_p;
	bt_name_s = name_s;
	bt_service_data_p = data_p;

	if (service_job_p)
		{
			SetServiceJobName (service_job_p, name_s);
		}
}


OperationStatus BlastTool :: GetStatus ()
{
	return bt_job_p -> sj_status;
}


const uuid_t &BlastTool :: GetUUID () const
{
	return bt_job_p -> sj_id;
}


const char *BlastTool :: GetName () const
{
	return bt_name_s;
}


BlastTool :: ~BlastTool ()
{	
}


void BlastTool :: PreRun ()
{
	bt_job_p -> sj_status = OS_STARTED;
}


void BlastTool :: PostRun ()
{
	bt_job_p -> sj_status = OS_SUCCEEDED;
}

