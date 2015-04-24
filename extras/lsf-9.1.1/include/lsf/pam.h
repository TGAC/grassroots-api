/* $RCSfile: pam.h,v $Revision: 1.13 $Date: 2013/01/09 03:11:05 $ */

#ifndef _PAM_H_
#define _PAM_H_

#include <sys/types.h>
/* #include <lsf/lsf.h> */

#define PAM_VERSION			2

#define PAM_VCL_OVERRIDE_ST_TASKS	0x01
#define PAM_VCL_OUT_PARAM_PENDING	0x02
#define PAM_VCL_QUIET_OUTPUT		0x04
#define PAM_VCL_TAGSTDOUT_ON            0x08
#define PAM_VCL_QUIET_REPORT            0x10 /* report is not printed if true */

#include "pm_msg.h"

#if defined(SOLARIS)
#include <dlfcn.h> 
#endif

#if defined(hpux) || defined(__hpux)
#include <dl.h>
#elif defined(_AIX) || defined (__hpux) || defined(CRAYX1)
#elif defined(_MACOS_X_)
#include "../../core/lib/dlfcn_mac.h"
#elif defined(WIN32)
#else
#include <dlfcn.h>
#endif /* hpux */

#ifndef _LSF_H_ /* defined in lsf.h as well */ 

#include "../../lsf/lib/lproto.h"
typedef struct _task_params {
    char *cmdline ;
    char *node ;
    char **env ;
} ls_task_params_t ;

typedef struct _task {
    ls_task_params_t *params ;
    pid_t pid ;
    unsigned int taskId ;
} ls_task_t ;


typedef struct _rmi_buffer {
    char *invoker_url;
    char *storage;
    size_t size;
} rmi_buffer_t ;

typedef int (*rmi_handler_func_t)(rmi_buffer_t *buffer) ;

typedef struct _rmi_handler {
    char *handler_name ;
    rmi_handler_func_t handler ;
    char *desc ; 
} rmi_handler_t ;
#endif /* !_LSF_H_ */

typedef int (*pm_comm_callback_func_t)(void *handle, void *arg);

typedef struct _comm_proto {
    int                      fd;
    void                     *callback_arg;
    pm_comm_callback_func_t  read;
    pm_comm_callback_func_t  write;
    pm_comm_callback_func_t  except;
} pm_comm_proto_t;

#endif /* ! _PAM_H_ */
