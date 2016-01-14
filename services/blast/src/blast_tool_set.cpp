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
	const size_t size = bts_tools.size ();

	if (size > 0)
		{
			for (size_t i = 0; i < size; ++ i)
				{
					BlastTool *tool_p = bts_tools.back ();

					bts_tools.pop_back ();

					if (tool_p)
						{
							delete tool_p;
						}
				}
		}
}


bool BlastToolSet :: ClearEntry (BlastTool *tool_to_clear_p)
{
	bool removed_flag = false;
	const size_t size = bts_tools.size ();

	if (size > 0)
		{
			for (size_t i = 0; i < size; ++ i)
				{
					BlastTool *tool_p = bts_tools [i];

					if (tool_p == tool_to_clear_p)
						{
							bts_tools [i] = 0;
							removed_flag = true;
						}
				}
		}

	return removed_flag;
}


void BlastToolSet :: AddTool (BlastTool *tool_p)
{
	bts_tools.push_back (tool_p);
}


BlastTool *BlastToolSet :: GetBlastTool (const uuid_t id)
{
	const size_t size = bts_tools.size ();

	if (size > 0)
		{
			for (size_t i = 0; i < size; ++ i)
				{
					BlastTool *tool_p = bts_tools [i];

					if (tool_p)
						{
							const uuid_t &tool_id = tool_p -> GetUUID ();

							if (uuid_compare (id, tool_id) == 0)
								{
									return tool_p;
								}
						}		/* if (tool_p) */
				}
		}

	return 0;
}


bool BlastToolSet :: AreBlastToolsSynchronous ()
{
	bool sync_flag = true;

	return sync_flag;
}


BlastTool *BlastToolSet :: GetNewBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s)
{
	BlastTool *tool_p = CreateBlastTool (job_p, name_s, working_directory_s);

	if (tool_p)
		{
			AddTool (tool_p);
		}

	return tool_p;
}


size_t BlastToolSet :: GetSize () const
{
	return bts_tools.size ();
}
