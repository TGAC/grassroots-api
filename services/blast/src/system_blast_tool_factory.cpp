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
 * system_blast_factory.cpp
 *
 *  Created on: 24 Feb 2016
 *      Author: tyrrells
 */

#include <stdexcept>

#include "system_blast_tool_factory.hpp"
#include "system_blast_tool.hpp"
#include "streams.h"



SystemBlastToolFactory :: SystemBlastToolFactory (const json_t *service_config_p)
	: ExternalBlastToolFactory (service_config_p)
{

}



SystemBlastToolFactory *SystemBlastToolFactory :: CreateSystemBlastToolFactory (const json_t *service_config_p)
{
	SystemBlastToolFactory *factory_p = 0;

	try
		{
			factory_p = new SystemBlastToolFactory (service_config_p);
		}
	catch (std :: exception &e_r)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create new SystemBlastToolFactory, error \"%s\"", e_r.what ());
		}

	return factory_p;
}


SystemBlastToolFactory :: ~SystemBlastToolFactory ()
{

}


BlastTool *SystemBlastToolFactory :: CreateBlastTool (ServiceJob *job_p, const char *name_s, const BlastServiceData *data_p)
{
	BlastTool *tool_p = 0;

	try
		{
			tool_p = new SystemBlastTool (job_p, name_s, data_p, ebtf_program_name_s);
		}
	catch (std :: exception &e_r)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create new SystemBlastTool, error \"%s\"", e_r.what ());
		}

	return tool_p;
}


bool SystemBlastToolFactory :: AreToolsAsynchronous () const
{
	return false;
}


