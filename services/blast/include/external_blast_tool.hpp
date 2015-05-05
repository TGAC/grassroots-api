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
	ExternalBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s);
	virtual ~ExternalBlastTool ();

	virtual bool ParseParameters (ParameterSet *params_p, const char *filename_s);

protected:
	ByteBuffer *ebt_buffer_p;
	TempFile *ebt_output_p;
	const char *ebt_working_directory_s;

	const char *GetOutputData ();

	virtual bool AddArg (const char * const arg_s);
};


#endif /* EXTERNAL_BLAST_TOOL_HPP_ */
