/*
 * system_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include "system_blast_tool.hpp"
#include "streams.h"


SystemBlastTool :: SystemBlastTool (Service *service_p)
: ExternalBlastTool (service_p)
{

}

SystemBlastTool :: ~SystemBlastTool ()
{

}

OperationStatus SystemBlastTool :: Run ()
{
	OperationStatus status = OS_SUCCEEDED;

	PreRun ();
	PrintLog (STM_LEVEL_INFO, "%s %d: SystemBlastTool :: Run", __FILE__, __LINE__);
	PostRun ();

	return status;
}
