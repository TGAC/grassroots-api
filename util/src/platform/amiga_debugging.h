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

