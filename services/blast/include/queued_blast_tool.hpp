/*
 * queued_blast_tool.hpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#ifndef QUEUED_BLAST_TOOL_HPP_
#define QUEUED_BLAST_TOOL_HPP_

#include "external_blast_tool.hpp"

/**
 * A class that will run Blast as an LSF job.
 */
class BLAST_SERVICE_LOCAL QueuedBlastTool : public ExternalBlastTool
{
public:
	QueuedBlastTool (Service *service_p, const char *name_s);
	virtual ~QueuedBlastTool ();

	virtual OperationStatus Run ();
};



#endif /* QUEUED_BLAST_TOOL_HPP_ */
