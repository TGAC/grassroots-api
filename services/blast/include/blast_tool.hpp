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
#ifndef BLAST_TOOL_HPP
#define BLAST_TOOL_HPP

#include <vector>

#include "blast_service_api.h"

#include "byte_buffer.h"

#include "service_job.h"
#include "blast_formatter.h"


/* forward declaration */
struct BlastServiceData;

/**
 * The base class for running Blast.
 */
class BLAST_SERVICE_LOCAL BlastTool
{
public:

	BlastTool (ServiceJob *job_p, const char *name_s, const BlastServiceData *data_p);

	virtual ~BlastTool ();

	virtual OperationStatus Run () = 0;

	virtual bool ParseParameters (ParameterSet *params_p) = 0;

	virtual bool SetInputFilename (const char * const filename_s) = 0;

	virtual bool SetUpOutputFile () = 0;

	void PreRun ();
	
	void PostRun ();

	virtual OperationStatus GetStatus ();

	virtual const char *GetResults (BlastFormatter *formatter_p) = 0;

	virtual void ClearResults () = 0;

	const uuid_t &GetUUID () const;

	const char *GetName () const;

	static void SetBlastToolType (const char *type_s);

	static const char *bt_tool_type_s;

protected:
	OperationStatus bt_status;
	const char *bt_name_s;
	ServiceJob *bt_job_p;
	const BlastServiceData *bt_service_data_p;
};


/*
* I can see that potentially we could use a variety of methods:
* 
* 1. Fork a new process.
* 2. Create a DBApplication and run it inline.
* 3. Create a DBApplication and run it in a separate thread using pthreads or similar.
* 4. Create a job submission script and call it, this would be on HPC or similar.
* 
* So we need a way to call any of these and check on their status, the obvious answer
* is subclassing with virtual calls.
*/


/**
 * A class that will run Blast within the main process
 * of the iRods server.
 */
/*
class BLAST_SERVICE_LOCAL InlineBlastTool : public BlastTool 
{
public:
	
	virtual OperationStatus Run ();
};
*/

/**
 * A class that will run Blast in a separate thread on
 * the iRods server.
 */
/*
class BLAST_SERVICE_LOCAL ThreadedBlastTool : public BlastTool 
{
public:
	
	virtual OperationStatus Run ();
};
*/


#ifdef __cplusplus
extern "C" 
{
#endif


/**
 * Get a newly created BlastTool
 * 
 * @return The BlastTool or <code>NULL</code> upon error.
 */
BLAST_SERVICE_API BlastTool *CreateBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s);


/**
 * Free a BlastTool
 * 
 * @param The BlastTool to deallocate.
 */
BLAST_SERVICE_API void FreeBlastTool (BlastTool *tool_p);



/**
 * Run Blast using the parameters that have been previously using ConvertArguments.
 * 
 * @param tool_p The BlastTool to use.
 * @return <code>true</code> if the tool completed successfully, <code>false</code>
 * otherwise.
 */
BLAST_SERVICE_API OperationStatus RunBlast (BlastTool *tool_p);


BLAST_SERVICE_API OperationStatus GetBlastStatus (BlastTool *tool_p);


BLAST_SERVICE_API bool IsBlastToolSynchronous ();


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BLAST_TOOL_HPP */

