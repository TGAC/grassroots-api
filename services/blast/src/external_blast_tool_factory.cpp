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
 * external_blast_tool_factory.cpp
 *
 *  Created on: 25 Feb 2016
 *      Author: tyrrells
 */

#include <stdexcept>

#include "external_blast_tool_factory.hpp"
#include "blast_service.h"


ExternalBlastToolFactory :: ExternalBlastToolFactory (const json_t *config_p)
{
	ebtf_program_name_s = GetJSONString (config_p, BS_COMMAND_NAME_S);

	if (!ebtf_program_name_s)
		{
			ebtf_program_name_s = "blastn";
		}
}


ExternalBlastToolFactory :: ~ExternalBlastToolFactory ()
{

}

