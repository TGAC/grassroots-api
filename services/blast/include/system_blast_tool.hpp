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
 * system_blast_tool.hpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#ifndef SYSTEM_BLAST_TOOL_HPP_
#define SYSTEM_BLAST_TOOL_HPP_

#include "external_blast_tool.hpp"

/**
 * A class that will run Blast as a system process.
 */
class BLAST_SERVICE_LOCAL SystemBlastTool : public ExternalBlastTool
{
public:
	SystemBlastTool (ServiceJob *service_job_p, const char *name_s, const char *working_directory_s, const char *blast_program_name_s);
	virtual ~SystemBlastTool ();

	virtual OperationStatus Run ();

	virtual bool ParseParameters (ParameterSet *params_p, const char *filename_s);

	virtual const char *GetResults ();
};




#endif /* SYSTEM_BLAST_TOOL_HPP_ */
