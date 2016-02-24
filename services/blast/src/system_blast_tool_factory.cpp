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


#include "system_blast_tool_factory.hpp"
#include "system_blast_tool.hpp"



SystemBlastToolFactory *SystemBlastToolFactory :: CreateSystemBlastToolFactory (const json_t *service_config_p)
{

	return 0;
}


SystemBlastToolFactory :: ~SystemBlastToolFactory ()
{

}


BlastTool *SystemBlastToolFactory :: CreateBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s)
{
	BlastTool *tool_p = 0;
	const BlastServiceData *data_p = 0;
	const char *program_s = 0;

	tool_p = new SystemBlastTool (job_p, name_s, data_p, working_directory_s, program_s);

	return tool_p;
}


bool SystemBlastToolFactory :: AreToolsAsynchronous () const
{
	return false;
}


