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


const char *BlastTool :: bt_tool_type_s = "system";


void BlastTool :: SetBlastToolType (const char *type_s)
{
	BlastTool :: bt_tool_type_s = type_s;
}


/**
 * TODO: Don't like that this is not dealt with by the subclassed
 * BlastTool itself. Need to rework to be like this
 *
 * @return <code>true</code> if the BlastTool runs synchronous, <code>
 * false</code> if it runs asynchronously.
 */
bool IsBlastToolSynchronous ()
{
	bool sync_flag = true;

	if (strcmp (BlastTool :: bt_tool_type_s, "drmaa") == 0)
		{
			sync_flag = false;
		}

	return sync_flag;
}


BlastTool *CreateBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s)
{
	BlastTool *tool_p = 0;
	BlastServiceData *data_p = (BlastServiceData *) (job_p -> sj_service_p -> se_data_p);
  const char *program_name_s = GetJSONString (data_p -> bsd_base_data.sd_config_p, "blast_command");
	
	if (!program_name_s)
		{
			program_name_s = "blastn";
		}	


	/**
	 * In the future, this would query the system to determine which type
	 * of blast tool to use, probably set by an admin. 
	 */
	#ifdef DRMAA_ENABLED
	if (strcmp (BlastTool :: bt_tool_type_s, "drmaa") == 0)
		{
			bool async_flag = true;

			if (program_name_s)
				{
					DrmaaBlastTool *drmaa_tool_p = 0;
					const json_t *json_p = json_object_get (data_p -> bsd_base_data.sd_config_p, "drmaa_cores_per_search");

					try
						{
							drmaa_tool_p = new DrmaaBlastTool (job_p, name_s, working_directory_s, program_name_s, async_flag);
						}
					catch (std :: bad_alloc ex)
						{
							PrintErrors (STM_LEVEL_WARNING, "Failed to create drmaa blast tool");
						}

					if (json_p)
						{
							if (json_is_integer (json_p))
								{
									drmaa_tool_p -> SetCoresPerSearch (json_integer_value (json_p));
								}
						}

					tool_p = drmaa_tool_p;
				}
		}
	#endif

	if (!tool_p)
		{
			tool_p = new SystemBlastTool (job_p, name_s, working_directory_s, program_name_s);
		}


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


OperationStatus RunBlast (BlastTool *tool_p)
{
	OperationStatus status = OS_IDLE;

	tool_p -> PreRun  ();
	status = tool_p -> Run ();
	tool_p -> PostRun  ();

	return status;
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


