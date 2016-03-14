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

	try
		{
			factory_p = new DrmaaBlastToolFactory (service_config_p);
		}
	catch (std :: bad_alloc &alloc_r)
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate DrmaaBlastToolFactory");
		}

	return factory_p;
}



DrmaaBlastToolFactory :: DrmaaBlastToolFactory (const json_t *service_config_p)
	: ExternalBlastToolFactory (service_config_p)
{

}


DrmaaBlastToolFactory :: ~DrmaaBlastToolFactory ()
{

}


BlastTool *DrmaaBlastToolFactory :: CreateBlastTool (ServiceJob *job_p, const char *name_s, const BlastServiceData *data_p)
{
	DrmaaBlastTool *drmaa_tool_p = 0;
	bool async_flag = true;

	try
		{
			drmaa_tool_p = new DrmaaBlastTool (job_p, name_s, data_p, ebtf_program_name_s, async_flag);
		}
	catch (std :: exception &ex_r)
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create drmaa blast tool, error \"%s\"", ex_r.what ());
		}

	if (drmaa_tool_p)
		{
			int i = 0;
			const char *value_s = NULL;

			/* Set the number of cores per job */
			if (GetJSONInteger (data_p -> bsd_base_data.sd_config_p, "drmaa_cores_per_search", &i))
				{
					drmaa_tool_p -> SetCoresPerSearch (i);
				}

			/* Set up any email notifications */
			value_s = GetJSONString (data_p -> bsd_base_data.sd_config_p, "email_notifications");

			if (value_s)
				{
					const char **addresses_ss = (const char **) AllocMemoryArray (2, sizeof (const char *));

					if (addresses_ss)
						{
							*addresses_ss = value_s;
						}

					if (! (drmaa_tool_p -> SetEmailNotifications (addresses_ss)))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set email notifications for drmaa tool");
						}

					FreeMemory (addresses_ss);
				}		/* if (json_p) */


			/* Set the queue to use */
			value_s = GetJSONString (data_p -> bsd_base_data.sd_config_p, "queue");
			if (value_s)
				{
					if (!SetDrmaaToolQueueName (drmaa_tool_p, value_s))
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "SetDrmaaToolQueueName failed to set name to \"%s\"", value_s);
						}
				}


		}

	return drmaa_tool_p;
}


bool DrmaaBlastToolFactory :: AreToolsAsynchronous () const
{
	return true;
}


