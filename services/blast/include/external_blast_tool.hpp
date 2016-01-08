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
 * external_blast_tool.hpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#ifndef EXTERNAL_BLAST_TOOL_HPP_
#define EXTERNAL_BLAST_TOOL_HPP_

#include "blast_tool.hpp"
#include "byte_buffer.h"
#include "temp_file.hpp"


/**
 * A class that will run Blast as a forked process.
 */
class BLAST_SERVICE_LOCAL ExternalBlastTool : public BlastTool
{
public:
	static const char * const EBT_INPUT_SUFFIX_S;
	static const char * const EBT_LOG_SUFFIX_S;

	ExternalBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s, const char *blast_program_name_s);
	virtual ~ExternalBlastTool ();

	virtual bool ParseParameters (ParameterSet *params_p);

	virtual bool SetInputFilename (const char * const filename_s);

	virtual bool SetUpOutputFile ();

	virtual const char *GetResults ();

	virtual void ClearResults ();

protected:
	ByteBuffer *ebt_buffer_p;
	TempFile *ebt_output_p;
	const char *ebt_working_directory_s;
	const char *ebt_blast_s;

	//const char *GetOutputData ();

	virtual bool AddArg (const char * const arg_s);
	virtual bool AddArgsPair (const char *key_s, const char *value_s);
	virtual bool AddArgsPairFromIntegerParameter (const ParameterSet *params_p, const Tag tag, const char *key_s, const bool unsigned_flag);

	char *GetJobFilename (const char * const prefix_s, const char * const suffix_s);


};


#endif /* EXTERNAL_BLAST_TOOL_HPP_ */

