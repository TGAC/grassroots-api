#include <cstdlib>

#include <syslog.h>

#include "blast_tool.hpp"

#include "io_utils.h"

#include "byte_buffer.h"
#include "string_utils.h"

#include "system_blast_tool.hpp"


BlastTool *CreateBlastTool (Service *service_p)
{
	/**
	 * In the future, this would query the system to determine which type
	 * of blast tool to use, probably set by an admin. 
	 */
	return new SystemBlastTool (service_p);
}


void FreeBlastTool (BlastTool *tool_p)
{
	delete tool_p;
}



BlastTool :: BlastTool (Service *service_p)
{
	bt_status = OS_IDLE;
	bt_service_p = service_p;
}


OperationStatus BlastTool :: GetStatus () const
{
	return bt_status;
}


BlastTool :: ~BlastTool ()
{	
	std :: vector <char *> :: size_type i;
	
	for (i = bt_allocated_args.size (); i > 0; -- i)
		{
			char *data_s = bt_allocated_args.back ();
			
			bt_allocated_args.pop_back ();
			
			free (data_s);
		}
}


OperationStatus RunBlast (BlastTool *tool_p)
{
	OperationStatus status = tool_p -> Run ();
		
	return status;
}


void BlastTool :: AddArgument (char *arg_s, bool newly_allocated_flag)
{
	bt_command_line_args.push_back (arg_s);	
	
	if (newly_allocated_flag)
		{
			bt_allocated_args.push_back (arg_s);			
		}
}


/*
OperationStatus InlineBlastTool :: Run ()
{
	OperationStatus status = OS_SUCCEEDED;

	PreRun ();
	WriteToLog (NULL, LOG_INFO, "%s %d: InlineBlastTool :: Run", __FILE__, __LINE__);		
	PostRun ();

	return status;
}


OperationStatus ThreadedBlastTool :: Run ()
{
	OperationStatus status = OS_SUCCEEDED;
	
	PreRun ();
	WriteToLog (NULL, LOG_INFO, "%s %d: ThreadedBlastTool :: Run", __FILE__, __LINE__);	
	PostRun ();

	return status;
}
*/


void BlastTool :: PreRun ()
{
	bt_status = OS_STARTED;

	SetCurrentServiceStatus (bt_service_p, bt_status);
}


void BlastTool :: PostRun ()
{
	bt_status = OS_SUCCEEDED;

	SetCurrentServiceStatus (bt_service_p, bt_status);
}


void BlastTool :: PringArgsToLog ()
{
	std :: vector <char *> :: size_type i;
	const std :: vector <char *> :: size_type num_args = bt_command_line_args.size ();
	
	for (i = 0; i < num_args; ++ i)
		{
			WriteToLog (NULL, LOG_INFO, "%s %d: arg [%d]=\"%s\"\n", __FILE__, __LINE__, (int) i, bt_command_line_args [i]);	
		}
}

