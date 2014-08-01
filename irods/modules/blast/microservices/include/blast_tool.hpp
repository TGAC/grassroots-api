#ifndef BLAST_TOOL_HPP
#define BLAST_TOOL_HPP

#include <vector>

#include "msParam.h"

/*
typedef struct BlastInterface 
{
	int bi_num_args;
	char **bi_args_ss;	
} BlastInterface;
*/

class BlastTool
{
public:

	BlastTool ();

	virtual ~BlastTool ();

	virtual bool Run () = 0;

	void AddArgument (char *arg_s, bool newly_allocated_flag);

	void PringArgsToLog ();

private:
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

class ForkedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


class InlineBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


class ThreadedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};


class QueuedBlastTool : public BlastTool 
{
public:
	
	virtual bool Run ();	
};



#ifdef __cplusplus
extern "C" 
{
#endif


BlastTool *CreateBlastTool ();

void FreeBlastTool (BlastTool *tool_p);

/**
 * Convert the iRods arguments into an argc, argv set 
 */
bool ConvertArguments (BlastTool *tool_p, msParamArray_t *params_p);

bool RunBlast (BlastTool *tool_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BLAST_TOOL_HPP */

