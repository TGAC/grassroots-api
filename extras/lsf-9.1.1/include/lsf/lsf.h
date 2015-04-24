/*
 * Copyright International Business Machines Corp,1992-2013.
 */
#ifndef _LSF_H_
#define _LSF_H_

/* $RCSfile: base.h,v $Revision: 1.25.6.12.2.5.2.11.2.15.4.3.4.34.2.53 $Date: 2013/02/21 11:01:11 $
 ****************************************************************************
 * Copyright International Business Machines Corp,2005-2013.
 *
 * Load Sharing Facility
 *
 * Header file for all components of load sharing facility.
 *
 ****************************************************************************/

#ifndef _EGO_H_
#define _EGO_H_

#if defined (__crayx1)
#if !defined (CRAYX1)
#define CRAYX1 1
#endif
#endif

#include <stdio.h>

#if !defined (WIN32)
#include <sys/wait.h>
#endif

#if !defined(PASSWD_FILE_LS)
#define PASSWD_FILE_LS "passwd.lsfuser"
#endif
#if !defined(PASSWORD_LEN)
#define PASSWORD_LEN 64
#endif
#if defined(WIN32)
#define RECORD_LEN 128
#ifndef  FD_SETSIZE
#define FD_SETSIZE 2048
#endif
/* NB: windows.h MUST be included after FD_SETSIZE is defined! */ 
/* winsock2.h must be included before windows.h */
#include <winsock2.h>
/* avoid the confilict to "WSPiApi.h" in the Windows Platform SDK 2003 SP1,
 * which does not allow "extern C" before including the head in the cpp code.
 * we hope MS will fix the problem in the future.
 */
#ifdef WIN64
#define _WSPIAPI_COUNTOF(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif
#include <ws2tcpip.h>
#include <MSWSock.h>
#include <windows.h>

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#ifndef PID_T_WIN32
#define PID_T_WIN32
typedef long pid_t;
#endif
#else
#include <unistd.h>
#include <syslog.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
/* Ultrix sys/resource.h isn't safe for multiple inclusion... */
#if !defined(RUSAGE_SELF)
#if !defined(_CRAY) || defined (CRAYX1) 
#include <sys/resource.h>
#endif /* _CRAY */
#endif
#endif /* WIN32 */

#ifndef INT32JOBID
#ifdef WIN32
typedef __int64 LS_LONG_INT;
typedef unsigned __int64 LS_UNS_LONG_INT;
#define LS_LONG_FORMAT "%I64d"
#define LS_UNS_LONG_FORMAT "%I64u"
#elif defined (__alpha)
typedef long long int LS_LONG_INT;
typedef unsigned long long LS_UNS_LONG_INT;
#define LS_LONG_FORMAT ("%ld")
#define LS_UNS_LONG_FORMAT ("%lu")
#else
typedef long long int LS_LONG_INT;
typedef unsigned long long LS_UNS_LONG_INT;
#define LS_LONG_FORMAT ("%lld")
#define LS_UNS_LONG_FORMAT ("%llu")
#endif /* WIN32 */
#else
typedef int LS_LONG_INT;
#define LS_LONG_FORMAT ("%d")
#define LS_UNS_LONG_FORMAT ("%u")
#endif /* INT32JOBID */

#if defined(WIN32)
struct rlimit {
    long rlim_cur;
    long rlim_max;
};
/* EXTERNAL_RUSAGE flags is used to disable rusage stuct when
   building symphony because rusage is defined in ACE which symphony
   uses.  This will have no effect on normal utopia builds.
*/
#if !(defined(EXTERNAL_RUSAGE))
struct  rusage {
        struct timeval ru_utime;        /* user time used */
        struct timeval ru_stime;        /* system time used */

        /* Following is retained for compatibility with other platforms */
        long    ru_maxrss;
#define ru_first        ru_ixrss
        long    ru_ixrss;               /* integral shared memory size */
        long    ru_idrss;               /* integral unshared data " */
        long    ru_isrss;               /* integral unshared stack " */
        long    ru_minflt;              /* page reclaims */
        long    ru_majflt;              /* page faults */
        long    ru_nswap;               /* swaps */
        long    ru_inblock;             /* block input operations */
        long    ru_oublock;             /* block output operations */
        long    ru_msgsnd;              /* messages sent */
        long    ru_msgrcv;              /* messages received */
        long    ru_nsignals;            /* signals received */
        long    ru_nvcsw;               /* voluntary context switches */
        long    ru_nivcsw;              /* involuntary " */
#define ru_last         ru_nivcsw
        long    ru_ioch;                /* # of characters read/written */
};
#endif

#define RLIM_INFINITY   0x7fffffff

#ifndef LOG_EMERG
#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* critical conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_WARNING     4       /* warning conditions */
#define LOG_NOTICE      5       /* normal but signification condition */
#define LOG_INFO        6       /* informational */
#define LOG_DEBUG       7       /* debug-level messages */
#endif

#if 0
#ifndef _TIME_T
#define _TIME_T
#ifdef WIN64
typedef __int64 time_t;               /* For times in seconds */
#else
typedef long time_t;               /* For times in seconds */
#endif
#endif
#else
#include <io.h>
#endif /* _TIME_T */

#ifndef _OFF_T
#define _OFF_T
typedef long off_t;                /* For file offsets and sizes */
#endif /* _OFF_T */

#endif /* WIN32 */

#ifdef SOLARIS
#if defined(SOLARIS2_5) || defined(SOLARIS2_7)
#include <sys/resource.h>
#else
#include <sys/rusage.h>
#endif
#include <sys/sysmacros.h>
#endif /* SOLARIS */

/*Only applied to Solaris 2.5 and below*/
#if defined(SNPRINTF4SOL2) 
#include "intlib/snprintf4sol2.h"
#endif

#ifdef SN_SVR4
#include <sys/sysmacros.h>
#include <sys/resource.h>
#endif /* SN_SVR4 */

#ifdef SVR4
#include <netdb.h>
#else
#if !defined(WIN32)
#include <sys/wait.h>
#endif /* WIN32 */
#endif /* SVR4 */

#if !defined(WIN32) && !defined(_WIN64)
#ifndef SOCKET
typedef int SOCKET;
#endif
#define INVALID_SOCKET -1
#define SOCK_INVALID(c) ((c) < 0 )
#else
#define SOCK_INVALID(c) ((c) == INVALID_SOCKET)
#endif

#if defined(_CRAY) && !defined(CRAYX1)
#ifndef MAXPATHLEN

#define MAXPATHLEN 1024
#endif /* maaxpathlen */
#ifndef CRAYT3E
struct rlimit {
    long rlim_cur;
    long rlim_max;
};
#endif /* !CRAYT3E */

/* cray doesn't have struct rusage but and struct jtab is similar. */
#include <sys/jtab.h>
struct	rusage {
	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */

	/* Following is retained for compatibility with other platforms */
	long	ru_maxrss;
#define ru_first	ru_ixrss
	long	ru_ixrss;		/* integral shared memory size */
	long	ru_idrss;		/* integral unshared data " */
	long	ru_isrss;		/* integral unshared stack " */
	long	ru_minflt;		/* page reclaims */
	long	ru_majflt;		/* page faults */
	long	ru_nswap;		/* swaps */
	long	ru_inblock;		/* block input operations */
	long	ru_oublock;		/* block output operations */
	long	ru_msgsnd;		/* messages sent */
	long	ru_msgrcv;		/* messages received */
	long	ru_nsignals;		/* signals received */
	long	ru_nvcsw;		/* voluntary context switches */
	long	ru_nivcsw;		/* involuntary " */
#define ru_last		ru_nivcsw
	long    ru_ioch;                /* # of characters read/written */
};
#endif /* cray */


/* non-standard location for MAXHOSTNAMELEN definition on SCO */
#ifdef _M_XENIX
# include <sys/socket.h>
#endif /* _M_XENIX */

#if defined(LSF2_25) && !defined(LinuxQsNet)
#define _VERSION_STR_  "IBM Platform LSF/HPC AlphaServer SC V2.6 UK1"
#elif defined(DIST_HPC)
#define _VERSION_STR_  "IBM Platform LSF HPC 7.0 for Dist System"
#elif defined(OTTAWA)
#define _VERSION_STR_  "IBM Platform Lava 7.0"
#else
#define _VERSION_STR_  "IBM Platform LSF 9.1"
#endif

#ifdef LSF_WORKGROUP
#define _WORKGROUP_STR_  " Workgroup Edition"
#else
#define _WORKGROUP_STR_ ""
#endif

#ifdef MINOR_VERSION
#define _MINOR_STR_  "."MINOR_VERSION
#else
#define _MINOR_STR_  ""
#endif

#ifdef FOURTH_VERSION
#define _FOURTH_STR_  "."FOURTH_VERSION
#else
#define _FOURTH_STR_  ""
#endif

#ifdef BUILD_NUMBER
#define _BUILD_STR_  " build "BUILD_NUMBER 
#else 
#define _BUILD_STR_   "" 
#endif

#ifdef BUILD_NOTE
#define _NOTE_STR_  "  notes: "BUILD_NOTE"\n" 
#else 
#define _NOTE_STR_   "" 
#endif

#ifdef HOT_FIX
#define _HOTFIX_STR_  "  fixes: "HOT_FIX"\n" 
#else 
#define _HOTFIX_STR_   "" 
#endif

#ifdef BUILD_OS
#define _OS_STR_  "  binary type: "BUILD_OS"\n" 
#else 
#define _OS_STR_   "" 
#endif

#if defined(__STDC__) || defined(WIN32)

#ifdef REL_DATE
#define _DATE_STR_  REL_DATE 
#else /* REL_DATE */
#define _DATE_STR_  __DATE__ 
#endif /* REL_DATE */

#else /* defined(__STDC__) || defined(WIN32) */

#ifdef REL_DATE
#define _DATE_STR_  REL_DATE 
#else /* REL_DATE */
#define _DATE_STR_  "" 
#endif /* REL_DATE */

#endif /* defined(__STDC__) || defined(WIN32) */
#if defined(SLURM_HPC)
#define _BUILD_INFO_ _MINOR_STR_""_FOURTH_STR_""_BUILD_STR_" for SLURM, "_DATE_STR_\
                                       "\nCopyright International Business Machines Corp, 1992-2013.\n" \
                                       "US Government Users Restricted Rights - Use, duplication " \
                                       "or disclosure restricted by GSA ADP Schedule Contract with IBM Corp.\n\n" \
                                      _OS_STR_\
                                      _NOTE_STR_\
                                      _HOTFIX_STR_
#else
#define _BUILD_INFO_ _MINOR_STR_""_FOURTH_STR_""_BUILD_STR_""_WORKGROUP_STR_", "_DATE_STR_\
                                      "\nCopyright International Business Machines Corp, 1992-2013.\n" \
                                      "US Government Users Restricted Rights - Use, duplication " \
                                      "or disclosure restricted by GSA ADP Schedule Contract with IBM Corp.\n\n" \
                                      _OS_STR_\
                                      _NOTE_STR_\
                                      _HOTFIX_STR_
#endif

#define _LS_VERSION_ (_VERSION_STR_""_BUILD_INFO_)


/*--end of proj#84288 enhancement--*/

/*Problem 87958: If only reset xdrs's position, then set the specified position, 
  macro works fine for HP_UX 11i v3.*/
#if defined(_HPUX11_) || defined(_HPUXIA64_)
#include <rpc/xdr.h>
#ifdef XDR_SETPOS
#undef XDR_SETPOS
#endif
#define XDR_SETPOS(xdrs, pos)\
        (*(xdrs)->x_ops->x_setpostn)(xdrs, 0);\
        (*(xdrs)->x_ops->x_setpostn)(xdrs, pos)
#ifdef xdr_setpos
#undef xdr_setpos
#endif
#define xdr_setpos(xdrs, pos)\
	(*(xdrs)->x_ops->x_setpostn)(xdrs, 0);\
	(*(xdrs)->x_ops->x_setpostn)(xdrs, pos)
#endif /*end of HPUX*/
/*End of Problem 87958*/

/* please using following definitions for XDR version comparing */
#define LSF_XDR_VERSION2_0          1 	      
#define LSF_XDR_VERSION2_1          2 	      
#define LSF_XDR_VERSION2_2          3 	      
#define LSF_XDR_VERSION3_0          4 	      
#define LSF_XDR_VERSION3_1          5 	      
#define LSF_XDR_VERSION3_2          6 	      
#define LSF_XDR_VERSION3_2_2        7 	      
#define LSF_XDR_VERSION4_0          8 	      
#define LSF_XDR_VERSION4_1          9 	      
#define LSF_XDR_VERSION4_2          10 	      
#define LSF_XDR_VERSION5_0          11 	      
#define LSF_XDR_VERSION5_1          12 	
#define LSF_XDR_VERSION6_0          13
#define LSF_XDR_VERSION6_1          14
#define LSF_XDR_VERSION6_2          15    
#define EGO_XDR_VERSION_1_1         16    
#define LSF_XDR_VERSION7_0          17
#define EGO_XDR_VERSION_1_2         LSF_XDR_VERSION7_0
#define LSF_XDR_VERSION7_0_EP1      18
#define LSF_XDR_VERSION7_0_EP2      19
#define LSF_XDR_VERSION7_0_EP3      20
#define LSF_XDR_VERSION7_0_EP4      21
#define LSF_XDR_VERSION7_0_EP5      22
#define LSF_XDR_VERSION7_0_EP6      23
#define LSF_XDR_VERSION8_0          24
#define LSF_XDR_VERSION8_0_2        25  
#define LSF_XDR_VERSION9_1          26
#define LSF_XDR_VERSION9_1_1        27
#define EGO_XDR_VERSION_1_2_2       LSF_XDR_VERSION7_0_EP1
#define EGO_XDR_VERSION_1_2_3       LSF_XDR_VERSION7_0_EP2
#define EGO_XDR_VERSION_1_2_4       LSF_XDR_VERSION8_0
#define EGO_XDR_VERSION_1_2_5       LSF_XDR_VERSION9_1

#define LSF_XDR_V706_RV0_1          1
#define LSF_XDR_V706_RV0_2          2

#define EGO_XDR_VERSION EGO_XDR_VERSION_1_2_5  /* unsigned char used for version 
					* i.d. within struct LSFHeader 
                                        * 1 for 2.0 and 2.0a
                                        * 2 for 2.1 and 2.1a
                                        * 3 for 2.2 
					* 4 for 3.0, 3.0a, 3.0b
					* 5 for 3.1
					* 6 for 3.2 3.21
					* 7 for 3.22
					* 8 for 4.0
					* 9 for 4.1
					* 10 for 4.2
					* 11 for 5.0
					* 12 for 5.1
					* 13 for 6.0
					* 14 for 6.1
					* 15 for 6.2
					* 17 for 7.0
					* 18 for 7.0 EP1 
					* 19 for 7.0 EP2 
					*/

extern char const *LOG_VERSION; /* version used for logging within LSF
				       daemon log files */

#if defined(__STDC__) || defined(__cplusplus) || defined(SN_SVR4) || defined(WIN32)
#define P_(s) s
#include <stdarg.h>
#else
#define P_(s) ()
#include <varargs.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * constant definitions for suite maximums.
 */

#define LSF_DEFAULT_SOCKS	15   /* appl's default # of remote connects */
#define MAXLINELEN		512  /* max. line length of a file */
#define MAXLSFNAMELEN           40   /* max. name len in config file */
#define MAXLSFNAMELEN_70_EP1    128  /* max. name len in config file for the version of EP1 */

#define MAXSRES                 32   /* max. number of configured static
                                      * resources */
#define MAXRESDESLEN            256  /* description of a resource */
#define NBUILTINDEX	        11   /* num of built-in load indices */
#define MAXTYPES                128  /* max number of host types */
#define MAXMODELS               1024+2 /* max number of host models,
                                        * 1024 is the host model numbers that you can add,
                                        * 2 is the model number that the system add, and
                                        * the 2 models are UNKNOWN_AUTO_DETECT and DEFAULT */
#define MAXMODELS_70            128  /* max number of host models after 3.2 version*/
#define MAXTYPES_31             25   /* MAXTYPES before 3.2 version */
#define MAXMODELS_31            30   /* MAXMODELS before 3.2 version */
#define MAXFILENAMELEN          256  /* max len. of a filename */
#define MAXEVARS                30   /* max. number of extra env. variables */

#define GENMALLOCPACE           1024 /* general malloc pace */
#define MAXLOGCLASSLEN   1024  /* max. len. of all log classes */

/* fd of 1st socket bound to privileged port number for RES connection */
#define FIRST_RES_SOCK	20     

/*
 * Type of the status argument for ls_rwait(), wait(), waitpid(), and wait3()
 */
#ifdef HAVE_UNION_WAIT
#define LS_WAIT_T	union wait
#define LS_STATUS(s)	((s).w_status)
#else
#define LS_WAIT_INT
#define LS_WAIT_T	int
#define LS_STATUS(s)	(s)
#endif

/**
 * \addtogroup index_ls_loadinfo index_ls_loadinfo
 * define statements used by ls_loadinfo.
 */
/*@{*/

/* Index into load vector and resource table */
#define R15S           0  /**< 15-second exponentially averaged CPU run queue 
                           length. */
#define R1M            1  /**< 1-minute exponentially averaged CPU run queue 
                           length. */
#define R15M           2  /**< 15-minute exponentially averaged CPU run queue 
                           length. */
#define UT             3  /**< CPU utilization exponentially averaged over the 
                           last minute (from 0.0 to 1.0).*/
#define PG             4  /**< Memory paging rate exponentially averaged over 
                           the last minute, in pages per second. */
#define IO             5  /**< Disk I/O rate exponentially averaged over the 
                           last minute, in KBytes per second. */
#define LS             6  /**< Number of current login users. */
#define IT             7  /**< Idle time of the host (keyboard not touched on 
                           all logged in sessions), in minutes. */
#define TMP            8  /**< Available free disk space in /tmp, in MBytes. */
#define SWP            9  /**< Amount of currently available swap space, in 
                           MBytes. */
#define MEM            10 /**< Amount of currently available memory, in MBytes. */
#define USR1           11 /**< */
#define USR2           12 /**< */
/*@}*/


#if defined(_CRAY) && !defined(CRAYX1)
	    /* cray floats are 8 bytes! Note that 0x7fffffff is rounded to
                2^31 on non-cray so we have to do the same on cray to
                inter-operate with other platforms.
             */
#define INFINIT_LOAD    (float) (0x7fffffff + 1)
#define INFINIT_FLOAT   (float) (0x7fffffff + 1)
#elif defined(_SX) /* sx4 IBM float1 */
#define INFINIT_LOAD    (float) (0x7ffffc00)
#define INFINIT_FLOAT   (float) (0x7ffffc00)
#else
#define INFINIT_LOAD    (float) (0x7fffffff)
#define INFINIT_FLOAT   (float) (0x7fffffff)
#endif /* cray */

#define INFINIT_INT         0x7fffffff
#ifndef INT32JOBID
#ifdef WIN32
#define INFINIT_LONG_INT    0x7fffffffffffffff
#else
#define INFINIT_LONG_INT    0x7fffffffffffffffLL
#endif /* WIN32 */
#else
#define INFINIT_LONG_INT    0x7fffffff
#endif /* INT32JOBID */

#define INFINIT_SHORT  0x7fff

#define DEFAULT_RLIMIT     -1

#define LSF_RLIMIT_CPU      0            /* cpu time in milliseconds */
#define LSF_RLIMIT_FSIZE    1            /* maximum file size */
#define LSF_RLIMIT_DATA     2            /* data size */
#define LSF_RLIMIT_STACK    3            /* stack size */
#define LSF_RLIMIT_CORE     4            /* core file size */
#define LSF_RLIMIT_RSS      5            /* resident set size */
#define LSF_RLIMIT_NOFILE   6            /* open files */
#define LSF_RLIMIT_OPEN_MAX 7            /* (from HP-UX) */
#define LSF_RLIMIT_VMEM     8            /* maximum swap mem */
#define LSF_RLIMIT_SWAP     LSF_RLIMIT_VMEM
#define LSF_RLIMIT_RUN      9            /* max wall-clock time limit */
#define LSF_RLIMIT_PROCESS  10           /* process number limit */
#define LSF_RLIMIT_THREAD   11           /* thread number limit (introduced in LSF6.0) */
#define LSF_RLIM_NLIMITS    12           /* number of resource limits */

#define LSF_RLIM_NLIMITS5_1 11           /* number of resource limits 
                                          * before thread-limit is introduced 
					  * in LSF6.0
					  */

#if defined(hpux) || defined(__hpux)
#if !defined(HPUX1010)
#define RLIM_INFINITY   0x7fffffff
#endif
#endif

#if defined(hpux) || defined(__hpux)
#ifndef __seteuid__
#define __seteuid__
#define seteuid(x) setresuid(-1,x,-1)
#define setegid(x) setresgid(-1,x,-1)
#endif
#endif

/** 
 * \addtogroup defs_ls_mode defs_ls_mode 
 *  Eligibility mode settings 
 */
/*@{*/
#define LSF_NULL_MODE    0  			/**< This is just a dummy mode value */
#define LSF_LOCAL_MODE   1			/**< The routine searches through the remote task lists to see
						 * if taskname is on a list. If found, the task is considered eligible for remote execution,
						 * otherwise the task is considered ineligible.*/
#define LSF_REMOTE_MODE  2			/**< The routine searches through the local task lists to see
						 * if taskname is on a list. If found, the task is considered ineligible for remote
						 * execution, otherwise the task is considered eligible.*/
/*@}*/

#define RF_MAXHOSTS 5

/** 
 * \addtogroup commands_ls_rfcontrol commands_ls_rfcontrol 
 *  ls_rfcontrol() commands 
 */
/*@{*/
/* ls_rfcontrol() commands */
#define RF_CMD_MAXHOSTS 0         
/**< Allows the caller to specify the number of connected hosts. When a remote file
 * operation is being serviced at a host for the first time, a connection is made to the
 * Remote Execution Server's (RES) file server process on the remote host. If the
 * number of connections reaches RF_MAXHOSTS defined in lsf.h, the least recently
 * used connection which does not have any files open on that host is broken. The
 * RF_CMD_MAXHOSTS command allows you to change the maximum number of
 * connections. The new maximum is specified in arg.*/

#define RF_CMD_RXFLAGS 2          
/**< Terminates the connection with the RES's file server process on host arg. arg is a
 * pointer to the remote host name cast to int.*/
/*@}*/
/*
 * Special signals returned in status codes from \ref ls_rwait.
 */
/** 
 * \addtogroup return_values_waittid return_values_waittid 
 *  Special signals returned in status codes from \ref ls_rwait.
 */
/*@{*/
#if defined(WIN32)
#define REX_STATUS_TIMEOUT    125      /**< Timeout trying to connect to the remote RES. */
#elif defined(_AIX)
#define STATUS_TIMEOUT        149      /**< Timed_out status code for rpid */
#else
#define STATUS_TIMEOUT        125      /**< Timed_out status code for rpid */
#endif
#if defined(_AIX)
#define STATUS_IOERR          148      /**< The remote task failed with an I/O error.*/
#define STATUS_EXCESS         147      /**< Too many tasks are currently executing.  */
#define STATUS_REX_NOMEM      146      /**< RES failed to allocate memory */
#define STATUS_REX_FATAL      145      /**< Fatal error, check RES err log */
#define STATUS_REX_CWD        144      /**< Cannot change to current working directory */
#define STATUS_REX_PTY        143      /**< RES cannot allocate a pty */
#define STATUS_REX_SP         142      /**< RES cannot allocate socket pair*/
#define STATUS_REX_FORK       141      /**< RES failed to fork the taks */
#define STATUS_REX_AFS        140      /**< RES failed to set AFS tokens */
#define STATUS_REX_UNKNOWN    139      /**< Internal error in RES */
#define STATUS_REX_NOVCL      138      /**< RES failed to load VCL object */
#define STATUS_REX_NOSYM      137      /**< Failed to loacte any VCL handler */
#define STATUS_REX_VCL_INIT   136      /**< res_vcl_node_init failed */
#define STATUS_REX_VCL_SPAWN  135      /**< res_vcl_pre_spawn failed */
#define STATUS_REX_EXEC       134      /**< execit() failed */
#define STATUS_REX_MLS_INVAL  133      /**< Invalid MLS label */
#define STATUS_REX_MLS_CLEAR  132      /**< Not enough clearance */
#define STATUS_REX_MLS_RHOST  131      /**< rhost.conf reject require MLS label */
#define STATUS_REX_MLS_DOMIN  130      /**< Require label doesn't dominate */
#define STATUS_DENIED         129      /**< User permission denied */
#else
#define STATUS_IOERR          124      /**< The remote task failed with an I/O error.*/
#define STATUS_EXCESS         123      /**< Too many tasks are currently executing.  */
#define STATUS_REX_NOMEM      122      /**< RES failed to allocate memory */
#define STATUS_REX_FATAL      121      /**< Fatal error, check RES err log */
#define STATUS_REX_CWD        120      /**< Cannot change to current working directory */
#define STATUS_REX_PTY        119      /**< RES cannot allocate a pty */
#define STATUS_REX_SP         118      /**< RES cannot allocate socket pair*/
#define STATUS_REX_FORK       117      /**< RES failed to fork the taks */
#define STATUS_REX_AFS        116      /**< RES failed to set AFS tokens */
#define STATUS_REX_UNKNOWN    115      /**< Internal error in RES */
#define STATUS_REX_NOVCL      114      /**< RES failed to load VCL object */
#define STATUS_REX_NOSYM      113      /**< Failed to loacte any VCL handler */
#define STATUS_REX_VCL_INIT   112      /**< res_vcl_node_init failed */
#define STATUS_REX_VCL_SPAWN  111      /**< res_vcl_pre_spawn failed */
#define STATUS_REX_EXEC       110      /**< execit() failed */
#define STATUS_REX_MLS_INVAL  109      /**< Invalid MLS label */
#define STATUS_REX_MLS_CLEAR  108      /**< Not enough clearance */
#define STATUS_REX_MLS_RHOST  107      /**< rhost.conf reject require MLS label */
#define STATUS_REX_MLS_DOMIN  106      /**< Require label doesn't dominate */
#define STATUS_DENIED         105      /**< User permission denied */
#endif
/*@}*/
/* fatal error happens during the initialization of parallel
 * application. This is used by nios only
 */
#define REX_FATAL_ERROR(s)     (((s) == STATUS_REX_NOVCL) \
                                 || ((s) == STATUS_REX_NOSYM) \
                                 || ((s) == STATUS_REX_NOMEM) \
                                 || ((s) == STATUS_REX_FATAL) \
                                 || ((s) == STATUS_REX_CWD) \
                                 || ((s) == STATUS_REX_PTY) \
                                 || ((s) == STATUS_REX_VCL_INIT) \
                                 || ((s) == STATUS_REX_VCL_SPAWN) \
                                 || ((s) == STATUS_REX_MLS_INVAL) \
                                 || ((s) == STATUS_REX_MLS_CLEAR) \
                                 || ((s) == STATUS_REX_MLS_RHOST) \
                                 || ((s) == STATUS_REX_MLS_DOMIN))

/**
 * \addtogroup options_rexec options_rexec
 * define statements used by \ref ls_rexecv.
 */
/*@{*/
#define   REXF_USEPTY   0x00000001        /**< Use a remote pseudo-terminal. */
#define   REXF_CLNTDIR  0x00000002        /**< Use the local client's current working directory as the current working directory for
					   * remote execution (see \ref ls_chdir). */
#define   REXF_TASKPORT 0x00000004        /**< Request the remote RES to create a task port and return its number to the LSLIB.
					   * The application program can later call \ref ls_conntaskport to connect to the port. */
#define   REXF_SHMODE   0x00000008        /**< Enable shell mode support if the REXF_USEPTY flag is also given. This flag is ignored
					   * if REXF_USEPTY is not given. This flag should be specified for submitting interactive
					   * shells, or applications which redefine the ctrl-C and ctrl-Z keys (e.g., jove). */
#define   REXF_TASKINFO 0x00000010        /**< Request remote to forward
                                           * the task information */
#define   REXF_REQVCL   0x00000020        /**< Rex sets plugin option */
#define   REXF_SYNCNIOS 0x00000040        /**< NIOS synchronize IO */
#define   REXF_TTYASYNC	0x00000080        /**< Asyncronous rtty_ */
#define   REXF_STDERR   0x00000100        /**< Enable STDERR support */
#define   REXF_UNLINK_CHILD    0x00000200  /**< Res unlink child. */
#define   REXF_DELETE_CHILD    0x00000400  /**< Res deletes child. */
#define   REXF_WRITE_ACCT     0x00000800   /**< Res writes acct file before exit. */

/*@}*/
/**
 * \addtogroup options_ls_load options_ls_load
 * define statements used by \ref ls_load.
 */
/*@{*/

/* flags for placement decision */
#define EXACT         0x01 /**< Exactly *numhosts hosts are desired. If EXACT 
                            * is set, either exactly *numhosts hosts are 
                            * returned, or the call returns an error. If EXACT 
                            * is not set, then up to *numhosts hosts are 
                            * returned. If *numhosts is zero, then the EXACT flag
                            * is ignored and as many hosts in the load sharing 
                            * system as are eligible (that is, those that 
                             satisfy the resource requirements) are returned. */ 
#define OK_ONLY       0x02 /**< Return only those hosts that are currently in 
                            * the `ok' state. If OK_ONLY is set, those hosts 
                            * that are busy, locked, or unavail are not 
                            * returned. If OK_ONLY is not set, then some or 
                            * all of the hosts whose status are not `ok' may 
                            * also be returned, depending on the value of 
                            * numhosts and whether the EXACT flag is set. */
#define NORMALIZE     0x04 /**< Normalize CPU load indices. If NORMALIZE is 
                            * set, then the CPU run queue length load indices 
                            * r15s, r1m, and r15m of each host returned are 
                            * normalized. See Administering IBM Platform LSF for
                            * the concept of normalized queue length. Default 
                            * is to return the raw queue length. The options 
                            * EFFECTIVE and NORMALIZE are mutually exclusive. */
#define LOCALITY      0x08 /**<  The locality of the hosts*/
#define IGNORE_RES    0x10 /**< Ignore the status of RES when determining the 
                            * hosts that are considered to be `ok'. If 
                            * IGNORE_RES is specified, then hosts with RES not 
                            * running are also considered to be `ok' during host 
                            * selection. */
#define LOCAL_ONLY    0x20 /**<  Local cluster only */
#define DFT_FROMTYPE  0x40 /**< Return hosts with the same type as the fromhost 
                            * which satisfy the resource requirements. By default 
                            * all host types are considered. */		
#define ALL_CLUSTERS  0x80  /**<  All clusters */
#define EFFECTIVE     0x100 /**< If EFFECTIVE is set, then the CPU run queue 
                             * length load indices of each host returned are 
                             * effective load. See Administering IBM Platform LSF
                             * for the concept of effective queue length. 
                             * Default is to return the raw queue length. The 
                             * options EFFECTIVE and NORMALIZE are mutually 
                             * exclusive. */

/* only return info about clusters I can receive job from */
#define RECV_FROM_CLUSTERS 0x200 /**<  Only return info about clusters I can receive job from*/
#define NEED_MY_CLUSTER_NAME 0x400 /**<  Need my cluster name*/

/* only return info about clusters I can send job to */
#define SEND_TO_CLUSTERS   0x400 /**<  Only return info about clusters I can send job to*/

/* no host sorting is needed */
#define NO_SORT		0x800 /**<  No host sorting is needed*/

/* exclusive resource definition enforcement */
#define EXCLUSIVE_RESOURCE 0x1000 /**<  Exclusive resource definition enforcement*/
/* desktop cluster return unlicensed hosts load info */
#define DT_CLUSTER_LOAD 0x2000 /**<  Desktop cluster return unlicensed hosts load info*/

#define AC_JOB_VM_IGN 0x10000 /**<  adaptive cluster master return all hosts info*/
#define AC_HOST_IGN   0x20000 /**<  adaptive cluster master ignores achosts info*/

/* grid broker flag */
#define GRID_BROKER 0x4000 /**<  grid broker flag from command request*/

#define INCLUDE_EXPIRED 0x8000 /* include expired host in return*/


/*@}*/


/* options flag for ls_clusterinfo */
#define FROM_MASTER   0x01

/* flags for ls_initrex() */
#define KEEPUID       0x01

/**
 * \addtogroup commands_rescontrol commands_rescontrol
 * \ref ls_rescontrol commands.
 */
/*@{*/
/* ls_rescontrol commands */
#define RES_CMD_REBOOT          1           
                                  /**< Restart the RES. If the RES is in service, it will keep serving until all 
                                   * remote tasks exit, meanwhile starting another RES to serve new clients. */
#define RES_CMD_SHUTDOWN        2           
                                  /**< Shutdown the RES. The RES will not accept new tasks 
                                   * and will die after all current remote tasks exit. */
#define RES_CMD_LOGON           3           
                                  /**< Enable task logging, so that resource usage information 
                                   *can be logged to a file (see lsf.acct). */
#define RES_CMD_LOGOFF          4 /**< Disable task logging. */
/*@}*/

/**
 * \addtogroup defs_ls_limcontrol defs_ls_limcontrol
 * define statements used by \ref ls_limcontrol.
 */
/*@{*/
/* ls_limcontrol commands */
#define LIM_CMD_REBOOT          1     /**< Command to reboot the LIM. */
#define LIM_CMD_SHUTDOWN        2     /**< Command to shutdown the LIM. */
#define LIM_CMD_REMOVEHOST      3     /**< Command to remove the host*/
#define LIM_CMD_ACTIVATE	4     /**< Command to active the host*/
#define LIM_CMD_DEACTIVATE	5     /**< Command to deactive the host*/
#define LIM_CMD_ELIM_ENV	6     /**< Command to set elim env*/
/*@}*/


/*
 * Host-related interface data structures
 */

struct connectEnt {                       /* host connection information */
    char *hostname;
    SOCKET   csock[2];
};

#define INTEGER_BITS       32  /* bits for a int */
/* get number of integers according to i */
#define GET_INTNUM(i) ((i)/INTEGER_BITS + 1)


/**
 * \addtogroup status_ls_load status_ls_load
 * define statements used by \ref ls_load.
 */
/*@{*/

/*
 * Host status from LIM     (higher order 16 bits used for additional info)
 */
#define LIM_UNAVAIL    0x00010000  /**< The host Load Information Manager (LIM)
                                    * is unavailable. (e.g. the host is down or 
                                    * there is no LIM ). If LIM is unavailable the 
                                    * other information in the hostLoad structure 
                                    * is meaningless. */
#define LIM_LOCKEDU    0x00020000  /**< The host's LIM is locked by the root, 
                                    * LSF administrator or a user. */
#define LIM_LOCKEDW    0x00040000  /**< The host's LIM is locked by its run 
                                    * windows. */
#define LIM_BUSY       0x00080000  /**< The host is busy (overloaded). */
#define LIM_RESDOWN    0x00100000  /**< The host's Remote Execution Server (RES) 
                                    * is not available. */
#define LIM_UNLICENSED 0x00200000  /**< The host has no software license. */
#define LIM_SBDDOWN    0x00400000  /**< sbatchd is down */
#define LIM_LOCKEDM    0x00800000  /**<  Locked by master lim */
                                   
#define LIM_OK_MASK    0x02bf0000  /**< Mask used to get rid of other status;
				    * don't consider LIM_SBDDOWN */
#define LIM_PEMDOWN    0x01000000  /**<  PEM down or hang */
#define LIM_EXPIRED    0x02000000  /**< dynamic host become expired */
#define LIM_ACOK       0x04000000  /* adaptive cluster host ok */
#define LIM_RLAUP      0x40000000  /**< RLA daemon is up and running */
#define LIM_LOCKEDU_RMS 0x80000000 /**< This bit is only used with in
                                    * SIERRA_PRESTO. It forces MBD
                                    * to close the host.*/
                                   /* Refer to Kite#29531. */
/*@}*/



/* macros for testing host status */
/* information unavailable */
#define LS_ISUNAVAIL(status)     (((status) != NULL) && (((status[0]) & LIM_UNAVAIL) != 0))

/* host is busy */
#define LS_ISBUSYON(status, index)  \
      (((status) != NULL) && (((status[1 + (index)/INTEGER_BITS]) & (1 << (index)%INTEGER_BITS)) != 0))

#define LS_ISBUSY(status)       (((status) != NULL) && (((status[0]) & LIM_BUSY) != 0))

/* host will be closed by MBD. Kite#29531 */
#define LS_ISRMSLOCK(status) (((status) != NULL) && (((status[0]) & LIM_LOCKEDU_RMS) != 0))

/* host is locked by user */
#define LS_ISLOCKEDU(status)    (((status) != NULL) && (((status[0]) & LIM_LOCKEDU) != 0))

/* host is locked by time window */
#define LS_ISLOCKEDW(status)    (((status) != NULL) && (((status[0]) & LIM_LOCKEDW) != 0))

/* host is locked by master lim */
#define LS_ISLOCKEDM(status)    (((status) != NULL) && (((status[0]) & LIM_LOCKEDM) != 0))

/* host is locked */
#define LS_ISLOCKED(status)     (((status) != NULL) && (((status[0]) & (LIM_LOCKEDU | LIM_LOCKEDW | LIM_LOCKEDM)) != 0))

/* res is down */
#define LS_ISRESDOWN(status)    (((status) != NULL) && (((status[0]) & LIM_RESDOWN) != 0))

/* sbatchd is down */
#define LS_ISSBDDOWN(status)    (((status) != NULL) && (((status[0]) & LIM_SBDDOWN) != 0))

#define LS_ISPEMDOWN(status)    (((status[0]) & LIM_PEMDOWN) != 0)

/* no software license */
#define LS_ISUNLICENSED(status)	(((status) != NULL) && (((status[0]) & LIM_UNLICENSED) != 0))

/* adaptive cluster host ok */
#define LS_ISACOK(status)       (((status) != NULL) && (((status[0]) & LIM_ACOK) != 0))

/* host is ok */
#define LS_ISOK(status)         (((status) != NULL) && ((status[0] & LIM_OK_MASK) == 0))

/* host is ok except that no res or sbd is running */

/* host is ok except that no res or sbd is running
 * because we introduce a new state of lim - LIM_LOCKURMS, which
 * indicates that though LIM is ok, but RMS is down, and MBD
 * should close the host, we need to reset this bit so that
 * LS_ISOKNRES(status) still returns the original value
 */
#define LS_ISOKNRES(status) (((status) != NULL) && (((status[0] & ~(LIM_RLAUP) & ~(LIM_LOCKEDU_RMS)) & ~(LIM_RESDOWN | LIM_SBDDOWN | LIM_PEMDOWN | LIM_ACOK)) == 0))
 
#define LS_ISEXPIRED(status)  (((status) != NULL) && (((status[0]) & LIM_EXPIRED) != 0))

#define LS_ISRLAUP(status)    (((status) != NULL) && !LS_ISSBDDOWN(status) && (((status[0]) & LIM_RLAUP) != 0))


/**
 * \brief  Struct containing placement information
 */
struct placeInfo {          
   char  hostName[MAXHOSTNAMELEN]; /**<  The host name.*/
   int   numtask;                  /**<  The number of task.*/
};


/**
 * \brief  The host load information
 */
struct hostLoad {           /* struct containing host load information */
   char  hostName[MAXHOSTNAMELEN]; /**< Host name */
   int   *status; /**< Status of host */
   float *li; /**< Vector contains load information on various resources on a host */
};


/**
 * \ingroup defs_ls_info  
 *  resource's value type
 */
enum valueType {
    LS_BOOLEAN,  /**<  Bool value */
    LS_NUMERIC,  /**<  Numeric value */ 
    LS_STRING,   /**<  String value */ 
    LS_EXTERNAL  /**<  External value */ 
};

#if !defined(WIN32)
#define BOOLEAN  LS_BOOLEAN
#define NUMERIC  LS_NUMERIC
#define STRING   LS_STRING
#define EXTERNAL LS_EXTERNAL
#endif

/**
 * \ingroup defs_ls_info   
 *   the hosts ordered type based on the resource 
 */
enum orderType {
	INCR, /**<  From the lowest to the highest value of the resource */
	DECR, /**<  From the highest to the lowest value of the resource */
	NA    /**<  The resource cannot be used to order hosts */
};

/**
 * \addtogroup defs_ls_info defs_ls_info
 * define statements used by \ref ls_info.
 */
/*@{*/
#define RESF_BUILTIN     0x01    /**< Indicate whether this resource is builtin to LSF or configured by the LSF
				  * administrator (external). */
#define RESF_DYNAMIC     0x02    /**< Indicate whether the value of this resource can change dynamically or is static.
				  * Information about dynamic resources for a host can be retrieved through
				  * \ref ls_load. Information about static resources can be retrieved through
				  * \ref ls_gethostinfo. */
#define RESF_GLOBAL      0x04    /**< Indicate whether the resource name is defined for every host in the cluster. The
 				  * value of the resource is specific to each host. This type of resource is also called a
 				  * non-shared resource. */
#define RESF_SHARED      0x08    /**< Indicate whether the resource is a shared resource. A shared resource is a resource
				  * whose value is shared by more than one host, and the resource may be defined only
				  * on a subset of the hosts.
				  * \n The interval component applies to resources with dynamic values. It indicates
				  * how frequently (in seconds) the resource value is evaluated.
				  * \n The set of host types hostTypes in the lsInfo structure is a list of all defined host
				  * architectures in the cluster. All machines that can run the same binaries are
				  * generally considered to be of the same host type.
				  * \n The set of host models hostModels in lsInfo structure is a list of all defined
				  * computer models in the cluster. Generally, machines of the same host type that have
				  * exactly the same performance characteristics are considered to be the same model.*/
#define RESF_LIC         0x10    /**<  License static value. */
#define RESF_EXTERNAL    0x20    /**<  External resource defined. */
#define RESF_RELEASE     0x40    /**<  Resource can be released when job is suspended. */
#define RESF_DEFINED_IN_RESOURCEMAP  0x80  /**<  Set if resource seen in */
                                           /**<  Resourcemap */
#define RESF_NON_CONSUMABLE  0x100         /**<  Resource can be reserved with rusage */
#define RESF_REDEFINABLE     0x200  	   /**<  Builtin resource that can be redefined in the cluster file. */
#define RESF_ESRES 0x400         	   /**<  Resources collected by External static lims*/ 
#define RESF_MEGA_CRITERIA 0x800 /**<  Resources can be megahost criteria*/ 
#define RESF_APPEARS_IN_RESOURCEMAP 0x8000 /**<  Resource is configured in Resourcemap */ 
/*@}*/

/**
 * \brief  Resource Information
 */ 
struct resItem {
    char name[MAXLSFNAMELEN];	 /**<  Resource name */
    char des[MAXRESDESLEN];      /**<  Description of the resource */
    enum valueType valueType;    /**<  Type of value */
    enum orderType orderType;    /**<  Type of resource order */
    int  flags;                  /**<  Resource attribute flags */
    int  interval;               /**<  How often it is evaluated */
};

/**
 * \brief  Information of LS configurations
 */ 
struct lsInfo {                  
    int    nRes;                 /**<  Number of resource in the cluster */
    struct resItem *resTable;    /**<  All resource items in the cluster */
    int    nTypes;                               /**<  Number of host type */
    char   hostTypes[MAXTYPES][MAXLSFNAMELEN];   /**<  Type name */
    int    nModels;                              /**<  Number of host model */
    char   hostModels[MAXMODELS][MAXLSFNAMELEN]; /**<  Model name */
    char   hostArchs[MAXMODELS][MAXLSFNAMELEN_70_EP1];  /**<  Architecture name */
    int    modelRefs[MAXMODELS];  /**<  Number of hosts of this architecture */
    float  cpuFactor[MAXMODELS];  /**<  The cpu factor for different host model */
    int    numIndx;          	  /**<  Number of load indices */
    int    numUsrIndx;      	  /**<  Number of user defined load indices */
};


#if defined(COMPAT32) && defined(INT32JOBID)

/* a LSF32 version of struct lsInfo, which does not have
 *     (1)char   hostArchs[MAXMODELS][MAXLSFNAMELEN_70_EP1];
 *     (2)int    modelRefs[MAXMODELS];
 * 
 * This structure is used by the wrapper function of ls_info()
 * to pass the correct structure back to the function caller. Otherwise
 * the caller may access the wrong fields/memory in the structure.
 *
 * See SUN_HPC_TEST 16777
 */

/**
 * \brief  
 */ 
struct lsInfo32 {                              /* information of LS configurations" */            
    int    nRes;                               /**<  The number of Res*/
    struct resItem *resTable;                  /**<  The res table*/
    int    nTypes;                  	       /**<  The number of types*/
    char   hostTypes[MAXTYPES][MAXLSFNAMELEN]; /**<  All host types*/
    int    nModels;   			       /**<  The number of models*/
    char   hostModels[MAXMODELS][MAXLSFNAMELEN];    /**<  All host models*/
    float  cpuFactor[MAXMODELS];    		    /**<  The CPU factor*/
    int    numIndx;          /**<  The number of load indices*/
    int    numUsrIndx;       /**<  The number of user defined load indices*/
};

#endif


/* cluster status */
#define CLUST_STAT_OK             0x01
#define CLUST_STAT_UNAVAIL        0x02
#define CLUST_STAT_RECV_FROM      0x04
#define CLUST_STAT_SEND_TO        0x08

/* authenication */
#define IS_DEFAULT_AUTH(auth) (auth == NULL || auth[0] == '\0')

/**
 * \brief  The information of a cluster
 */
struct clusterInfo {
    char  clusterName[MAXLSFNAMELEN];			/**<  The cluster name */
    int   status;					/**<  The cluster status*/
    char  masterName[MAXHOSTNAMELEN];			/**<  The master host name*/
    char  managerName[MAXLSFNAMELEN];			/**<  The manager name*/
    int   managerId;					/**<  The manager ID*/
    int   numServers;      				/**< Number of hosts that run LSF servers */
    int   numClients;      				/**< Number of client-only hosts */
    int   nRes;           				/**< Num of resources */
    char  **resources;     				/**< List of available resources*/
    int   nTypes;					/**<  The number of types */
    char  **hostTypes;    				/**< List of available host types */
    int   nModels;       				/**<  The number of models*/
    char  **hostModels;    				/**< List of available host models */
    int   nAdmins;         				/**< Num of administrators */
    int   *adminIds;       				/**< List of uids of administrator */
    char  **admins;        				/**< List of administrator names */
    int   analyzerLicFlag; 				/**< This flag is not used, kept for compatibility */
    int   jsLicFlag;       				/**< Cluster wide js licensing */
    char  afterHoursWindow[MAXLINELEN];  		/**< Cluster wide After-Hour window */
    char  preferAuthName[MAXLSFNAMELEN];		/**< Prefer authenication method name.
			    		   		 * read from lsf.cluster file. */
    char  inUseAuthName[MAXLSFNAMELEN];  		/**< Authenication method name in use.
							 * negotiated between clusters during
							 * cluster info exchange. */
};

/* Encode xtype_id_t and xkey_t into a 32-bits long.
 * This can decrease XDR data size if there're lots data fields.
 */
#define XTYPE_ID_BITS  (12)
#define XKEY_ID_BITS   (32-XTYPE_ID_BITS)
#define XKEY_ID_MASK  ((0x1 << XKEY_ID_BITS) - 1)

#define OBJTYPE_UNKNOWN (0)
#define OBJTYPE_CLUSTER (OBJTYPE_UNKNOWN+1)
#define OBJTYPE_HOST    (OBJTYPE_UNKNOWN+2)
#define OBJTYPE_NODE    (OBJTYPE_UNKNOWN+3)
#define OBJTYPE_SOCKET  (OBJTYPE_UNKNOWN+4)
#define OBJTYPE_CACHE   (OBJTYPE_UNKNOWN+5)
#define OBJTYPE_CORE    (OBJTYPE_UNKNOWN+6)
#define OBJTYPE_PU      (OBJTYPE_UNKNOWN+7)
#define OBJTYPE_GROUP   (OBJTYPE_UNKNOWN+8)
#define OBJTYPE_LAST    (OBJTYPE_UNKNOWN+9)

#define ATTR_KEY_UNKNOWN (0)
#define ATTR_KEY_NAME    (ATTR_KEY_UNKNOWN+1)
#define ATTR_KEY_FLAGS   (ATTR_KEY_UNKNOWN+2)
#define ATTR_KEY_CACHE_SIZE (ATTR_KEY_UNKNOWN+3)
#define ATTR_KEY_NODE_ALLMEM   (ATTR_KEY_UNKNOWN+4)
#define ATTR_KEY_NODE_USED_MEM (ATTR_KEY_UNKNOWN+5)
#define ATTR_KEY_CACHE_DEPTH (ATTR_KEY_UNKNOWN+6)
#define ATTR_KEY_AFFINITY_BIND_ENABLE (ATTR_KEY_UNKNOWN+7)
#define ATTR_KEY_NODE_INUSE (ATTR_KEY_UNKNOWN+8)
#define ATTR_KEY_HOST_SUPPORT_AFFINITY (ATTR_KEY_UNKNOWN+9)
#define ATTR_KEY_AFFINITY_NODEID (ATTR_KEY_UNKNOWN+10)
#define ATTR_KEY_LAST   (ATTR_KEY_UNKNOWN+11)

typedef enum {
    XTYPE_UNKNOWN = 0,
    XTYPE_INT,
    XTYPE_UINT,
    XTYPE_FLOAT,
    XTYPE_LONG64,
    XTYPE_STRING
} xtype_id_t;

/**
 * \brief  
 */ 
typedef struct {
    union {
        int           intv;
        unsigned int  uintv;
        float         floatv;
	 LS_LONG_INT longv;
        char         *stringv;		
    } u;
} xvalue_t;

/**
 * \brief  
 */ 
typedef struct {
    xtype_id_t xtype;
    xvalue_t   xvalue;
} xdata_t;

typedef int (*xarray_obj_init_func_t)(void *);
typedef int (*xarray_obj_kill_func_t)(void *);
typedef int (*xtree_node_cntl)(void *);

/**
 * \brief  Array information summary
 */ 
typedef struct {
    int   count;            /**< The number of data elements*/
    int   capacity;         /**< Data length */
    int   capdelta;         /**< Iteration number for reallocing data*/
    int   datasz;           /**< Data size of each element in the array*/
    
    void *data;             /**< Store child node or node attribute*/
                                
    xarray_obj_init_func_t objInit;   /**< Initial function for data*/
    xarray_obj_kill_func_t objKill;   /**< Destroyed function for data*/
} xarray_t;
    
typedef unsigned int objid_t;
typedef unsigned int objtype_t;
typedef unsigned int xkey_t;

/**
 * \brief  Key/value pair summary
 */ 
typedef struct {
    xkey_t   key;  /**<  key map name */
    xdata_t  data; /**< Data value*/
} xattr_t;

/**
 * \brief  Tree node summary
 */ 
typedef struct {
    objtype_t  xtype;           /**< Node type, such as NUMA, socket, core, cache, PU */
    objid_t    xid;             /**< Physical index number */
    xarray_t   attrs;           /**< Node attribute array */
    xarray_t   children;        /**<Child node array */
} xtree_t;

typedef xtree_t* lsf_topology_t;

#define lsf_topology_traverse(t, f) xtree_traverse(((xtree_t *)t), ((xtree_node_cntl)f))

/**
 * \brief  The information of a host
 */
struct hostInfo {
    char  hostName[MAXHOSTNAMELEN];	/**<  The host name*/
    char  *hostType;			/**<  The host type*/
    char  *hostModel;			/**<  The host model*/
    float cpuFactor;			/**<  The cpu factor*/
    int   maxCpus;			/**<  The max num of cpus*/
    int   maxMem;			/**<  The max mem of the host*/
    int   maxSwap;			/**<  The max swap of the host*/
    int   maxTmp; 			/**<  The max tmp of the host*/
    int   nDisks;			/**<  The num of disks*/
    int   nRes;				/**<  The num of res*/
    char  **resources;			/**<  The host resources*/
    int   nDRes;			/**<  The number of DResources*/
    char  **DResources;			/**<  Exclusive resources*/
    char  *windows;			/**<  Run Windows*/	
    int   numIndx;			/**<  The num of index*/
    float *busyThreshold;		/**<  The busy threshold*/	
    char  isServer;			/**<  If the host is a server*/
    char  licensed;			/**<  If the host is licensed*/
    int   rexPriority;			/**<  The rex priority*/
    int   licFeaturesNeeded; 		/**<  Needed license features, refer to \ref defs_ls_lic*/

/** 
 * \addtogroup defs_ls_lic defs_ls_lic 
 *  options 
 */
/*@{*/	
#define LSF_BASE_LIC 		 0	/**<  Base license*/
#define LSF_BATCH_LIC_OBSOLETE	 1	/**<  Obsolete batch license*/
#define LSF_JS_SCHEDULER_LIC 	 2  	/**<  JS Scheduler license*/
#define LSF_JS_LIC 		 3	/**<  JS license*/
#define LSF_CLIENT_LIC 		 4	/**<  Client license*/
#define LSF_MC_LIC 		 5	/**<  MultiCluster license*/
#define LSF_ANALYZER_SERVER_LIC  6	/**<  Analysis server license*/
#define LSF_EXPRESS_HPC_LIC      6	/**<  Express HPC license*/
#define LSF_MAKE_LIC 		 7 	/**<  Make licence*/
/*
 * HPC5.x 39029
 * LSF_PARALLEL_LIC is obsoleted.
 * All code handling LSF_PARALLEL_LIC
 * have been removed. However internal data structure
 * was not changed therefore, we still need this entry
 * here. 
 */
#define LSF_PARALLEL_LIC         8 	/**<  Parallel license*/
#define LSF_FLOAT_CLIENT_LIC     9	/**<  Float client license*/
#define LSF_FTA_LIC              10	/**<  FTA license*/
#define LSF_AFTER_HOURS_LIC	 11	/**<  After hour license*/
#define LSF_RESOURCE_PREEMPT_LIC 12	/**<  Preempt resource license*/
#define LSF_BACCT_LIC            13	/**<  Bacct license*/
#define LSF_STANDARD_LIC         13     /**< LSF Standard Edition */
#define LSF_SCHED_FAIRSHARE_LIC  14	/**<  Fairshare schedule license*/
#define LSF_SCHED_RESERVE_LIC    15	/**<  Reserve shedule license*/
#define LSF_SCHED_PREEMPTION_LIC 16	/**<  Preemption schedule license*/
#define LSF_SCHED_PARALLEL_LIC   17	/**<  Parallel schedule license*/
#define LSF_SCHED_ADVRSV_LIC     18	/**<  Advance Reserve schedule license*/
#define LSF_API_CLIENT_LIC       19 	/**<  API client license*/

#define LSF_EXPRESS_LIC          20     /**<  Express Edition license*/
#define CLUSTERWARE_MANAGER_LIC    20  /**< None used */
#define LSF_MANAGER_LIC 	 21	/**<  LSF Manager license*/
#define LSF_PCC_HPC_LIC          22 	/**< "platform_hpc" feature*/
#define sCLUSTERWARE_LIC 	 23 	/**< "Obsoleted: s-Clusterware" OEM for S&C */
#define LSF_ADVANCED_LIC         23 	/**< "LSF Advanced Edition */
#define OTTAWA_MANAGER_LIC       24	/**<  Ottawa Manager license*/

#define SYMPHONY_MANAGER_ONLINE_LIC 25	/**<  Symphony Manager online license*/
#define SYMPHONY_MANAGER_BATCH_LIC  26	/**<  Symphony Manager batch license*/
#define PLATFORM_LSFXL           27     /**<  Platform LSF/XL license*/
#define LSF_DUALCORE_X86_LIC     28	/**<  DualCore x86 license*/
#define LSF_SSCHED_LIC           29     /**<  session scheduler license*/
#define LSF_WORKGROUP_LIC        30     /**<  lsf_workgroup license*/
#define LSF_NUM_LIC_TYPE         31     /**<  Doesn't contain lsf_workgroup*/
#define LSF_WG_NUM_LIC_TYPE      2      /**<  Total license feature for workgroup edition*/
#define LSF_NO_NEED_LIC          32     /**<  No needed license*/

/*@}*/
    int licClass;                /**< License class needed */
    int cores;                   /**< Number of cores per physical CPU */
#ifndef INET6_ADDRSTRLEN
# define INET6_ADDRSTRLEN 46
#endif
    char hostAddr[INET6_ADDRSTRLEN];    /**<  IP address of this host */
    int pprocs;                   	/**< Num physical processors. */
    					/* 82185 - cores_per_proc and cores are both needed for backwards compatibility.
    					 * cores is used for licencing enforcement and cores_per_proc is needed
    					 * for ncpus computation. */
    int cores_per_proc;           	/**< Num cores per processor. */
    int threads_per_core;         	/**< Num threads per core. */
    lsf_topology_t top;                /** host topology informaion */
};

typedef enum {
    PRODUCT_ALL = 0,
    PRODUCT_LSF,
    PRODUCT_SYMPHONY,
    PRODUCT_PCM,
    PRODUCT_NUM 
} product_identification_t;

/* The entitlement request information
 */
struct entitlementInfoRequest {
    product_identification_t product;
};

/* The information of the product entitlement
 */
struct entitlementInfo {
    char *name;                   /* product edition name */
    char *version;                /* product edition version */
    time_t expiry;                /* 0 implies no expiry */
    int number;                   /* 0 implies unlimited */
    int used;                     
    char *restrictions;          
    char *misc;
    char *infoString;             /* product edition information string */

    /* for symphony */
    int entitled;              /* the key is entiled (1) or not entitled (0) or not available (-1) */
    /* entitlement description for display and log */
    char* entitledDesc; /* edition name for edition entitlement; */
                                   /*  "entitled" or "not entitled" or "not available" for add-on entitlement */
};

/* The information of the product entitlements
 */
struct entitlementInfoReply {
    int  errCode;                 /* see  Load Sharing error numbers */
    char *clName;                 /* cluster name */
    char *masterName;             /* master host name */
    char *buildDate;              /* LIM build date string */
    char *additionalInfo; 
    int nEntitlements;
    struct entitlementInfo *entitlements;

    int nAddons;
    struct entitlementInfo *addons;
}; 

/* Any of the follow licenses can serve as a Batch license */
#define HAS_BATCH_LICENSES(featureEnabled) \
( \
      (featureEnabled & (1 << LSF_EXPRESS_LIC)) \
  || (featureEnabled & (1 << LSF_STANDARD_LIC)) \
  || (featureEnabled & (1 << LSF_ADVANCED_LIC)) \
  || (featureEnabled & (1 << LSF_MANAGER_LIC)) \
  || (featureEnabled & (1 << LSF_WORKGROUP_LIC)) \
)

#define HAS_SYMPHONY_LICENSES(featureEnabled) \
( \
  (featureEnabled & (1 << SYMPHONY_MANAGER_ONLINE_LIC)) \
  || (featureEnabled & (1 << SYMPHONY_MANAGER_BATCH_LIC)) \
)

#define HAS_STANDARD_LICENSES(featureEnabled) \
( \
  (featureEnabled & (1 << LSF_STANDARD_LIC)) \
)

#define HAS_EXPRESS_LICENSES(featureEnabled) \
( \
  (featureEnabled & (1 << LSF_EXPRESS_LIC)) \
)

#define HAS_ADVANCED_LICENSES(featureEnabled) \
( \
  (featureEnabled & (1 << LSF_ADVANCED_LIC)) \
)

/**
 * \brief  The configurational parameter
 */
struct config_param {          /* configurational parameter */
    char *paramName;           /**<  The pointer to the configuration parameter name 
    				* defined in the *confPath/lsf.conf 
    				* or /etc/lsf.conf file if confPath is NULL.*/
    char *paramValue;          /**<  The config_param data structure must initially contain NULL 
    				* and is then modified to point to a result string if a matching 
				* paramName is found in the lsf.conf file.*/
};
/**
 * \brief  LSF resource usage
 */
struct	lsfRusage {
	double  ru_utime;	        /**< User time used */
	double  ru_stime;	        /**< System time used */
	double	ru_maxrss;              /**< Max rss */
	double	ru_ixrss;		/**< Integral shared text size */
	double	ru_ismrss;	        /**< Ultrix only: integral shared
	                                 * memory size */
	double	ru_idrss;		/**< Integral unshared data */
	double	ru_isrss;		/**< Integral unshared stack */
	double	ru_minflt;		/**< Page reclaims */
	double	ru_majflt;		/**< Page faults */
	double	ru_nswap;		/**< Swaps */
	double	ru_inblock;		/**< Block input operations */
	double	ru_oublock;		/**< Block output operations */
	double  ru_ioch;                /**< Hp only: \# of characters
	                                 * read/written */
	double	ru_msgsnd;		/**< Messages sent */
	double	ru_msgrcv;		/**< Messages received */
	double	ru_nsignals;		/**< Signals received */
	double	ru_nvcsw;		/**< Voluntary context switches */
	double	ru_nivcsw;		/**< Involuntary */
	double  ru_exutime;             /**< Convex only: exact user time used */
};

/**
 * \brief  LSF Acct record
 */
struct lsfAcctRec {
    int pid;                  /**<  Pid */
    char *username;           /**<  User name*/
    int exitStatus;           /**<  Exit status*/
    time_t dispTime;          /**<  Display time*/
    time_t termTime;          /**<  Terminate time*/
    char *fromHost;           /**<  From host*/
    char *execHost;           /**<  Execution host*/
    char *cwd;                /**<  Current working directory*/
    char *cmdln;              /**<  Command line*/
    struct lsfRusage lsfRu;   /**<  LSF resource usage*/
};

/* data structures related to API_CONF */

struct confNode {
    struct confNode *leftPtr;	/* go to when condition satisfied */
    struct confNode *rightPtr;	/* go to when condition unsatisfied */
    struct confNode *fwPtr;	/* go to after this node is processed */
    char	*cond;		/* condition with this node */
    int		beginLineNum;	/* beginning line number with this node */
    int		numLines;	/* number of lines in this node */
    char	**lines;	/* text lines with this node */
    char	tag;		/* used only for building and traversing
				   parse tree */
    char        *fname;         /* file name of the node */
};

struct pStack {
    int top;
    int size;
    struct confNode **nodes;
};

struct confHandle {
    struct confNode *rootNode;	/* root of the parse tree */
    char            *fname;	/* filename */
    struct confNode *curNode;	/* current node while traversing tree */ 
    int	            lineCount;	/* line count within a node */
    struct pStack   *ptrStack;	/* stack for traversing tree */
};

struct lsConf {
    struct confHandle *confhandle;	/* cannot be modified */
    int         numConds;		/* cannot be modified */
    char        **conds;		/* cannot be modified */
    int		*values;		/* assigned by user */
};

struct sharedConf {
    struct lsInfo *lsinfo;
    int		numCls;
    char	**clusterNames;
    char 	**servers;
};

/**
 * \brief lsSharedResourceInstance contains following fields
 */
typedef struct lsSharedResourceInstance {
    char *value;        /**< Value associated with the resource */
    int  nHosts;        /**< Hosts associated with the resource. Only
                         * necessary for shared resources  */
    char **hostList;    /**< Host list*/

} LS_SHARED_RESOURCE_INST_T;

/**
 * \brief lsSharedResourceInfo contains following fields
 */
typedef struct lsSharedResourceInfo {
    char *resourceName;  /**< Resource name */
    int  nInstances;     /**< Number of instances */
    LS_SHARED_RESOURCE_INST_T  *instances;    /**< Pointer to the next instance */
} LS_SHARED_RESOURCE_INFO_T;

struct clusterConf {
    struct clusterInfo *clinfo;
    int                numHosts;
    struct hostInfo    *hosts;
    int                defaultFeatures;
    int                numShareRes;
    LS_SHARED_RESOURCE_INFO_T *shareRes;
};

/* end of data structures related to API_CONF */

/* A job's CPU and memory usage when it is running */
 
/**
 * \brief Information about a process with its process ID pid.
 */
struct pidInfo {
    int pid;          /**< Process id */
    int ppid;         /**< Parent's process id */ 
    int pgid;         /**< Processes' group id */
    int jobid;        /**< Process' cray job id (only on Cray) */
}; /* information about a process with its process ID pid. */

/**
 * \brief Information about job using resource.
 */
struct jRusage {
    int mem;          /**< Total resident memory usage in kbytes of all
                       * currently running processes in given process
                       * groups. */
    int swap;         /**< Total virtual memory usage in kbytes of all
                       * currently running processes in given process
		       * groups. */
    int utime;        /**< Cumulative total user time in seconds */
    int stime;        /**< Cumulative total system time in seconds */
    int npids;        /**< Number of currently active processes in given
		       * process groups. */
    struct pidInfo *pidInfo; 
                      /**< Array of currently active processes' information */
    int npgids;       /**< Number of currently active process groups */
    int *pgid;        /**< Array of currently active process group ids */
    int nthreads;     /**< Number of currently active threads in given 
		       * process groups. */
};

/* license reproting
 */
#define NUM_SUBS 2
#define LEN_SUBS 64
#define NUM_CLASS_TYPE 3

struct licUsage {
    int licDisplayMask; /* Which license features will be displayed */
    int usingDemoLicense; /* Whether it's using DEMO license */
    float total[LSF_NUM_LIC_TYPE]; /* The total number of each license feature */
    float inUse[LSF_NUM_LIC_TYPE]; /* The in-use number of each license feature */
};

struct hostClassInfo {
    int   numHosts;
    int   numCpus;
    int   numCores;
};

typedef struct lim_licensekey {
    int              nkey;       /**< number of key */
    char**           keyname;   
} lim_licensekey_t;

typedef struct lim_licensekey_info {
    char* name;        /**< symphong feature name */
    char* keyName;     /**< name of symphong key */
    int licensed;      /**< the key is vaild (1) or not (0) */
    int numLicense;    /**< number license */
    char* notice;      /**< NOTICE string */
} lim_licensekey_info_t;

typedef struct lim_licensekey_reply {
    int              maxcore;    /**< max core that a symphony cluster supports */
    int              nkey;       /**< number of key */
    lim_licensekey_info_t** keyinfo;
} lim_licensekey_reply_t;

struct lsfLicUsage {
    struct licUsage licUsage;
    struct hostClassInfo hostInfo[NUM_CLASS_TYPE];
    char   substitution[NUM_SUBS][LEN_SUBS];
    char   cluster[MAXFILENAMELEN];
};

/* structure to announce mastership to specified host
 */
struct limHostAnnReq {
    int       nhosts;                        /* number of hosts */
    char      **hostnames;                   /* a list of host name */
    int       count;                         /* number of announcement */
};

/* Struct of Daemon Supportability */
struct param_entry {
#define HAS_PARAM_VALUE		0x001  /* the parameter has configured value */
#define HAS_PARAM_DEFAULT	0x002  /* the parameter has default value */

    int flags;         /* used above mask to check the value and default_value*/
    char *key;         /* a pointer point to parameter key */
    char *value;       /* a pointer point to parameter value */
    char *default_value;       /* a pointer point to parameter default */
};

typedef struct params_key_value_pair
{
    int num_params;		/* the total number of parameters pair */
    char *daemon_time;		/* the time on daemon side */
    struct param_entry *param;	/* one parameter pair entry */
} PKVP;
   
/**
 * \brief rusage information per host
 */
struct hRusage {
    char *name;       /**< The host name */
    int mem;          /**< Total resident memory usage in kbytes of all
                       * currently running processes in given host. */
    int swap;         /**< Total virtual memory usage in kbytes of all
                       * currently running processes in given process
                       * groups. */
    int utime;        /**< Cumulative total user time in seconds on given host*/
    int stime;        /**< Cumulative total system time in seconds on given host*/
    PKVP hostExtendInfoPKVPs; /**< extended info for future information passing, e.g.: exit code, pid/pgids */
};

struct jobRusagePack {
    struct jRusage *jrusage;
    struct jRusage *max_jrusage;
    int num_hrusage;
    struct hRusage *hrusage;
    int pamPid;
};

/*
 * Load Sharing error numbers 
 */

#define LSE_NO_ERR              0     /* initial value */
#define LSE_BAD_XDR             1     /* Error during XDR */
#define LSE_MSG_SYS             2     /* Failed in sending/receiving a msg */
#define LSE_BAD_ARGS            3     /* supplied arguments invalid */
#define LSE_MASTR_UNKNW         4     /* cannot find out the master LIM*/
#define LSE_LIM_DOWN            5     /* LIM does not respond */
#define LSE_PROTOC_LIM          6     /* LIM protocol error */
#define LSE_SOCK_SYS            7     /* A socket operation has failed */
#define LSE_ACCEPT_SYS          8     /* Failed in a accept system call */
#define LSE_BAD_TASKF           9     /* Bad LSF task file format*/
#define LSE_NO_HOST             10    /* No enough ok hosts found by LIM*/
#define LSE_NO_ELHOST           11    /* No host is found eligible by LIM */
#define LSE_TIME_OUT            12    /* communication timed out */
#define LSE_NIOS_DOWN           13    /* nios has not been started. */
#define LSE_LIM_DENIED          14    /* Operation permission denied by LIM */
#define LSE_LIM_IGNORE          15    /* Operation ignored by LIM */
#define LSE_LIM_BADHOST         16    /* host name not recognizable by LIM*/
#define LSE_LIM_ALOCKED         17    /* LIM already locked */
#define LSE_LIM_NLOCKED         18    /* LIM was not locked. */
#define LSE_LIM_BADMOD          19    /* unknown host model. */
#define LSE_SIG_SYS             20    /* A signal related system call failed*/
#define LSE_BAD_EXP             21    /* bad resource req. expression*/
#define LSE_NORCHILD            22    /* no remote child */
#define LSE_MALLOC              23    /* memory allocation failed */
#define LSE_LSFCONF             24    /* unable to open lsf.conf */
#define LSE_BAD_ENV             25    /* bad configuration environment */
#define LSE_LIM_NREG            26    /* Lim is not a registered service*/
#define LSE_RES_NREG            27    /* Res is not a registered service*/
#define LSE_RES_NOMORECONN      28    /* RES is serving too many connections*/
#define LSE_BADUSER             29    /* Bad user ID for REX */
#define LSE_RES_ROOTSECURE      30    /* Root user rejected          */
#define LSE_RES_DENIED          31    /* User permission denied      */
#define LSE_BAD_OPCODE          32    /* bad op code */
#define LSE_PROTOC_RES          33    /* RES Protocol error */
#define LSE_RES_CALLBACK        34    /* RES callback fails          */
#define LSE_RES_NOMEM           35    /* RES malloc fails            */
#define LSE_RES_FATAL           36    /* RES system call error       */
#define LSE_RES_PTY             37    /* RES cannot alloc pty        */
#define LSE_RES_SOCK            38    /* RES socketpair fails        */
#define LSE_RES_FORK            39    /* RES fork fails              */
#define LSE_NOMORE_SOCK         40    /* Privileged socks run out    */
#define LSE_WDIR                41    /* getwd() failed */
#define LSE_LOSTCON             42    /* Connection has been lost    */
#define LSE_RES_INVCHILD        43    /* No such remote child        */
#define LSE_RES_KILL            44    /* Remote kill permission denied */
#define LSE_PTYMODE             45    /* ptymode inconsistency       */
#define LSE_BAD_HOST            46    /* Bad hostname                */
#define LSE_PROTOC_NIOS         47    /* NIOS protocol error     */
#define LSE_WAIT_SYS            48    /* A wait system call failed */
#define LSE_SETPARAM            49    /* Bad parameters for setstdin */
#define LSE_RPIDLISTLEN         50    /* Insufficient list len for rpids */
#define LSE_BAD_CLUSTER         51    /* Invalid cluster name */
#define LSE_RES_VERSION         52    /* Incompatible versions of tty params */
#define LSE_EXECV_SYS           53    /* Failed in a execv() sys call*/
#define LSE_RES_DIR             54    /* No such directory */
#define LSE_RES_DIRW            55    /* The directory may not be accessible*/
#define LSE_BAD_SERVID          56    /* the service ID is invalid */
#define LSE_NLSF_HOST           57    /* request from a non lsf host */
#define LSE_UNKWN_RESNAME       58    /* unknown resource name specified */
#define LSE_UNKWN_RESVALUE      59    /* unknown resource value */
#define LSE_TASKEXIST           60    /* the task already registered */
#define LSE_BAD_TID             61    /* the task does not exist */
#define LSE_TOOMANYTASK         62    /* the task table is full */
#define LSE_LIMIT_SYS           63    /* A resource limit sys call failed*/
#define LSE_BAD_NAMELIST        64    /* bad index name list */
#define LSE_NO_LICENSE		65    /* no software license for host */
#define LSE_LIM_NOMEM           66    /* lim malloc failure */
#define LSE_NIO_INIT            67    /* nio not initialized. */
#define LSE_CONF_SYNTAX         68    /* Bad lsf.conf/lsf.sudoers syntax */
#define LSE_FILE_SYS            69    /* A file operation failed */
#define LSE_CONN_SYS            70    /* A connect sys call failed */
#define LSE_SELECT_SYS          71    /* A select system call failed */
#define LSE_EOF                 72    /* Reached the end of file */
#define LSE_ACCT_FORMAT         73    /* Bad lsf.acct file format */
#define LSE_BAD_TIME            74    /* Bad time specification */
#define LSE_FORK                75    /* Unable to fork child */
#define LSE_PIPE                76    /* Failed to setup pipe */
#define LSE_ESUB                77    /* esub/eexec file not found */
#define LSE_DCE_EXEC            78    /* dce task exec fail */
#define LSE_EAUTH               79    /* external authentication failed */
#define LSE_NO_FILE             80    /* cannot open file */
#define LSE_NO_CHAN             81    /* out of communication channels */
#define LSE_BAD_CHAN            82    /* bad communication channel */
#define LSE_INTERNAL            83    /* internal library error */
#define LSE_PROTOCOL            84    /* protocol error with server */
#define LSE_THRD_SYS            85    /* A thread system call failed (NT only)*/
#define LSE_MISC_SYS            86    /* A system call failed */
#define LSE_LOGON_FAIL          87    /* Failed to logon user (NT only) */
#define LSE_RES_RUSAGE          88    /* Failed to get rusage from RES */
#define LSE_NO_RESOURCE         89    /* no shared resource defined */
#define LSE_BAD_RESOURCE        90    /* Bad resource name */
#define LSE_RES_PARENT          91    /* res child Failed to contact parent */
#define LSE_NO_PASSWD           92    /* no password for user */
#define LSE_SUDOERS_CONF        93    /* lsf.sudoers file error */
#define LSE_SUDOERS_ROOT        94    /* lsf.sudoers not owned by root */
#define LSE_I18N_SETLC          95    /* i18n setlocale failed */
#define LSE_I18N_CATOPEN        96    /* i18n catopen failed */
#define LSE_I18N_NOMEM          97    /* i18n malloc failed */
#define LSE_NO_MEM              98    /* Cannot alloc memory */
#define LSE_REGISTRY_SYS        99    /* A registry system call failed (NT) */
#define LSE_FILE_CLOSE         100    /* Close a NULL-FILE pointer */
#define LSE_LIMCONF_NOTREADY   101    /* LIM configuration is not ready yet */
#define LSE_MASTER_LIM_DOWN    102    /* for LIM_CONF master LIM down */
#define LSE_MLS_INVALID        103    /* invalid MLS label */
#define LSE_MLS_CLEARANCE      104    /* not enough clearance */
#define LSE_MLS_RHOST          105    /* reject by rhost.conf */
#define LSE_MLS_DOMINATE       106    /* require label not dominate */
#define LSE_NO_CAL             107    /* Win32: No more connections can be 
				       * made to this remote computer at this 
				       * time because there are already as 
				       * many connections as the computer can 
				       * accept. */
#define LSE_NO_NETWORK         108    /* Win32: The network location cannot be 
				       * reached. For information about 
				       * network troubleshooting, see Windows 
				       * Help. */
#define LSE_GETCONF_FAILED     109    /* Failed to get configuration
                                       * information from hosts specified by
                                       * the LSF_SERVER_HOSTS parameter in
                                       * lsf.conf */
#define LSE_TSSINIT            110    /* Win32: terminal service not properly 
                                       * initialized */
#define LSE_DYNM_DENIED        111    /* Dynamic addHost denied */
#define LSE_LIC_OVERUSE        112    /* In license overuse status */
#define LSE_EGOCONF            113    /* unable to open ego.conf */
#define LSE_BAD_EGO_ENV        114    /* bad configuration environment */
#define LSE_EGO_CONF_SYNTAX    115    /* Bad ego.conf syntax */
#define LSE_EGO_GETCONF_FAILED 116    /* Failed to get configuration
                                       * information from hosts specified by
                                       * the EGO_SERVER_HOSTS parameter in
                                       * ego.conf 
                                       */
#define LSE_NS_LOOKUP          117    /* name service lookup failure */
#define LSE_BAD_PASSWD         118    /* User password incorrect */

#define LSE_UNKWN_USER         119    /* User name is not in password database */
#define LSE_NOT_WINHOST        120    /* The host is not a Windows host */
#define LSE_NOT_MASTERCAND     121    /* The host is not a master candidate host */
#define LSE_HOST_UNAUTH        122    /* Permission denied. This command must
                                       * be issued from a master, master
                                       * candidate, or local host */
#define LSE_UNRESOLVALBE_HOST  123    /* master Lim can not reslove the host
                                       * name of slave lim */ 
#define LSE_RESOURCE_NOT_CONSUMABLE 124 /* resource not consumable */
#define LSE_SHUTDOWN 125 /* host is in exiting loop for rejected by clusters */
#define LSE_BAD_SYNTAX         126    /* Bad string syntax */
#define LSE_LIVE_PERSIST       127    /* Live reconfig persist failure */
#define LSE_LIVE_FAIL          128    /* Live reconfig exception failure */
#define LSE_BAD_HOST_TYPE      129    /* The host type can not be recognized */
#define LSE_INVALID_LICENSE    130    /* Invalid license */
#define LSE_NO_ENTITLEMENT     131    /* No entitlement found */
#define LSE_SLOTS_IN_RUSAGE    132    /* for example, "rsuage[slots=2]" */
#define LSE_INVALID_EXCLAMATION_MARK    133    /* for example, "order[!!slots]" */
#define LSE_INVALID_MASTERHOSTS_NUMBER    134    /* invalid master hosts number */
#define LSE_REMOVE_JOBVM       135    /* Cannot remove Dynamic cluster job container */
#define LSE_INVALID_AFFINITY_RESREQ  136 /* Invalid affinity[] resReq */
#define LSE_NERR               137    /* Moving number, size of ls_errmsg[] */




#define LSE_ISBAD_RESREQ(s)	(((s) == LSE_BAD_EXP) \
				 || ((s) == LSE_UNKWN_RESNAME) \
				 || ((s) == LSE_UNKWN_RESVALUE))

#define LSE_SYSCALL(s)          (((s) == LSE_SELECT_SYS) \
				 || ((s) == LSE_CONN_SYS) \
				 || ((s) == LSE_FILE_SYS) \
				 || ((s) == LSE_MSG_SYS) \
				 || ((s) == LSE_SOCK_SYS) \
				 || ((s) == LSE_ACCEPT_SYS) \
				 || ((s) == LSE_SIG_SYS) \
				 || ((s) == LSE_WAIT_SYS) \
				 || ((s) == LSE_EXECV_SYS) \
				 || ((s) == LSE_LIMIT_SYS) \
				 || ((s) == LSE_PIPE) \
				 || ((s) == LSE_ESUB) \
				 || ((s) == LSE_REGISTRY_SYS) \
				 || ((s) == LSE_MISC_SYS))

/* The following macro: TIMEVAL(), CLOCKIT(), CLOCKIT_START_BLOCK(),
 * and CLOCKIT_END_BLOCK() are kept here only for backward compatibility
 * purpose, they should not be used in any new development.
 * Instead, please use TIMEIT(), TIMEVAL2(), TIMEIT_START_BLOCK(),
 * and TIMEIT_END_BLOCK().
 */ 
#define TIMEVAL(level,func,val) \
                    { if (timinglevel > level) { \
                          struct timeval before, after; \
                          struct timezone tz; \
                          gettimeofday(&before, &tz); \
                          func; \
                          gettimeofday(&after, &tz); \
                          val = (int)((after.tv_sec - before.tv_sec)*1000 + \
                               (after.tv_usec-before.tv_usec)/1000); \
                       } else { \
                              func; \
                              val = 0; \
                         } \
                    }

#if defined(WIN32)
#define CLOCKIT(level, func, ticks, ut, st) \
{ \
  func; ut=0; st=0; \
}

#define CLOCKIT_START_BLOCK(level) \
{

#define CLOCKIT_END_BLOCK(level, ticks, name) \
}

#else /* WIN32 */ 
#define CLOCKIT(level, func, ticks, ut, st) \
{ if (timinglevel > level && ticks > 0) { \
      struct tms   buf; \
      struct tms   buf2; \
      times(&buf); \
      func; \
      times(&buf2); \
      ut = (float)(buf2.tms_utime - buf.tms_utime)/ticks; \
      st = (float)(buf2.tms_stime - buf.tms_stime)/ticks; \
  } else { \
      func; ut=0; st=0; \
  }\
}

/* Same as CLOCKIT above, except this can be used to clock a block of code */
#define CLOCKIT_START_BLOCK(level) \
{ struct tms   _before; \
  struct tms   _after; \
  if (timinglevel > level) { \
      times(&_before); \
  }

#define CLOCKIT_END_BLOCK(level, ticks, name) \
  if (timinglevel > level) { \
      float ut, st; \
      times(&_after); \
      ut = (float)(_after.tms_utime - _before.tms_utime)/ticks; \
      st = (float)(_after.tms_stime - _before.tms_stime)/ticks; \
      ls_syslog(LOG_INFO,"L%d %s ut %.2f s st %.2f s", level, name, ut, st); \
  } \
}
#endif /* WIN32 */

/* New timing macro: TIMEIT(), TIMEVAL2(),
 * and TIMEIT_START_BLOCK()/TIEMIT_END_BLOCK() are introduced to profile
 * function with both cputime and wall clock runtime usage.
 * For cputime usage, clockticks holds the number of clock ticks per second.
 * On Linux it can be obtained using sysconf(_SC_CLK_TCK);
 * On Solaris and others the CLK_TCK is defined in <time.h>, however in 
 * POSIX-1996 this symbol is mentioned as obsolescent;
 * We don't have such method available on Windows. Currently, we leave it
 * unsupported. We could find other ways to obtain cpu usage on Windows
 * in the future.
 */
#ifdef WIN32
/* On windows, we define dummy tms and times() call in lib.nt.c,
 * just to get code compiled.
 */
struct tms {
    long tms_utime;
    long tms_stime;
    long tms_cutime;
    long tms_cstime;
};
extern long times(struct tms *);
#endif

typedef struct ls_timeval {
    float rtime;
    float utime;
    float stime;
} LS_TIMEVAL_T;

#define LS_TIMEVAL_ZERO(tv) \
{                           \
    tv.rtime = 0.0;         \
    tv.utime = 0.0;         \
    tv.stime = 0.0;         \
}
                                                                                
#define LS_TIMEVAL_INC(tv, newtv) \
{                                 \
    tv.rtime += newtv.rtime;      \
    tv.utime += newtv.utime;      \
    tv.stime += newtv.stime;      \
}

#define LOG_TIME_MSG(level, name, tv, count, msg) \
{ if (timinglevel > level) { \
      ls_syslog(LOG_INFO, \
"L%d %s rtime %.2f ms, utime %.2f ms, stime %.2f ms, count %d %s", \
		level, name, tv.rtime, tv.utime, tv.stime, count, msg); \
  } \
}

#define LOG_TIMESTAMP(level, funcName, msg) \
{ if ( timinglevel > level) { \
      struct timeval current; \
      gettimeofday(&current, NULL); \
      ls_syslog(LOG_INFO, "L%d %s: %s, current time: %ld s %ld us", \
        level, funcName, msg, current.tv_sec, current.tv_usec); \
  } \
}

#define TIMEIT(level, func, name) \
{ if (timinglevel > level && clockticks > 0) { \
      struct timeval _before, _after; \
      struct timezone _tz; \
      struct tms _buf, _buf2; \
      gettimeofday(&_before, &_tz); \
      times(&_buf); \
      func; \
      gettimeofday(&_after, &_tz); \
      times(&_buf2); \
      ls_syslog(LOG_INFO,"L%d %s rtime %.2f ms, utime %.2f ms, stime %.2f ms", \
		level, \
		name, \
		(_after.tv_sec - _before.tv_sec)*1000.0 + \
		(_after.tv_usec - _before.tv_usec)/1000.0, \
		1000.0*(_buf2.tms_utime - _buf.tms_utime)/clockticks, \
		1000.0*(_buf2.tms_stime - _buf.tms_stime)/clockticks); \
  } else { \
      func; \
  } \
}

#define TIMEIT_FORK(level, pid, name) \
{ if (timinglevel > level && clockticks > 0) { \
      struct timeval _before, _after; \
      struct timezone _tz; \
      struct tms _buf, _buf2; \
      gettimeofday(&_before, &_tz); \
      times(&_buf); \
      pid =fork(); \
      gettimeofday(&_after, &_tz); \
      times(&_buf2); \
      if (pid > 0) {    \
        ls_syslog(LOG_INFO,"L%d %s rtime %.2f ms, utime %.2f ms, stime %.2f ms", \
    	level, \
    	name, \
    	(_after.tv_sec - _before.tv_sec)*1000.0 + \
    	(_after.tv_usec - _before.tv_usec)/1000.0, \
    	1000.0*(_buf2.tms_utime - _buf.tms_utime)/clockticks, \
    	1000.0*(_buf2.tms_stime - _buf.tms_stime)/clockticks); \
      }\
  } else { \
      pid =fork(); \
  } \
}


#define TIMEVAL2(level, func, tv) \
{ if (timinglevel > level && clockticks > 0) { \
      struct timeval _before, _after; \
      struct timezone _tz; \
      struct tms _buf, _buf2; \
      gettimeofday(&_before, &_tz); \
      times(&_buf); \
      func; \
      gettimeofday(&_after, &_tz); \
      times(&_buf2); \
      tv.rtime = (_after.tv_sec - _before.tv_sec)*1000.0 + \
		 (_after.tv_usec - _before.tv_usec)/1000.0; \
      tv.utime = 1000.0*(_buf2.tms_utime - _buf.tms_utime)/clockticks; \
      tv.stime = 1000.0*(_buf2.tms_stime - _buf.tms_stime)/clockticks; \
  } else { \
      func; \
      tv.rtime = 0.0; \
      tv.utime = 0.0; \
      tv.stime = 0.0; \
  }\
}

#define TIMEIT_START_BLOCK(level) \
{ struct tms _buf, _buf2; \
  struct timeval _before, _after; \
  struct timezone _tz; \
  if (timinglevel > level) { \
      gettimeofday(&_before, &_tz); \
      times(&_buf); \
  }

#define TIMEIT_END_BLOCK(level, name) \
  if (timinglevel > level) { \
      float rt, ut, st; \
      gettimeofday(&_after, &_tz); \
      times(&_buf2); \
      rt = (_after.tv_sec - _before.tv_sec)*1000.0 + \
	   (_after.tv_usec - _before.tv_usec)/1000.0; \
      ut = 1000.0*(_buf2.tms_utime - _buf.tms_utime)/clockticks; \
      st = 1000.0*(_buf2.tms_stime - _buf.tms_stime)/clockticks; \
      ls_syslog(LOG_INFO,"L%d %s rtime %.2f ms, utime %.2f ms, stime %.2f ms", \
                level, name, rt, ut, st); \
  } \
}

/* Log classes for logging diagnostic messages
 */
#define LC_SCHED    0x00000001 /* Log msg regarding to job scheduling */
#define LC_EXEC     0x00000002 /* Log the steps for executing a job */
#define LC_TRACE    0x00000004 /* Log the walk steps of the program */
#define LC_COMM     0x00000008 /* Log msg pertaining to communications */
#define LC_XDR      0x00000010 /* Log everything transfered by XDR */
#define LC_CHKPNT   0x00000020 /* Log msg regarding to chkpnt process */
#define LC_LICENCE  0x00000040 /* Log msg related to license management */
#define LC_LICENSE  0x00000040 /* Log msg related to license management */
#define LC_FILE     0x00000080 /* Log msg related to file transfers */
#define LC_AFS      0x00000100 /* Log msg related to AFS */
#define LC_AUTH     0x00000200 /* Log msg related to authentication */
#define LC_HANG     0x00000400 /* Mark the places where program might hang */
#define LC_MULTI    0x00000800 /* Log msg related to multi-cluster */
#define LC_SIGNAL   0x00001000 /* Log msg related to signals */
#define LC_DCE      0x00002000 /* Log msg related to DCE */
#define LC_PIM      0x00004000 /* Log msg related to PIM */
#define LC_MEMORY   0x00004000 /* Log msg related to MEMORY allocation */
#define LC_SYS      0x00008000 /* Log msg related to system calls */
#define LC_JLIMIT   0x00010000 /* Log msg related to job slot limits */
#define LC_FAIR     0x00020000 /* Log msg related to fair share policy */
#define LC_PREEMPT  0x00040000 /* Log msg related to preemption policy */
#define LC_PEND     0x00080000 /* Log msg related to job pending reasons */
#define LC_EEVENTD  0x00100000 /* Log msg related to eeventd */
#define LC_LOADINDX 0x00200000 /* Log msg related to load index */
#define LC_RESOURCE 0x00200000 /* Log msg related to resource broker */
			       /* NOTE: reuse LC_LOADINDX because they have
                                * similar meaning */
#define LC_JGRP     0x00400000 /* Log msg related to job group */
#define LC_JARRAY   0x00800000 /* Log msg related to job array */
#define LC_MPI      0x01000000 /* Log msg related to MPI message */
#define LC_ELIM     0x02000000 /* Log msg related to ELIM */
#define LC_M_LOG    0x04000000 /* Log msg related to multievent logging */
#define LC_PERFM    0x08000000 /* Log msg related to performance */
#define LC_DLOG     0x10000000 /* Log msg related to dlogd */
#define LC_HPC      0x20000000 /* Log msg related to rms, sgi, and other
				* topology aware scheduling plugins
				*/
#define LC_LICSCHED 0x40000000 /* Log msg related to License Scheduler */

#define LC_XDRVERSION 0x80000000  /* Log msg related to XDR Version */
#define LC_FLEX     0x80000000 /* Log msg related to flex */

#define LC_ADVRSV   LC_DLOG  /* Log msg related to Advance Reservation */
#define LC_RESREQ   LC_M_LOG   /* Log msg related to Multipass resReq, 
                                                           * including compound and alternative
                                                           * resReq.
                                                           */
#if 0
#define LC_AC       LC_EEVENTD /* log msg related to adaptive cluster */
#endif
/*
  *end of Log classes 
 */

/* Define a new set of Log classes LC2_*** for log class control in the furture, the existing LC_** will be 
 *  mapped to LC2_*** correspondingly, project_133893
 */
#define LC2_SCHED    1/* Log msg regarding to job scheduling */
#define LC2_EXEC     2 /* Log the steps for executing a job */
#define LC2_TRACE    3/* Log the walk steps of the program */
#define LC2_COMM     4 /* Log msg pertaining to communications */
#define LC2_XDR      5 /* Log everything transfered by XDR */
#define LC2_CHKPNT   6 /* Log msg regarding to chkpnt process */
#define LC2_LICENCE  7 /* Log msg related to license management */
#define LC2_LICENSE  8 /* Log msg related to license management.*/
#define LC2_FILE     9 /* Log msg related to file transfers */
#define LC2_AFS      10 /* Log msg related to AFS */
#define LC2_AUTH     11 /* Log msg related to authentication */
#define LC2_HANG     12/* Mark the places where program might hang */
#define LC2_MULTI    13/* Log msg related to multi-cluster */
#define LC2_SIGNAL   14 /* Log msg related to signals */
#define LC2_DCE      15 /* Log msg related to DCE */
#define LC2_PIM      16 /* Log msg related to PIM */
#define LC2_MEMORY   17/* Log msg related to MEMORY allocation */
#define LC2_SYS      18 /* Log msg related to system calls */
#define LC2_JLIMIT   19 /* Log msg related to job slot limits */
#define LC2_FAIR     20 /* Log msg related to fair share policy */
#define LC2_PREEMPT  21 /* Log msg related to preemption policy */
#define LC2_PEND     22 /* Log msg related to job pending reasons */
#define LC2_EEVENTD  23 /* Log msg related to eeventd */
#define LC2_LOADINDX 24 /* Log msg related to load index */
#define LC2_RESOURCE 25 /* Log msg related to resource broker */
			       /* NOTE: reuse LC_LOADINDX because they have
                                * similar meaning */
#define LC2_JGRP     26 /* Log msg related to job group */
#define LC2_JARRAY   27 /* Log msg related to job array */
#define LC2_MPI      28 /* Log msg related to MPI message */
#define LC2_ELIM     29 /* Log msg related to ELIM */
#define LC2_M_LOG    30 /* Log msg related to multievent logging */
#define LC2_PERFM    31 /* Log msg related to performance */
#define LC2_DLOG     32 /* Log msg related to dlogd */
#define LC2_HPC      33 /* Log msg related to rms, sgi, and other
				* topology aware scheduling plugins
				*/
#define LC2_LICSCHED 34 /* Log msg related to License Scheduler */

#define LC2_XDRVERSION 35  /* Log msg related to XDR Version */
#define LC2_FLEX     36 /* Log msg related to flex */

#define LC2_ADVRSV   37  /* Log msg related to Advance Reservation */
#define LC2_RESREQ   38   /* Log msg related to Multipass resReq, 
                                                           * including compound and alternative
                                                           * resReq.
                                                           */

#define LC2_USER1      39   /* Log class enhancement, new pre-defined LSF log classes */   
#define LC2_USER2      40   /* Log class enhancement, new pre-defined LSF log classes */
#define LC2_SCHMOD_DEFAULT		41
#define LC2_SCHMOD_FCFS			42
#define LC2_SCHMOD_FAIRSHARE	    43
#define LC2_SCHMOD_LIMIT        44 
#define LC2_SCHMOD_PARALLEL		45
#define LC2_SCHMOD_RESERVE		46
#define LC2_SCHMOD_MC			47
#define LC2_SCHMOD_PREEMPTION	    48
#define LC2_SCHMOD_ADVRSV		49
#define LC2_SCHMOD_PS			50
#define LC2_SCHMOD_APS			51
#define LC2_SCHMOD_CPUSET		52
#define LC2_GUARANTEE		        53
#define LC2_INTERNAL_TEST	        54
#define LC2_LIVECONF                    55   /* Log msg related to Live Reconfig */
#define LC2_PERFTIME                    56
#define LC2_MC_RESOURCE                 57
#define LC2_ENTITLEMENT                 58 /* product edition entitlement */


/*RFC#3633*/
#define LC2_KRB                              59
#define LC2_DC                          60 /* Dynamic Cluster */

#define LC2_CGROUP                      61 /* Log msg related to cgroup operations. */
#define LC2_TOPOLOGY                    62 /*log msg realted to hardware topology*/
#define LC2_AFFINITY                    63 /*log msg related to affinity*/
#define LC2_LSF_PE              64      /* Log msg related to LSF PE integration */
#define LC2_G_FAIR                      65 /* global fairshare scheduling */

/*
  *end of Log classes 
 */


/* Extended log levels 
 */
#define LOG_DEBUG1  LOG_DEBUG + 1  /* log messages for finding bugs */
#define LOG_DEBUG2  LOG_DEBUG + 2  /* log very detailed messages */
#define LOG_DEBUG3  LOG_DEBUG + 3

/*
 * LSF event numbers.
 */
#define LSF_EVENT_LIM_DOWN	1  /* The lim on a host has gone down. */
#define LSF_EVENT_RES_DOWN	2  /* The res on a host has gone down. */
#define LSF_EVENT_SBD_DOWN	3  /* The sbatchd on a host has gone down. */
#define LSF_EVENT_HOST_UNLIC	4  /* A host has become unlicensed. */
#define LSF_EVENT_MASTER_ELECT	5  /* A new master has been elected. */
#define LSF_EVENT_MASTER_RESIGN	6  /* An old master has given up being master. */
#define LSF_EVENT_MBD_UP	7  /* mbatchd has come up and is ready to schedule. */
#define LSF_EVENT_MBD_DOWN	8  /* mbatchd has gone down as a result of error. */
#define LSF_EVENT_MBD_RECONFIG	9  /* mbatchd is going to restart. */
#define LSF_EVENT_WORKDIR_FULL	10 /* The LSF_SHAREDIR has become full. */
#define LSF_EVENT_HOST_OPENED   11 /* host is opened by admin. */
#define LSF_EVENT_HOST_CLOSED   12 /* host is closed by admin. */
#define LSF_EVENT_QUEUE_OPENED  13 /* queue is opened by admin. */
#define LSF_EVENT_QUEUE_CLOSED  14 /* queue is closed by admin. */
#define LSF_EVENT_SCH_DOWN      15 /* MBSCHD down abnormally. */ 
#define LSF_EVENT_LIC_OVERUSE   16 /* Class license, dualcore or MC license overuse occurs */
/*
 * LSF NIOS return code
 */
#define LSF_NIOS_REQUEUE	127 /* nios receive requeue command */


/*
 * Function declarations
 */

extern int     lserrno;
extern int     masterLimDown; /* for LIM_CONF Master LIM down */
extern int     ls_nerr;
extern char    *ls_errmsg[];
extern int     logclass;
extern char    *logclassArray;
extern int     timinglevel;
extern int     clockticks;

/* 
 * Version of the last lim/res that was contacted.
 * -1 indicates the lim/res has not been called.
 */
extern int     lsf_lim_version;	/* version of lim */

/**
 * \page ls_readconfenv ls_readconfenv
 * Reads the LSF configuration parameters from the *confPath/lsf.conf file. 
 *
 * \ref ls_readconfenv reads the LSF configuration parameters from the 
 * *confPath/lsf.conf file. If confPath is NULL, the LSF configurable parameters 
 * are read from the ${LSF_ENVDIR:-/etc}/lsf.conf file. See lsf.conf. The input 
 * paramList is an array of data structures that are defined in <lsf/lsf.h>. The 
 * paramName parameter in the config_param data structure should be the pointer 
 * to the configuration parameter name defined in the *confPath/lsf.conf or 
 * /etc/lsf.conf file if confPath is NULL. The paramValue parameter in the 
 * config_param data structure must initially contain NULL and is then modified 
 * to point to a result string if a matching paramName is found in the lsf.conf 
 * file. A typical data structure declaration is as follows: 
 * 
 * struct config_param paramList[] =
 * \n {
 * \n \#define LSF_CONFDIR  0
 * \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; {"LSF_CONFDIR", NULL},
 * \n \#define LSF_LOGDIR   1
 * \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; {"LSF_LOGDIR", NULL},
 * \n \#define LSF_SERVERDIR 2
 * \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; {"LSF_SERVERDIR", NULL},
 * \n \#define LSF_RES_DEBUG 3
 * \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; {"LSF_RES_DEBUG", NULL},
 * \n \#define LSF_NPARAMS  4
 * \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; {NULL, NULL},
 * \n }; 
 *
 * By calling \ref ls_readconfenv (paramList, "/localpath/etc"), it is possible to read 
 * in the required parameters and use the defined constants as indices for 
 * referencing the parameters when needed. If a certain parameter name is not 
 * found in the lsf.conf file, then its paramValue will remain NULL on return. 
 *
 * <b>\#include <lsf/lsf.h> 
 *
 * int ls_readconfenv(struct config_param *paramList, 
 *                               char *confPath)</b>
 *
 * @param *paramlist 
 * An array of data structures that are defined in <lsf/lsf.h>. 
 *
 * @param *confPath
 * The config file's path.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref config_param
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful. 
 *
 * @return int:-1 
 * \n Function failed. 
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none 
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 *
 * @see 
 * none
 * 
 */
extern int     ls_readconfenv P_((struct config_param *, char *));
  
/**
 * \page ls_placereq ls_placereq
 * Returns the most suitable host() for the task() with regards to current load 
 * conditions and the task's resource requirements. 
 *
 * \ref ls_placereq sends a task placement request to the LIM. The LIM returns a 
 * set of most suitable host() for the task(), taking into account the current 
 * load conditions and the task's resource requirements. Hostnames may be 
 * duplicated for hosts that have sufficient resources to accept multiple tasks 
 * (for example, multiprocessors). 
 *
 * If \ref ls_placereq is successful, an array of host names is returned and *num 
 * is set to reflect the number of returned hosts. Otherwise, NULL is returned 
 * and lserrno is set to indicate the error. 
 *
 * The routine returns a pointer to a dynamically allocated array of strings 
 * which can be freed in subsequent calls. 
 *
 * <b>\#include <lsf/lsf.h> 
 *
 * char **ls_placereq(char *resreq, int *num, int options, 
 *                   char *fromhost)</b>
 *
 *
 * @param *resreq 
 * The input parameter resreq is a resource requirement expression that 
 * characterizes the resource needs of a single task. You can retrieve this 
 * parameter by calling \ref ls_eligible or the application can supply its own. See 
 * Administering IBM Platform LSF for more information about resource requirement
 * expressions. The names used for resource requirements are defined by the LSF 
 * administrator in the configuration file LSF_CONFDIR/lsf.shared. You can obtain 
 * the available resource names by calling \ref ls_info or running the LSF utility 
 * program lsinfo(). If resreq is NULL, then the default is assumed, which is to 
 * require a host of the same type as the local host with low 1-minute average 
 * CPU queue length and paging rate. 
 *
 * @param *num 
 * The input parameter *num is the number of hosts requested. If *num is zero, 
 * then all eligible hosts are requested. If *num is NULL, then a single host 
 * is requested. 
 *
 * @param options 
 * The input parameter options is constructed from the bitwise inclusive OR of 
 * zero or more of the flags that are defined in <lsf/lsf.h>. These flags are 
 * documented in \ref options_ls_load. 
 *
 * @param *fromhost 
 * fromhost is the host from which the task originates when LIM makes the 
 * placement decision. Preference is given to fromhost over remote hosts that 
 * do not have significantly lighter loads or greater resources. This preference 
 * avoids unnecessary task transfer and reduces overhead. If fromhost is NULL, 
 * then the local host is assumed. 
 *
 * <b>Data Structures:</b>
 * \par
 * none
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * \ref options_ls_load
 *
 * @return char **:An array of host names is returned. 
 * \n Function was successful.
 * @return NULL 
 * \n Function failed. 
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none 
 *
 * \b Files:
 * \par
 * none
 *
 * @see \ref ls_placeofhosts  
 * @see \ref ls_loadadj  
 * @see \ref ls_eligible  
 * @see \ref ls_info   
 * @see \ref ls_load  
 *
 */ 
extern char    **ls_placereq P_((char *resreq, int *numhosts, int options,
				char *fromhost));
				
/**
 * \page ls_placeofhosts ls_placeofhosts
 * Returns the most suitable host() for the task() from a set of candidate hosts 
 * with regards to current load conditions and the task's resource requirements.
 * 
 * \ref ls_placeofhosts sends a task placement request to the LIM. The LIM returns 
 * a set of most suitable host() for the task(), taking into account the current 
 * load conditions and the task's resource requirements. Hostnames may be 
 * duplicated for hosts that have sufficient resources to accept multiple tasks 
 * (for example, multiprocessors). 
 *
 * If \ref ls_placeofhosts is successful, an array of host names is returned and 
 * *num is set to reflect the number of returned hosts. Otherwise, 
 * \ref ls_placeofhosts returns NULL and sets lserrno to indicate the error. 
 *
 * The routine returns a pointer to a dynamically allocated array of strings 
 * which can be freed in subsequent calls. 
 *
 * <b>\#include <lsf/lsf.h> 
 *
 * char **ls_placeofhosts(char *resreq, int *num, int options, 
 *                        char *fromhost, char **hostlist, int listsize)</b>
 *
 * @param *resreq 
 * resreq is a resource requirement expression that characterizes the resource 
 * needs of a single task. You can retrieve this parameter by calling 
 * \ref ls_eligible or the application can supply its own. See Administering 
 * IBM Platform LSF for more information about resource requirement expressions.
 * The names used for resource requirements are defined by the LSF administrator 
 * in the configuration file LSF_CONFDIR/lsf.shared. You can obtain the available 
 * resource names by calling \ref ls_info or running the LSF utility program 
 * lsinfo(). If resreq is NULL, then the default is assumed, which is to require 
 * a host of the same type as the local host with low 1-minute average CPU queue 
 * length and paging rate. 
 *
 * @param *num 
 * *num is the number of hosts requested. If *num is zero, then all eligible 
 * hosts are requested. If *num is NULL, then a single host is requested. 
 *
 * @param options 
 * options is constructed from the bitwise inclusive OR of zero or more of the 
 * flags that are defined in <lsf/lsf.h>. These flags are documented in 
 * \ref options_ls_load 
 *
 * @param *fromhost 
 * fromhost is the host from which the task originates when LIM makes the 
 * placement decision. Preference is given to fromhost over remote hosts that 
 * do not have significantly lighter loads or greater resources. This preference 
 * avoids unnecessary task transfer and reduces overhead. If fromhost is NULL, 
 * then the local host is assumed. 
 *
 * @param **hostlist 
 * Specifies a list of candidate hosts from which \ref ls_placeofhosts can choose 
 * suitable hosts. 
 *
 * @param listsize 
 *  The number of host entry in hostlist. 
 *
 *
 * <b>Data Structures:</b>
 * \par
 * none
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * \ref options_ls_load
 *
 * @return char **:An array of host names is returned. 
 * \n Function was successful.
 * @return NULL 
 * \n Function failed. 
 *
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none 
 *
 * \b Files:
 * \par
 * none
 *
 * @see \ref ls_placereq  
 * @see \ref ls_loadadj  
 * @see \ref ls_eligible  
 * @see \ref ls_info 
 * @see \ref ls_load  
 */				
extern char    **ls_placeofhosts P_((char *resreq, int *numhosts, 
                                int options, char *fromhost, char **hostlist, 
                                int listsize));
extern char    **ls_placeoftype P_((char *resreq, int *numhosts, 
                                int options, char *fromhost, char *hosttype));
                                
  
/**
 * \page ls_load ls_load
 * This routine returns the dynamic load information of qualified hosts. 
 * 
 * \ref ls_load returns all load indices. The result of this call is an array of 
 * hostLoad data structures as defined in <lsf/lsf.h>. The status component of
 * the hostLoad structure is an array of integers. The high order 16 bits of the
 * first int are used to mark the operation status of the host. Possible 
 * states defined in <lsf/lsf.h> are as follows: \ref status_ls_load.
 * 
 * The low order 16 bits of the first int are reserved. The other int()
 * of the status array is used to indicate the load status of the host. If any
 * of these bits is set, then the host is considered to be busy (overloaded).
 * Each bit (starting from bit 0) in int() represents one load index that 
 * caused the host to be busy. If bit i is set then the load index corresponding
 * to li[i] caused the host to be busy. An int can be used to for 32 load 
 * indices. If number of load indices on the host, both built-in and user defined,
 * are more than 32, more than one int will be used. 
 * 
 * Programmers can use macros to test the status of a host. The most commonly 
 * used macros include: 
 * 
 * - LS_ISUNAVAIL(status) 
 * - LS_ISBUSY(status) 
 * - LS_ISBUSYON(status, index) 
 * - LS_ISLOCKEDU(status) 
 * - LS_ISLOCKEDW(status) 
 * - LS_ISLOCKED(status) 
 * - LS_ISRESDOWN(status) 
 * - LS_ISUNLICENSED(status) 
 * - LS_ISOK(status) 
 * 
 * In the hostLoad data structure, the li vector contains load information on 
 * various resources on a host. The elements of the load vector are determined 
 * by the namelist parameter. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * 
 * struct hostLoad *ls_load(char *resreq, int *numhosts,
 *                          int options, char *fromhost)</b>
 *
 * @param *resreq 
 * resreq is a char string describing resource requirements. Only the load
 * vectors of the hosts satisfying the requirements will be returned. If resreq
 * is NULL, the load vectors of all hosts will be returned.
 *
 * @param *numhosts 
 * numhosts is the address of an int which initially contains the number of
 * hosts requested. If *numhosts is 0, request information on as many hosts as 
 * satisfy resreq. If numhosts is NULL, requests load information on one (1) 
 * host. If numhosts is not NULL, then *numhosts will contain the number of 
 * hostLoad records returned on success.
 *
 * @param options
 * options is constructed from the bitwise inclusive OR of zero or more of the 
 * following flags, as defined in <lsf/lsf.h>: \ref options_ls_load.
 * 
 * @param *fromhost
 * fromhost is the name of the host from which a task might be transferred. This
 * parameter affects the host selection in such a way as to give preference to
 * fromhost if the load on other hosts is not much better. If fromhost is NULL,
 * the local host is assumed. 
 *
 * <b>Data Structures:</b>
 * \par
 * \ref hostLoad
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * \ref status_ls_load
 * \n\ref options_ls_load
 *
 * @return struct hostLoad *:An array of hostLoad struct containing load information.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n$LSF_CONFDIR/lsf.cluster.cluster_name 
 *
 * @see \ref ls_loadinfo 
 * @see \ref ls_loadofhosts 
 */                                
extern struct  hostLoad *ls_load P_((char *resreq, int *numhosts, int options,
				char *fromhost));
				
/**
 * \page ls_loadofhosts ls_loadofhosts
 * Returns all load indices except those specified.
 * 
 * This routine returns the dynamic load information of qualified hosts. 
 *
 * \ref ls_loadofhosts returns all load indices except for the ones excluded b the 
 * parameters. The result of this call is an array of hostLoad data structures 
 * as defined in <lsf/lsf.h>. The status component of the hostLoad structure is 
 * an array of integers. The high order 16 bits of the first int are used to 
 * mark the operation status of the host. Possible states defined in <lsf/lsf.h> 
 * are as follows: \ref status_ls_load.
 * 
 * The low order 16 bits of the first int are reserved. The other int() 
 * of the status array is used to indicate the load status of the host. If any 
 * of these bits is set, then the host is considered to be busy (overloaded). 
 * Each bit (starting from bit 0) in int() represents one load index that 
 * caused the host to be busy. If bit i is set then the load index corresponding 
 * to li[i] caused the host to be busy. An int can be used to for 32 load 
 * indices. If number of load indices on the host, both built-in and user 
 * defined, are more than 32, more than one int will be used. 
 * 
 * Programmers can use macros to test the status of a host. The most commonly 
 * used macros include: 
 * 
 * - LS_ISUNAVAIL(status) 
 * - LS_ISBUSY(status) 
 * - LS_ISBUSYON(status, index) 
 * - LS_ISLOCKEDU(status) 
 * - LS_ISLOCKEDW(status) 
 * - LS_ISLOCKED(status) 
 * - LS_ISRESDOWN(status) 
 * - LS_ISUNLICENSED(status) 
 * - LS_ISOK(status) 
 * 
 * In the hostLoad data structure, the li vector contains load information on 
 * various resources on a host. The elements of the load vector are determined 
 * by the namelist parameter. 
 *
 * The returned hostLoad array is sorted according to the order section of the 
 * resource requirements, resreq (or, if not specified, the 1-minute average CPU 
 * queue length and paging rate), with the lightest loaded host being the first. 
 *
 * hostlist is an array of listsize host or cluster names. If not NULL, then only 
 * load information about hosts in this list will be returned. 
 *
 * <b>\#include <lsf/lsf.h> 
 *
 * struct hostLoad *ls_loadofhosts(char *resreq, int *numhosts, 
 *                  int options, char *fromhost, char **hostlist, 
 *                  int listsize)</b>
 *
 * @param *resreq 
 * resreq is a char string describing resource requirements. Only the load 
 * vectors of the hosts satisfying the requirements will be returned. If resreq 
 * is NULL, the load vectors of all hosts will be returned. 
 *
 * @param *numhosts 
 * numhosts is the address of an int which initially contains the number of 
 * hosts requested. If *numhosts is 0, request information on as many hosts as 
 * satisfy resreq. If numhosts is NULL, requests load information on one (1) host. 
 * If numhosts is not NULL, then *numhosts will contain the number of hostLoad 
 * records returned on success. 
 *
 * @param options 
 * options is constructed from the bitwise inclusive OR of zero or more of the 
 * following flags, as defined in <lsf/lsf.h>: \ref options_ls_load.
 *
 * @param *fromhost 
 * fromhost is the name of the host from which a task might be transferred. This 
 * parameter affects the host selection in such a way as to give preference to 
 * fromhost if the load on other hosts is not much better. If fromhost is NULL, 
 * the local host is assumed.
 *
 * @param **hostlist 
 * hostlist is an array of listsize host or cluster names. If not NULL, then only 
 * load information about hosts in this list will be returned. 
 *
 * @param listsize listsize gives the number of host or cluster names in hostlist.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref hostLoad
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * \ref status_ls_load
 * \n\ref options_ls_load
 *
 * @return struct hostLoad *:An array of hostLoad struct containing load information.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name 
 *
 * @see \ref ls_loadinfo 
 * @see \ref ls_load 
 */				
extern struct  hostLoad *ls_loadofhosts P_((char *resreq, int *numhosts,
				int options, char *fromhost, char **hostlist,
				int listsize));
extern struct  hostLoad *ls_loadoftype P_((char *resreq, int *numhosts,
				int options, char *fromhost, char *hosttype));
				
	
/**
 * \page ls_loadinfo ls_loadinfo
 * Returns the requested load indices of the hosts that satisfy specified resource
 * requirements.  
 * 
 * This routine returns the dynamic load information of qualified hosts. 
 *
 * \ref ls_loadinfo returns the requested load indices of the hosts that satisfy the 
 * specified resource requirements. The result of this call is an array of hostLoad 
 * data structures as defined in <lsf/lsf.h>. The status component of the hostLoad 
 * structure is an array of integers. The high order 16 bits of the first int 
 * are used to mark the operation status of the host. Possible states defined in 
 * <lsf/lsf.h> are as follows: \ref status_ls_load.
 *
 * \ref ls_load returns all load indices. The result of this call is an array of 
 * hostLoad data structures as defined in <lsf/lsf.h>. The status component of 
 * the hostLoad structure is an array of integers. The high order 16 bits of the 
 * first int are used to mark the operation status of the host. Possible 
 * states defined in <lsf/lsf.h> are as follows: \ref status_ls_load.
 * 
 * The low order 16 bits of the first int are reserved. The other int() 
 * of the status array is used to indicate the load status of the host. If any 
 * of these bits is set, then the host is considered to be busy (overloaded). 
 * Each bit (starting from bit 0) in int() represents one load index that 
 * caused the host to be busy. If bit i is set then the load index corresponding 
 * to li[i] caused the host to be busy. An int can be used to for 32 load 
 * indices. If number of load indices on the host, both built-in and user defined, 
 * are more than 32, more than one int will be used. 
 * 
 * Programmers can use macros to test the status of a host. The most commonly 
 * used macros include: 
 * 
 * - LS_ISUNAVAIL(status) 
 * - LS_ISBUSY(status) 
 * - LS_ISBUSYON(status, index) 
 * - LS_ISLOCKEDU(status) 
 * - LS_ISLOCKEDW(status) 
 * - LS_ISLOCKED(status) 
 * - LS_ISRESDOWN(status) 
 * - LS_ISUNLICENSED(status) 
 * - LS_ISOK(status) 
 * 
 * In the hostLoad data structure, the li vector contains load information on 
 * various resources on a host. The elements of the load vector are determined 
 * by the namelist parameter. 
 *
 * The returned hostLoad array is sorted according to the order section of the 
 * resource requirements, resreq (or, if not specified, the 1-minute average CPU 
 * queue length and paging rate), with the lightest loaded host being the first. 
 *
 * hostlist is an array of listsize host or cluster names. If not NULL, then only 
 * load information about hosts in this list will be returned. 
 *
 * <b>\#include <lsf/lsf.h> 
 *
 * struct hostLoad *ls_loadinfo(char *resreq, int *numhosts, 
 *                 int options, char *fromhost, char **hostlist, 
 *                 int listsize, char ***namelist)</b>
 *
 *
 * @param *resreq 
 * resreq is a char string describing resource requirements. Only the load 
 * vectors of the hosts satisfying the requirements will be returned. If resreq 
 * is NULL, the load vectors of all hosts will be returned. 
 * 
 * @param *numhosts 
 * numhosts is the address of an int which initially contains the number of 
 * hosts requested. If *numhosts is 0, request information on as many hosts as 
 * satisfy resreq. If numhosts is NULL, requests load information on one (1) host. 
 * If numhosts is not NULL, then *numhosts will contain the number of hostLoad 
 * records returned on success. 
 * 
 * @param options 
 * options is constructed from the bitwise inclusive OR of zero or more of the 
 * following flags, as defined in <lsf/lsf.h>: \ref options_ls_load.
 * 
 * @param *fromhost 
 * fromhost is the name of the host from which a task might be transferred. This 
 * parameter affects the host selection in such a way as to give preference to 
 * fromhost if the load on other hosts is not much better. If fromhost is NULL, 
 * the local host is assumed. 
 * 
 * @param **hostlist 
 * An array of listsize host or cluster names. 
 * 
 * @param listsize listsize gives the number of host or cluster names in hostlist.
 * 
 * @param ***namelist 
 * namelist is an input/output parameter. On input it points to a null-terminated 
 * list of names of indices whose values will be returned in the li vector for 
 * each host selected. Setting namelist to point to NULL returns all indices. On 
 * return it points to a null-terminated list of the names of the indices returned 
 * in the li load vector for each host. Each element of the load vector is a 
 * floating point number between 0.0 and INFINIT_LOAD (defined in lsf.h). The 
 * index value is set to INFINIT_LOAD to indicate an invalid or unknown value for 
 * an index. The indices in the li vector are ordered such that li[i] contains 
 * the value of index namelist[i]. If index namelist[i] is causing the host to be 
 * busy, then LS_ISBUSYON(status, i) will be TRUE. When the input namelist is NULL 
 * the output namelist is ordered such that the li vector can be indexed using 
 * constants defined in <lsf/lsf.h> as listed below: \ref index_ls_loadinfo. 
 *
 * <b>Data Structures:</b>
 * \par
 * \ref hostLoad
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * \ref status_ls_load
 * \n\ref options_ls_load
 * \n\ref index_ls_loadinfo
 *
 * @return struct hostLoad *:An array of hostLoad struct containing load information.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n$LSF_CONFDIR/lsf.cluster.cluster_name 
 *
 * @see \ref ls_load 
 * @see \ref ls_loadofhosts 
 */				
extern struct  hostLoad *ls_loadinfo P_((char *resreq, int *numhosts, 
				int options, char *fromhost, char **hostlist, 
			        int listsize, char ***indxnamelist));
			        
/**
 * \page ls_loadadj ls_loadadj
 * Sends a load adjustment request to LIM after the execution host or hosts have 
 * been selected outside the LIM by the calling application.
 * 
 * \ref ls_loadadj sends a load adjustment request to LIM after the execution host 
 * or hosts have been selected outside the LIM by the calling application. Use 
 * this call only if a placement decision is made by the application without 
 * calling \ref ls_placereq (for example, a decision based on the load information 
 * from an earlier \ref ls_load call). This request keeps LIM informed of task 
 * transfers so that the potential load increase on the destination host() 
 * provided in placeinfo are immediately taken into consideration in future LIM 
 * placement decisions. listsize gives the total number of entries in placeinfo. 
 *
 * \ref ls_loadadj returns 0 on success, otherwise -1 is returned and lserrno is 
 * set to indicate the error. 
 *
 * \ref ls_loadadj can adjust all load indices with the exception of ls, it, r15m 
 * and external load indices. 
 *
 * <b>\#include <lsf/lsf.h> 
 *
 * int ls_loadadj(char *resreq, struct placeInfo *hostlist, 
 *                int listsize)</b>
 *
 *
 *
 * @param *resreq 
 * resreq is a resource requirement expression (which can be NULL) that describes 
 * the resource requirements for which the load must be adjusted. These typically 
 * are the resource requirements for the previously placed task. 
 * LIM adjusts the host load indices according to the resource requirement. If 
 * NULL is specified, then LIM assumes that the task or tasks are both CPU and 
 * memory intensive (this is the default). 
 * 
 * @param *hostlist 
 * placeinfo is a pointer to an array of placeInfo structures. A placeInfo 
 * structure contains a hostname, and an int, numtask, that represents 
 * a particular number of tasks. The host load indices (specified by resreq) 
 * of all the hosts that are specified in the array are increased by the number 
 * of tasks specified. Each task is assumed to have the same resource 
 * requirements. The requirements are those specified in resreq. 
 * 
 * @param listsize listsize gives the number of host or cluster names in hostlist.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref placeInfo
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful. 
 *
 * @return int:-1 
 * \n Function failed. 
 *
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * none
 *
 * @see \ref ls_placereq  
 * @see \ref ls_placeofhosts  
 * @see \ref ls_eligible  
 * @see \ref ls_info  
 * @see \ref ls_load  
 */			        
extern int     ls_loadadj P_((char *resreq, struct placeInfo *hostlist, 
                             int listsize));
                             
/**
 * \page ls_eligible ls_eligible
 * \brief Checks to see if a task is eligible for remote execution.
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists contain
 * information about the eligibility of tasks for remote execution and their resource
 * requirement characteristics. LSLIB maintains two task lists: local and remote. The
 * local list contains tasks (i.e. UNIX processes) that must be executed on the local
 * host (for example, ps, uptime, hostname). The remote list contains tasks that are
 * suitable for remote execution (for example, compress), together with their resource
 * requirements.
 * 
 * Task lists are generated and stored in memory by reading the system task file()
 * and the .lsftask file in the user's home directory. The task lists can be updated and
 * displayed using the command lsrtasks(). See the LSF User's Guide for detailed
 * information on the use of task lists and resource requirements.
 * 
 * \ref ls_eligible checks whether or not taskname is eligible for remote execution
 * and, if so, obtains its resource requirements. \ref ls_eligible returns TRUE if
 * taskname is eligible for remote execution, FALSE otherwise.
 * 
 * resreq is an output parameter; you supply the character array. If taskname is
 * eligible for remote execution, the resource requirements associated with taskname
 * in the remote task lists are copied into resreq. If no resource requirements are
 * associated with taskname, an empty string is copied into resreq.
 *
 * <b>\#include <lsf/lsf.h>
 * \n int ls_eligible(char *taskname, char *resreq, char mode)</b>
 * 
 * @param *taskname The task that is being checked to see if it can be remotely executed.
 * @param *resreq The resource requirements associated with the task taskname. If no resource
 * requirements are associated with taskname, an empty string is copied into resreq.
 * @param mode One of two constants defined in <lsf/lsf.h>.
 * \n If mode is LSF_LOCAL_MODE, the routine searches through the remote task lists to see
 * if taskname is on a list. If found, the task is considered eligible for remote execution,
 * otherwise the task is considered ineligible.
 * \n If mode is LSF_REMOTE_MODE, the routine searches through the local task lists to see
 * if taskname is on a list. If found, the task is considered ineligible for remote
 * execution, otherwise the task is considered eligible.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:TRUE 
 * \n Returned if the task can be remotely executed.
 * @return int:FALSE 
 * \n Returned if the task can not be remotely executed.
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.task \n
 * $LSF_CONFDIR/lsf.task.cluster_name \n
 * $HOME/.lsftask
 * 
 * @see \ref ls_resreq 
 * @see \ref ls_listrtask 
 * @see \ref ls_listltask 
 * @see \ref ls_insertrtask 
 * @see \ref ls_insertltask 
 * @see \ref ls_deletertask 
 * @see \ref ls_deleteltask 
 */                             
extern int     ls_eligible P_((char *task, char *resreqstr, char mode));

/**
 * \page ls_resreq ls_resreq
 * \brief Searches a remote task list for a task name and returns the resource
 * requirements. 
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists
 * contain information about the eligibility of tasks for remote execution and
 * their resource requirement characteristics. LSLIB maintains two task lists:
 * local and remote. The local list contains tasks (i.e. UNIX processes) that 
 * must be executed on the local host (for example, ps, uptime, hostname). The 
 * remote list contains tasks that are suitable for remote execution (for 
 * example, compress), together with their resource requirements. 
 * 
 * Task lists are generated and stored in memory by reading the system task file()
 * and the .lsftask file in the user's home directory. The task lists can be 
 * updated and displayed using the command lsrtasks(). See the LSF User's Guide 
 * for detailed information on the use of task lists and resource requirements. 
 * 
 * \ref ls_resreq is a simplified version of \ref ls_eligible which searches the remote
 * task list for taskname and returns the resource requirements associated with 
 * the task if found otherwise NULL is returned. 
 * 
 * ls_resreq() returns a pointer to static data which can be overwritten by 
 * subsequent calls. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n char *ls_resreq(char *taskname)</b>
 * 
 * @param *taskname The name of the task being sought. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return char *:Requirements 
 * \n Returns the resource requirements associated with the taskname. 
 * @return NULL 
 * \n Unable to find the taskname on the remote task list. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.task 
 * \n $LSF_CONFDIR/lsf.task.cluster_name 
 * \n $HOME/.lsftask 
 * 
 * @see \ref ls_eligible  
 * @see \ref ls_listrtask  
 * @see \ref ls_listltask  
 * @see \ref ls_insertrtask  
 * @see \ref ls_insertltask  
 * @see \ref ls_deletertask  
 * @see \ref ls_deleteltask  
 *
 */
extern char *  ls_resreq P_((char *task));

/**
 * \page ls_insertrtask ls_insertrtask
 * Adds a specified task to the remote task list. 
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists 
 * contain information about the eligibility of tasks for remote execution and 
 * their resource requirement characteristics. LSLIB maintains two task lists: 
 * local and remote. The local list contains tasks (i.e. UNIX processes) that 
 * must be executed on the local host (for example, ps, uptime, hostname). 
 * The remote list contains tasks that are suitable for remote execution (for 
 * example,compress), together with their resource requirements. 
 * 
 * Task lists are generated and stored in memory by reading the system task file() 
 * and the .lsftask file in the user's home directory. The task lists can be 
 * updated and displayed using the command lsrtasks(). See the LSF User's Guide 
 * for detailed information on the use of task lists and resource requirements. 
 * 
 * \ref ls_insertrtask adds the specified task to the remote task list. 
 * 
 * <b>\#include <lsf/lsf.h>
 * \n int ls_insertrtask(char *task)</b>
 * 
 * @param *task task is a char string containing the name of the task to be 
 * inesrted and, for a remote task, optionally a resource requirement string, 
 * separated by `/'. 
 * \n For example, 
 * \n "cc/select[swp>20 || (mem>10 && pg<5)] order[swp:pg] rusage[swp=20]"  
 * \n See Administering IBM Platform LSF for a description of the resource requirement
 * string. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:0 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  $LSF_CONFDIR/lsf.task 
 *  \n $LSF_CONFDIR/lsf.task.cluster_name 
 *  \n $HOME/.lsftask 
 * 
 * @see \ref ls_eligible 
 * @see \ref ls_resreq  
 * @see \ref ls_listrtask  
 * @see \ref ls_listltask  
 * @see \ref ls_insertltask  
 * @see \ref ls_deletertask   
 * @see \ref ls_deleteltask 
 */
extern int     ls_insertrtask P_((char *task));

/**
 * \page ls_insertltask ls_insertltask
 * Adds the specified task to the local task list. 
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists 
 * contain information about the eligibility of tasks for remote execution and 
 * their resource requirement characteristics. LSLIB maintains two task lists: 
 * local and remote. The local list contains tasks (i.e. UNIX processes) that 
 * must be executed on the local host (for example, ps, uptime, hostname). The 
 * remote list contains tasks that are suitable for remote execution 
 * (for example, compress),together with their resource requirements. 
 * 
 * Task lists are generated and stored in memory by reading the system task file() 
 * and the .lsftask file in the user's home directory. The task lists can be 
 * updated and displayed using the command lsltasks(). See the LSF User's Guide 
 * for detailed information on the use of task lists and resource requirements. 
 * 
 * \ref ls_insertltask adds the specified task to the local task list. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n int ls_insertltask(char *task)</b>
 * 
 * @param *task Task is a char string containing the name of the task to be 
 * inesrted. 
 * \n For example, 
 * \n "cc/select[swp>20 || (mem>10 && pg<5)] order[swp:pg] rusage[swp=20]"  
 * \n See Administering IBM Platform LSF for a description of the resource requirement
 * string. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:0 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  $LSF_CONFDIR/lsf.task 
 *  \n $LSF_CONFDIR/lsf.task.cluster_name 
 *  \n $HOME/.lsftask 
 * 
 * @see \ref ls_eligible 
 * @see \ref ls_resreq  
 * @see \ref ls_listrtask  
 * @see \ref ls_listltask  
 * @see \ref ls_insertrtask  
 * @see \ref ls_deletertask   
 * @see \ref ls_deleteltask  
 */
extern int     ls_insertltask P_((char *task));

/**
 * \page ls_deletertask ls_deletertask
 * \brief Deletes a specified task from the remote task list.
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists contain
 * information about the eligibility of tasks for remote execution and their resource
 * requirement characteristics. LSLIB maintains two task lists: local and remote. The
 * local list contains tasks (i.e. UNIX processes) that must be executed on the local
 * host (for example, ps, uptime, hostname). The remote list contains tasks that are
 * suitable for remote execution (for example, compress), together with their resource
 * requirements.
 * 
 * Task lists are generated and stored in memory by reading the system task file()
 * and the .lsftask file in the user's home directory. The task lists can be updated and
 * displayed using the command lsrtasks(). See the LSF User's Guide for detailed
 * information on the use of task lists and resource requirements.
 * 
 * \ref ls_deletertask deletes the specified task from the remote task list.
 *
 *
 * <b>\#include <lsf/lsf.h>
 * \n int ls_deletertask(char *task)</b>
 *
 * @param *task task is a char string containing the name of the task to be deleted and, for a
 * remote task, optionally a resource requirement string, separated by '/'.
 * \n For example,
 * \n "cc/select[swp>20 || (mem>10 && pg<5)] order[swp:pg] rusage[swp=20]"
 * \n See Administering IBM Platform LSF for a description of the resource requirement
 * \n string.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful.        
 * @return int:-1 
 * \n Function failed.                   
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSF_CONFDIR/lsf.task
 * \n $LSF_CONFDIR/lsf.task.cluster_name
 * \n $HOME/.lsftask
 *
 * @see \ref ls_eligible 
 * @see \ref ls_resreq 
 * @see \ref ls_listrtask 
 * @see \ref ls_listltask 
 * @see \ref ls_insertrtask 
 * @see \ref ls_insertltask 
 * @see \ref ls_deleteltask 
 */

extern int     ls_deletertask P_((char *task));

/**
 * \page ls_deleteltask ls_deleteltask
 * 
 * \brief Deletes the specified task from the local task list.
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists contain
 * information about the eligibility of tasks for remote execution and their resource
 * requirement characteristics. LSLIB maintains two task lists: local and remote. The
 * local list contains tasks (i.e. UNIX processes) that must be executed on the local
 * host (for example, ps, uptime, hostname). The remote list contains tasks that are
 * suitable for remote execution (for example, compress), together with their resource
 * requirements.
 * 
 * Task lists are generated and stored in memory by reading the system task file()
 * and the .lsftask file in the user's home directory. The task lists can be updated and
 * displayed using the command lsltasks(). See the LSF User's Guide for detailed
 * information on the use of task lists and resource requirements.
 * 
 * \ref ls_deleteltask deletes the specified task from the local task list.
 * \n task is a char string containing a task name.
 * 
 * For example,
 * \n "cc/select[swp>20 || (mem>10 && pg<5)] order[swp:pg] rusage[swp=20]"
 * \n See Administering IBM Platform LSF for a description of the resource requirement
 * string.
 *
 * <b>\#include <lsf/lsf.h>
 * \n int ls_deleteltask(char *task)</b>
 *
 * @param *task The task to be deleted from the local task list.
 *
 * <b>Data Structures:</b>
 *\par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful.  
 * @return int:-1 
 * \n Function failed.            
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSF_CONFDIR/lsf.task
 * \n $LSF_CONFDIR/lsf.task.cluster_name
 * \n $HOME/.lsftask
 *
 * @see \ref ls_eligible 
 * @see \ref ls_resreq 
 * @see \ref ls_listrtask 
 * @see \ref ls_listltask 
 * @see \ref ls_insertrtask 
 * @see \ref ls_insertltask 
 * @see \ref ls_deletertask 
 */
extern int     ls_deleteltask P_((char *task));

/**
 * \page ls_listrtask ls_listrtask
 * Returns the contents of the user's remote task list. 
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists 
 * contain information about the eligibility of tasks for remote execution and 
 * their resource requirement characteristics. LSLIB maintains two task lists: 
 * local and remote. The local list contains tasks (i.e. UNIX processes) that 
 * must be executed on the local host (for example, ps, uptime, hostname). The 
 * remote list contains tasks that are suitable for remote execution (for example, 
 * compress), together with their resource requirements. 
 * 
 * Task lists are generated and stored in memory by reading the system task file() 
 * and the .lsftask file in the user's home directory. The task lists can be 
 * updated and displayed using the command lsrtasks(). See the LSF User's Guide 
 * for detailed information on the use of task lists and resource requirements. 
 * 
 * \ref ls_listrtask returns the contents of a user's remote task list in tasklist. 
 * Memory for tasklist is allocated as needed and freed by the next call to 
 * \ref ls_listrtask. If sortflag is non-zero, then the returned task list is sorted 
 * alphabetically. Each of the returned tasks is a char string consisting of 
 * a task name optionally followed by `/' and the associated resource requirement 
 * string. \ref ls_listrtask return the number of items in the tasklist on success, 
 * -1 on error. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n int ls_listrtask(char ***taskList, int sortflag)</b>
 * 
 * @param ***taskList The task list to be accessed. 
 * @param sortflag sortflag
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:Number of Items in Task List 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  $LSF_CONFDIR/lsf.task 
 *  \n $LSF_CONFDIR/lsf.task.cluster_name 
 *  \n $HOME/.lsftask 
 * 
 * @see \ref ls_eligible  
 * @see \ref ls_resreq  
 * @see \ref ls_listltask  
 * @see \ref ls_insertrtask  
 * @see \ref ls_insertltask  
 * @see \ref ls_deletertask  
 * @see \ref ls_deleteltask  
 */
extern int     ls_listrtask P_((char ***taskList, int sortflag));

/**
 * \page ls_listltask ls_listltask
 * Returns the contents of the user's local task list. 
 * 
 * LSLIB library routine for manipulating task lists stored by LSLIB. Task lists 
 * contain information about the eligibility of tasks for remote execution and 
 * their resource requirement characteristics. LSLIB maintains two task lists: 
 * local and remote. The local list contains tasks (i.e. UNIX processes) that 
 * must be executed on the local host (for example, ps, uptime, hostname). The 
 * remote list contains tasks that are suitable for remote execution (for example, 
 * compress), together with their resource requirements. 
 * 
 * Task lists are generated and stored in memory by reading the system task file() 
 * and the .lsftask file in the user's home directory. The task lists can be 
 * updated and displayed using the command lsltasks(). See the LSF User's Guide 
 * for detailed information on the use of task lists and resource requirements. 
 * 
 * \ref ls_listltask returns the contents of a user's local task list in tasklist. 
 * Memory for tasklist is allocated as needed and freed by the next call to 
 * \ref ls_listltask. If sortflag is non-zero, then the returned task list is sorted 
 * alphabetically. Each of the returned tasks is a char string consisting of 
 * a task name optionally followed by `/' and the associated resource requirement 
 * string. \ref ls_listltask return the number of items in the tasklist on success, 
 * -1 on error. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n int ls_listltask(char ***taskList, int sortflag)</b>
  * 
 * @param ***taskList The task list to be accessed. 
 * @param sortflag sortflag
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 *  
 * @return int:Number of Items in Task List 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  $LSF_CONFDIR/lsf.task 
 *  \n $LSF_CONFDIR/lsf.task.cluster_name 
 *  \n $HOME/.lsftask 
 * 
 * @see \ref ls_eligible  
 * @see \ref ls_resreq  
 * @see \ref ls_listrtask  
 * @see \ref ls_insertrtask  
 * @see \ref ls_insertltask  
 * @see \ref ls_deletertask  
 * @see \ref ls_deleteltask  
 */
extern int     ls_listltask P_((char ***taskList, int sortflag));

/**
 * \page ls_findmyconnections ls_findmyconnections
 * \brief Finds established connections to hosts.
 * 
 * \ref ls_findmyconnections retrieves the list of hosts with which the application has
 * established a connection. A connection is established upon the first successful
 * return from an \ref ls_connect, \ref ls_rtask, \ref ls_rtaske, \ref ls_rexecv, or
 * \ref ls_rexecve call. The returned host name list is terminated by a NULL value. The
 * function maintains a static array, each element of which points to a host name. This
 * array is overwritten the next time \ref ls_findmyconnections is called. If a
 * connection is not found, the first element of the array is a NULL pointer.
 * 
 * Any program using these routines must call \ref ls_initrex first.
 *
 * Any program using these routines must be setuid to root if LSF_AUTH is not defined
 * in the lsf.conf file.
 *
 * <b>\#include <lsf/lsf.h>
 * \n char **ls_findmyconnections(void)</b>
 *
 * @param none 
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return char **:A list of hostnames which members in the connection table.
 * \n Function was successful.   
 * @return Null
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 * error conditions for this function. 
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref ls_connect 
 * @see \ref ls_isconnected 
 * @see \ref ls_rtask 
 * @see \ref ls_rtaske 
 * @see \ref ls_rexecv 
 * @see \ref ls_rexecve 
 * @see \ref ls_initrex 
 */
extern char ** ls_findmyconnections P_((void));

/**
 * \page ls_isconnected ls_isconnected
 * Tests if the specified host is currently connected with the application. 
 * 
 * \ref ls_isconnected tests if the specified host is currently connected with the 
 * application. A connection is established on the first successful return from 
 * an \ref ls_connect, \ref ls_rtask, \ref ls_rtaske, \ref ls_rexecv, or \ref ls_rexecve call. 
 * 
 * Any program using these routines must call \ref ls_initrex first. 
 * 
 * Any program using these routines must be setuid to root if LSF_AUTH is not 
 * defined in the lsf.conf file. 
 * 
 * <b>\#include <lsf/lsf.h>  
 * \n int ls_isconnected(char *hostName)</b>
 * 
 * @param *hostname 
 * The host that is tested for connection with an application. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:non-zero 
 * \n Function was successful. 
 * @return int:0 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  none
 * 
 * @see \ref ls_connect   
 * @see \ref ls_findmyconnections   
 * @see \ref ls_rtask   
 * @see \ref ls_rtaske   
 * @see \ref ls_rexecv   
 * @see \ref ls_rexecve 
 */
extern int     ls_isconnected P_((char *hostName));
extern int     ls_lostconnection P_((void));

/**
 * \page ls_getclustername ls_getclustername
 * \brief Returns the name of the local load sharing cluster.
 * 
 * This routines provides access to LSF cluster configuration information.
 * \ref ls_getclustername returns the name of the local load sharing cluster defined
 * in the configuration files.
 *
 * <b>\#include <lsf/lsf.h>
 * \n char *ls_getclustername(void)</b>
 *
 * @param none 
 *  
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 *
 * @return char *:A pointer to cluster name.
 * \n Function successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
  *none
 *
 * <b>Files:</b>
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 *
 * @see \ref ls_info 
 * @see \ref ls_getmastername 
 * @see \ref ls_getmodelfactor 
 */
extern char    *ls_getclustername P_((void));

/**
 * \page ls_clusterinfo ls_clusterinfo
 * \brief Returns general information about LSF clusters.
 * 
 * This routine returns general configuration information about LSF clusters.
 * 
 * \ref ls_clusterinfo returns an array of clusterInfo data structures, as defined in
 * <lsf/lsf.h>. Each entry contains information about one cluster. The information
 * includes cluster name, cluster status, the master host name, LSF primary
 * administrator login name (for backward compatibility), LSF primary administrator
 * user Id (for backward compatibility), total number of server hosts, total number of
 * client hosts, available resource names, host types, host models, total number of LSF
 * administrators, LSF administrator user Ids and LSF administrator login names.
 * 
 * The parameter resreq is designed to select eligible clusters that satisfy the given
 * resource requirements from candidate clusters. This parameter is currently
 * ignored. clusterlist gives a list of cluster names whose information should be
 * returned, if they satisfy the resreq. If clusterlist is NULL, then all clusters known
 * to LSF satisfying resreq will be returned. listsize gives the size of the
 * clusterlist. If numhosts is not NULL, then *numhosts will be modified to return
 * the number of clusters selected. The parameter options is currently ignored.
 * 
 * This routine returns a pointer to dynamically allocated data which can be freed in
 * subsequent calls.
 *
 * <b>\#include <lsf/lsf.h>
 * \n struct clusterInfo *ls_clusterinfo(char *resreq,
 * 					int *numclusters, char **clusterlist,
 * 					int listsize, int options)</b>
 *
 * @param *resreq Select eligible clusters that satisfy the given resource requirements from candidate
 * clusters. This parameter is currently ignored.
 * @param *numclusters If numclusters is not NULL, then *numclusters will be modified to return the
 * number of clusters selected.
 * @param **clusterlist Gives a list of cluster names whose information should be returned, if they satisfy
 * the resreq. If clusterlist is NULL, then all clusters known to LSF satisfying
 * resreq will be returned.
 * @param listsize The size of the clusterlist.
 * @param option The parameter options is currently ignored.
 *
 * <b>Data Structures:</b>
 * \par
 * clusterInfo
 *
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return struct clusterInfo *:A pointer to the cluster information.
 * \n Function was successful.  
 * @return NULL 
 * \n Function failed.    
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 *
 * @see none
 */
extern struct clusterInfo *ls_clusterinfo P_((char *, int *, char **, int, int));

/**
 * \page ls_sharedresourceinfo ls_sharedresourceinfo
 * \brief Returns shared resource information in dynamic values.
 * 
 * \ref ls_sharedresourceinfo returns the requested shared resource information in
 * dynamic values. The result of this call is a chained data structure as defined in
 * <lsf/lsf.h>, which contains requested information.
 *
 * <b>\#include <lsf/lsf.h>
 * \n lsSharedResourceInfo *ls_sharedresourceinfo(
 * \n &nbsp;&nbsp;&nbsp; char **resources,
 * \n &nbsp;&nbsp;&nbsp; int *numResources, char *hostName,
 * \n &nbsp;&nbsp;&nbsp; int options)</b>
 * 
 * @param **resources resources is an array of NULL terminated strings storing requesting resource
 * names. If set to NULL, the call returns all shared resources defined in the cluster.
 * @param *numResources numResources is an input/output parameter. On input it indicates how many
 * resources are requested. Value 0 means requesting all shared resources. On return
 * it contains qualified number of resources.
 * @param *hostName hostName is a string containing a host name. Only shared resource available on the
 * specified host will be returned. If hostName is set to NULL, shared resource
 * available on all hosts will be returned.
 * @param options options is reserved for future use. Currently, it should be set to 0.
 * 
 * <b>Data Structures:</b>
 * \par
 * lsSharedResourceInfo
 * \n lsSharedResourceInstance
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return struct lsSharedResourceInfo *:A list of lsSharedResourceInfo structure. 
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 * 
 * @see none
 */
extern struct lsSharedResourceInfo *ls_sharedresourceinfo P_((char **, int *, char *, int));

/**
 * \page  ls_getmastername ls_getmastername
 * Returns the name of the host running the local load sharing cluster's master 
 * LIM. 
 * 
 * This routines provides access to LSF cluster configuration information. 
 * 
 * \ref ls_getmastername returns the name of the host running the local load sharing 
 * cluster's master LIM. 
 * 
 * <b>\#include <lsf/lsf.h>
 * \n char *ls_getmastername(void)</b>
 * 
 * @param none 
 *
 * <b>Data Structures:</b>   
 * \par                        
 * none                 
 *                             
 * <b>Define Statements:</b> 
 * \par                        
 * none                        
 
 * @return char *:A pointer to static space holding master name.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name 
 * 
 * @see \ref ls_info   
 * @see \ref ls_getclustername  
 * @see \ref ls_getmodelfactor  
 */
extern char    *ls_getmastername P_((void));

/**
 * \page ls_getmyhostname ls_getmyhostname
 * Deprecated. Returns the name used throughout LSF to represent the local host. 
 * 
 * \ref ls_getmyhostname returns the name used throughout LSF to represent the local 
 * host. 
 * 
 * This routine returns a pointer to static data which can be overwritten by 
 * subsequent calls. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n char *ls_getmyhostname(void)</b>
 * 
 * @param none 
 *   
 * <b>Data Structures:</b>   
 * \par                        
 * none                 
 *                             
 * <b>Define Statements:</b> 
 * \par                        
 * none                        
 *
 * @return char *:The name used throughout LSF to represent the local host.
 * \n Function was successful. 
 * @return NULL 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  none
 * 
 * @see \ref ls_getmyhostname2  
 */
extern char    *ls_getmyhostname P_((void));

/**
 * \page ls_getmyhostname2 ls_getmyhostname2
 * New. Returns the name used throughout LSF to represent the local host. 
 * 
 * \ref ls_getmyhostname2 returns the name used throughout LSF to represent the local
 * host. 
 * 
 * This function supports the use of both IPv4 and IPv6 protocols. Call this 
 * function rather than \ref ls_getmyhostname. 
 * 
 * This routine returns a pointer to static data which can be overwritten by 
 * subsequent calls. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n char *ls_getmyhostname2(void)</b>
 * 
 * @param none 
 *  
 * <b>Data Structures:</b>   
 * \par                        
 * none                 
 *                             
 * <b>Define Statements:</b> 
 * \par                        
 * none                        
 *
 * @return char *:The name used throughout LSF to represent the local host.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_getmyhostname  
 */
extern char    *ls_getmyhostname2 P_((void));

/**
 * \page ls_gethostinfo ls_gethostinfo
 * 
 * \brief Returns an array of hostInfo data structures.
 * 
 * This routine obtains static resource information about hosts. Static resources
 * include configuration information as determined by LSF configuration files (see
 * lsf.shared and lsf.cluster) as well as others determined automatically by LIM
 * at start up.
 * 
 * \ref ls_gethostinfo returns an array of hostInfo data structures, as defined in
 * <lsf/lsf.h>. Each entry contains information about one host, including the host
 * type, the host model, its CPU normalization factor, the number of CPUs, its
 * maximum memory, swap and tmp space, number of disks, the resources available
 * on the host, the run windows during which the host is available for load sharing,
 * the busy thresholds for the host, whether the host is a LSF server, and the default
 * priority used by the RES for remote tasks executing on that host. The windows field
 * will be set to "-" if the host is always open. The busyThreshold field is an array of
 * floating point numbers specifying the load index thresholds that LIM uses to
 * consider a host as busy. The size of the array is indicated by the numIndx field. The
 * order of the array elements is the same as the load indicies returned by \ref ls_load.
 * 
 * This routine returns a pointer to dynamically allocated data structures which can
 * be freed in subsequent calls.
 *
 * <b>\#include <lsf/lsf.h>
 * \n struct hostInfo *ls_gethostinfo(char *resreq, int *numhosts, char **hostlist, 
 * int listsize, int options)</b>
 *
 * @param *resreq resreq specifies resource requirements that a host must satisfy 
 * if it is to be included in the hostInfo array returned. See Administering 
 * IBM Platform LSF for information about resource requirement string syntax. If this
 * parameter is a NULL pointer or is an empty string, then the default resource 
 * requirement will be used, which is to return all hosts.
 * @param *numhosts numhosts is the address of an int which, if it is not NULL, 
 * will contain the number of hostInfo records returned on success.
 * @param **hostlist hostlist gives a list of hosts or clusters whose information 
 * is returned if they satisfy the requirements in resreq. If hostlist is NULL, 
 * all hosts known to LSF that satisfy the requirements in resreq are returned.
 * @param listsize listsize gives the size of the hostlist.
 * @param options options is constructed from the bitwise inclusive OR of zero 
 * or more of the flags that are defined in <lsf/lsf.h>. These flags are 
 * documented in \ref ls_load.
 *
 * <b>Data Structures:</b>
 *\par
 * hostInfo
 *
 * <b>Define Statements:</b>
 * \par
 * \ref defs_ls_lic
 *
 * @return struct hostInfo *:A list of hostInfo struct.
 * \n Function successful.
 * @return NULL 
 * \n Function failed.
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 *
 * @see \ref ls_gethosttype 
 * @see \ref ls_gethostmodel 
 * @see \ref ls_gethostfactor 
 * @see \ref ls_load 
 */
extern struct  hostInfo *ls_gethostinfo P_((char *, int *, char **, int, int));
extern char    *ls_getISVmode P_((void));
extern int ls_isshutdown P_((void));
extern int     ls_isPartialLicensingEnabled P_((void));

#if defined(COMPAT32) && defined(INT32JOBID)
extern struct  lsInfo32  *ls_info P_((void));
extern struct  lsInfo    *ls_info40 P_((void));
extern void    info40to32  P_((struct lsInfo32*, struct lsInfo*));
#else

/**
 * \page ls_info ls_info
 * Returns a pointer to an lsInfo structure. 
 *     
 * This routine provides access to LSF cluster configuration information.
 * On success, \ref ls_info returns a pointer to an lsInfo structure, which contains 
 * complete load sharing configuration information. This information includes the 
 * name of the cluster, the name of the current cluster master host, the set of 
 * defined resources, the set of defined host types and models, the CPU factors 
 * of the host models, and all load indices (resTable[0] through 
 * resTable[numIndx - 1]), including the site defined external load indices 
 * (resTable[MAX + 1] through resTable[MAX + numUsrIndx]). 
 * 
 * The set of defined resource items is a list of all resources that may be 
 * assigned to various hosts in the cluster. The resource names can be used 
 * to build expressions for querying information about hosts, or for describing 
 * how tasks are to be scheduled. New resources may be defined as desired by 
 * the LSF administrator.  
 * 
 * The valueType component of the resItem structure indicates whether the type 
 * of the resource is NUMERIC, STRING, or BOOLEAN. 
 * 
 * The orderType indicates how hosts should be ordered from best to worst based 
 * on the resource. If the orderType is INCR, the hosts should be ordered from 
 * the lowest to the highest value for that resource; if DECR, they should be 
 * ordered from the highest to lowest. If the orderType is NA, then the resource 
 * cannot be used to order hosts. 
 * 
 * The flags component is used to indicate the attributes of the resource. It is 
 * formed from the bitwise inclusive OR of zero or more of the following flags, 
 * as defined in <lsf/lsf.h>: Defined in \ref defs_ls_info
 * 
 * The interval component applies to resources with dynamic values. It indicates 
 * how frequently (in seconds) the resource value is evaluated. 
 * 
 * The set of host types hostTypes in the lsInfo structure is a list of all 
 * defined host architectures in the cluster. All machines that can run the 
 * same binaries are generally considered to be of the same host type. 
 * 
 * The set of host models hostModels in lsInfo structure is a list of all defined 
 * computer models in the cluster. Generally, machines of the same host type that 
 * have exactly the same performance characteristics are considered to be the 
 * same model. 
 * 
 * <b>\#include <lsf/lsf.h>
 * \n struct lsInfo *ls_info(void)</b>
 * 
 * @param none 
 *
 * <b>Data Structures:</b>
 * \par
 * \ref lsInfo
 * \n \ref resItem
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref defs_ls_info
 * 
 * @return struct lsInfo *:A pointer to the information of LS configurations.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name 
 * 
 * @see \ref ls_getclustername  
 * @see \ref ls_getmastername  
 * @see \ref ls_getmodelfactor 
 */
extern struct  lsInfo    *ls_info P_((void));
#endif

extern char ** ls_indexnames P_((struct lsInfo *)); 
extern int     ls_isclustername P_((char *)); 

/**
 * \page  ls_gethosttype ls_gethosttype
 * Returns the type of the specified host.
 *
 * This routine obtains static resource information about hosts.Static resources
 * include configuration information as determined by LSF configuration files 
 * (see lsf.shared and lsf.cluster) as well as others determined automatically 
 * by LIM at start up.
 *
 * \ref ls_gethosttype returns the type of the specified host.
 * This routine returns a pointer to dynamically allocated data structures 
 * which can be freed in subsequent calls.  
 *
 * <b>\#include <lsf/lsf.h> 
 * \n char *ls_gethosttype(char *hostname)</b>            
 *
 * @param *hostname 
 * The host whose type is to be determined.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return char *:The host type.
 * \n Function was successful. 
 * @return NULL 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 *
 * <b>Equivalent line commands:</b>
 *\par
 * none
 *
 * \b Files:
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name  
 *
 * @see \ref ls_gethostfactor 
 * @see \ref ls_gethostmodel 
 * @see \ref ls_gethostinfo 
 *
 */
extern char    *ls_gethosttype P_((char *hostname));

/**
 * \page ls_getmodelfactor ls_getmodelfactor
 * Returns the CPU normalization factor of the specified host model. 
 * 
 * This routines provides access to LSF cluster configuration information. 
 * 
 * \ref ls_getmodelfactor returns the CPU normalization factor of the specified 
 * host model as defined in the LSF configuration files. This factor is based 
 * on the the host model's CPU speed relative to other host models in the load 
 * sharing system. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n float *ls_getmodelfactor(char *modelname)</b>
 * 
 * @param *modelname 
 * The model for which the CPU nominalization factor is returned. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return float *:A pointer to the CPU factor of the given CPU model.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  $LSF_CONFDIR/lsf.shared 
 *  \n $LSF_CONFDIR/lsf.cluster.cluster_name 
 * 
 * @see \ref ls_info  
 * @see \ref ls_getclustername 
 * @see \ref ls_getmastername   
 */
extern float   *ls_getmodelfactor P_((char *modelname));

/**
 * \page ls_gethostfactor ls_gethostfactor
 * 
 * \brief Returns a pointer to a floating point number that contains the CPU factor of the
 * specified host.
 *
 * This routine obtains static resource information about hosts. Static resources
 * include configuration information as determined by LSF configuration files (see
 * lsf.shared and lsf.cluster) as well as others determined automatically by LIM
 * at start up.
 * 
 * \ref ls_gethostfactor returns a pointer to a floating point number that contains
 * the CPU factor of the specified host.
 * 
 * This routine returns a pointer to dynamically allocated data structures which can
 * be freed in subsequent calls.
 *
 * <b>\#include <lsf/lsf.h>
 * \n float *ls_gethostfactor(char *hostname)</b>
 *
 * @param *hostname The host to which the floating point number is returned.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return float *:A pointer to the CPU factor of the specified host.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 *
 * @see \ref ls_gethosttype 
 * @see \ref ls_gethostmodel 
 * @see \ref ls_gethostinfo 
 */
extern float   *ls_gethostfactor P_((char *hostname));
extern int     ls_gethostfactor4GridBroker P_((char *hostname, float *cpufactor));
/**
 * \page ls_gethostmodel ls_gethostmodel
 * 
 * \brief Returns the model of the specified host.
 * 
 * This routine obtains static resource information about hosts. Static resources
 * include configuration information as determined by LSF configuration files (see
 * lsf.shared and lsf.cluster) as well as others determined automatically by LIM
 * at start up.
 * 
 * \ref ls_gethostmodel returns the model of the specified host.
 * This routine returns a pointer to dynamically allocated data structures which can
 * be freed in subsequent calls.
 *
 * <b>\#include <lsf/lsf.h>
 * \n char *ls_gethostmodel(char *hostname)</b>
 *
 * @param *hostname The host whose model is to be determined.
 *
 * <b>Data Structures:</b>
 * \par
 * none 
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return char *:A pointer to the host model of the given host.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 *
 * @see \ref ls_gethosttype 
 * @see \ref ls_gethostfactor 
 * @see \ref ls_gethostinfo 
 */
extern char    *ls_gethostmodel P_((char *hostname));
extern int     *ls_gethostrespriority P_((char *hostname));

/**
 * \page ls_lockhost ls_lockhost
 * Locks the local host for a specified number of seconds. 
 * 
 * \ref ls_lockhost prevents a host from being selected by the master LIM for task 
 * or job placement. If the host is locked for 0 seconds, it remains locked until 
 * it is explicitly unlocked by \ref ls_unlockhost. Indefinitely locking a host is 
 * useful if a job or task must run exclusively on the local host, or if machine 
 * owners want private control over their machines. 
 *
 * A program using \ref ls_lockhost must be setuid to root in order for the LSF 
 * administrator or any other user to lock a host. 
 *
 * To lock a host, use the setuid function (int setuid(uid_t uid)) to set the 
 * effective user id of the calling process to root or LSF administrator. 
 * On success, this API changes the status of the local host to indicate that 
 * it has been locked by the user.
 *
 * <b>\#include <lsf/lsf.h> 
 *
 * int ls_lockhost(time_t duration)</b>
 *
 * @param duration 
 * The number of seconds the local host is locked. 0 seconds locks a host 
 * indefinitely. 
 *
 * <b>Data Structures:</b>
 * \par
 * none
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n The function was successful. 
 *
 * @return int:-1 
 * \n The function failed. 
 *
 * \b Errors:
 * \par
 * On failure, lserrno is set to indicate the error. If the host is already 
 * locked, \ref ls_lockhost sets lserrno to LSE_LIM_ALOCKED. 
 *
 * <b>Equivalent line commands:</b>
 * \par
 * lsadmin limlock 
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * \n $LSF_CONFDIR/lsf.shared 
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name 
 *
 * @see \ref ls_limcontrol  
 * @see \ref ls_unlockhost  
 *
 */
extern int     ls_lockhost P_((time_t duration));

/**
 * \page ls_unlockhost ls_unlockhost
 * \brief Unlocks a locked local host.
 * 
 * \ref ls_unlockhost unlocks a host locked by \ref ls_lockhost. On success,
 * \ref ls_unlockhost changes the status of the local host to indicate that it is no longer
 * locked by the user.
 * 
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 * the lsf.conf file.
 *
 * <b>\#include <lsf/lsf.h>
 * \n int ls_unlockhost(void)</b>
 * 
 * @param none 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:0 
 * \n The function was successful.
 * @return int:-1 
 * \n The function failed.
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. In particular,
 * \ref ls_unlockhost sets lserrno to LSE_LIM_NLOCKED if the host is not locked.
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * lsadmin limunlock
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 * \n $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 * 
 * @see \ref ls_limcontrol 
 * @see \ref ls_lockhost  
 */
extern int     ls_unlockhost P_((void));

/**
 * \page ls_limcontrol ls_limcontrol
 * Shuts down or reboots a host's LIM. 
 * 
 * To remove a host from a cluster, use \ref ls_limcontrol to shut down the host's 
 * LIM. Next, to return a removed host to a cluster, use \ref ls_limcontrol to reboot 
 * its LIM. When you reboot the LIM, the configuration files are read again and 
 * the previous LIM status of the host is lost. 
 * 
 * The use of \ref ls_limcontrol is restricted to root and the LSF administrator as 
 * defined in the file LSF_CONFDIR/lsf.cluster.cluster_name. 
 * 
 * Any program using this API must be setuid to root if LSF_AUTH is not defined 
 * in the lsf.conf file. 
 * 
 * <b>\#include <lsf/lsf.h> 
 * \n int ls_limcontrol(char *hname, int opCode)</b>
 * 
 * @param *hname 
 * Specifies the host 
 * @param opCode
 * \ref defs_ls_limcontrol 
 * \n Specifies the shutdown or reboot LIM command. 
 *   
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref defs_ls_limcontrol
 * 
 * @return int:0 
 * \n The function was successful. 
 * @return int:-1 
 * \n The function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * lsadmin limstartup \n
 * lsadmin limshutdown \n
 * lsadmin limrestart \n
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * \n $LSF_CONFDIR/lsf.shared 
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name 
 *  
 * @see \ref ls_lockhost  
 * @see \ref ls_unlockhost  
 */
extern int     ls_limcontrol P_((char *hostname, int opCode));    
extern void    ls_remtty P_((int ind, int enableIntSus));
extern void    ls_loctty P_((int ind));

/**
 * \page ls_sysmsg ls_sysmsg
 * \brief Obtains LSF error messages.
 * 
 * \ref ls_sysmsg is a LSLIB library routine for obtaining LSF error messages. The
 * global variable lserrno, maintained by LSLIB, indicates the error number of the
 * most recent LSLIB call that caused an error.
 * 
 * \ref ls_sysmsg returns a character string that contains an error message that
 * corresponds to the current value of lserrno. If an LSLIB call failed due to a system
 * call, then the system error message as indicated by errno is included in the error
 * message.
 *
 * <b>\#include <lsf/lsf.h>
 * \n char *ls_sysmsg(void)</b>
 * 
 * @param none 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return char *:Return Load Sharing System error message defined by lserrno. 
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 * error conditions for this function. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * lsf/lsf.h
 * 
 * @see \ref ls_perror 
 * @see \ref ls_errlog 
 *
 */
extern char    *ls_sysmsg P_((void));

/**
 * \page ls_perror ls_perror
 * Prints LSF error messages.
 * 
 * \ref ls_perror is a LSLIB library routine for printing LSF error messages. The 
 * global variable lserrno, maintained by LSLIB, indicates the error number of
 *  the most recent LSLIB call that caused an error. 
 *
 * \ref ls_perror prints on the standard error output the char string usrMsg, 
 * if it is not a null pointer, followed by a colon and a space, then an error 
 * message that describes the error that corresponds to the value of lserrno, 
 * followed by a newline. If an LSLIB call failed due to a system error, then 
 * the system error message as indicated by errno is included in the error 
 * message.
 *
 * <b>\#include <lsf/lsf.h> 
 * 
 * void ls_perror(char *usrMsg)</b>
 *
 * @param *usrMsg 
 * The standard output error string. 
 *
 * <b>Data Structures:</b>
 * \par
 * none
 * \n
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return void
 * \n There is no return value.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none 
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref ls_sysmsg  
 * @see \ref ls_errlog  
 *
 */
extern void    ls_perror P_((char *usrMsg));
#if defined(WIN32)
extern int getPCClientPass(char** ppName, char** ppPass);
#endif

/* external routines related to API_CONF */

extern struct lsConf *ls_getconf P_((char *));
extern void ls_freeconf P_((struct lsConf * ));
extern struct sharedConf *ls_readshared P_((char *));
extern struct clusterConf *ls_readcluster P_((char *, struct lsInfo *));
extern struct clusterConf *ls_readcluster_ex P_((char *, struct lsInfo *, int));

/* end of external routines related to API_CONF */

extern int     _ls_initdebug P_((char *appName));
#if !(defined(_AIX) || defined(__AIX))
extern void    ls_syslog P_((int level, const char *fmt, ...))
#if defined (__GNUC__) && defined (CHECK_ARGS)  /* use GCC argument checking */
    __attribute__((format(printf, 2, 3)))
#endif  /* __GNUC__ */
        ;
#endif

#if !(defined(_AIX) || defined(__AIX))

/**
 * \page ls_errlog ls_errlog
 * 
 * \brief Logs error messages.
 * 
 * \ref ls_errlog is a LSLIB library routine for logging LSF error messages. The global
 * variable lserrno, maintained by LSLIB, indicates the error number of the most
 * recent LSLIB call that caused an error.
 * 
 * \ref ls_errlog is very similar to the fprintf() function, except that it prints out the
 * time before it prints other information. You can specify an additional conversion
 * char 'm' in the format to represent the error message that corresponds to
 * lserrno. This function is typically used by load sharing applications running in the
 * background (such as daemons) to log error messages to a log file.
 * 
 * The vector of error message strings, ls_errmsg[ ], is provided for the benefit of
 * application programs. You can use lserrno as an index into this table to obtain the
 * corresponding LSF error message. The global variable ls_nerr indicates the size of
 * the table.
 *
 * <b>\#include <lsf/lsf.h>
 * \n void ls_errlog(FILE *fp, char *fmt, ...)</b>
 *
 * @param *fp  File pointer where the error messages are logged.
 * @param *fmt  Format string of error messages.
 * @param ls_errmsg Vector of error message strings, provided for the benefit of application programs.
 * @param lserrno Index into this table to obtain the corresponding LSF error message.
 * @param ls_nerr Indicates the size of the table.
 *
 * <b>Data Structures:</b>
 *\par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 *none
 *
 * @return void
 * \n There is no return value.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 * error conditions for this function. 
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * lsf/lsf.h
 *
 * @see \ref ls_perror 
 * @see \ref ls_sysmsg 
 */
extern void	ls_errlog P_((FILE *fp, const char *fmt, ...))
#if defined (__GNUC__) && defined (CHECK_ARGS) /* use GCC argument checking */	
	__attribute__((format(printf, 2, 3)))
#endif  /* __GNUC__ */
	;
#endif

extern void	ls_verrlog P_((FILE *fp, const char *fmt, va_list ap));

/**
 * \page ls_fdbusy ls_fdbusy
 * 
 * \brief Tests if a specified file descriptor is in use or reserved by LSF.
 *
 * \ref ls_fdbusy tests if a specified file descriptor is in use or reserved by LSF. The
 * possible descriptors used by LSF are those used for contacting the LIM and for
 * remote execution. fd is the file descriptor to test. This call is typically used when an
 * application wants to close all unneeded file descriptors.
 * 
 * Any program using this routine must call \ref ls_initrex first.
 * 
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 * the lsf.conf file.
 *
 * <b>\#include <lsf/lsf.h>
 * \n int ls_fdbusy(int fd)</b>
 *
 * @param fd fd is the file descriptor to test.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:True 
 * \n Returned if fd is in use or reserved by LSF.     
 * @return int:False 
 * \n Returned if fd is not in use or reserved by LSF.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 * error conditions for this function. 
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref ls_initrex 
 */
extern int      ls_fdbusy P_((int fd));

/**
 * \page ls_getmnthost ls_getmnthost
 * Returns the name of the file server containing a specific file. 
 * 
 * \ref ls_getmnthost returns the name of the file server that exports the file system 
 * containing file, where file is a relative or absolute path name. For remote 
 * files, use \ref ls_rgetmnthost instead. 
 * 
 * <b>\#include <lsf/lsf.h>
 * \n char *ls_getmnthost(char *file)</b>
 * 
 * @param *file 
 * The relative or absolute path name for the file server. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return char *:Returns the host where a file system is mounted. 
 * \n Function was successful.
 * @return NULL 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to d
 * etect error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 *  none
 * 
 * @see \ref ls_rgetmnthost  
 */
extern char     *ls_getmnthost P_((char *fn));    
extern int      ls_servavail P_((int, int));
extern int	ls_getpriority P_((int *priority));
extern int	ls_setpriority P_((int newPriority));
    
extern void ls_ruunix2lsf P_((struct rusage *rusage, struct lsfRusage *lsfRusage));
extern void ls_rulsf2unix P_((struct lsfRusage *lsfRusage, struct rusage *rusage));
extern void cleanLsfRusage P_((struct lsfRusage *));
extern void cleanRusage P_((struct rusage *));

/* routines related to lsf.acct file operation */
extern int getBEtime P_((char *, char, time_t *));
/* end of routines related to lsf.acct file operation */

/* routines related to event generation */
extern int	ls_postevent P_((int, char *, char **, int));
extern int	ls_postmultievent P_((int, char *, char **, int, int));

extern int      ls_limhostann(struct limHostAnnReq *);
extern void ls_freelim_licensekey(struct lim_licensekey *);
extern void ls_freelim_licensekey_reply(struct lim_licensekey_reply *);
extern int ls_getLicenseInfo(struct lim_licensekey* req, struct lim_licensekey_reply* ack);

extern struct entitlementInfoReply *ls_getentitlementinfo P_((struct entitlementInfoRequest*));
extern void ls_freeentitlementinfo P_(( struct  entitlementInfoReply *));

#ifndef NON_ANSI

typedef int (*exceptProto)(void);

/* struct used to ge the ext resource from plugin shared object */
struct extResInfo {
    char *name;         /* resource name string pointer */
    char *type;         /* string pointer of value type */
    char *interval;     /* how often it is evaluated */
    char *increasing;   /* the increasing directory */
    char *des;          /* string pointer of description of the resource */
};

#if defined(SOLARIS)

/* LIM plugin functions */
extern int    lim_vcl_get_eres_version P_((void));
extern struct extResInfo *lim_vcl_get_eres_def P_((char *));
extern char   *lim_vcl_get_eres_loc P_((char *));
extern char   *lim_vcl_get_eres_val P_((char *));

#endif

#endif /* ifndef NON_ANSI */
#undef P_

#if defined(__cplusplus)
}
#endif

/* only on the following platform, the isspace() is not correct on some 8 
   bit char 
*/
#if defined(WIN32)
#include <sys/types.h>
#ifdef isspace
#undef isspace
#endif
#define  isspace(c) ((c==0x20 || c==0x09 || c==0x0a || c==0x0b || c==0x0c || c==0x0d) ? 8:0)
#endif /* (WIN32) */

/*
 * RFC #725
 * added by Bing(BJ) 2006/06/08
 */
#define MAXFULLFILENAMELEN     4096 /* max len. of a filename */
#define MAXFULLPATHNAMELEN     2*MAXFULLFILENAMELEN
#define MAXFULLMSGSIZE         4*MAXFULLFILENAMELEN

#define UNKNOWN_AUTO_DETECT "UNKNOWN_AUTO_DETECT"

/** \defgroup live_config_actions live_config_actions
 * Live Configuration Actions
 * @{
 */
/*
 * The order of the array must be kept
 * you could only append new actions before LiC_ACTION_END
 * otherwise it would break the object+action checking
 */
typedef enum {
    LiC_ACTION_ADDMEMBER  = 0,  /**< Add a new identity with key-values into 
                                 * a group item 
                                 */
    LiC_ACTION_RMMEMBER,           /**< Remove an identity from a group item */
    LiC_ACTION_UPDATE,                /**< Update an identity with key-values settings */
    LiC_ACTION_CREATE,                 /**< Create an identity */
    LiC_ACTION_DELETE,                 /**< Delete an identity. Requires confirmation when deleting a usergroup or host*/
    /*any actions after LiC_ACTION_DELETE only work as sort of enhanced actions*/    
    LiC_ACTION_ENFORCEDELETE,   /**< Delete an identity without confirmation */    
    LiC_ACTION_DISABLE,                /**< Disable live reconfig feature by primary LSF Admin*/    
    LiC_ACTION_END              /**< The total number of actions supported by live configuration */
} livConfAction_t;
/*@}*/

/** \defgroup live_config_objects live_config_objects
 * Live Configuration Object Types 
 * @{
 */
/*
 * The order of the array must be kept
 * you could only append new actions before LiC_OBJECT_END
 * otherwise it would break the object+action checking
 */
typedef enum {
    LiC_OBJECT_USER = 0,     /**< Follows a user identity */
    LiC_OBJECT_USERGROUP,    /**< Follows a usergroup identity */
    LiC_OBJECT_HOST,         /**< Follows a host identity */
    LiC_OBJECT_HOSTGROUP,    /**< Follows a host group identity */
    LiC_OBJECT_HOSTPARTITION,/**< Follows a host partation identity */
    LiC_OBJECT_QUEUE,        /**< Follows a queue identity */
    LiC_OBJECT_LIMIT,        /**< Follows a limit identity */
    LiC_OBJECT_APPLICATION,  /**< Follows an application identity */
    LiC_OBJECT_COMPUTEUNIT,  /**< Follows an compute unit identity */
    LiC_OBJECT_GUARPOOL,     /**< Follows a guaranteed resource pool identity [For future support]*/
    LiC_OBJECT_SERVICECLASS, /**< Follows a service class identity [For future support]*/
    LiC_OBJECT_ADVRSV,       /**< Follows an advance reservation identity [For future support]*/
    LiC_OBJECT_RESOURCEMAP,  /**< Follows an resource map identity [For future support]*/
    LiC_OBJECT_RESOURCERSV,  /**< Follows an resource reservation identity [For future support]*/
    LiC_OBJECT_BASE_HOST,  /**< Follows a host identity in base part [For future support]*/
    LiC_OBJECT_END           /**< The total number of objects supported by live configuration*/
} livConfObject_t;
/*@}*/

/**
 * @{
 * \brief Live configuration request.
 */
typedef struct liveConfReq {
    livConfAction_t            actions;     /**< Sets the actions in \ref live_config_actions. Action includes  
								   *ADDMEMBER, RMMEMBER, UPDATE, CREATE(ADD), DELETE.
								   */
    livConfObject_t            object_type; /**< Sets the types in \ref live_config_objects. Object types include
                                                             *USER, USERGROUP, HOSTGROUP, QUEUE, LIMIT, GPOOL, HOST
                                                             */
    char              *identity;            /**< Object name */
    char              *key_value;           /**< Key-value pair string.  
								   *For example, "[MXJ=...][; JL/U=...][; EXIT_RATE=...]"
							          */
    char              *comments;            /**< Comments */
    int                 seq;                  /**< The bconf sequence id; 0 for all requests except delete usergroup. 
     	                                              *For delete usergroup, first send seq = 0 to get confirmation and a 
     	                                              *new seq from mbatchd, then call mbatchd with the same request 
     	                                              *and the new seq.
							    */
} LiveConfReq;
/*@}*/


#define LiC_REMOVE_ACTIVE_REQ "REMOVE_ACTIVE_REQ"

enum ScavengType {
     NO_SA,           /* normal host */
     DESKTOP_SA, /* desktop harvesting */
     SERVER_SA,   /* server harvesting */
     VM_SA           /* IS harvesting */
};

/**
 * @{
 * \enum types for the affinity scheduling.
 */
typedef enum {
    PU_NONE = 0,
    PU_HOST,
    PU_NUMA,
    PU_SOCKET,
    PU_CORE,
    PU_THREAD,
    PU_MAX
} PU_t;

typedef enum {
    MEMBIND_UNDEFINED = 0,
    MEMBIND_LOCALONLY,
    MEMBIND_LOCALPREFER
} memBindPolicy_t;

typedef enum {
    DISTRIBUTE_UNDEFINED = 0,
    DISTRIBUTE_ANY,
    DISTRIBUTE_BALANCE,
    DISTRIBUTE_PACK
} distributeMethod_t;
/*@}*/

#ifdef WIN32
#define PLATFORM_REGISTRY_TOP "SOFTWARE\\Platform Computing Corporation"
#define IBM_REGISTRY_TOP      "SOFTWARE\\IBM Platform"
#endif
#ifdef WIN64
#define PLATFORM_32BIT_REGISTRY_TOP "SOFTWARE\\Wow6432Node\\Platform Computing Corporation"
#define IBM_32BIT_REGISTRY_TOP      "SOFTWARE\\Wow6432Node\\IBM Platform"
#endif


#endif /* _EGO_H_ */


#define LSF_VERSION LSF_XDR_VERSION9_1_1 /* unsigned char used for version 
					* i.d. within struct LSFHeader 
                                        * 1 for 2.0 and 2.0a
                                        * 2 for 2.1 and 2.1a
                                        * 3 for 2.2 
					* 4 for 3.0, 3.0a, 3.0b
					* 5 for 3.1
					* 6 for 3.2 3.21
					* 7 for 3.22
					* 8 for 4.0
					* 9 for 4.1
					* 10 for 4.2
					* 11 for 5.0
					* 12 for 5.1
					* 13 for 6.0
					* 14 for 6.1
					* 15 for 6.2
					* 17 for 7.0
					* 18 for 7.0 EP1 
					*/
#define LSF_CURRENT_VERSION  "9.1.1.0"
#define LSF_CURRENT_VERSION_STRIPPED  "9.1.1"

/*--------------------Begin of version string definition--------------------*/
/*Below defintion is introduced by 72905
* Macros IDENTIFIER, COMMENTS, BUILD_NUMBER, REL_DATE should be defined at
* build time (by uncomments related definition in make.misc).
*
* For the detail information about how to define these macro, please refer 
* Product Versioning Functional Specification 
*/
/*Copyright string*/
#define LSF_PRODUCT_COPYRIGHT_STR "Copyright International Business Machines Corp 1992-2013.\n" \
                                  "US Government Users Restricted Rights - Use, duplication " \
                                  "or disclosure restricted by GSA ADP Schedule Contract with IBM Corp."

/*Product name string*/
#define LSF_NAME_STR "IBM Platform LSF "
#ifdef IDENTIFIER
#define LSF_IDENTIFIER_STR " (" IDENTIFIER ")"
#else
#define LSF_IDENTIFIER_STR ""
#endif
#define LSF_PRODUCT_NAME_STR LSF_NAME_STR LSF_IDENTIFIER_STR

/*Comments string*/
#ifdef COMMENTS
#define LSF_PRODUCT_COMMENT_STR COMMENTS
#else
#define LSF_PRODUCT_COMMENT_STR ""
#endif

/*Build number string*/
#ifdef BUILD_NUMBER
#define LSF_PRODUCT_BUILD_STR " build " BUILD_NUMBER 
#else 
#define LSF_PRODUCT_BUILD_STR   "" 
#endif

/*build date string*/
/*if REL_DATE is not defined, LSF_PRODUCT_BUILD_DATE_STR in builddate.h will be
* used
*/
#ifdef REL_DATE
#define LSF_PRODUCT_BUILD_DATE_STR    REL_DATE 
#endif

/*Full product version string*/
/*When modify macros below, please don't forget to modify XYZ_STR macros too
*/
#define LSF_PRODUCT_MAJOR_VERSION               9
#define LSF_PRODUCT_MINOR_VERSION               1
#define LSF_PRODUCT_MAINTAIN_VERSION            1
#define LSF_PRODUCT_FOURTH_VERSION              0

#define LSF_PRODUCT_MAJOR_VERSION_STR           "9"
#define LSF_PRODUCT_MINOR_VERSION_STR           "1"
#define LSF_PRODUCT_MAINTAIN_VERSION_STR        "1"
#define LSF_PRODUCT_FOURTH_VERSION_STR          "0"

/* for product version, we do not care about maintain version if it is 0 */
#define LSF_PRODUCT_VERSION_STR     LSF_PRODUCT_MAJOR_VERSION_STR "." \
                                    LSF_PRODUCT_MINOR_VERSION_STR "." \
                                    LSF_PRODUCT_MAINTAIN_VERSION_STR

/* for file version, it is used for display in attribute */
#define LSF_FILE_VERSION_STR        LSF_PRODUCT_MAJOR_VERSION_STR "." \
                                    LSF_PRODUCT_MINOR_VERSION_STR "." \
                                    LSF_PRODUCT_MAINTAIN_VERSION_STR "." \
                                    LSF_PRODUCT_FOURTH_VERSION_STR \
                                    LSF_PRODUCT_BUILD_STR


#ifndef WIN_VERSION_NUMBER
#define WIN_VERSION_NUMBER 65535
#endif

/*After the -V enhancement by proj#84288. 
  * the above _LS_VERSION_ becomes fully informative but 
  * we should keep the original output of lsid.
  * we define the string here again 
  */

#define _VERSION_STR_LSID_  "Platform LSF 9.1.1.0"

#if defined(__STDC__) || defined(WIN32)

#define _LSID_VERSION_ (_VERSION_STR_LSID_", " _DATE_STR_"\n" LSF_PRODUCT_COPYRIGHT_STR "\n")

#else /* defined(__STDC__) || defined(WIN32) */

#define _LSID_VERSION_ (_VERSION_STR_LSID_"\n" LSF_PRODUCT_COPYRIGHT_STR "\n")

#endif /* defined(__STDC__) || defined(WIN32) */

/*--------------------End of version string definition--------------------*/

#if defined(__STDC__) || defined(__cplusplus) || defined(SN_SVR4) || defined(WIN32)
#define P_(s) s
#include <stdarg.h>
#else
#define P_(s) ()
#include <varargs.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* data structures related to LIBNIO */
#define NIO_STDIN_ON                        0x01 
#define NIO_STDIN_OFF                       0x02 
#define NIO_TAGSTDOUT_ON                    0x03 
#define NIO_TAGSTDOUT_OFF                   0x04 

#define NIO_TASK_STDINON                    0x01 
#define NIO_TASK_STDINOFF                   0x02 
#define NIO_TASK_ALL                        0x03 
#define NIO_TASK_CONNECTED                  0x04 

enum nioType {
    NIO_STATUS,                        /* remote child exit status ready */
    NIO_STDOUT,                        /* stdout data ready */
    NIO_EOF,                           /* EOF from remote task */
    NIO_IOERR,                         /* Read error from remote stream */
    NIO_REQUEUE,       		       /* the task should be requeued */
    NIO_STDERR                         /* stderr data ready */
};

struct nioEvent {
    int tid;                    /* the task having io activity */
    enum nioType type;          /* the type of io */
    int status;                 /* status if ioType is LSNIO_STATUS */
};

struct nioInfo {
    int num;                    /* number of items in ioTask list */
    struct nioEvent *ioTask;
};
/* end of data structures related to LIBNIO */

extern int     ls_initdebug P_((char *appName));
/* routines related to LIBNIO */
extern int     ls_nioinit P_((SOCKET sock));
#if !(defined(_AIX) || defined(__AIX))
extern int     ls_nioselect P_((int, fd_set *, fd_set *, fd_set *, 
				struct nioInfo ** , struct timeval *)); 
#endif
extern int     ls_nioctl P_((int, int));
extern int     ls_nionewtask P_((int, SOCKET));
extern int     ls_nioremovetask P_((int));
extern int     ls_niowrite P_((char *, int));
extern int     ls_nioclose P_((void));
extern int     ls_nioread P_((int, char *, int));
extern int     ls_niotasks P_((int, int *, int));
extern int     ls_niostatus P_((int, int *, struct rusage *));
extern int     ls_niokill P_((int));
extern int     ls_niosetdebug P_((int));
#if defined(WIN32)
extern int     ls_niodump P_((HANDLE, int, int, char *));
#else
extern int     ls_niodump P_((int, int, int, char *));
#endif /* WIN32 */
/* end of routines related to LIBNIO */

extern int     lsf_res_version;	/* version of res */

/*lsf.init.c*/

/**
 * \page ls_initrex ls_initrex
 * \brief Initializes the LSF library for remote execution. 
 * 
 * \ref ls_initrex initializes the LSF library for remote execution. This routine 
 * must be called before any other remote execution LSLIB library routines 
 * can be used. 
 * 
 * Two remote execution security options are supported in LSF. The first option 
 * is to set the effective user ID of an LSF application to root, as other UNIX 
 * applications that access remote resources (e.g., rlogin) do. Using this option, 
 * numports of the application's file descriptors are bound to privileged ports by 
 * ls_initrex(). These sockets are used only for remote connections to RES. If 
 * numports is 0, then the system will use the default value LSF_DEFAULT_SOCKS 
 * defined in <lsf/lsf.h>. If successful, the number of socket descriptors 
 * starting from FIRST_RES_SOCK (defined in <lsf/lsf.h>) that are actually bound 
 * to privileged ports is returned, -1 otherwise. To use this option for 
 * authentication, the application must be installed as setuid to root. The second
 * security option is to use an authentication daemon supporting the Ident 
 * protocol (RFC 931/1413/1414). In this case, this routine returns the value of 
 * the input parameter numports if it succeeds, -1 otherwise. 
 * 
 *\ref ls_initrex selects the security option according to the following rule: if 
 * the application program invoking it has the effective uid of root, then 
 * privileged ports are created; otherwise, no such port is created, and RES will 
 * contact an authentication daemon on a connection request (see \ref ls_connect). 
 * 
 * Currently, the only option that can be specified in options is KEEPUID, which 
 * instructs \ref ls_initrex to preserve the current user ID. If the KEEPUID bit is 
 * not set in options (i.e. options is zero), then \ref ls_initrex will change the 
 * real, effective and saved user ID to the real user ID. 
 * 
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 * the lsf.conf file. 
 * 
 * This function must be called before calling any other remote execution 
 * function. 
 *
 * <b>
 * \#include <lsf/lsf.h> 
 *
 *  int ls_initrex(int num, int options)
 * </b> 
 *
 * @param num The number of socket to be creat.
 * @param options Flags for \ref ls_initrex. Currently its value is KEEPUID.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:numports 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_connect  
 */

extern int     ls_initrex P_((int, int));

#define ls_init(a, b) ls_initrex((a), (b))  /* for backward compatibility */

/*lib.nios.c*/
/**
 * \page ls_donerex ls_donerex
 * \brief Kills the Network I/O server (NIOS) and restores the tty environment before a
 * remote execution connection is closed.
 * 
 * \ref ls_donerex kills the Network I/O server (NIOS) and restores the tty
 * environment before a remote execution connection is closed. You need to call this
 * routine only if a remote execution was started by either \ref ls_rtask or
 * \ref ls_rtaske, and the option REXF_USEPTY was set. If the application exits without
 * calling this routine, the terminal may behave abnormally on occasion. If the option
 * REXF_USEPTY is not specified when either \ref ls_rtask or \ref ls_rtaske is called,
 * there is no need to call \ref ls_donerex.
 * 
 * Any program using this routine must call \ref ls_initrex first.
 * 
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 * the lsf.conf file.
 * 
 * <b>
 * \#include <lsf/lsf.h>
 *
 * int ls_donerex(void)
 * </b>
 * 
 * @param none   
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful.
 * @return int:-1 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 * 
 * @see \ref ls_initrex 
 * @see \ref ls_rtask 
 * @see \ref ls_rtaske 
 */
 
extern int	ls_donerex P_((void));

extern int	ls_niossync P_((int));

/**
 * \page ls_setstdin ls_setstdin
 * \brief Allows an application program to query and specify how stdin is assigned 
 * to a specific subset of remote tasks. 
 * 
 * \ref ls_setstdin gives an application program the ability to query and specify how 
 * stdin is assigned to remote tasks. It allows you to assign stdin to a specific 
 * subset of remote tasks. You can change this setting at any time. 
 *
 * \ref ls_setstdin turns on or off the delivery of standard input to specific remote
 * tasks. Other remote tasks are not affected by this call. 
 *
 * By default, a remote task is set to receive standard input. Note that remote 
 * tasks only receive standard input if the current stdin mode is remote. Hence, 
 * if the application is running in local stdin mode (see the description of 
 * \ref ls_stdinmode), \ref ls_setstdin is not effective. 
 * 
 * Upon success, \ref ls_setstdin returns zero. On failure, -1 is returned, and the 
 * error code is stored in lserrno. 
 *
 * Any program using this routine must call \ref ls_initrex first. 
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in 
 * the lsf.conf file. 
 *
 * <b>
 * \#include <lsf/lsf.h>
 *
 * int ls_setstdin(int on, int *tidlist, int len)
 * </b>
 * 
 * @param on If on is non-zero and the current stdin mode is remote, then the tasks
 * given by tidlist receive the standard input. If on is zero, the tasks will not
 *  receive standard input.  
 * @param *tidlist tidlist gives the list of task IDs of the remote tasks to be 
 * operated upon. 
 * @param len The number of entries. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful.
 * @return int:-1 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 * 
 * @see \ref ls_stdinmode 
 * @see \ref ls_getstdin 
 * @see \ref ls_initrex 
 */
 
extern int	ls_setstdin P_((int on, int *rpidlist, int len));

/**
 * \page ls_getstdin ls_getstdin
 * \brief Allows an application program to query and specify how stdin is assigned
 * to remote tasks. 
 *
 * \ref ls_getstdin gives an application program the ability to query and specify 
 * how stdin is assigned to remote tasks. It allows you to assign stdin to to 
 * all remote tasks. You can change this setting at any time.
 * 
 * \ref ls_getstdin gets the list of remote task IDs that receive (or do not receive)
 * standard input. If on is non-zero, the task IDs of the remote tasks that are
 * enabled to receive standard input are stored in tidlist. maxlen is the size of 
 * the tidlist array. If on is zero, then the IDs of remote tasks whose standard 
 * input is disabled are returned. The ID of a task is assigned by the LSLIB when 
 * \ref ls_rtask is called.
 *
 * Upon success, \ref ls_getstdin returns the number of entries stored in tidlist. On
 * failure, -1 is returned, and the error code is stored in lserrno. In particular, if there
 * are more than maxlen remote task IDs to be returned, lserrno is set to LSE_RPIDLISTLEN.
 *
 * Any program using this routine must call \ref ls_initrex first.
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 * the lsf.conf file.
 * 
 * <b>
 * \#include <lsf/lsf.h> 
 *
 * int ls_getstdin(int on, int *tidlist, int maxlen)
 * </b>
 * 
 * @param on 
 * If on is non-zero, the task IDs of the remote tasks that are enabled to 
 * receive standard input are stored in tidlist. If on is zero, then the IDs of 
 * remote tasks whose standard input is disabled are returned. The ID of a task is
 * assigned by the LSLIB when \ref ls_rtask is called. 
 * @param *tidlist tidlist gives the list of task IDs of the remote tasks to be 
 * operated upon.
 * @param maxlen
 * The size of the tidlist array. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:Number of Entries Stored.
 * \n The function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. If there are more 
 * than maxlen remote task IDs to be returned, lserrno is set to LSE_RPIDLISTLEN. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_stdinmode  
 * @see \ref ls_setstdin  
 * @see \ref ls_initrex  
 */

extern int	ls_getstdin P_((int on, int *rpidlist, int maxlen));

extern int	ls_setstdout P_((int on, char *format));

/**
 * \page ls_stdinmode ls_stdinmode
 * \brief Allows an application program to query and specify how stdin is assigned
 * to remote tasks on a local application. 
 * 
 * \ref ls_stdinmode gives an application program the ability to query and specify 
 * how stdin is assigned to remote tasks. It allows you to assign stdin to the local
 *  program only. You can change this setting at any time. 
 *
 * \ref ls_stdinmode specifies whether standard input is read by the calling (local) 
 * application or its remote children. 
 *
 * This routine returns 0 on success; otherwise, it returns -1 and sets lserrno to
 *  indicate the error 
 * 
 * Any program using this routine must call \ref ls_initrex first. 
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in 
 * the lsf.conf file. 
 *
 * <b>
 * \#include <lsf/lsf.h>
 *
 * int ls_stdinmode(int remote)
 * </b>
 * 
 * @param remote If remote is non-zero, then the application will not read subsequent
 * standard input, and the remote children will read standard input. This mode of
 * operation is called the remote stdin mode. Remote stdin mode is the default. 
 * In remote stdin mode, standard input is read by the Network I/O Server (NIOS) 
 * and forwarded to the appropriate remote tasks. If remote is zero, then the 
 * application reads the subsequent standard input, and it is not forwarded to remote
 *  children. This mode of operation is called the local stdin mode. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful.
 * @return int:-1 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 * 
 * @see \ref ls_setstdin 
 * @see \ref ls_getstdin 
 * @see \ref ls_initrex 
 */
 
extern int	ls_stdinmode P_((int onoff));

/**
 * \page ls_stoprex ls_stoprex
 * \brief Stops the Networks I/O Server and restores the local tty environment. 
 * 
 * \ref ls_stoprex stops the Network I/O Server (NIOS) and restores the local tty
 *  environment. This routine is necessary only for those LSF applications that
 *  explicitly catch job control signals (that is, SIGTSTP) and eventually suspend
 *  themselves. For most applications, the default SIGTSTP handler in the LSF
 *  library provides the desired behavior, without the requirement of calling 
 * this routine. 
 *
 * When NIOS receives a SIGTSTP signal, it sends the signal to all its remote tasks.
 * If the local client also needs to be stopped, then \ref ls_stoprex must be called 
 * to stop the NIOS and restore the tty environment for the parent application first. 
 * 
 * Any program using this routine must call \ref ls_initrex first. 
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in 
 * the lsf.conf file. 
 *
 * <b>
 * \#include <lsf/lsf.h>
 *
 * int ls_stoprex(void) 
 * </b>
 * 
 * @param none  
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful.
 * @return int:-1 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 * 
 * @see \ref ls_initrex 
 */

extern int      ls_stoprex P_((void));

/*lib.res.c*/

/**
 * \page ls_chdir ls_chdir
 * \brief \ref ls_chdir sets the application's working directory on the remote host to the
 * directory specified by clntdir. 
 *
 * If the application subsequently requests remote execution with the flag REXF_CLNTDIR, 
 * the Remote Execution Server (RES) uses the application's working directory on the 
 * remote host, instead of the application's local current working directory, as the 
 * current working directory for the remote execution. The RES keeps a working directory
 * for each application, which is initialized as the user's home directory. The 
 * application can call this routine to change its working directory on a particular 
 * host. clntdir must be the full pathname of a valid directory on the host host.
 * 
 * Any program using this routine must call \ref ls_initrex first.
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 * the lsf.conf file.
 * 
 * Sets an application's working directory on a remote host to a specified directory.
 * 
 * <b>
 * \#include <lsf/lsf.h>
 * 
 * int ls_chdir(char *host, char *clntdir)
 * </b> 
 *
 * @param *host The remote host containing the client directory.
 * @param *clntdir The full pathname of a valid directory on the host host.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 
 * \n Function was successful.
 * @return int:-1 
 * \n Function failed.
 * @return int:-2 
 * \n A warning if the RES fails to check clntdir due to permission denial. This 
 * is a temporary mechanism to get around the root uid mapping problem of NFS.
 *
 * \b Errors:                                             
 * \par                                                   
 * Systems that conform to the Single UNIX specification are not required to detect
 * error conditions for this function.  
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see none
 */

extern int      ls_chdir P_((char *, char *));

/**
 * \page ls_connect ls_connect
 * \brief Sets up an initial connection with the Remote Execution Server (RES) on a
 * specified remote host.
 *
 * This routine returns general configuration information about LSF clusters.
 * 
 * \ref ls_connect sets up an initial connection with the Remote Execution Server (RES) 
 * on a specified remote host. You can then use this connection for future remote 
 * execution or control interactions. This routine is called automatically if a 
 * connection is not set up when a remote execution request is made (see \ref ls_rexecv). 
 * The explicit invocation of this routine has performance advantages in certain 
 * cases, typically for parallel applications. The routine returns immediately when 
 * the connection is established rather than waiting for the completion of the 
 * possibly time consuming authentication and status checking process by the 
 * RES (see res()). The application can set up initial connections with many 
 * remote hosts simultaneously, overlapping the authentication processes on all 
 * remote hosts. On successful completion, this routine returns a socket descriptor 
 * through which the connection has been established. If the caller's effective uid 
 * is root, this socket has been bound to a privileged port during \ref ls_initrex. 
 * \ref ls_connect uses a socket created by the preceding invocation of \ref ls_initrex 
 * and invokes connect() to connect to the specified host. If the connection fails, 
 * -1 is returned and the socket is closed.
 * 
 * The successful return of \ref ls_connect does not mean that the RES has granted 
 * remote execution permission, it means that the authentication process has been 
 * initiated. If the RES does not grant remote execution permission, an error is 
 * returned in the next interaction with the RES. Calling \ref ls_connect multiple 
 * times with the same host name does not create multiple connections; the same 
 * connection is always used. 
 * 
 * <b>
 * \#include <lsf/lsf.h>
 *
 * int ls_connect(char *host)
 * </b>
 *
 * @param *host The host that is set up with a Remote Execution Server.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:Returns socket descriptor through which a connection has been set up. 
 * \n Function was successful.
 * @return int:-1 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 * error conditions for this function. 
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref ls_rexecv 
 * @see \ref ls_initrex 
 * @see \ref ls_isconnected 
 * @see \ref ls_findmyconnections 
 */
 
extern SOCKET   ls_connect P_((char *));

/**
 * \page ls_rkill ls_rkill
 * \brief Signals a remote task 
 * 
 * \ref ls_rkill sends the signal sig to the remote task tid and all its children 
 * that belong to the same UNIX process group. tid is the remote task ID that is 
 * returned by \ref ls_rtask or \ref ls_rtaske. If sig is zero, then this 
 * call only polls the existence of the remote task tid. 
 * 
 * Any program using this routine must call \ref ls_initrex first. 
 * 
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 *  the lsf.conf file. 
 * 
 * <b>
 * \#include <lsf/lsf.h>
 * \n \#include <signal.h> 
 *
 * int ls_rkill(int tid, int sig) 
 * </b>
 * 
 * @param sig The signal sent to the tid. 
 * @param tid The remote task ID returned by \ref ls_rtask or \ref ls_rtaske. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int:0 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rtask  
 * @see \ref ls_rtaske  
 */
 
extern int      ls_rkill P_((int, int));

/**
 * \page ls_rsetenv ls_rsetenv
 * \brief Sets up environment variables on a remote host. 
 * 
 * \ref ls_rsetenv sets up the environment variables given in envp on the specified 
 * remote host. envp is a pointer to an array of strings of the form variable=value. 
 * When the environment variables are set, all remote tasks on the remote host 
 * acquire the environment setting until another call to this routine overrides it. 
 * A default set of environment variables is set up for the remote host if this 
 * routine is never called (see \ref ls_rtask). This call is typically used to propagate
 * changes in the local environment to the remote hosts to which the application 
 * has connections. 
 * 
 * Any program using this routine must call \ref ls_initrex first. 
 * 
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in 
 * the lsf.conf file. 
 *
 * <b>
 * \#include <lsf/lsf.h> 
 * 
 * int ls_rsetenv(char *host, char **envp) 
 * </b>
 *
 * @param *host The remote host upon which the environment is being set. 
 * @param **envp A pointer to an array of strings of the form variable=value. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int: 0 
 * \n The function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rtask  
 * @see \ref ls_initrex  
 */

extern int	ls_rsetenv P_((char *host, char **env));

extern int	ls_rsetenv_async P_((char *host, char **env));

/*lib.resctrl.c*/

/**                                                                                                                             
 * \page ls_rescontrol ls_rescontrol
 * \brief Controls and maintains the Remote Execution Server.                                                                            
 *                                                                                                                                
 * This library routine is used by the LSF administrator or authorized users to                                                   
 * control and maintain the Remote Execution Server (RES).                                                                        
 * 
 * <b>
 * \#include <lsf/lsf.h>                                                                                                          
 *                                                                                                                                
 * int ls_rescontrol(char *host, int opCode, int data)                                                                            
 * </b>                                                                                                                               
 *                                                                                                                                
 * @param *host                                                                                                                   
 * The host argument is used to specify the host name of the machine whose RES is                                                 
 * to be operated upon.                                                                                                           
 *                                                                                                                                
 * @param opCode                                                                                                                  
 * The command is specified by the opCode argument and the data argument is used                                                  
 * to supply an extra parameter for a particular opCode. The supported values are                                                 
 * \ref commands_rescontrol.                                                                                                     
 *                                                                                                                                
 * @param data                                                                                                                    
 * The data argument is optionally used with RES_CMD_LOGON to specify a CPU time                                                  
 * threshold in msec, so that RES will log resource information only for tasks                                                    
 * that consumed more than the specified CPU time.                                                                                
 *                                                                                                                                
 * <b>Data Structures:</b>                                                                                                      
 * \par                                                                                                                           
 * none                                                                                                                                                                                                                                                       
 *                                                                                                                                
 * <b>Define Statements:</b>                                                                                                    
 * \par                                                                                                                           
 * \ref commands_rescontrol                                                                                                      
 *                                                                                                                                
 * @return int:0                                                                                                              
 * \n Function was successful.                                                                                                       
 *                                                                                                                                
 * @return int:-1                                                                                                             
 * \n Function failed.                                                                                                                                                                                                                                             
 *                                                                                                                                
 * \b Errors:                                                                                                                     
 * \par                                                                                                                           
 * If the function fails, then lserrno is set to indicate the error. In particular,                                               
 * \ref ls_rescontrol will set lserrno to LSE_BAD_OPCODE if the opCode is not from                                                   
 * the list above.                                                                                                                
 *                                                                                                                                
 * <b>LSLIB calls for remote execution services: </b>                                                                             
 * \par                                                                                                                           
 * These routines allow programs to make use of LSF remote execution services.                                                    
 * Such services include support for maintaining standard I/O transparency to                                                     
 * and from remote machines, establishing, using, and terminating remote                                                          
 * connections, transferring terminal and environment variable settings to                                                        
 * remote processes, executing remote tasks and so on.                                                                            
 * \n\n All rex routines require that the header <lsf/lsf.h> is included.                                                         
 * \n\n The following routines are supported:                                                                                     
 * \n\n \ref ls_initrex                                                                                                        
 * \n Initiate remote execution                                                                                                   
 * \n\n \ref ls_connect                                                                                                        
 * \n Establish a remote connection                                                                                               
 * \n\n \ref ls_isconnected                                                                                                    
 * \n Check for an established connection                                                                                         
 * \n\n \ref ls_findmyconnections                                                                                             
 * \n List hosts with open remote connections                                                                                     
 * \n\n \ref ls_rexecv                                                                                                          
 * \n Remote execv                                                                                                                
 * \n\n \ref ls_rexecve                                                                                                       
 * \n Remote execve                                                                                                               
 * \n\n \ref ls_rtask                                                                                                          
 * \n Start a remote task                                                                                                         
 * \n\n \ref ls_rtaske                                                                                                        
 * \n Start a remote task with a new environment                                                                                  
 * \n\n \ref ls_stdinmode                                                                                                     
 * \n Assign stdin to local or remote tasks                                                                                       
 * \n\n \ref ls_getstdin                                                                                                      
 * \n List the remote task IDs that receive (or do not receive) standard input                                                    
 * \n\n \ref ls_setstdin                                                                                                      
 * \n Specify how stdin is assigned to remote tasks.                                                                              
 * \n\n \ref ls_rwait                                                                                                       
 * \n Wait for a remote task to exit                                                                                              
 * \n\n \ref ls_chdir                                                                                                       
 * \n Change the remote current working directory                                                                                 
 * \n\n \ref ls_rsetenv                                                                                                        
 * \n Set environment on remote host                                                                                              
 * \n\n \ref ls_rkill                                                                                                          
 * \n Kill a remote task                                                                                                          
 * \n\n \ref ls_donerex                                                                                                       
 * \n Restore terminal settings after remote execution                                                                            
 * \n\n \ref ls_fdbusy                                                                                                         
 * \n Test if a specified file descriptor is in use or reserved by LSF                                                            
 * \n\n \ref ls_stoprex                                                                                                        
 * \n Stop the network I/O server                                                                                                 
 * \n\n \ref ls_conntaskport                                                                                                   
 * \n Connect to the remote task port.                                                                                            
 *                                                                                                                                
 * <b>Equivalent line commands:</b>                                                                                             
 * \par                                                                                                                           
 * none                                                                                                                           
 *                                                                                                                                
 * \b Files:                                                                                                                      
 * \par                                                                                                                           
 * none                                                                                                                           
 *                                                                                                                                
 * @see                                                                                                                           
 * none                                                                                                                           
 *                                                                                                                                
 * \b LIMITATIONS:                                                                                                                 
 * \par                                                                                                                           
 * Although the level of transparency for remote execution in LSF is high, minor                                                  
 * parts of the UNIX execution environment are not propagated to remote hosts.                                                    
 * One such example is the UNIX process group.                                                                                                                                                                                                                   
 */                                                                                                                              

extern int	ls_rescontrol P_((char *host, int opcode, int options));

/*lib.reslog.c*/
/* routines related to lsf.acct file operation */

/**
 * \page ls_getacctrec ls_getacctrec
 * \brief Reads the next record from the opened log file created by RES. 
 * 
 * \ref ls_getacctrec reads the next record from the opened log file created by RES 
 * (see lsf.acct). It returns a pointer to the lsfAcctRec structure. 
 * 
 * Memory for the lsfAcctRec structure is statically allocated and will be 
 * overwritten by the next \ref ls_getacctrec call.
 * 
 * <b>
 * \#include <stdio.h>
 * \n\#include <lsf/lsf.h> 
 * 
 * struct lsfAcctRec *ls_getacctrec(FILE *fp, int *lineNum)
 * struct lsfAcctRec_ext *ls_getacctrec_ext(FILE *fp, int *lineNum)
 * </b>
 *
 * @param *fp File pointer of lsf.acct file.
 * @param *lineNum Line number of lsf.acct file.
 * 
 * <b>Data Structures:</b>
 * \par
 * \ref lsfAcctRec
 * \n \ref lsfRusage
 * \n \ref hRusage 
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return  struct lsfAcctRec *: A pointer to an lsfAcctRec record.
 * \n Function was successful. 
 * 
 * @return NULL 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none 
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see
 * none 
 */
 
extern struct lsfAcctRec *ls_getacctrec P_((FILE *, int *));
extern struct lsfAcctRec_ext *ls_getacctrec_ext P_((FILE *, int *));

extern int ls_putacctrec P_((FILE *, struct lsfAcctRec *));
extern int ls_putacctrec_ext P_((FILE *, struct lsfAcctRec_ext *));

/* end of routines related to lsf.acct file operation */

extern struct resLogRecord *ls_readrexlog P_((FILE *));

/*lib.rex.c*/

/**
 * \page ls_rexecv ls_rexecv
 * \brief Executes a program on a specified remote host. 
 * 
 * This routine is for executing remote tasks. It is modeled after the UNIX fork 
 * and execv system calls. 
 * 
 * \ref ls_rexecv executes a program on the specified remote host. The program name 
 * is given in argv[0] and the arguments are listed in argv. This routine is 
 * basically a remote execv. If a connection with the Remote Execution Server(RES)
 * on host has not been set up previously, \ref ls_connect is invoked to automatically
 * establish the connection. The remote execution environment is set up to be 
 * exactly the same as the local one and is cached by the remote RES server. 
 * 
 * The caller of this routine is typically a child process which terminates when 
 * the remote task is over. This routine does not return if successful. It returns
 *  -1 on failure. 
 * 
 * Any program using this routine must call  \ref ls_initrex first. 
 * 
 * Any program using these routines must be setuid to root if LSF_AUTH is not 
 * defined in the lsf.conf file. 
 * 
 * The remote file operations make use of a Remote File 
 * Server on the remote host. When this RFS shuts down, its status will be reported
 * to its client. The client should ignore this status. 
 * 
 * <b>
 * \#include <lsf/lsf.h> 
 *
 * int ls_rexecv(char *host, char **argv, int options) 
 * </b>
 *
 * @param *host The remote host where the program is executed. 
 * @param **argv The program being used. 
 * @param options \ref options_rexec
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref options_rexec
 * 
 * @return None 
 * \n Function was successful. 
 * 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 * error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rexecve  
 * @see \ref ls_rtask  
 * @see \ref ls_rtaske  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_chdir  
 * @see \ref ls_conntaskport  
 * @see \ref ls_initrex  
 */

extern int     ls_rexecv P_((char *, char **, int));

/**
 * \page ls_rexecve ls_rexecve
 * \brief Executes a program on a specified remote host. 
 * 
 * This routine is for executing remote tasks. It is modeled after the UNIX fork 
 * and execv system calls. 
 * 
 * \ref ls_rexecve executes a program on the specified remote host. The program name 
 * is given in argv[0] and the arguments are listed in argv. This routine is 
 * basically a remote execv. If a connection with the Remote Execution Server (RES)
 *  on host has not been set up previously, \ref ls_connect is invoked to automatically
 *  establish the connection. The remote execution environment is set up to be 
 * exactly the same as the local one and is cached by the remote RES server. 
 * 
 * \ref ls_rexecve is the same as \ref ls_rexecv except that it provides the support of 
 * setting up a new environment specified by the string array **envp. When envp 
 * is a NULL pointer, it means using the remote RES server's cached environment, 
 * otherwise using the new one. A minimal default environment (HOME, SHELL, USER, 
 * and PATH) is initially cached when a remote execution connection is established 
 * and the cached environment is updated whenever the remote execution environment 
 * is changed by \ref ls_rsetenv or any of the routines on this man page. 
 * 
 * The caller of this routine is typically a child process which terminates when 
 * the remote task is over. This routine does not return if successful. It 
 * returns -1 on failure. 
 * 
 * Any program using this routine must call \ref ls_initrex first. 
 * 
 * Any program using these routines must be setuid to root if LSF_AUTH is not 
 * defined in the lsf.conf file. 
 * 
 * The remote file operations make use of a Remote File 
 * Server on the remote host. When this RFS shuts down, its status will be reported
 *  to its client. The client should ignore this status. 
 * 
 * <b>
 * \#include <lsf/lsf.h> 
 * 
 * int ls_rexecve(char *host, char **argv, int options, char **envp) 
 * </b> 
 *
 * @param *host The remote host where the program is executed. 
 * @param **argv The program being used. 
 * @param options \ref options_rexec
 * @param **envp Environment pointer which is used for set up remote environment.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref options_rexec
 * 
 * @return None 
 * \n Function was successful. 
 * 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to 
 * detect error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rexecv  
 * @see \ref ls_rtask  
 * @see \ref ls_rtaske  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_chdir  
 * @see \ref ls_conntaskport  
 * @see \ref ls_rsetenv  
 * @see \ref ls_initrex  
 * 
 */

extern int     ls_rexecve P_((char *, char **, int, char **));

extern int     ls_rexecv2 P_((char *, char **, int));
extern SOCKET  ls_startserver P_((char *, char **, int));

/*lib.rtask.c*/

/**
 * \page ls_rtask ls_rtask
 * \brief Starts a remote task on a specified host. 
 * 
 * This routine is for executing remote tasks. It is modeled after the UNIX fork 
 * and execv system calls. 
 * 
 * \ref ls_rtask starts a remote task on the specified host. This routine is basically
 *  a remote fork followed by an execv. The arguments are identical to those of 
 * \ref ls_rexecv. \ref ls_rtask is typically used by a parallel application to execute 
 * multiple remote tasks efficiently. When a remote task finishes, a SIGUSR1 signal 
 * is delivered back to the application, and its status can be collected by calling 
 * \ref ls_rwait or \ref ls_rwaittid. \ref ls_rtask returns a unique task ID to be used by 
 * the application to differentiate outstanding remote tasks. It returns -1 on 
 * failure. 
 * 
 * Any program using this routine must call \ref ls_initrex first. 
 * 
 * Any program using these routines must be setuid to root if LSF_AUTH is not 
 * defined in the lsf.conf file. 
 * 
 * The remote file operations make use of a Remote File 
 * Server on the remote host. When this RFS shuts down, its status will be reported
 *  to its client. The client should ignore this status. 
 * 
 * <b>
 * \#include <lsf/lsf.h> 
 * 
 * int ls_rtask(char *host, char **argv, int options) 
 * </b> 
 *
 * @param *host The remote host where the program is executed. 
 * @param **argv The program being used. 
 * @param options \ref options_rexec
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref options_rexec
 * 
 * @return int:Unique TaskID 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 *  error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rexecv  
 * @see \ref ls_rexecve  
 * @see \ref ls_rtaske  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_chdir  
 * @see \ref ls_conntaskport  
 * @see \ref ls_rsetenv  
 * @see \ref ls_initrex  
 */
 
extern int     ls_rtask P_((char *, char **, int));

/**
 * \page ls_rtaske ls_rtaske
 * \brief Starts a remote task on a specified host. 
 * 
 * This routine is for executing remote tasks. It is modeled after the UNIX fork 
 * and execv system calls. 
 *
 * \ref ls_rtaske starts a remote task on the specified host. This routine is basically 
 * a remote fork followed by an execv. The arguments are identical to those of 
 * \ref ls_rexecv. \ref ls_rtask is typically used by a parallel application to execute 
 * multiple remote tasks efficiently. When a remote task finishes, a SIGUSR1 signal 
 * is delivered back to the application, and its status can be collected by calling 
 * \ref ls_rwait or \ref ls_rwaittid. \ref ls_rtask returns a unique task ID to be used by the 
 * application to differentiate outstanding remote tasks. It returns -1 on failure. 
 * 
 * \ref ls_rtaske is the same as \ref ls_rtask except that it provides the support of setting 
 * up a new environment specified by the string array **envp. When envp is a NULL 
 * pointer, it means using the remote RES server's cached environment, otherwise using 
 * the new one. A minimal default environment (HOME, SHELL, USER, and PATH) is initially 
 * cached when a remote execution connection is established and the cached environment 
 * is updated whenever the remote execution environment is changed by ls_rsetenv or 
 * any of the routines on this man page. 
 * 
 * Any program using this routine must call ls_initrex first. 
 * 
 * Any program using these routines must be setuid to root if LSF_AUTH is not 
 * defined in the lsf.conf file. 
 * 
 * The remote file operations make use of a Remote File 
 * Server on the remote host. When this RFS shuts down, its status will be reported
 *  to its client. The client should ignore this status. 
 * 
 * <b>
 * \#include <lsf/lsf.h> 
 * 
 * int ls_rtaske(char *host, char **argv, int options, char **envp)  
 * </b> 
 *
 * @param *host The remote host where the program is executed. 
 * @param **argv The program being used. 
 * @param options \ref options_rexec
 * @param **envp Environment pointer which is used for set up remote environment.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref options_rexec
 * 
 * @return int:Unique TaskID 
 * \n Function was successful. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * Systems that conform to the Single UNIX specification are not required to detect
 *  error conditions for this function.  
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rexecv  
 * @see \ref ls_rexecve  
 * @see \ref ls_rtask  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_chdir   
 * @see \ref ls_conntaskport  
 * @see \ref ls_rsetenv  
 * @see \ref ls_initrex  
 */

extern int     ls_rtaske P_((char *, char **, int, char **));
extern int     ls_rtask2 P_((char *, char **, int, char **));

/*lib.rwait.c*/

/**
 * \page ls_rwait ls_rwait
 * \brief Collects the status of a remote task started by \ref ls_rtask or \ref ls_rtaske. 
 * 
 * \ref ls_rwait collects the status of a remote child (task) that has been started 
 * by \ref ls_rtask or \ref ls_rtaske. This call is similar to the UNIX wait3() call, 
 * except that the child is located on a remote host. 
 *
 * If a remote child's status is successfully obtained, then the remote task ID 
 * (which is returned by an earlier \ref ls_rtask or \ref ls_rtaske call) is returned. 
 * Also, if status is not NULL, the status of the exited child is stored in the 
 * structure pointed to by status. If ru is not NULL, and the remote child's machine 
 * supports the rusage structure in its wait3() call, the resource usage information 
 * of the exited child is stored in the structure pointed to by ru. Only the ru_utime 
 * and ru_stime fields are set in the structure if the remote child's machine does not 
 * support the rusage structure in the wait3() call. If the remote child is run on a 
 * different platform than the parent, then only the fields in the resource structure 
 * that are common between the two platforms are filled in (the rusage structure is not 
 * identical across all platforms). If the child runs on a 64-bit machine, and the 
 * parent runs on a 32-bit machine, each of the values in the rusage structure that 
 * will overflow on a 32-bit machine are set to LONG_MAX. 
 * 
 * The \ref ls_rwait call are automatically restarted when the parent receives a signal 
 * while awaiting termination of a remote child process, unless the SV_INTERRUPT bit 
 * has been set for the signal (see sigaction()). 
 * 
 * Use the blocking mode of \ref ls_rwait with care. If there are both local and remote 
 * children, \ref ls_rwait take care only of remote children; none of them will return 
 * even though a local child has exited. In such cases, you can call wait(), \ref ls_rwait
 * and/or \ref ls_rwaittid via signal handlers (for SIGCHLD and SIGUSR1, respectively) to 
 * process local and remote children. 
 * 
 * When a remote child terminates, SIGUSR1 is sent to the parent process. Thus, 
 * \ref ls_rwait is typically called from inside the SIGUSR1 signal handler of the parent 
 * process. 
 * 
 * Any program using these routines must call \ref ls_initrex first. 
 * 
 * Any program using these routines must be setuid to root if LSF_AUTH is not defined 
 * in the lsf.conf file. 
 * 
 * The remote file operations make use of a Remote File Server 
 * on the remote host. When this RFS shuts down, its status will be reported to its 
 * client. The client should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n\#include <sys/wait.h>
 * \n\#include <sys/time.h>
 * \n\#include <sys/resource.h>
 * \n\#include <lsf/lsf.h>
 * 
 * int ls_rwait(LS_WAIT_T *status, int options, struct rusage *ru)  
 * </b> 
 *
 * @param *status  \ref return_values_waittid
 * @param options If specified as 0, and there is at least one remote child, 
 * then the calling host is blocked until a remote child exits. If options is 
 * specified to be WNOHANG, the routine checks for any exited (remote) child and 
 * returns immediately. The options parameter may be extended to provide more 
 * options in the future.  
 * @param *ru The structure where the resource usage information of the exited 
 * child is stored. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none 
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref return_values_waittid
 * 
 * @return char:remote task ID \n The function was successful. 
 * @return int:-1 \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rwaittid  
 * @see \ref ls_rtask  
 * @see \ref ls_rtaske  
 */

extern int     ls_rwait P_((LS_WAIT_T *, int, struct rusage *));

/**
 * \page ls_rwaittid ls_rwaittid
 * \brief Provides support for collecting the status of a specified remote task.  
 * 
 * \ref ls_rwaittid is modelled after the UNIX waitpid() system call. It provides 
 * support for collecting the status of the remote task whose task ID is tid. 
 *
 * If a remote child's status is successfully obtained, then the remote task ID 
 * (which is returned by an earlier \ref ls_rtask or \ref ls_rtaske call) is returned. 
 * Also, if status is not NULL, the status of the exited child is stored in the 
 * structure pointed to by status. If ru is not NULL, and the remote child's 
 * machine supports the rusage structure in its wait3() call, the resource usage 
 * information of the exited child is stored in the structure pointed to by ru. 
 * Only the ru_utime and ru_stime fields are set in the structure if the remote 
 * child's machine does not support the rusage structure in the wait3() call. 
 * If the remote child is run on a different platform than the parent, then only 
 * the fields in the resource structure that are common between the two platforms 
 * are filled in (the rusage structure is not identical across all platforms). 
 * If the child runs on a 64-bit machine, and the parent runs on a 32-bit machine, 
 * each of the values in the rusage structure that will overflow on a 32-bit 
 * machine are set to LONG_MAX. 
 * 
 * \ref ls_rwaittid behaves identically to \ref ls_rwait if tid has a value of zero. 
 * If tid is less than 0, it returns -1 and sets lserrno to LSE_BAD_ARGS 
 * 
 * Use the blocking mode of \ref ls_rwaittid with care. If there are both local 
 * and remote children, \ref ls_rwaittid take care only of remote children; none 
 * of them will return even though a local child has exited. In such cases, you 
 * can call wait(), \ref ls_rwait and/or \ref ls_rwaittid via signal handlers (for 
 * SIGCHLD and SIGUSR1, respectively) to process local and remote children. 
 *  
 * 
 * Any program using these routines must call \ref ls_initrex first. 
 * 
 * Any program using these routines must be setuid to root if LSF_AUTH is not defined 
 * in the lsf.conf file. 
 * 
 * The remote file operations make use of a Remote File Server 
 * on the remote host. When this RFS shuts down, its status will be reported to its 
 * client. The client should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n\#include <sys/wait.h>
 * \n\#include <sys/time.h>
 * \n\#include <sys/resource.h>
 * \n\#include <lsf/lsf.h>
 * 
 * int ls_rwaittid(int tid, LS_WAIT_T *status, int options, struct rusage *ru)  
 * </b> 
 *
 * @param tid The ID of the remote task being accessed.
 * @param *status  \ref return_values_waittid
 * @param options If options is set to 0, and there is at least one remote task, 
 * the calling host is blocked until the specific task exits. If options is WNOHANG(non-blocking), 
 * it reads the child's status if the child is dead, otherwise it returns immediately with 0. 
 * If the status of the child is successfully read, the remote task ID is returned.
 * @param *ru The structure where the resource usage information of the exited 
 * child is stored. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none 
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref return_values_waittid
 * 
 * @return char:remote task ID \n The function was successful. 
 * @return int:-1 \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. If tid is less 
 * than 0, it returns -1 and sets lserrno to LSE_BAD_ARGS
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * 
 * @see \ref ls_rwait  
 * @see \ref ls_rtask  
 * @see \ref ls_rtaske  
 */

extern int     ls_rwaittid P_((int, LS_WAIT_T *, int, struct rusage *));

/*lib.rf.c*/

/**
 * \page ls_conntaskport ls_conntaskport
 * \brief Connects a socket to a remote task port.
 * 
 * \ref ls_conntaskport connects a socket to the task port that was created by the
 * remote RES for the remote task tid returned by an \ref ls_rtask or \ref ls_rtaske
 * call. You must have started the remote task with the REX_TASKPORT flag (see
 * \ref ls_rtask).
 *  
 * Any program using this routine must call \ref ls_initrex first.
 *  
 * Any program using this API must be setuid to root if LSF_AUTH is not defined in
 * the lsf.conf file.
 *
 * <b>
 * \#include <lsf/lsf.h>
 * 
 * int ls_conntaskport(int tid)
 * </b>
 *
 * @param tid  The ID of the remote task being accessed.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:A connected socket. 
 * \n Function was successful
 * @return int:-1 
 * \n Function failed
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref ls_initrex 
 */

extern SOCKET  ls_conntaskport P_((int tid));   

/**
 * \page ls_ropen ls_ropen
 * \brief Opens a file on a remote host. 
 * 
 * This routine performs operations on files located on remote hosts. This call 
 * corresponds to the UNIX open() system calls. 
 * 
 * \ref ls_ropen opens the named file located on the remote host. A remote file 
 * descriptor, rfd, is returned on success. You can use this descriptor in 
 * subsequent remote file calls as an argument. 
 * 
 * Either the RES must be running at the remote host to service any remote file 
 * operation or rcp() be available. 
 * 
 * \ref ls_initrex must be called before calling any remote file operation. 
 * 
 * This remote file operation makes use of a Remote File Server on the remote host.
 *  When this RFS shuts down, its status will be reported to its client. The client
 *  should ignore this status. 
 *      
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h> 
 *
 * int ls_ropen(char *host, char *path, int flags, int mode) 
 * </b>
 * 
 * @param *host The host where the file to be opened is located. 
 * @param *path The path name to the file to be opened. 
 * @param flags Options for openning the file on the remote host.
 * @param mode  File access permission related options.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int: 0
 * \n Response is the same from its UNIX counterpart. 
 * 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_rread  
 * @see \ref ls_rwrite  
 * @see \ref ls_rlseek  
 * @see \ref ls_rclose  
 * @see \ref ls_rstat  
 * @see \ref ls_rfstat  
 * @see \ref ls_rgetmnthost  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_initrex  
 */

extern int ls_ropen P_((char *host, char *fn, int flags, int mode));

/**
* \page ls_rclose ls_rclose
* \brief Performs a close operation on a file on a remote host.
* 
* This routine performs operations on files located on remote hosts. This call 
* corresponds to the UNIX close() system calls. 
* 
* \ref ls_rclose performs a close operation on an opened file that is referenced 
* by rfd. The file has been previously opened using \ref ls_ropen. 
* 
* Either the RES must be running at the remote host to service any remote file 
* operation or rcp() be available. 
* 
* \ref ls_initrex must be called before calling any remote file operation. 
* 
* This remote file operation makes use of a Remote File Server on the remote 
* host. When this RFS shuts down, its status will be reported to its client. 
* The client should ignore this status. 
* 
* <b>
* \#include <sys/types.h>
* \n\#include <unistd.h>
* \n\#include <sys/stat.h>
* \n\#include <fcntl.h>
* \n\#include <limits.h>
* \n\#include <lsf/lsf.h> 
*  
* int ls_rclose(int rfd)
* </b>
* 
* @param rfd 
* References the file that is to be closed. 
* 
* <b>Data Structures:</b>
* \par
* none
* 
* <b>Define Statements:</b>
* \par
* none
* 
* @return int:0 
* \n Response is the same from its UNIX counterpart. 
* 
* @return int:-1 
* \n Function failed. 
* 
* \b Errors:
* \par
* If the function fails, lserrno is set to indicate the error. 
* 
* <b>Equivalent line commands:</b>
* \par
* none 
* 
* \b Files:
* \par
* none
* 
* @see \ref ls_ropen  
* @see \ref ls_rread  
* @see \ref ls_rwrite  
* @see \ref ls_rlseek  
* @see \ref ls_rstat  
* @see \ref ls_rfstat  
* @see \ref ls_rgetmnthost  
* @see \ref ls_rfcontrol  
* @see \ref ls_initrex  
*/

extern int ls_rclose P_((int rfd));

/**
 * \page ls_rwrite ls_rwrite
 * \brief Performs a write operation on a file on a remote host. 
 * 
 * This routine performs operations on files located on remote hosts. This call 
 * corresponds to the UNIX write() system calls. 
 * 
 * \ref ls_rwrite performs a read operation on an opened file that is referenced 
 * by rfd. The file has been previously opened using \ref ls_ropen.   
 * 
 * Either the RES must be running at the remote host to service any remote file 
 * operation or rcp() be available.
 * 
 * \ref ls_initrex must be called before calling any remote file operation. 
 * 
 * This remote file operation makes use of a Remote File Server on the remote 
 * host. When this RFS shuts down, its status will be reported to its client. 
 * The client should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n\#include <unistd.h>
 * \n\#include <sys/stat.h>
 * \n\#include <fcntl.h>
 * \n\#include <limits.h>
 * \n\#include <lsf/lsf.h> 
 *  
 * int ls_rwrite(int rfd, char *buf, int len)
 * </b>
 * 
 * @param rfd 
 * References the file that is to be written. 
 * @param *buf Write buffer.
 * @param len  Buffer length.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int: 0
 * \n Response is the same from its UNIX counterpart. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none 
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_ropen  
 * @see \ref ls_rread  
 * @see \ref ls_rlseek  
 * @see \ref ls_rclose  
 * @see \ref ls_rstat  
 * @see \ref ls_rfstat  
 * @see \ref ls_rgetmnthost  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_initrex  
 */

extern int ls_rwrite P_((int rfd, char *buf, int len));

/**
 * \page ls_rread ls_rread
 * \brief Performs a read operation on a file on a remote host. 
 * 
 * This routine performs operations on files located on remote hosts. This call 
 * corresponds to the UNIX read() system calls. 
 * 
 * \ref ls_rread performs a read operation on an opened file that is referenced by 
 * rfd. The file has been previously opened using \ref ls_ropen. 
 * 
 * Either the RES must be running at the remote host to service any remote file 
 * operation or rcp() be available. 
 * 
 * \ref ls_initrex must be called before calling any remote file operation. 
 * 
 * This remote file operation makes use of a Remote File Server on the remote 
 * host. When this RFS shuts down, its status will be reported to its client. The 
 * client should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h> 
 * 
 * int ls_rread(int rfd, char *buf, int len)
 * </b> 
 *
 * @param rfd References the file that is to be read. 
 * @param *buf Read Buffer.
 * @param len  Buffer length.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int: 0
 * \n Response is the same from its UNIX counterpart. 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_ropen  
 * @see \ref ls_rwrite  
 * @see \ref ls_rlseek  
 * @see \ref ls_rclose  
 * @see \ref ls_rstat  
 * @see \ref ls_rfstat  
 * @see \ref ls_rgetmnthost  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_initrex  
 */

extern int ls_rread P_((int rfd, char *buf, int len));

/**
 * \page ls_rlseek ls_rlseek
 * \brief Performs a seek operation on a file on a remote host. 
 * 
 * This routine performs operations on files located on remote hosts. This call 
 * corresponds to the UNIX lseek() system calls. 
 * 
 * \ref ls_rlseek performs a read operation on an opened file that is referenced by 
 * rfd. The file has been previously opened using \ref ls_ropen. 
 * 
 * Either the RES must be running at the remote host to service any remote file 
 * operation or rcp() be available. 
 * 
 * \ref ls_initrex must be called before calling any remote file operation. 
 * 
 * This remote file operation makes use of a Remote File Server on the remote 
 * host. When this RFS shuts down, its status will be reported to its client. 
 * The client should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h> 
 *
 * off_t ls_rlseek(int rfd, off_t offset, int whence) 
 * </b>
 * 
 * @param rfd References the file that is to be sought. 
 * @param offset Offset of the file.
 * @param whence Whence.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int: 0
 * \n Response is the same from its UNIX counterpart. 
 * @return int:-1
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_ropen  
 * @see \ref ls_rread  
 * @see \ref ls_rwrite  
 * @see \ref ls_rclose  
 * @see \ref ls_rstat  
 * @see \ref ls_rfstat  
 * @see \ref ls_rgetmnthost  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_initrex  
 */

extern off_t ls_rlseek P_((int rfd, off_t offset, int whence));


extern int ls_runlink P_((char *host, char *fn));

/**
 * \page ls_rfstat ls_rfstat
 * \brief Obtains information about a file located on a remote host. 
 * 
 * This routine performs operations on files located on remote hosts. This call 
 * corresponds to the UNIX fstat() system calls. 
 * 
 * \ref ls_rfstat obtains information about a file located on the remote host. Because
 *  different platforms have different fields in the stat structure, only the 
 * following fields are updated: st_dev, st_ino, st_mode, st_nlink, st_uid, st_gid,
 *  st_rdev, st_size, st_atime, st_mtime, and st_ctime. 
 * 
 * Either the RES must be running at the remote host to service any remote file 
 * operation or rcp() be available. 
 * 
 * \ref ls_initrex must be called before calling any remote file operation. 
 * 
 * This remote file operation makes use of a Remote File Server on the remote host.
 *  When this RFS shuts down, its status will be reported to its client. The client
 *  should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h> 
 * 
 * int ls_rfstat(int rfd, struct stat *buf) 
 * </b> 
 *
 * @param rfd References the file that is to be accessed. 
 * @param *buf Pointer of the stat Structure which will be filled by the performance of
 * fstat() on the remote system..
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int: 0
 * \n Response is the same from its UNIX counterpart. 
 * 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_ropen  
 * @see \ref ls_rread  
 * @see \ref ls_rwrite  
 * @see \ref ls_rlseek  
 * @see \ref ls_rclose  
 * @see \ref ls_rstat  
 * @see \ref ls_rgetmnthost  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_initrex  
 */

extern int ls_rfstat P_((int rfd, struct stat *buf));

/**
 * \page ls_rstat ls_rstat
 * \brief Obtains information about a file located on a remote host. 
 * 
 * This routine performs operations on files located on remote hosts. This call 
 * corresponds to the UNIX stat() system calls. 
 * 
 * \ref ls_rstat obtains information about a file located on the remote host. Because
 *  different platforms have different fields in the stat structure, only the 
 * following fields are updated: st_dev, st_ino, st_mode, st_nlink, st_uid, st_gid, 
 * st_rdev, st_size, st_atime, st_mtime, and st_ctime. 
 * 
 * Either the RES must be running at the remote host to service any remote file 
 * operation or rcp() be available. 
 * 
 * \ref ls_initrex must be called before calling any remote file operation. 
 * 
 * This remote file operation makes use of a Remote File Server on the remote host. 
 * When this RFS shuts down, its status will be reported to its client. The client 
 * should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h> 
 *
 * int ls_rstat(char *host, char *path, struct stat *buf) 
 * </b> 
 *
 * @param *host The remote host containing the file to be analyzed. 
 * @param *path  The path name to the file to be opened.
 * @param *buf  Pointer of the stat Structure which will be filled by the performance of
 * stat() on the remote system.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return int: 0
 * \n Response is the same from its UNIX counterpart. 
 * 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_ropen  
 * @see \ref ls_rread  
 * @see \ref ls_rwrite  
 * @see \ref ls_rlseek  
 * @see \ref ls_rclose  
 * @see \ref ls_rfstat  
 * @see \ref ls_rgetmnthost  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_initrex  
 */
extern int ls_rstat P_((char *host, char *fn, struct stat *buf)); 

/**
 * \page ls_rgetmnthost ls_rgetmnthost
 * \brief Obtains the name of the file server that exports a specified file system. 
 * 
 * This routine performs operations on files located on remote hosts. 
 * 
 * \ref ls_rgetmnthost obtains the name of the file server that exports the file system
 *  mounted on host that contains file, where file is a relative or absolute path 
 * name. If host is NULL, the local host name is assumed. This call corresponds to 
 * the \ref ls_getmnthost call. 
 * 
 * Either the RES must be running at the remote host to service any remote file 
 * operation or rcp() be available. 
 * 
 * \ref ls_initrex must be called before calling any remote file operation. 
 * 
 * This remote file operation makes use of a Remote File Server on the remote host. 
 * When this RFS shuts down, its status will be reported to its client. The client 
 * should ignore this status. 
 * 
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h> 
 * 
 * char *ls_rgetmnthost(char *host, char *file) 
 * </b> 
 *
 * @param *host The host containing the file. 
 * @param *file The file to be accessed. 
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return char *:Hostname 
 * \n The function was successful. 
 * @return NULL 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_ropen  
 * @see \ref ls_rread  
 * @see \ref ls_rwrite  
 * @see \ref ls_rlseek  
 * @see \ref ls_rclose  
 * @see \ref ls_rstat  
 * @see \ref ls_rfstat  
 * @see \ref ls_rfcontrol  
 * @see \ref ls_initrex  
 */
 
extern char *ls_rgetmnthost P_((char *host, char *fn));

/**
* \page ls_rfcontrol ls_rfcontrol
* \brief Controls the behavior of remote file operations.  
* 
* This routine performs operations on files located on remote hosts. 
* 
* \ref ls_rfcontrol controls the behavior of remote file operations. Possible 
* commands are: \ref commands_ls_rfcontrol
* 
* Either the RES must be running at the remote host to service any remote file 
* operation or rcp() must be available. 
* 
* \ref ls_initrex must be called before calling any remote file operation. 
* 
* This remote file operation makes use of a Remote File Server on the remote host. 
* When this RFS shuts down, its status will be reported to its client. The client 
* should ignore this status. 
* 
* <b>
* \#include <sys/types.h>
* \n \#include <unistd.h>
* \n \#include <sys/stat.h>
* \n \#include <fcntl.h>
* \n \#include <limits.h>
* \n \#include <lsf/lsf.h> 
* 
* int ls_rfcontrol(int command, int arg) 
* </b> 
*
* @param command \ref commands_ls_rfcontrol
* @param arg A pointer to the remote host name cast to int. 
* 
* <b>Data Structures:</b>
* \par
* none
* 
* <b>Define Statements:</b>
* \par
* \ref commands_ls_rfcontrol
* 
* @return int:0 
* \n The function was successful. 
* 
* @return int:-1 
* \n Function failed. 
* 
* \b Errors:
* \par
* If the function fails, lserrno is set to indicate the error. 
* 
* <b>Equivalent line commands:</b>
* \par
* none
* 
* \b Files:
* \par
* none
* 
* @see \ref ls_ropen  
* @see \ref ls_rread  
* @see \ref ls_rwrite  
* @see \ref ls_rlseek  
* @see \ref ls_rclose  
* @see \ref ls_rstat  
* @see \ref ls_rfstat  
* @see \ref ls_rgetmnthost  
* @see \ref ls_initrex  
*/

extern int ls_rfcontrol P_((int command, int arg));

/**
 * \page ls_rfterminate ls_rfterminate
 * \brief Closes connection to the RES's file server process.   
 * 
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h> 
 * 
 * int ls_rfterminate(char *host) 
 * </b> 
 *
 * @param *host The host names.
 * 
 * <b>Data Structures:</b>
 * \par
 * none
 * 
 * <b>Define Statements:</b>
 * \par
 * none 
 *
 * @return int:0 
 * \n The function was successful. 
 * 
 * @return int:-1 
 * \n Function failed. 
 * 
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error. 
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * none
 * 
 * \b Files:
 * \par
 * none
 * 
 * @see \ref ls_ropen  
 * @see \ref ls_rread  
 * @see \ref ls_rwrite  
 * @see \ref ls_rlseek  
 * @see \ref ls_rclose  
 * @see \ref ls_rstat  
 * @see \ref ls_rfstat  
 * @see \ref ls_rgetmnthost  
 * @see \ref ls_initrex  
 */
 
extern int ls_rfterminate P_((char *host));


/**
 * \page ls_createdir ls_createdir
 * \brief creates a directory on a remote host.
 *
 * This routine creates a directory on the remote host.
 *
 * \ref ls_createdir creates the named directory path on the remote host.
 *
 * RES must be running on the remote host.
 *
 * \ref ls_initrex must be called before calling this function.
 *
 *
 * <b>
 * \#include <sys/types.h>
 * \n \#include <unistd.h>
 * \n \#include <sys/stat.h>
 * \n \#include <fcntl.h>
 * \n \#include <limits.h>
 * \n \#include <lsf/lsf.h>
 *
 * int ls_createdir(char *host, char *path, int userId)
 * </b>
 *
 * @param *host The host for the new directory.
 * @param *path The path name for the new directory. 
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int: 0
 * \n 0 indicates a successful creation.
 *
 * @return int:-1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * none
 *
 */

extern int ls_createdir P_((const char *host, const char *path));


#ifndef NON_ANSI
#ifndef _PAM_H_ /* defined in pam.h as well */

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

typedef int (*rmi_handler_func_t)(rmi_buffer_t *buffer);

typedef struct _rmi_handler {
    char *handler_name ;
    rmi_handler_func_t handler ;
    char *desc ;
} rmi_handler_t ;

#endif /* !_PAM_H_ */


#if defined(SOLARIS)
/* RES plugin functions */
extern int    res_vcl_version P_((void));
extern int    res_vcl_cluster_init P_((int ));
extern int    res_vcl_node_init P_((int *, char ***, char ***, 
				    rmi_handler_t **, size_t *));
extern int    res_vcl_pre_spawn P_((ls_task_params_t *));
extern int    res_vcl_pre_exec P_((ls_task_t *));
extern int    res_vcl_atexit P_((ls_task_t *));
extern int    res_vcl_node_finalize P_((void));

/* PAM plugin functions */
extern int   pam_vcl_init P_((int *, char ***, char ***, char **, int , 
                              ls_task_params_t **, int *, rmi_handler_t **, 
                              size_t *, int *));
extern int   pam_vcl_set_taskinfo P_((ls_task_t *));
extern int   pam_vcl_finalize P_((void));
extern int   pam_vcl_set_rusage P_((int , struct jRusage *));
#endif
#endif /* ifndef NON_ANSI */
#if defined(__cplusplus)
}
#endif
#endif
/**
 * \page lslib lslib
 * \brief Application Programming Interface (API) library routines for LSF services
 *
 * LSLIB routines allow application programs to contact the Load Information 
 * Manager (LIM) and Remote Execution Server (RES) daemons in order to obtain LSF 
 * services. These services include obtaining static system configuration information 
 * and dynamic load information for the hosts in distributed clusters, obtaining task 
 * placement advice from LIM, executing tasks (UNIX processes) on remote hosts with 
 * a high degree of transparency using RES, remote file operations across hosts that 
 * do not share file systems, performing remote terminal I/O and signal operations, 
 * and other related functions. You can build distributed applications on top of LSLIB 
 * to effectively exploit the resources available on the network, improving application 
 * performance and resource accessibility. 
 * 
 * \par
 * Library routines for obtaining load sharing cluster configuration information.
 * \li \ref  ls_info            
 * \li \ref ls_getclustername  
 * \li \ref ls_getmastername   
 * \li \ref ls_getmodelfactor 
 *  
 * \n\par   
 * Library routines for obtaining host configuration information. 
 * \li \ref ls_gethosttype  
 * \li \ref ls_gethostmodel  
 * \li \ref ls_gethostfactor  
 * \li \ref ls_gethostinfo  
 *
 * \n\par 
 * Library routines for obtaining host load information. An application can 
 * use this information to make task placement decisions instead of using LIM's. 
 * \li \ref ls_loadinfo  
 * \li \ref ls_load  
 * \li \ref ls_loadofhosts  
 * \li \ref ls_sharedresourceinfo  
 *
 * \n\par
 * Library routines implementing LIM's task placement policy. These routines 
 * include calls to obtain task placement information, and calls to adjust host 
 * load measures. 
 * \li \ref ls_placereq  
 * \li \ref ls_placeofhosts  
 * \li \ref ls_loadadj  
 * 
 * \n\par
 * Library routines for displaying and manipulating the local and remote task 
 * lists. These lists specify the eligibility of various types of tasks for remote 
 * execution, and their resource requirement characteristics. 
 * \li \ref ls_eligible  
 * \li \ref ls_listrtask  
 * \li \ref ls_listltask  
 * \li \ref ls_insertrtask  
 * \li \ref ls_insertltask  
 * \li \ref ls_deletertask  
 * \li \ref ls_deleteltask 
 *  
 * \n\par
 * <b>REMOTE EXECUTION</b>
 * \n\par
 * Library routines related to remote execution, including initiation, connection 
 * and remote environment manipulation. 
 * 
 * \n\par
 * Library routines for establishing and querying remote connections. 
 * \li \ref ls_connect  
 * \li \ref ls_isconnected  
 * \li \ref ls_findmyconnections  
 * 
 * \n\par
 * Library routines for executing remote tasks. 
 * \li \ref ls_rexecv  
 * \li \ref ls_rexecve  
 * \li \ref ls_rtask  
 * \li \ref ls_rtaske  
 *
 * \n\par
 * Library routines for querying and manipulating stdin for remote tasks. 
 * \li \ref ls_stdinmode  
 * \li \ref ls_getstdin  
 * \li \ref ls_setstdin  
 *
 * \n\par
 * Wait for a remote or local task, then return its status. 
 * \li \ref ls_rwait  
 * \li \ref ls_rwaittid  
 *
 * \n\par
 * Library routines for operations on files on remote hosts. 
 * \li \ref ls_ropen  
 * \li \ref ls_rread  
 * \li \ref ls_rwrite  
 * \li \ref ls_rlseek  
 * \li \ref ls_rclose  
 * \li \ref ls_rstat  
 * \li \ref ls_rfstat  
 * \li \ref ls_rgetmnthost  
 * \li \ref ls_rfcontrol  
 *
 * \n\par
 * Library routine for load sharing error messages.
 * \li \ref ls_perror  
 * \li \ref ls_sysmsg  
 * \li \ref ls_errlog  
 *
 * \n\par
 * Library routines for administering and controlling the LSF system.
 * \li \ref ls_limcontrol  
 * \li \ref ls_lockhost  
 * \li \ref ls_unlockhost      
 * 
 * \par 
 * \note 
 * \par 
 * All LSLIB routines require that the LSF header file <lsf/lsf.h> be included.
 * \par
 * Many LSLIB APIs return a pointer to an array or structure. These data 
 * structures are in static storage or on the heap. The next time the routine 
 * is called, the storage is overwritten or freed.                           
 * \par
 * Any program using LSLIB routines that change the state of the LSF system 
 * must be setuid to root if LSF_AUTH is not defined in the lsf.conf file.    
 * \par
 * Any program using LSLIB routines for remote execution services or for operations on
 * files on remote hosts must call ls_initrex() before calling any of the other routines.  
 * \par
 * On systems which have both System V and BSD programming interfaces, LSLIB 
 * typically requires the BSD programming interface. On System V-based versions 
 * of UNIX, for example SGI IRIX, it is normally necessary to link applications 
 * using LSLIB with the BSD compatibility library.
 * \par
 * On AFS systems, the following needs to be added to the end of your linkage 
 * specifications when linking with LSLIB (assuming your AFS library path is 
 * /usr/afsws):                                 
 * \par
 *  For HP-UX and Solaris,                     
 * \par
 *  -lc -L/usr/afsws/lib -L/usr/afsws/lib/afs -lsys -lrx -llwp /usr/afsws/lib/afs/util.a
 * \par
 *  For other platforms,                 
 * \par
 *  -lc -L/usr/afsws/lib -L/usr/afsws/lib/afs -lsys -lrx -llwp  
 *                                                          
 * <b>Files:</b> 
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 * \n $LSF_CONFDIR/lsf.shared
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name
 * \n $LSF_CONFDIR/lsf.task
 * \n $LSF_CONFDIR/lsf.task.cluster_name
 *
 * @see lslibapis
 */
