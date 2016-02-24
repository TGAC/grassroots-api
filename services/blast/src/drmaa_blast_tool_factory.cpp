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
 * drmaa_blast_tool_factory.cpp
 *
 *  Created on: 24 Feb 2016
 *      Author: tyrrells
 */

#include "json_util.h"
#include "streams.h"
#include "drmaa_blast_tool_factory.hpp"
#include "drmaa_blast_tool.hpp"




DrmaaBlastToolFactory	*DrmaaBlastToolFactory :: CreateDrmaaBlastToolFactory (const json_t *service_config_p)
{
	DrmaaBlastToolFactory *factory_p = 0;

	const char *command_s = GetJSONString (service_config_p, BS_COMMAND_NAME_S);

	if (command_s)
		{

		}

	if (!success_flag)
		{
			throw std::bad_alloc ();
		}


	try
		{
			factory_p = new DrmaaBlastToolFactory (service_config_p);
		}
	catch (std :: bad_alloc &alloc_r)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate SystemBlastFormatter");
		}

	return factory_p;
}



DrmaaBlastToolFactory :: ~DrmaaBlastToolFactory ()
{

}


BlastTool *DrmaaBlastToolFactory :: CreateBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s)
{
	BlastTool *tool_p = 0;
	const BlastServiceData *data_p = 0;
	const char *program_s = 0;
	bool async_flag = true;

	tool_p = new DrmmaBlastTool (job_p, name_s, data_p, working_directory_s, program_s, async_flag);

	return tool_p;
}


bool DrmaaBlastToolFactory :: AreToolsAsynchronous () const
{
	return false;
}


