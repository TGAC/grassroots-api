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

class ExternalBlastTool;

typedef bool (ExternalBlastTool :: *ParseArgCallback) (const char *arg_s);

/**
 * A class that will run Blast as a forked process.
 */
class BLAST_SERVICE_LOCAL ExternalBlastTool : public BlastTool
{
public:
	ExternalBlastTool (ServiceJob *job_p, const char *name_s);
	virtual ~ExternalBlastTool ();

	virtual bool ParseParameters (ParameterSet *params_p);

protected:
	ByteBuffer *ebt_buffer_p;
	TempFile *ebt_input_p;
	TempFile *ebt_output_p;
	ParseArgCallback ebt_arg_callback;

	const char *GetOutputData ();

	bool AddArgToInternalBuffer (const char * const arg_s);
};


#endif /* EXTERNAL_BLAST_TOOL_HPP_ */
