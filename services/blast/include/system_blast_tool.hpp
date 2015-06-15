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
