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
	const char *command_line_s = GetByteBufferData (ebt_buffer_p);
	SetCurrentServiceStatus (bt_service_p, OS_STARTED);
	int res = system (command_line_s);

	OperationStatus status = OS_FAILED;

	if (res == 0)
		{

		}

	return status;
}



