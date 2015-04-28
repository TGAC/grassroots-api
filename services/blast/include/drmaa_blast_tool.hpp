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
	DrmaaBlastTool (ServiceJob *service_job_p, const char *name_s, const char *working_directory_s);
	virtual ~DrmaaBlastTool ();

	virtual OperationStatus Run ();

	virtual const char *GetResults ();

private:
	DrmaaTool *dbt_drmaa_tool_p;

	bool AddArgToDrmaaTool (const char * const arg_s);

};


#endif /* DRMAA_BLAST_TOOL_HPP_ */
