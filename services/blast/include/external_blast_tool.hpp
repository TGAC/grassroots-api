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
 * A class that will run Blast using an external application such as blastn
 * rather than calling Blast library routines directly.
 */
class BLAST_SERVICE_LOCAL ExternalBlastTool : public BlastTool
{
public:
	/**
	 * This is the string that will get appended to each
	 * input file used by the ExternalBlastTool.
	 */
	static const char * const EBT_INPUT_SUFFIX_S;

	/**
	 * This is the string that will get appended to each
	 * log file created when an ExternalBlastTool is ran.
	 */
	static const char * const EBT_LOG_SUFFIX_S;

	ExternalBlastTool (ServiceJob *job_p, const char *name_s, const BlastServiceData *data_p, const char *blast_program_name_s);

	/**
	 * The ExternalBlastTool destructor.
	 */
	virtual ~ExternalBlastTool ();

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


	virtual bool SetInputFilename (const char * const filename_s);

	virtual bool SetUpOutputFile ();

	/**
	 * Get the results after the ExternalBlastTool has finished
	 * running.
	 *
	 * @param formatter_p The BlastFormatter to convert the results
	 * into a different format. If this is 0, then the results will be
	 * returned without any conversion.
	 * @return The results as a c-style string or 0 upon error.
	 */
	virtual char *GetResults (BlastFormatter *formatter_p);


protected:
	ByteBuffer *ebt_buffer_p;
	TempFile *ebt_output_p;
	const char *ebt_working_directory_s;
	const char *ebt_blast_s;
	uint32 ebt_output_format;

	//const char *GetOutputData ();

	virtual bool AddArg (const char * const arg_s);
	virtual bool AddArgsPair (const char *key_s, const char *value_s);
	virtual bool AddArgsPairFromIntegerParameter (const ParameterSet *params_p, const Tag tag, const char *key_s, const bool unsigned_flag, const bool required_flag);

	char *GetJobFilename (const char * const prefix_s, const char * const suffix_s);


};


#endif /* EXTERNAL_BLAST_TOOL_HPP_ */

