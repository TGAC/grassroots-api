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

#include "msParam.h"

/**
 * The base class for running Blast.
 */
class BlastTool
{
public:

	BlastTool ();

	virtual ~BlastTool ();

	virtual bool Run () = 0;

	/**
	 * Add an argument to Blast prior to its run.
	 * 
	 * @param arg_s The argument to add.
	 * @param newly_allocated_flag If arg_s uses a memory allocation outside of iRods management
	 * set this to <code>true</code> and this BlastTool destructor will deallocate it.
	 */ 
	void AddArgument (char *arg_s, bool newly_allocated_flag);

	void PringArgsToLog ();

	
	void PreRun ();
	
	void PostRun ();

	

protected:
	bool bt_running_flag;


	/** All of the command line arguments */
	std :: vector <char *> bt_command_line_args;

	/** 
	 * The arguments that were newly allocated and will
	 * need freeing to ensure there are not any memory
	 * leaks.
	 */
	std :: vector <char *> bt_allocated_args;
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
 * A class that will run Blast as a forked process.
 */
class ForkedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


/**
 * A class that will run Blast within the main process
 * of the iRods server.
 */
class InlineBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


/**
 * A class that will run Blast in a separate thread on
 * the iRods server.
 */
class ThreadedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


/**
 * A class that will run Blast as a job sumbmission 
 * script for the HPC or similar.
 */
class QueuedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
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
BlastTool *CreateBlastTool ();


/**
 * Free a BlastTool
 * 
 * @param The BlastTool to deallocate.
 */
void FreeBlastTool (BlastTool *tool_p);


/**
 * Convert the parameters passed in by iRods into a format to 
 * be used by the BlastTool.
 * 
 * @param tool_p The BlastTool to use.
 * @param params_p The array of iRods parameters.
 * @return <code>true</code> if the parameters were converted successfully, <code>false</code>
 * otherwise.
 */
bool ConvertArgumentsArray (BlastTool *tool_p, msParamArray_t *params_p);


bool ConvertKeyValueArgument (BlastTool *tool_p, msParam_t *param_p);

/**
 * Run Blast using the parameters that have been previously using ConvertArguments.
 * 
 * @param tool_p The BlastTool to use.
 * @return <code>true</code> if the tool completed successfully, <code>false</code>
 * otherwise.
 */
bool RunBlast (BlastTool *tool_p);


bool IsBlastRunning (BlastTool *tool_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BLAST_TOOL_HPP */

