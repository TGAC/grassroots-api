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
	ExternalBlastTool (Service *service_p);
	virtual ~ExternalBlastTool ();

	virtual bool ParseParameters (ParameterSet *params_p);

private:
	ByteBuffer *ebt_buffer_p;
	TempFile *ebt_input_p;
	TempFile *ebt_output_p;
};


#endif /* EXTERNAL_BLAST_TOOL_HPP_ */
