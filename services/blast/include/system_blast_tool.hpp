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
	/**
	 * Create a SystemBlastTool for a given ServiceJob.
	 *
	 * @param service_job_p The ServiceJob to associate with this SystemBlastTool.
	 * @param name_s The name to give to this SystemBlastTool.
	 * @param data_p The BlastServiceData for the Service that will run this SystemBlastTool.
	 * @param blast_program_name_s The name of blast command line executable that this SystemBlastTool
	 * will call to run its blast job.
	 */
	SystemBlastTool (ServiceJob *service_job_p, const char *name_s, const BlastServiceData *data_p, const char *blast_program_name_s);

	/**
	 * The SystemBlastTool destructor.
	 */
	virtual ~SystemBlastTool ();

	/**
	 * Parse a ParameterSet to configure a BlastTool prior
	 * to it being ran.
	 *
	 * @param params_p The ParameterSet to parse.
	 * @return <code>true</code> if the BlastTool was configured
	 * successfully and is ready to be ran, <code>false</code>
	 * otherwise.
	 * @see BlastTool::ParseParameters
	 */
	virtual bool ParseParameters (ParameterSet *params_p);


	/**
	 * Run this BlastTool
	 *
	 * @return The OperationStatus of this BlastTool after
	 * it has started running.
	 */
	virtual OperationStatus Run ();
};




#endif /* SYSTEM_BLAST_TOOL_HPP_ */
