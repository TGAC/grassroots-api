/*
 * queued_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include "queued_blast_tool.hpp"
#include "streams.h"


QueuedBlastTool :: QueuedBlastTool (ServiceJob *job_p)
: ExternalBlastTool (job_p)
{

}

QueuedBlastTool :: ~QueuedBlastTool ()
{

}


OperationStatus QueuedBlastTool :: Run ()
{
	PreRun ();
	PrintLog (STM_LEVEL_INFO, "%s %d: SystemBlastTool :: Run", __FILE__, __LINE__);
	PostRun ();

	return bt_job_p -> sj_status;
}

