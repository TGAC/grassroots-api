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
 * drmaa_blast_tool_factory.hpp
 *
 *  Created on: 24 Feb 2016
 *      Author: tyrrells
 */

#ifndef SERVICES_BLAST_INCLUDE_DRMAA_BLAST_TOOL_FACTORY_HPP_
#define SERVICES_BLAST_INCLUDE_DRMAA_BLAST_TOOL_FACTORY_HPP_


#include "blast_tool_factory.hpp"

/**
 * The base class for generating system blast tools
 */
class BLAST_SERVICE_LOCAL DrmaaBlastToolFactory : public BlastToolFactory
{
public:
	static DrmaaBlastToolFactory	*CreateDrmaaBlastToolFactory (const json_t *service_config_p);

	virtual ~DrmaaBlastToolFactory ();

	virtual BlastTool *CreateBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s);

	virtual bool AreToolsAsynchronous () const;

};


#endif /* SERVICES_BLAST_INCLUDE_DRMAA_BLAST_TOOL_FACTORY_HPP_ */
