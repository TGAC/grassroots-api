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
 * blast_tool_factory.cpp
 *
 *  Created on: 24 Feb 2016
 *      Author: tyrrells
 */

#include <cstring>

#include "blast_service.h"
#include "blast_tool_factory.hpp"
#include "streams.h"

#include "drmaa_blast_tool_factory.hpp"
#include "system_blast_tool_factory.hpp"


BlastToolFactory *BlastToolFactory :: GetBlastToolFactory (const json_t *service_config_p)
{
	BlastToolFactory *factory_p = 0;
	const char *value_s = GetJSONString (service_config_p, BS_TOOL_TYPE_NAME_S);


	if (value_s)
		{
			if (strcmp (value_s, "drmaa") == 0)
				{
					#ifdef DRMAA_ENABLED
					factory_p = new DrmaaBlastToolFactory;

					if (!factory_p)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create DrmaaBlastToolFactory");
						}
					#endif
				}

		}


	if (!factory_p)
		{
			factory_p = new SystemBlastToolFactory;
		}


	return factory_p;

}


BlastToolFactory :: ~BlastToolFactory ()
{

}

