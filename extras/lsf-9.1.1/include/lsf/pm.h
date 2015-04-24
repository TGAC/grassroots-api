/* $RCSfile: pm.h,v $Revision: 1.3 $Date: 2013/01/09 03:11:08 $
 ****************************************************************************
 *
 * process manager library (pmlib)
 *
 * this header should be included by all programs calling pmlib
 *
 ****************************************************************************/

#ifndef _PMLIB_H
#define _PMLIB_H

#if defined(__STDC__) || defined(__cplusplus) || defined(SN_SVR4) || defined(WIN32)
#define P_(s) s
#include <stdarg.h>
#else
#define P_(s) ()
#include <varargs.h>
#endif

#include "pam.h"

typedef struct PM_Version {
    int major;
    int minor;
} PM_Version_t;

extern char *PM_RMI_URL_PAM;
extern char *PM_RMI_URL_RES;

extern void *pm_handle P_((void));
extern int pm_rmi P_((void *handle, char *url, char *handler_name, rmi_buffer_t *buffer));
extern int pm_init P_((void *handle, int *argc, char ***argv, char ***env,
        int *taskId, rmi_handler_t *hdlr_tbl, size_t hdlr_tbl_size,
        pm_comm_proto_t *comm_proto));
extern int PM_Send_Signal P_((void *handle, int rank, int signum, 
        void *context, int contextLen));
extern int PM_Sig_Context P_((void *handle, void *context, int *contextLen));
extern int PM_Sig_Unblock P_((void *handle, int signo));
extern void PM_AtExit P_((void *handle));
extern int PM_Close P_((void *handle, int fd));
extern int pm_spawn P_((void *handle, char *cmdline, char **envp, char *host, int *tid));
extern int pm_shutdown P_((void *handle, char *reason));
extern void pm_perror P_((void *handle, char *usrmsg));
extern char *pm_sperror P_((void *handle, char *usrmsg));
extern PM_Version_t *pm_version P_((void));
extern void PM_Pause P_((void *handle, int id, char *msg));
extern void PM_Sentinel_Push P_((void *handle, void *sentinelAddr,
        char *funcname));
extern void PM_Sentinel_Pop P_((void *handle, void *sentinelAddr,
        char *funcname));

void (*PM_Post_Signal_Handler P_((int signo, void (*handler)(int)))) P_((int));

extern int readVPGEntry P_((FILE *fp, char *host, int *, char *, char *));

#undef PM_ERROR
#define PM_ERROR(id, desc) \
    PM_ERR_ ## id,

typedef enum _PM_Error_t_ {
#include "errors.def.h"
    PM_ERR_LAST
} PM_Error_t;

extern PM_Error_t PM_Errno;



#define PM_Success 0
#define PM_Fail    -1

#endif /* _PMLIB_H */
