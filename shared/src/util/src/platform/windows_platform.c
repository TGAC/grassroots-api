/*
** Copyright 2014-2016 The Earlham Institute
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

#include <windows.h>

#include "platform.h"

bool InitPlatform ()
{
	
	#if defined _DEBUG && defined _CRTDBG_MAP_ALLOC
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/* Set any memory allocation breakpoints here */
	_crtBreakAlloc = 608;
	#endif

	return true;
}

void ExitPlatform ()
{
}



uint32 Snooze (uint32 milliseconds)
{
	Sleep (milliseconds);

	return 0;
}
