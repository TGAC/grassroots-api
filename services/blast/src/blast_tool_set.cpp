/*
 * blast_tool_set.cpp
 *
 *  Created on: 23 Apr 2015
 *      Author: tyrrells
 */


#include "blast_tool_set.hpp"


BlastToolSet *CreateBlastToolSet (Service *service_p)
{
	return new BlastToolSet (service_p);
}


void FreeBlastToolSet (BlastToolSet *tools_p)
{
	delete tools_p;
}


BlastToolSet :: BlastToolSet (Service *service_p)
{
	bts_service_p = service_p;
}


BlastToolSet :: ~BlastToolSet ()
{
	for (size_t i = bts_tools.size () - 1; i >= 0; -- i)
		{
			bts_tools.pop_back ();
		}
}


void BlastToolSet :: AddTool (BlastTool *tool_p)
{
	bts_tools.push_back (tool_p);
}


BlastTool *BlastToolSet :: GetBlastTool (const uuid_t id)
{
	for (size_t i = bts_tools.size () - 1; i >= 0; -- i)
		{
			BlastTool *tool_p = bts_tools [i];
			const uuid_t &tool_id = tool_p -> GetUUID ();

			if (uuid_compare (id, tool_id) == 0)
				{
					return tool_p;
				}
		}

	return 0;
}


BlastTool *BlastToolSet :: GetNewBlastTool (ServiceJob *job_p, const char *name_s)
{
	BlastTool *tool_p = CreateBlastTool (job_p, name_s);

	if (tool_p)
		{
			AddTool (tool_p);
		}

	return tool_p;
}
