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
#ifdef __cplusplus
		extern "C" {
#endif

#ifndef DEBUGGING_UTILS_H
#define DEBUGGING_UTILS_H


#include <clib/debug_protos.h>

#ifdef __amigaos4__
	#include <proto/exec.h>
	#define KPRINTF IExec->DebugPrintF
//#define	KPRINTF(format, args...)((struct ExecIFace *) ((*(struct ExecBase **)4)->MainInterface))->DebugPrintF(format, ## args)	
#else
	#define KPRINTF KPrintF
#endif

#ifdef DEBUG
	#define DB(X)	(X)
#else
	#define DB(X)
#endif


#endif	/* #ifndef DEBUGGING_UTILS_H */

#ifdef __cplusplus
}
#endif

