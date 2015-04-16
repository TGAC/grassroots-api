#ifndef BLAST_TOOL_HPP
#define BLAST_TOOL_HPP

#include <vector>

#include "blast_service.h"


/**
 * The base class for running Blast.
 */
class BLAST_SERVICE_LOCAL BlastTool
{
public:

	BlastTool ();

	virtual ~BlastTool ();

	virtual bool Run () = 0;

	virtual bool ParseParameters (ParameterSet *params_p) = 0;

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

	char *bt_temp_input_filename_s;
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
class BLAST_SERVICE_LOCAL ForkedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


/**
 * A class that will run Blast within the main process
 * of the iRods server.
 */
class BLAST_SERVICE_LOCAL InlineBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


/**
 * A class that will run Blast in a separate thread on
 * the iRods server.
 */
class BLAST_SERVICE_LOCAL ThreadedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


/**
 * A class that will run Blast as a job sumbmission 
 * script for the HPC or similar.
 */
class BLAST_SERVICE_LOCAL QueuedBlastTool : public BlastTool 
{
public:

	virtual bool ParseParameters (ParameterSet *params_p);

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
BLAST_SERVICE_API BlastTool *CreateBlastTool ();


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
BLAST_SERVICE_API bool RunBlast (BlastTool *tool_p);


BLAST_SERVICE_API bool IsBlastRunning (BlastTool *tool_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BLAST_TOOL_HPP */

