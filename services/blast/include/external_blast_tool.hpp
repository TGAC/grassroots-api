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


	/**
	 * Create a ExternalBlastTool for a given ServiceJob.
	 *
	 * @param job_p The ServiceJob to associate with this ExternalBlastTool.
	 * @param name_s The name to give to this ExternalBlastTool.
	 * @param data_p The BlastServiceData for the Service that will run this ExternalBlastTool.
	 * @param blast_program_name_s The name of blast command line executable that this ExternalBlastTool
	 * will call to run its blast job.
	 */
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


	/**
	 * Set the input filename for the BlastTool to use.
	 *
	 * @param filename_s The filename.
	 * @return <code>true</code> if the input filename was set
	 * successfully, <code>false</code> otherwise.
	 */
	virtual bool SetInputFilename (const char * const filename_s);


	/**
	 * Set the output file parameter.
	 *
	 * @return <code>true</code> if the input filename was set
	 * successfully, <code>false</code> otherwise.
	 */
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
	/**
	 *
	 * This is the buffer used to build all of the command line
	 * arguments for the executable used to run the blast job.
	 */
	ByteBuffer *ebt_buffer_p;

	/**
	 *
	 * The TempFile read to get the query sequence
	 * to use as input to the BlastTool.
	 */
	TempFile *ebt_output_p;

	/**
	 *
	 * The working directory where all of the
	 * input, output and log files will be
	 * created.
	 */
	const char *ebt_working_directory_s;


	/**
	 *
	 * The name of the blast command line
	 * executable to use.
	 */
	const char *ebt_blast_s;

	/**
	 *
	 * The output format code to use.
	 */
	uint32 ebt_output_format;

	//const char *GetOutputData ();

	/**
	 * Add the argument to the command line arguments
	 * that this BlastTool will run with.
	 *
	 * @param arg_s The argument to add.
	 * @return <code>true</code> if the argument was added
	 * successfully, <code>false</code> otherwise.
	 */
	virtual bool AddArg (const char * const arg_s);

	/**
	 * Add a pair of arguments to the command line arguments
	 * that this BlastTool will run with.
	 *
	 * @param key_s The first argument to add.
	 * @param value_s The second argument to add.
	 * @return <code>true</code> if the arguments were added
	 * successfully, <code>false</code> otherwise.
	 */
	virtual bool AddArgsPair (const char *key_s, const char *value_s);

	/**
	 * Get the value of an integer-based Parameter and add it as key-value pair
	 * to the command line arguments
	 * that this BlastTool will run with.
	 *
	 * @param params_p The ParameterSet to get the Parameter from.
	 * @param tag The Tag for desired Parameter.
	 * @param key_s The key to use when creating the command line arguments.
	 * @param unsigned_flag Is the parameter value unsigned or not?
	 * @param required_flag If this is <code>true</code> then failure to find
	 * the Parameter will cause this function to return <code>false</code>. If the
	 * value is optional, then set this to <code>false</code>.
	 * @return <code>true</code> if the arguments were added
	 * successfully, <code>false</code> otherwise.
	 */
	virtual bool AddArgsPairFromIntegerParameter (const ParameterSet *params_p, const Tag tag, const char *key_s, const bool unsigned_flag, const bool required_flag);


	/**
	 * Create the filename to use for a ServiceJob.
	 *
	 * @param prefix_s The prefix to use for the filename.
	 * @param suffix_s The suffix to use from a set of constants.
	 * @return The filename or 0 upon error.
	 * @see EBT_INPUT_SUFFIX_S
	 * @see EBT_LOG_SUFFIX_S
	 * @see BS_OUTPUT_SUFFIX_S
	 */
	char *GetJobFilename (const char * const prefix_s, const char * const suffix_s);
};


#endif /* EXTERNAL_BLAST_TOOL_HPP_ */

