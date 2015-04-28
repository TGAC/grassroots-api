/*
 * blast_tool_set.hpp
 *
 *  Created on: 23 Apr 2015
 *      Author: tyrrells
 */

#ifndef BLAST_TOOL_SET_HPP_
#define BLAST_TOOL_SET_HPP_

#include <vector>
#include "blast_tool.hpp"

using namespace std;

class BLAST_SERVICE_LOCAL BlastToolSet
{
public:
	BlastToolSet (Service *service_p);
	~BlastToolSet ();

	void AddTool (BlastTool *tool_p);

	BlastTool *GetNewBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s);

	BlastTool *GetBlastTool (const uuid_t id);

private:
	vector <BlastTool *> bts_tools;
	Service *bts_service_p;
};


#ifdef __cplusplus
extern "C"
{
#endif


/**
 * Get a newly created BlastTool
 *
 * @return The BlastTool or <code>NULL</code> upon error.
 */
BLAST_SERVICE_API BlastToolSet *CreateBlastToolSet (Service *service_p);


/**
 * Free a BlastTool
 *
 * @param The BlastTool to deallocate.
 */
BLAST_SERVICE_API void FreeBlastToolSet (BlastToolSet *tool_p);


#ifdef __cplusplus
}
#endif


#endif /* BLAST_TOOL_SET_HPP_ */