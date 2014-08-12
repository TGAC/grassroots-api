#include "platform.h"

BOOLEAN InitPlatform ()
{
	
	#if defined _DEBUG && defined _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/* Set any memory allocation breakpoints here */
	_crtBreakAlloc = 608;
	#endif

	return TRUE;
}

void ExitPlatform ()
{}

