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
 * drmaa_blast_tool.hpp
 *
 *  Created on: 27 Apr 2015
 *      Author: billy
 */

#ifndef DRMAA_BLAST_TOOL_HPP_
#define DRMAA_BLAST_TOOL_HPP_

#include "external_blast_tool.hpp"
#include "drmaa_tool.h"

/**
 * A class that will run Blast as a drmaa process.
 */
class BLAST_SERVICE_LOCAL DrmaaBlastTool : public ExternalBlastTool
{
public:

	DrmaaBlastTool (ServiceJob *service_job_p, const char *name_s, const char *working_directory_s, const char *blast_program_name_s, bool async_flag);
	virtual ~DrmaaBlastTool ();

	virtual OperationStatus Run ();

	virtual OperationStatus GetStatus ();

	void SetCoresPerSearch (uint32 cores);

	bool SetEmailNotifications (const char **email_addresses_ss);

	virtual bool SetUpOutputFile ();

private:
	DrmaaTool *dbt_drmaa_tool_p;
	bool dbt_async_flag;
};


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get a newly created BlastTool
 *
 * @return The BlastTool or <code>NULL</code> upon error.
 */
BLAST_SERVICE_API BlastTool *CreateDrmaaBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s);


#ifdef __cplusplus
}
#endif


#endif /* DRMAA_BLAST_TOOL_HPP_ */
