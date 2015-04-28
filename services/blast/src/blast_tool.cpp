#include <cstdlib>

#include <syslog.h>

#include "blast_tool.hpp"

#include "io_utils.h"

#include "byte_buffer.h"
#include "string_utils.h"

#include "system_blast_tool.hpp"

#ifdef DRMAA_ENABLED
#include "drmaa_blast_tool.hpp"
#endif


BlastTool *CreateBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s)
{
	BlastTool *tool_p = 0;

	/**
	 * In the future, this would query the system to determine which type
	 * of blast tool to use, probably set by an admin. 
	 */
	#ifdef DRMAA_ENABLED
	tool_p = new DrmaaBlastTool (job_p, name_s, working_directory_s);
	#else
	tool_p = new SystemBlastTool (job_p, name_s, working_directory_s);
	#endif

	return tool_p;
}


void FreeBlastTool (BlastTool *tool_p)
{
	delete tool_p;
}



BlastTool :: BlastTool (ServiceJob *service_job_p, const char *name_s)
{
	bt_status = OS_IDLE;
	bt_job_p = service_job_p;
	bt_name_s = name_s;
}


OperationStatus BlastTool :: GetStatus () const
{
	return bt_status;
}


const uuid_t &BlastTool :: GetUUID () const
{
	return bt_job_p -> sj_id;
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
	OperationStatus status = OS_IDLE;

	tool_p -> PreRun  ();
	status = tool_p -> Run ();
	tool_p -> PostRun  ();

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
	bt_job_p -> sj_status = OS_STARTED;
}


void BlastTool :: PostRun ()
{
	bt_job_p -> sj_status = OS_SUCCEEDED;
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

