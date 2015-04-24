/* $RCSfile: lsbatch.h,v $Revision: 2.1303 $Date: 2013/02/21 06:26:27 $
 *-----------------------------------------------------------------
 * Copyright International Business Machines Corp,1992-2013.
 *
 * Lsbatch Distributed Batch Utility -- 
 *
 * Header file for all lsbatch components: applications, lsblib,
 *                                         mbatchd and sbatchd 
 *
 *------------------------------------------------------------------
 */

#ifndef LSBATCH_H
#define LSBATCH_H
#ifndef _LSF_H_
#include <lsf/lsf.h>
#endif /* _LSF_H_ */
#if defined(_M_XENIX) || defined(WIN32)
#ifndef MAXPATHLEN
#define MAXPATHLEN      1024
#endif
#endif /* _M_XENIX */
#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif
#define DEF_MAXJOBS_PER_PACK 0  /**< the default value when LSB_MAX_PACK_JOBS is not defined in lsf.conf or less than 0 */
/* if only everyone had <paths.h> */
#define _PATH_NULL      "/dev/null"

/**
* @internal
*/
#define SKIP_SPACES(word) \
    while ( word[0] == ' ' ) \
        word++;

#define FREEUP_ARRAY(num, vector) \
    if(vector != NULL) {\
        FREE_STRING_VECTOR_ENTRIES(num, vector); \
        FREEUP(vector);\
    }

    
/* event log version: 
 * each new major release requires to add a new line 
 */
#define  LSB_EVENT_VERSION3_0   3.0
#define  LSB_EVENT_VERSION3_1   3.1
#define  LSB_EVENT_VERSION3_2   3.2
#define  LSB_EVENT_VERSION4_0   4.0
#define  LSB_EVENT_VERSION4_1   4.1
#define  LSB_EVENT_VERSION4_2   4.2
#define  LSB_EVENT_VERSION5_0   5.0
#define  LSB_EVENT_VERSION5_1   5.1
#define  LSB_EVENT_VERSION6_0   6.0
#define  LSB_EVENT_VERSION6_1   6.1
#define  LSB_EVENT_VERSION6_2   6.2
#define  LSB_EVENT_VERSION7_0   7.0
#define  LSB_EVENT_VERSION7_0_1   7.01
#define  LSB_EVENT_VERSION7_0_2   7.02   
#define  LSB_EVENT_VERSION7_0_3   7.03
#define  LSB_EVENT_VERSION7_0_4   7.04
#define  LSB_EVENT_VERSION7_0_5   7.05
#define  LSB_EVENT_VERSION7_0_6   7.06
#define  LSB_EVENT_VERSION8_0   8.0 
#define  LSB_EVENT_VERSION8_0_2 8.02
#define  LSB_EVENT_VERSION8_3   8.3 
#define  LSB_EVENT_VERSION9_1   9.1
#define  LSB_EVENT_VERSION9_1_1 9.11

/* current event version number of the mbatchd */
#define  THIS_VERSION      "9.11"

#define  MAX_VERSION_LEN     12
#define  MAX_HPART_USERS     100  /* num of users per host partition */
#define  MAX_CHARLEN         20   /* max char limit, OS independent */
#define  MAX_LSB_NAME_LEN    60   /* the max length of name  */
#define  MAX_LSB_UG_NAME_LEN 512  /*the max length of user group*/
#define  MAX_LSB_HG_NAME_LEN 512  /*the max length of host group*/
#define  MAX_LSB_UG_HIERDEPTH 64  /*Maximum levels that a user group hierachy can have*/ 
#define  MAX_CMD_DESC_LEN    512  /* the max length of command */
#define  MAX_CALENDARS       256  /* for the local cluster */
#define  MAX_USER_EQUIVALENT 128  /* max num of user equivalent entries */
#define  MAX_USER_MAPPING    128  /* max num of user mapping entries */
#define  MAXDESCLEN          20 * 512 /* max external msg's description length */
#define  MAX_GROUPS          4096 /* num of user or host group */
#define  MAX_LSB_LP_LEN      2048 /*the max length of the lisenceproject*/
/*
 * RFC #725
 */
#define MAXFULLFILENAMELEN     4096 /* max len. of a filename */
#define MAXFULLPATHNAMELEN     2*MAXFULLFILENAMELEN
#define FILENAMEPADDING         128

#define  DEFAULT_MSG_DESC    "no description"

#undef MSGSIZE
#define MSGSIZE             4096 

/* RFC #725
 * extend the MSG size to 4*max filename len
 */
#define MAXFULLMSGSIZE         4*MAXFULLFILENAMELEN

/* host status (hStatus) bits */
/**
 * \addtogroup host_status host_status
 * The status of the host. It is the bitwise inclusive OR of some of the following:
 */
/*@{*/
#define HOST_STAT_OK              0x0    /**< Ready to accept and run jobs */
#define HOST_STAT_BUSY            0x01   /* Load is not good enough */
                                         /**< The host load is greater than a scheduling 
                                          *threshold. In this status, no new job will
 					  * be scheduled to run on this host. */																																																																				
#define HOST_STAT_WIND            0x02   /* Run windows are closed */
                                         /**< The host dispatch window is closed. 
                                          *In this status, no new job will be accepted.*/
#define HOST_STAT_DISABLED        0x04   /* Disabled by admin */
                                         /**< The host has been disabled by the LSF 
                                          *administrator and will not accept jobs. In this
                                          * status, no new job will be scheduled to 
                                          * run on this host.*/
#define HOST_STAT_LOCKED          0x08   /* Lim locked by admin */
                                         /**< The host is locked by a exclusive task.
                                          * In this status, no new job will be scheduled
                                          * to run on this host.*/
#define HOST_STAT_FULL            0x10   /**< Great than job limit */
       		 			 /**< The host has reached its job limit. 
       		 			  *In this status, no new job will be scheduled to run
       		  			  * on this host.*/
#define HOST_STAT_UNREACH         0x20   /**< The sbatchd on this host is unreachable. */
#define HOST_STAT_UNAVAIL         0x40   /**< The LIM and sbatchd on this host are unavailable. */
#define HOST_STAT_UNLICENSED      0x80   /**< The host does not have an LSF license. */
#define HOST_STAT_NO_LIM          0x100  /**< The host is running an sbatchd but not a LIM. */
#define HOST_STAT_EXCLUSIVE       0x200  /**< Running exclusive job */
#define HOST_STAT_LOCKED_MASTER   0x400  /**< Lim locked by master LIM */
#define HOST_STAT_REMOTE_DISABLED 0x800  /**< Close a remote lease host. This flag is 
					  * used together with HOST_STAT_DISABLED.*/
#define HOST_STAT_LEASE_INACTIVE  0x1000 /**< Close a remote lease host due to the 
					  * lease is renewing or terminating.*/

/* if LSF_HPC_EXTENTIONS="LSB_HCLOSE_BY_RES" is set in lsf.conf
 * host will be closed if RES is unavailable.
 */
#define HOST_STAT_DISABLED_RES 0x4000 /**< Host is disabled by RES */

/* Kite#29531 a bit set in hData->hStatus
 * to show whether the host is closed by
 * admin or closed because RMS is not available.
 */
#define HOST_STAT_DISABLED_RMS 0x8000 /**< Host is disabled by RMS */

/* lsf70 project scheduling, a removed host from mbatchd move into
 * a new status HOST_STAT_LOCKED_EGO
 */
#define HOST_STAT_LOCKED_EGO  0x10000    /**< The host is disabled by EGO */
#define HOST_CLOSED_BY_ADMIN  0x20000    /**< If none of the above hold, hStatus is set 
					  *to HOST_STAT_OK to indicate that the host
                                          * is ready to accept and run jobs.*/
#define HOST_STAT_CU_EXCLUSIVE    0x40000 /**< Running cu exclusive job */
#define HOST_STAT_EXPIRED         0x80000 /* lim report host as expired */

#define HOST_STAT_ACOK            0x100000 /**< Dynamic Cluster is ready for provision */

/* host is ok */
#define LSB_HOST_OK(status)          ((status & ~HOST_STAT_ACOK) == HOST_STAT_OK)

/* host is busy */
#define LSB_HOST_BUSY(status)        ((status & HOST_STAT_BUSY) != 0)

/* host is closed */
#define LSB_HOST_CLOSED(status)      ((status & (HOST_STAT_WIND | \
       		HOST_STAT_DISABLED |     \
       		HOST_STAT_LOCKED |       \
       		HOST_STAT_LOCKED_MASTER | \
       		HOST_STAT_FULL |         \
       		HOST_STAT_CU_EXCLUSIVE | \
       		HOST_STAT_EXCLUSIVE    | \
                                        HOST_STAT_LEASE_INACTIVE | \
       		HOST_STAT_NO_LIM)) != 0)

/* host is full */
#define LSB_HOST_FULL(status)        ((status & HOST_STAT_FULL) != 0)

/* host is unlicensed */
#define LSB_HOST_UNLICENSED(status)  ((status & HOST_STAT_UNLICENSED) != 0)

/* host is unreach */
#define LSB_HOST_UNREACH(status)     ((status & HOST_STAT_UNREACH) != 0)

/* host is unavail */
#define LSB_HOST_UNAVAIL(status)     ((status & HOST_STAT_UNAVAIL) != 0)

/* host is dc_ok */
#define LSB_HOST_ACOK(status)        ((status & HOST_STAT_ACOK) != 0)
 
 /*@}*/
 
 /* host busy reason bits */
/**
 * \addtogroup host_load_BusyReason host_load_BusyReason
 * If hStatus is HOST_STAT_BUSY, these indicate the host loadSched or loadStop
 * busy reason. If none of the thresholds have been exceeded, the value is
 * HOST_BUSY_NOT. Otherwise the value is the bitwise inclusive OR of some of the
 * following:
 */
/*@{*/
#define HOST_BUSY_NOT          0x000   /**< Host not busy*/
#define HOST_BUSY_R15S         0x001   /**< The 15 second average CPU run queue length is too high.*/
#define HOST_BUSY_R1M          0x002   /**< The 1 minute average CPU run queue length is too high.*/
#define HOST_BUSY_R15M         0x004   /**< The 15 minute average CPU run queue length is too high.*/
#define HOST_BUSY_UT           0x008   /**< The CPU utilization is too high.*/
#define HOST_BUSY_PG           0x010   /**< The paging rate is too high.*/
#define HOST_BUSY_IO           0x020   /**< The I/O rate is too high.*/
#define HOST_BUSY_LS           0x040   /**< There are too many login sessions.*/
#define HOST_BUSY_IT           0x080   /**< Host has not been idle long enough.*/
#define HOST_BUSY_TMP          0x100   /**< There is not enough free space in the file 
					* system containing /tmp.*/
#define HOST_BUSY_SWP          0x200   /**< There is not enough free swap space.*/
#define HOST_BUSY_MEM          0x400   /**< There is not enough free memory.*/

/* host is busy */
#define LSB_ISBUSYON(status, index)  \
      (((status[(index)/INTEGER_BITS]) & (1 << (index)%INTEGER_BITS)) != 0)
/*@}*/


/* queue status (qStatus) bits */
/**
 * \addtogroup queue_status queue_status
 * queue status (qStatus) bits
 */
/*@{*/
#define QUEUE_STAT_OPEN         0x01       /**< The queue is open to accept newly submitted
                                            * jobs.*/
#define QUEUE_STAT_ACTIVE       0x02       /**< The queue is actively dispatching jobs.
                                            * The queue can be inactivated and 
                                            * reactivated by the LSF administrator using 
                                            * \ref lsb_queuecontrol. The queue will also be
                                            * inactivated when its run or dispatch window 
                                            * is closed. In this case it cannot be
                                            * reactivated manually; it will be reactivated
                                            * by the LSF system when its run and dispatch
                                            * windows reopen.*/
#define QUEUE_STAT_RUN          0x04       /**< The queue run and dispatch windows are open.
                                            * The initial state of a queue at LSF boot time
                                            * is open and either active or inactive,
                                            * depending on its run and dispatch windows. */
#define QUEUE_STAT_NOPERM       0x08       /**< Remote queue rejecting jobs. */
#define QUEUE_STAT_DISC         0x10       /**< Remote queue status is disconnected. */
#define QUEUE_STAT_RUNWIN_CLOSE 0x20       /**< Queue run windows are closed. */
#define QUEUE_STAT_FAIRSHARE_DEFAULT 0x40       /**< Fairshare queue with null user share defined*/
/*@}*/

/* queue attribute (QAttrib) bits */
/**
 * \addtogroup queue_attribute queue_attribute
 * queue attribute (QAttrib) bits.
 */
/*@{*/
#define Q_ATTRIB_EXCLUSIVE        0x01     /**< This queue accepts jobs which
                     	                    * request exclusive execution. */
#define Q_ATTRIB_DEFAULT          0x02     /**< This queue is a default LSF queue. */
#define Q_ATTRIB_FAIRSHARE        0x04     /**< This queue uses the FAIRSHARE
                                            * scheduling policy. The user shares 
                                            * are given in userShares. */
#define Q_ATTRIB_PREEMPTIVE       0x08     /**< This queue uses the PREEMPTIVE
                     	                    * scheduling policy. */
#define Q_ATTRIB_NQS              0x10     /**< This is an NQS forward queue.
                                            * The target NQS queues are given in nqsQueues. For NQS forward
                                            * queues, the hostList, procJobLimit, windows, mig and windowsD
                                            * fields are meaningless. */ 																																																
#define Q_ATTRIB_RECEIVE          0x20     /**< This queue can receive jobs
                                            * from other clusters */
#define Q_ATTRIB_PREEMPTABLE      0x40     /**< This queue uses a preemptable
                     	                    * scheduling policy.  */
#define Q_ATTRIB_BACKFILL         0x80     /**< This queue uses a backfilling
                     	                    * policy. */
#define Q_ATTRIB_HOST_PREFER      0x100    /**< This queue uses a host
                     	                    * preference policy. */
#define Q_ATTRIB_NONPREEMPTIVE    0x200    /**< This queue can't preempt any
                     	                    * other another queue. */
#define Q_ATTRIB_NONPREEMPTABLE   0x400    /**< This queue can't be preempted
                     	                    * from any queue. */
#define Q_ATTRIB_NO_INTERACTIVE   0x800    /**< This queue does not accept batch
                     	                    * interactive jobs. */
#define Q_ATTRIB_ONLY_INTERACTIVE 0x1000   /**< This queue only accepts batch
                     	                    * interactive jobs. */
#define Q_ATTRIB_NO_HOST_TYPE     0x2000   /**< No host type related resource
                     	                    * name specified in resource
                     	                    * requirement. */
#define Q_ATTRIB_IGNORE_DEADLINE  0x4000   /**< This queue disables deadline
                     	                    * constrained resource scheduling. */
#define Q_ATTRIB_CHKPNT           0x8000   /**< Jobs may run as chkpntable. */
#define Q_ATTRIB_RERUNNABLE       0x10000  /**< Jobs may run as rerunnable. */
#define Q_ATTRIB_EXCL_RMTJOB      0x20000  /**< Excluding remote jobs when
       		                            * local jobs are present in
       		                            * the queue. */
#define Q_ATTRIB_MC_FAST_SCHEDULE 0x40000  /**< Turn on a multicluster fast
       		                            * scheduling policy. */
#define Q_ATTRIB_ENQUE_INTERACTIVE_AHEAD 0x80000  /**< Push interactive jobs
                     		                   * in front of other jobs in queue. */
#define Q_ATTRIB_DISPATCH_BY_QUEUE 0x40000 /**< Turn on fast dispatch/forward */

/* Only one of the following four flags could be TRUE. By default, the queue
 * is a local queue only(none of them is set.)
 *     0x100000 - 0xf00000 is used for MC attribute
 */

#define Q_MC_FLAG                  0xf00000  /**< Flags used by MultiCluster. */
#define Q_ATTRIB_LEASE_LOCAL       0x100000  /**< Lease and local. */
#define Q_ATTRIB_LEASE_ONLY        0x200000  /**< Lease only; no local. */
#define Q_ATTRIB_RMT_BATCH_LOCAL   0x800000  /**< Remote batch and local. */
#define Q_ATTRIB_RMT_BATCH_ONLY    0x400000  /**< Remote batch only. */

#define Q_ATTRIB_RESOURCE_RESERVE           0x1000000   /**< Memory reservation. */
#define Q_ATTRIB_FS_DISPATCH_ORDER_QUEUE    0x2000000	/**< Cross-queue fairshare. */
#define Q_ATTRIB_BATCH		            0x4000000   /**< Batch queue/partition */
#define Q_ATTRIB_ONLINE     0x8000000      		/**< Online partition */
#define Q_ATTRIB_INTERRUPTIBLE_BACKFILL     0x10000000  /**< Interruptible backfill */
#define Q_ATTRIB_APS                        0x20000000  /**< Absolute Priority
                                                         * scheduling (APS) value. */
#define Q_ATTRIB_NO_HIGHER_RESERVE          0x40000000  /**< No queue with RESOURCE_RESERVE
                                                         * or SLOT_RESERVE has higher
                                                         * priority than this queue. */
#define Q_ATTRIB_NO_HOST_VALID 0x80000000		/**< No host valid*/

/*@}*/


/* macros to check queue near real time attributes */
#define IS_ONLINE_QUEUE(Q)  	    ((Q)->qAttrib & Q_ATTRIB_ONLINE) 
#define IS_BATCH_QUEUE(Q)  	    ((Q)->qAttrib & Q_ATTRIB_BATCH) 

/* macros to check queue remote attributes */
#define IS_LEASE_LOCAL_QUEUE(Q)          (((Q)->qAttrib & Q_MC_FLAG) == Q_ATTRIB_LEASE_LOCAL) 
#define IS_LEASE_ONLY_QUEUE(Q)           (((Q)->qAttrib & Q_MC_FLAG) == Q_ATTRIB_LEASE_ONLY) 
#define IS_RMT_BATCH_LOCAL_QUEUE(Q)      (((Q)->qAttrib & Q_MC_FLAG) == Q_ATTRIB_RMT_BATCH_LOCAL)
#define IS_RMT_BATCH_ONLY_QUEUE(Q)       (((Q)->qAttrib & Q_MC_FLAG) == Q_ATTRIB_RMT_BATCH_ONLY)

#define IS_LEASE_QUEUE(Q)                (IS_LEASE_LOCAL_QUEUE(Q) || IS_LEASE_ONLY_QUEUE(Q))
#define IS_RMT_BATCH_QUEUE(Q)            (IS_RMT_BATCH_LOCAL_QUEUE(Q) || IS_RMT_BATCH_ONLY_QUEUE(Q))

#define IS_MC_QUEUE(Q)                   (IS_LEASE_QUEUE(Q) || IS_RMT_BATCH_QUEUE(Q))

#define SET_LEASE_LOCAL_QUEUE(Q)         ((Q)->qAttrib |= Q_ATTRIB_LEASE_LOCAL)
#define SET_LEASE_ONLY_QUEUE(Q)          ((Q)->qAttrib |= Q_ATTRIB_LEASE_ONLY )
#define SET_RMT_BATCH_LOCAL_QUEUE(Q)     ((Q)->qAttrib |= Q_ATTRIB_RMT_BATCH_LOCAL)
#define SET_RMT_BATCH_ONLY_QUEUE(Q)      ((Q)->qAttrib |= Q_ATTRIB_RMT_BATCH_ONLY)

#define CLR_MC_QUEUE_FLAG(Q)             ((Q)->qAttrib &= ~Q_MC_FLAG)


/* the bits 0x10000000 to 0x80000000 is reserved for internal use (daemons.h) */
/* definitions for qData field qAttrib2 */
#define Q_ATTRIB2_IGN_GUAR      0x01    /* Queue can ignore guaranteed
					 * resource pools constraints
					 */
#define  Q_ATTRIB2_JOB_ORDER_BY_MEM   0x02              /**< order job by
                                                             mem size */
#define  Q_ATTRIB2_AC_IGNORE_MACHINE_TTL 0x04           /**< ignore machine 
                                                             ttl */


/* exit code for mbatchd */
#define MASTER_NULL           200
#define MASTER_RESIGN         201
#define MASTER_RECONFIG       202
#define MASTER_FATAL          203
#define MASTER_MEM            204
#define MASTER_CONF           205
#define MASTER_EVENT          206
#define MASTER_DISABLE        207
#define MASTER_PRESTART     208

/* sub type of mbatchd die */
#define MBD_USER_CMD		1
#define MBD_NON_USER_CMD	2

/**
 * \addtogroup job_states job_states
 * define job states
 */
/*@{*/
#define JOB_STAT_NULL         0x00       /**< State null*/
#define JOB_STAT_PEND         0x01       /**< The job is pending, i.e., it 
				          * has not been dispatched yet.*/
#define JOB_STAT_PSUSP        0x02       /**< The pending job was suspended by its
					  * owner or the LSF system administrator.*/
#define JOB_STAT_RUN          0x04       /**< The job is running.*/
#define JOB_STAT_SSUSP        0x08       /**< The running job was suspended 
					  * by the system because an execution 
					  * host was overloaded or the queue run 
					  * window closed. (see \ref lsb_queueinfo, 
					  * \ref lsb_hostinfo, and lsb.queues.)
					  */
#define JOB_STAT_USUSP        0x10       /**< The running job was suspended by its owner or the LSF system
                            			 *administrator.*/
#define JOB_STAT_EXIT         0x20       /**< The job has terminated with a non-zero
					  * status - it may have been aborted due 
					  * to an error in its execution, or 
					  * killed by its owner or by the 
                                          * LSF system administrator.*/
#define JOB_STAT_DONE         0x40       /**< The job has terminated with status 0.*/
#define JOB_STAT_PDONE        (0x80)     /**< Post job process done successfully */
#define JOB_STAT_PERR         (0x100)    /**< Post job process has error */
#define JOB_STAT_WAIT         (0x200)    /**< Chunk job waiting its turn to exec */
#define JOB_STAT_RUNKWN       0x8000  /* Flag : Job status is UNKWN caused by losting contact with remote cluster */
#define JOB_STAT_UNKWN        0x10000    /**< The slave batch daemon (sbatchd) on 
                                          * the host on which the job is processed 
                                          * has lost contact with the master batch 
                                          * daemon (mbatchd).*/
/*@}*/

/**
 * \addtogroup event_types event_types
 * define statements used by \ref lsb_geteventrec. Events logged in lsb.events file 
 */
/*@{*/
#define    EVENT_JOB_NEW          1   /**< Submit a new job */
#define    EVENT_JOB_START        2   /**< mbatchd is trying to start a job */
#define    EVENT_JOB_STATUS       3   /**< Job's status change event */
#define    EVENT_JOB_SWITCH       4   /**< Job switched to another queue */
#define    EVENT_JOB_MOVE         5   /**< Move a pending job's position within a queue */
#define    EVENT_QUEUE_CTRL       6   /**< Queue's status changed by IBM Platform LSF
                                       * administrator (bhc operation) */
#define    EVENT_HOST_CTRL        7   /**< Host status changed by IBM Platform LSF
                                       * administrator (bhc operation) */
#define    EVENT_MBD_DIE          8   /**< Log parameters before mbatchd died */
#define    EVENT_MBD_UNFULFILL    9   /**< Action that was not taken because the 
                                       * mbatchd was unable to contact the sbatchd
       		                       * on the job's execution host */
#define    EVENT_JOB_FINISH       10  /**< Job finished (Logged in lsb.acct) */
#define    EVENT_LOAD_INDEX       11  /**< The complete list of load indices,
                                       * including external load indices */
#define    EVENT_CHKPNT           12  /**< Job checkpointed. */
#define    EVENT_MIG              13  /**< Job migrated */
#define    EVENT_PRE_EXEC_START   14  /**< The pre-execution command started */
#define    EVENT_MBD_START        15  /**< New mbatchd start event */
#define    EVENT_JOB_ROUTE        16  /**< The job has been routed to NQS */
#define    EVENT_JOB_MODIFY       17  /**< Job modification request */
#define    EVENT_JOB_SIGNAL       18  /**< Signal/delete a job */
#define    EVENT_CAL_NEW          19  /**< Add new calendar to the system */
#define    EVENT_CAL_MODIFY       20  /**< Calendar modified */
#define    EVENT_CAL_DELETE       21  /**< Delete a calendar in the system */
#define    EVENT_JOB_FORWARD      22  /**< Job forwarded to another cluster */
#define    EVENT_JOB_ACCEPT       23  /**< Job from a remote cluster dispatched */
#define    EVENT_STATUS_ACK       24  /**< Job status successfully sent to 
                                       * submission cluster */
#define    EVENT_JOB_EXECUTE      25  /**< Job started successfully on the 
                                       * execution host */
#define    EVENT_JOB_MSG          26  /**< Send a message to a job */
#define    EVENT_JOB_MSG_ACK      27  /**< The message has been delivered */
#define    EVENT_JOB_REQUEUE      28  /**< Job is requeued */
#define    EVENT_JOB_OCCUPY_REQ   29  /**< Submission mbatchd logs this after sending 
                                       * an occupy request to execution mbatchd */
#define    EVENT_JOB_VACATED      30  /**< Submission mbatchd logs this event after 
                                       * all execution mbatchds have vacated the
       		                       * occupied hosts for the job */
#define    EVENT_JOB_SIGACT       32  /**< A signal action on a job has been 
                                       * initiated or finished */
#define    EVENT_SBD_JOB_STATUS   34  /**< sbatchd's new job status */
#define    EVENT_JOB_START_ACCEPT 35  /**< sbatchd accepts job start */
#define    EVENT_CAL_UNDELETE     36  /**< Undelete a calendar in the system */
#define    EVENT_JOB_CLEAN        37  /**< Job is cleaned out of the core */
#define    EVENT_JOB_EXCEPTION    38  /**< Job exception was detected */
#define    EVENT_JGRP_ADD         39  /**< Adding a new job group */
#define    EVENT_JGRP_MOD         40  /**< Modifying a job group */
#define    EVENT_JGRP_CTRL        41  /**< Controlling a job group */
#define    EVENT_JOB_FORCE        42  /**< Forcing a job to start on specified 
                                       * hosts (brun operation) */
#define    EVENT_LOG_SWITCH       43  /**< Switching the event file lsb.events */
#define    EVENT_JOB_MODIFY2      44  /**< Job modification request */
#define    EVENT_JGRP_STATUS      45  /**< Log job group status */
#define    EVENT_JOB_ATTR_SET     46  /**< Job attributes have been set */
#define    EVENT_JOB_EXT_MSG      47  /**< Send an external message to a job */
#define    EVENT_JOB_ATTA_DATA    48  /**< Update data status of a message for a job */
#define    EVENT_JOB_CHUNK        49  /**< Insert one job to a chunk */
#define    EVENT_SBD_UNREPORTED_STATUS 50 /**< Save unreported sbatchd status */
#define    EVENT_ADRSV_FINISH     51  /**< Reservation finished */
#define    EVENT_HGHOST_CTRL      52  /**< Dynamic host group control */
#define    EVENT_CPUPROFILE_STATUS 53 /**< Saved current CPU allocation on service partition */
#define    EVENT_DATA_LOGGING     54  /**< Write out data logging file */
#define    EVENT_JOB_RUN_RUSAGE   55  /**< Write job rusage in lsb.stream */
#define    EVENT_END_OF_STREAM    56  /**< Stream closed and new stream opened. */
#define    EVENT_SLA_RECOMPUTE    57  /**< SLA goal is reavaluated */
#define	   EVENT_METRIC_LOG	  58  /**< Write performance metrics to lsb.stream*/
#define    EVENT_TASK_FINISH      59  /**< Write task finish log to ssched.acct */
#define    EVENT_JOB_RESIZE_NOTIFY_START  60  /**< Resize allocation is made */
#define    EVENT_JOB_RESIZE_NOTIFY_ACCEPT 61  /**< Resize notification action initialized */
#define    EVENT_JOB_RESIZE_NOTIFY_DONE 62 /**< Resize notification action completed */
#define    EVENT_JOB_RESIZE_RELEASE 63 /**< Job resize release request is received */
#define    EVENT_JOB_RESIZE_CANCEL 64 /**< Job resize cancel request is received */
#define    EVENT_JOB_RESIZE        65 /**< Job resize event for lsb.acct */
#define    EVENT_JOB_ARRAY_ELEMENT 66 /**< Saved array element's resource consumption 
                                       * for LSF simulator */
#define    EVENT_MBD_SIM_STATUS    67 /**< Saved LSF simulator status */
#define    EVENT_JOB_FINISH2       68 /**<  Extended stream event than JOB_FINISH*/
#define    EVENT_JOB_STARTLIMIT    69 /**< Stream event for job start limits */
#define    EVENT_JOB_STATUS2       70 /**< Job status log to lsb.status file */
#define    EVENT_JOB_PENDING_REASONS 71 /**< Pending reasons in pendingreason.dump file */
#define    EVENT_JOB_SWITCH2       72   /**< Job array switched to another queue */
#define    EVENT_JOB_ACCEPTACK     73  /** < Acknowledge the job acceptance >**/
#define    EVENT_JOB_PROVISION_START  74 /**< Dynamic Cluster provisioning requested */
#define    EVENT_JOB_PROVISION_END    75 /**< Dynamic Cluster provisioning finished */
#define    EVENT_JOB_FANOUT_INFO   76 /**< Job fan-out information is received */
#define    NUM_EVENT_TYPES         76 /**< Number of different event types. */
/*@}*/

/* event kind
 */
/*@{*/
#define   EVENT_JOB_RELATED     1   /**< it is a job related event */
#define   EVENT_NON_JOB_RELATED 0   /**< it is a non job related event */
/*@}*/

 /*
 * EXCLUSIVE PENDING REASONS
 * a job must stay pending as long as ONE of the exclusive reasons exists 
 */

/* Job Related Reasons (001 - 300)
 */
/**
* \addtogroup pending_reasons pending_reasons
* \brief          Each entry in the table contains one of the following pending reasons
*/
/*@{*/
#define PEND_JOB_REASON        0   /**< Virtual code; not a reason */
#define PEND_JOB_NEW           1   /**< A new job is waiting to be scheduled */
#define PEND_JOB_START_TIME    2   /**< The job is held until its specified start time */
#define PEND_JOB_DEPEND        3   /**< The job is waiting for its dependency condition(s) to be satisfied */
#define PEND_JOB_DEP_INVALID   4   /**< The dependency condition is invalid or never satisfied */
#define PEND_JOB_MIG           5   /**< The migrating job is waiting to be rescheduled */
#define PEND_JOB_PRE_EXEC      6   /**< The job's pre-exec command exited with non-zero status */
#define PEND_JOB_NO_FILE       7   /**< Unable to access jobfile */
#define PEND_JOB_ENV           8   /**< Unable to set job's environment variables */
#define PEND_JOB_PATHS         9   /**< Unable to determine the job's home or working directories */
#define PEND_JOB_OPEN_FILES    10  /**< Unable to open the job's input and output files */ 
#define PEND_JOB_EXEC_INIT     11  /**< Job execution initialization failed */
#define PEND_JOB_RESTART_FILE  12  /**< Unable to copy restarting job's checkpoint files */
#define PEND_JOB_DELAY_SCHED   13  /**< Scheduling of the job is delayed */
#define PEND_JOB_SWITCH        14  /**< Waiting for the re-scheduling of the job after switching queues */
#define PEND_JOB_DEP_REJECT    15  /**< An event is rejected by eeventd due to a syntax error */
#define PEND_JOB_JS_DISABLED   16  /**< A JobScheduler feature is not enabled */
#define PEND_JOB_NO_PASSWD     17  /**< Failed to get user password */
#define PEND_JOB_LOGON_FAIL    18  /**< The job is pending due to logon failure */
#define PEND_JOB_MODIFY        19  /**< The job is waiting to be re-scheduled after its parameters have been changed */
#define PEND_JOB_TIME_INVALID  20  /**< The job time event is invalid */
#define PEND_TIME_EXPIRED      21  /**< The job time event has expired */
#define PEND_JOB_REQUEUED      23  /**< The job has been requeued */
#define PEND_WAIT_NEXT         24  /**< Waiting for the next time event */
#define PEND_JGRP_HOLD         25  /**< The parent group is held */
#define PEND_JGRP_INACT        26  /**< The parent group is inactive */
#define PEND_JGRP_WAIT         27  /**< The group is waiting for scheduling */
#define PEND_JOB_RCLUS_UNREACH 28  /**< The remote cluster(s) are unreachable */
#define PEND_JOB_QUE_REJECT    29  /**< SNDJOBS_TO queue rejected by remote 
       	                            * clusters */
#define PEND_JOB_RSCHED_START  30  /**< Waiting for new remote scheduling 
       	                            * session */
#define PEND_JOB_RSCHED_ALLOC  31  /**< Waiting for allocation reply from
       	                            * remote clusters */
#define PEND_JOB_FORWARDED     32  /**< The job is forwarded to a remote cluster */
#define PEND_JOB_RMT_ZOMBIE    33  /**< The job running remotely is in a zombie state */
#define PEND_JOB_ENFUGRP       34  /**< Job's enforced user group share account
       	                            * not selected */
#define PEND_SYS_UNABLE        35  /**< The system is unable to schedule the job */
#define PEND_JGRP_RELEASE      36  /**< The parent group has just been released */
#define PEND_HAS_RUN           37  /**< The job has run since group active*/
#define PEND_JOB_ARRAY_JLIMIT  38  /**< The job has reached its running element limit*/
#define PEND_CHKPNT_DIR        39  /**< Checkpoint directory is invalid */
#define PEND_CHUNK_FAIL        40  /**< The first job in the chunk failed 
				    * (all other jobs in the chunk are set to PEND) */
#define PEND_JOB_SLA_MET       41  /**< Optimum number of running jobs for SLA has been reached */
#define PEND_JOB_APP_NOEXIST   42  /**< Specified application profile does not exist */
#define PEND_APP_PROCLIMIT     43  /**< Job no longer satisfies application 
                                    * PROCLIMIT configuration */
#define PEND_EGO_NO_HOSTS      44  /**< No hosts for the job from EGO */
#define PEND_JGRP_JLIMIT       45  /**< The specified job group has reached its job limit */
#define PEND_PREEXEC_LIMIT     46  /**< Job pre-exec retry limit */
#define PEND_REQUEUE_LIMIT     47  /**< Job re-queue limit */
#define PEND_BAD_RESREQ        48  /**< Job has bad res req */
#define PEND_RSV_INACTIVE      49  /**< Job's reservation is inactive */
#define PEND_WAITING_RESUME    50  /**< Job was in PSUSP with bad res req,
       	                            * after successful bmod  waiting for
       	                            * the user to bresume */
#define PEND_SLOT_COMPOUND     51  /**< Job slot request cannot satisfy
				    * compound resource requirement */
#define PEND_SLOT_ALTERNATIVE  52  /**< Job slot request cannot satisfy
				    * alternative resource requirement */
#define PEND_FANOUT_FAIL       53  /**< Fail to send fan-out information to other SBDs */

/*
 * Queue and System Related Reasons (301 - 599) 
 */
#define PEND_QUE_INACT             301   /**< The queue is inactivated by the administrator */
#define PEND_QUE_WINDOW            302   /**< The queue is inactivated by its time windows */
#define PEND_QUE_JOB_LIMIT         303   /**< The queue has reached its job slot limit */
#define PEND_QUE_USR_JLIMIT        304   /**< The user has reached the per-user job slot limit of the queue */
#define PEND_QUE_USR_PJLIMIT       305   /**< Not enough per-user job slots of the queue for the parallel job */
#define PEND_QUE_PRE_FAIL          306   /**< The queue's pre-exec command exited with non-zero status */
#define PEND_NQS_RETRY             307   /**< The job was not accepted by the NQS host,
					  *  Attempt again later */
#define PEND_NQS_REASONS           308   /**< Unable to send the job to an NQS host */
#define PEND_NQS_FUN_OFF           309   /**< Unable to contact NQS host */
#define PEND_SYS_NOT_READY         310   /**< The system is not ready for scheduling after reconfiguration */
#define PEND_SBD_JOB_REQUEUE       311   /**< The requeued job is waiting for rescheduling */
#define PEND_JOB_SPREAD_TASK       312   /**< Not enough hosts to meet the job's spanning requirement */
#define PEND_QUE_SPREAD_TASK       313   /**< Not enough hosts to meet the queue's spanning requirement */
#define PEND_QUE_PJOB_LIMIT        314   /**< The queue has not enough job slots for the parallel job */
#define PEND_QUE_WINDOW_WILL_CLOSE 315   /**< The job will not finish before queue's run window is closed */
#define PEND_QUE_PROCLIMIT	   316   /**< Job no longer satisfies queue 
                                          * PROCLIMIT configuration */
#define PEND_SBD_PLUGIN		   317   /**< Job requeued due to plug-in failure */
#define PEND_WAIT_SIGN_LEASE	   318   /**< Waiting for lease signing */
#define PEND_WAIT_SLOT_SHARE       319 /* waitint for scheduling for SLOT_SHARE*/

/* 
 * User Related Reasons (601 - 800)
 */
#define PEND_USER_JOB_LIMIT    601     /**< The job slot limit is reached */
#define PEND_UGRP_JOB_LIMIT    602     /**< A user group has reached its job slot limit */
#define PEND_USER_PJOB_LIMIT   603     /**< The job slot limit for the parallel job is reached */
#define PEND_UGRP_PJOB_LIMIT   604     /**< A user group has reached its job slot limit for the parallel job */
#define PEND_USER_RESUME       605     /**< Waiting for scheduling after resumed by user */
#define PEND_USER_STOP         607     /**< The job was suspended by the user while pending */
#define PEND_NO_MAPPING        608     /**< Unable to determine user account for execution */
#define PEND_RMT_PERMISSION    609     /**< The user has no permission to run the job on remote host/cluster */ 
#define PEND_ADMIN_STOP        610     /**< The job was suspended by LSF admin or root while pending */
#define PEND_MLS_INVALID       611     /**< The requested label is not valid */
#define PEND_MLS_CLEARANCE     612     /**< The requested label is above user allowed range */
#define PEND_MLS_RHOST         613     /**< The requested label rejected by /etc/rhost.conf */
#define PEND_MLS_DOMINATE      614     /**< The requested label does not dominate current label */
#define PEND_MLS_FATAL         615     /**< The requested label problem */
#define PEND_INTERNAL_STOP     616     /**< LSF internally bstoped a pending job */

/* 
 * NON-EXCLUSIVE PENDING REASONS
 * A job may still start even though non-exclusive reasons exist.
 */

/*
 * Host(sbatchd)-Job Related Reasons (1001 - 1300)
 */
#define PEND_HOST_RES_REQ      1001     /**< The job's resource requirements not satisfied */
#define PEND_HOST_NONEXCLUSIVE 1002     /**< The job's requirement for exclusive execution not satisfied */
#define PEND_HOST_JOB_SSUSP    1003     /**< Higher or equal priority jobs already suspended by system */
#define PEND_HOST_PART_PRIO    1004     /**< The job failed to compete with other jobs on host partition */
#define PEND_SBD_GETPID        1005     /**< Unable to get the PID of the restarting job */
#define PEND_SBD_LOCK          1006     /**< Unable to lock the host for exclusively executing the job */
#define PEND_SBD_ZOMBIE        1007     /**< Cleaning up zombie job */
#define PEND_SBD_ROOT          1008     /**< Can't run jobs submitted by root. 
					 * The job is rejected by the sbatchd */
#define PEND_HOST_WIN_WILL_CLOSE   1009 /**< Job can't finish on the host before
       		                         * queue's run window is closed */
#define PEND_HOST_MISS_DEADLINE    1010 /**< Job can't finish on the host before
       		                         * job's termination deadline */
#define PEND_FIRST_HOST_INELIGIBLE 1011 /**< The specified first execution host is 
					 * not eligible for this job at this time */
#define PEND_HOST_EXCLUSIVE_RESERVE 1012 /**< Exclusive job reserves slots on host */
#define PEND_FIRST_HOST_REUSE 1013       /**< Resized shadow job 
                                          * or non-first resReq of a compound resReq job
                                          * try to reuse the first execution host */
#define PEND_HOST_JOB_VM     1014        /**< Job virtual machine container host 
                                          * cannot be used for scheduling */
/* 
 * Host Related Reasons (1301 - 1600)
 */
#define PEND_HOST_DISABLED     1301   /**< The host is closed by the LSF administrator */
#define PEND_HOST_LOCKED       1302   /**< The host is locked by the LSF administrator */
#define PEND_HOST_LESS_SLOTS   1303   /**< Not enough job slots for the parallel job */
#define PEND_HOST_WINDOW       1304   /**< Dispatch windows are closed */
#define PEND_HOST_JOB_LIMIT    1305   /**< The job slot limit reached */
#define PEND_QUE_PROC_JLIMIT   1306   /**< The queue's per-CPU job slot limit is reached */ 
#define PEND_QUE_HOST_JLIMIT   1307   /**< The queue's per-host job slot limit is reached */
#define PEND_USER_PROC_JLIMIT  1308   /**< The user's per-CPU job slot limit is reached */
#define PEND_HOST_USR_JLIMIT   1309   /**< The host's per-user job slot limit is reached */
#define PEND_HOST_QUE_MEMB     1310   /**< Not a member of the queue */
#define PEND_HOST_USR_SPEC     1311   /**< Not a user-specified host */
#define PEND_HOST_PART_USER    1312   /**< The user has no access to the host partition */
#define PEND_HOST_NO_USER      1313   /**< There is no such user account */
#define PEND_HOST_ACCPT_ONE    1314   /**< Just started a job recently */
#define PEND_LOAD_UNAVAIL      1315   /**< Load info unavailable */
#define PEND_HOST_NO_LIM       1316   /**< The LIM is unreachable by the sbatchd */
#define PEND_HOST_UNLICENSED   1317   /**< The host does not have a valid LSF software license */
#define PEND_HOST_QUE_RESREQ   1318   /**< The queue's resource requirements are not satisfied */
#define PEND_HOST_SCHED_TYPE   1319   /**< The submission host type is not the same */
#define PEND_JOB_NO_SPAN       1320   /**< There are not enough processors to meet the job's spanning requirement. 
				       * The job level locality is unsatisfied. */
#define PEND_QUE_NO_SPAN       1321   /**< There are not enough processors to meet the queue's spanning requirement. 
				       * The queue level locality is unsatisfied. */
#define PEND_HOST_EXCLUSIVE    1322   /**< An exclusive job is running */
#define PEND_HOST_JS_DISABLED  1323   /**< Job Scheduler is disabled on the host. 
				       * It is not licensed to accept repetitive jobs. */
#define PEND_UGRP_PROC_JLIMIT  1324   /**< The user group's per-CPU job slot limit is reached */
#define PEND_BAD_HOST          1325   /**< Incorrect host, group or cluster name */
#define PEND_QUEUE_HOST        1326   /**< Host is not used by the queue */
#define PEND_HOST_LOCKED_MASTER  1327 /**< Host is locked by master LIM */
#define PEND_HOST_LESS_RSVSLOTS  1328 /**< Not enough reserved job slots at this time for specified reservation ID */
#define PEND_HOST_LESS_DURATION  1329 /**< Not enough slots or resources for whole duration of the job */
#define PEND_HOST_NO_RSVID       1330 /**< Specified reservation has expired or has been deleted */
#define PEND_HOST_LEASE_INACTIVE 1331 /**< The host is closed due to lease is inactive */
#define PEND_HOST_ADRSV_ACTIVE   1332 /**< Not enough job slot(s) while advance reservation is active */
#define PEND_QUE_RSVID_NOMATCH  1333  /**< This queue is not configured to send jobs to the cluster specified in the advance */
#define PEND_HOST_GENERAL       1334  /**< Individual host based reasons*/
#define PEND_HOST_RSV           1335  /**< Host does not belong to the specified 
                                       * advance reservation */
#define PEND_HOST_NOT_CU        1336  /**< Host does not belong to a compute unit 
                                       * of the required type */
#define PEND_HOST_CU_EXCL       1337  /**< A compute unit containing the host is 
                                       * used exclusively */
#define PEND_HOST_CU_OCCUPIED   1338  /**< CU-level excl. job cannot start since CU 
                                       * is occupied */
#define PEND_HOST_USABLE_CU     1339  /**< Insufficiently many usable slots on the 
                                       * host's compute unit */
#define PEND_JOB_FIRST_CU       1340  /**< No first execution compute unit satisfies
                                       * CU 'usablepercu' requirement. */
#define PEND_HOST_CU_EXCL_RSV   1341  /**< A CU containing the host is reserved 
                                       * exclusively */
#define PEND_JOB_CU_MAXCUS      1342  /**< Maxcus cannot be satisfied */
#define PEND_JOB_CU_BALANCE     1343  /**< Balance cannot be satisfied */
#define PEND_CU_TOPLIB_HOST     1344  /**< Cu not supported on toplib integration hosts */
#define PEND_HOST_PREEXEC_FAIL  1345  /**< Host is excluded from re-scheduling the job
                                       * because the job's pre_exec failed on this host. */
#define PEND_HOST_VNODE         1346  /**< Host is vnode, only specific HPC jobs can run */
/*
 * sbatchd Related Reasons (1601 - 1900)
 */
#define PEND_SBD_UNREACH       1601   /**< Cannot reach sbatchd */
#define PEND_SBD_JOB_QUOTA     1602   /**< Number of jobs exceed quota */
#define PEND_JOB_START_FAIL    1603   /**< The job failed in talking to the server to start the job */
#define PEND_JOB_START_UNKNWN  1604   /**< Failed in receiving the reply from the server when starting the job */
#define PEND_SBD_NO_MEM        1605   /**< Unable to allocate memory to run job. 
				       * There is no memory on the sbatchd. */
#define PEND_SBD_NO_PROCESS    1606   /**< Unable to fork process to run the job. 
				       * There are no more processes on the sbatchd. */
#define PEND_SBD_SOCKETPAIR    1607   /**< Unable to communicate with the job process */
#define PEND_SBD_JOB_ACCEPT    1608   /**< The slave batch server failed to accept the job */ 
#define PEND_LEASE_JOB_REMOTE_DISPATCH 1609 /**< Lease job remote dispatch failed */
#define PEND_JOB_RESTART_FAIL  1610   /**< Failed to restart job from last checkpoint */
/*
 * Load Related Reasons (2001 - 2300, 
 *     PEND_HOST_LOAD_MORE - (PEND_HOST_LOAD_MORE + PEND_RES_EXTEND_SIZE -1)
 */
#define PEND_HOST_LOAD         2001 /**< The load threshold is reached */

/*
 * Queue Resource Reservation Related Reasons (2301 - 2600,
 *     PEND_HOST_QUE_RUSAGE_MORE - 
 *                              (PEND_HOST_QUE_RUSAGE_MORE + PEND_RES_EXTEND_SIZE -1)
 */
#define PEND_HOST_QUE_RUSAGE   2301 /**< The queue's requirements for resource 
                                     * reservation are not satisfied.*/

/*
 * Jobs Resource Reservation Related Reasons (2601 - 2900, 
 *     PEND_HOST_JOB_RUSAGE_MORE - 
 *                               (PEND_HOST_JOB_RUSAGE_MORE + PEND_RES_EXTEND_SIZE -1)
 */
#define PEND_HOST_JOB_RUSAGE   2601 /**< The job's requirements for resource 
                                     * reservation are not satisfied.*/

/*
 * Remote Forwarding Related Reasons (2901 - 3200)
 */
#define PEND_RMT_JOB_FORGOTTEN        2901  /**< Remote job not recongized by remote
					     * cluster, waiting for rescheduling*/
#define PEND_RMT_IMPT_JOBBKLG         2902  /**< Remote import limit reached, waiting 
                                             * for rescheduling*/
#define PEND_RMT_MAX_RSCHED_TIME      2903  /**< Remote schedule time reached, 
                                             * waiting for rescheduling*/
#define PEND_RMT_MAX_PREEXEC_RETRY    2904  /**< Remote pre-exec retry limit reached,
                                             * waiting for rescheduling*/
#define PEND_RMT_QUEUE_CLOSED         2905  /**< Remote queue is closed*/
#define PEND_RMT_QUEUE_INACTIVE       2906  /**< Remote queue is inactive*/
#define PEND_RMT_QUEUE_CONGESTED      2907  /**< Remote queue is congested*/
#define PEND_RMT_QUEUE_DISCONNECT     2908  /**< Remote queue is disconnected*/
#define PEND_RMT_QUEUE_NOPERMISSION   2909  /**< Remote queue is not configured to accept
                                             * jobs from this cluster*/
#define PEND_RMT_BAD_TIME             2910  /**< Job's termination time exceeds the job
                                             * creation time on remote cluster*/
#define PEND_RMT_PERMISSIONS          2911  /**< Permission denied on the execution cluster*/
#define PEND_RMT_PROC_NUM             2912  /**< Job's required on number of processors
                                             * cannot be satisfied on the remote cluster*/
#define PEND_RMT_QUEUE_USE            2913  /**< User is not defined in the fairshare
                                             * policy of the remote queue*/
#define PEND_RMT_NO_INTERACTIVE       2914  /**< Remote queue is a non-interactive queue*/
#define PEND_RMT_ONLY_INTERACTIVE     2915  /**< Remote queue is an interactive-only queue*/
#define PEND_RMT_PROC_LESS            2916  /**< Job's required maximum number of 
                                             * processors is less then the minimum number*/
#define PEND_RMT_OVER_LIMIT           2917  /**< Job's required resource limit exceeds
                                             * that of the remote queue*/
#define PEND_RMT_BAD_RESREQ           2918  /**< Job's resource requirements do not
                                             * match with those of the remote queue*/
#define PEND_RMT_CREATE_JOB           2919  /**< Job failed to be created on the 
                                             *remote cluster*/
#define PEND_RMT_RERUN                2920  /**< Job is requeued for rerun on the
                                             * execution cluster*/
#define PEND_RMT_EXIT_REQUEUE         2921  /**< Job is requeued on the execution
                                             * cluster due to exit value*/
#define PEND_RMT_REQUEUE              2922  /**< Job was killed and requeued on
                                             * the execution cluster*/
#define PEND_RMT_JOB_FORWARDING       2923  /**< Job was forwarded to remote cluster*/
#define PEND_RMT_QUEUE_INVALID        2924  /**< Remote import queue defined for the
                                             * job in lsb.queues is either not
                                             * ready or not valid*/
#define PEND_RMT_QUEUE_NO_EXCLUSIVE   2925  /**< Remote queue is a non-exclusive queue*/
#define PEND_RMT_UGROUP_MEMBER        2926  /**< Job was rejected; submitter does not
                                             * belong to the specified User Group in the
       		                             * remote cluster or the user group does not
       		                             * exist in the remote cluster*/
#define PEND_RMT_INTERACTIVE_RERUN    2927  /**< Remote queue is rerunnable: can not
                                             * accept interactive jobs*/
#define PEND_RMT_JOB_START_FAIL       2928  /**< Remote cluster failed in talking
                                             * to server to start the job*/
#define PEND_RMT_FORWARD_FAIL_UGROUP_MEMBER   2930/**< Job was rejected; submitter does
                                                   * not belong to the specified User Group in the
                                                   * remote cluster or the user group does
                                                   * not exist in the remote cluster*/
#define PEND_RMT_HOST_NO_RSVID      2931      /**< Specified remote reservation has
                                               * expired or has been deleted*/                          
#define PEND_RMT_APP_NULL             2932    /**< Application profile could not be
                                               * found in the remote cluster.*/                            
#define PEND_RMT_BAD_RUNLIMIT     2933        /**< Job's required RUNLIMIT exceeds 
                                               * RUNTIME * JOB_RUNLIMIT_RATIO of
                                               * the remote cluster.*/
#define PEND_RMT_OVER_QUEUE_LIMIT  2934   /**< Job's required RUNTIME exceeds the
                                           * hard runtime limit in the remote queue.*/
#define PEND_RMT_WHEN_NO_SLOTS     2935   /**< Job will be pend when no slots available among remote queues.*/

#define PEND_RMT_ADJUST_ASKHOST       2936    /**< The job asked host was moved.*/
#define PEND_RMT_NO_VALID_ASKHOST     2937    /**< No valid host in job asked hosts.*/
#define PEND_RMT_BRUN_FAILED          2938    /**< Brun failed; job pending.*/
#define PEND_RMT_QUEUE_HOST           2939    /**< Host or host group is not used by remote queue.*/
#define PEND_RMT_UNFORWARD            2940    /**< Job has returned from remote cluster.*/
#define PEND_RMT_JOB_BOT              2941    /**< Job has returned due to a remote bbot request.*/
/* Asked cluster related */
#define PEND_RMT_ASKED_RESOURCE  2942   /**< Asked clusters do not satisfy job requirement. */
#define PEND_RMT_BAD_ASKED_CLUSTER   2943  /**< Incorrect asked cluster name. */
#define PEND_RMT_QUEUE_ASKED_CLUSTER  2944  /**< Asked cluster is not used by the queue*/

/* SUSPENDING REASONS */

/* 
 * General Resource Limits Related Reasons ( 3201 - 4800)
 */
#define PEND_GENERAL_LIMIT_USER		3201   /**< Resource limit defined on user 
                                                * or user group has been reached.*/
#define PEND_GENERAL_LIMIT_QUEUE	3501   /**< Resource (%s) limit defined
                                                * on queue has been reached. */
#define PEND_GENERAL_LIMIT_PROJECT	3801   /**< Resource limit defined on
                                                * project has been reached.*/
#define PEND_GENERAL_LIMIT_CLUSTER	4101   /**< Resource (%s) limit defined
                                                * cluster wide has been reached. */
#define PEND_GENERAL_LIMIT_HOST		4401   /**< Resource (%s) limit defined on
                                                * host and/or host group has 
                                                * been reached. */                                              
#define PEND_GENERAL_LIMIT_JOBS_USER	4701   /**< JOBS limit defined for the user
                                                * or user group has been reached.*/
#define PEND_GENERAL_LIMIT_JOBS_QUEUE	4702   /**< JOBS limit defined for the
                                                * queue has been reached.*/
#define PEND_GENERAL_LIMIT_JOBS_PROJECT	4703   /**< JOBS limit defined for the
                                                * project has been reached.*/
#define PEND_GENERAL_LIMIT_JOBS_CLUSTER	4704   /**< JOBS limit defined cluster-wide
                                                * has been reached.*/
#define PEND_GENERAL_LIMIT_JOBS_HOST	4705   /**< JOBS limit defined on host or
                                                * host group has been reached.*/
#define PEND_GENERAL_LIMIT_JOBS_LIC_PROJECT 4706   /**< JOBS limit defined for the
                                                * license project has been reached.*/

/*
 * Forward Slot Limits Related Reasons ( 4801 - 4899 )
 */
#define PEND_GENERAL_LIMIT_FWDSLOTS_QUEUE   4801  /* forward slot limit pending on queue */
#define PEND_GENERAL_LIMIT_FWDSLOTS_USER    4802  /* forward slot limit pending on user */
#define PEND_GENERAL_LIMIT_FWDSLOTS_PROJECT 4803  /* forward slot limit pending on project */
#define PEND_GENERAL_LIMIT_FWDSLOTS_CLUSTER 4804  /* forward slot limit pending on cluster */
#define IS_FWDSLOTS_LIMIT_REASON(id) ((id <= PEND_GENERAL_LIMIT_FWDSLOTS_CLUSTER ) && (id >= PEND_GENERAL_LIMIT_FWDSLOTS_QUEUE))

/* LSF2 Presto RLA-related reasons    (4900 - 4989) */
#define PEND_RMS_PLUGIN_INTERNAL                4900   /**< RMS scheduler plugin 
                                                        * internal error.*/
#define PEND_RMS_PLUGIN_RLA_COMM                4901   /**< RLA communication failure.*/
#define PEND_RMS_NOT_AVAILABLE                  4902   /**< RMS is not available.*/
#define PEND_RMS_FAIL_TOPOLOGY                  4903   /**< Cannot satisfy the topology 
                                                        * requirement.*/
#define PEND_RMS_FAIL_ALLOC                     4904   /**< Cannot allocate an RMS resource.*/
#define PEND_RMS_SPECIAL_NO_PREEMPT_BACKFILL    4905   /**< RMS job with special topology
                                                        * requirements cannot be preemptive
                                                        * or backfill job.*/
#define PEND_RMS_SPECIAL_NO_RESERVE             4906   /**< RMS job with special topology
                                                        * requirements cannot reserve slots.*/
#define PEND_RMS_RLA_INTERNAL                   4907   /**< RLA internal error.*/
#define PEND_RMS_NO_SLOTS_SPECIAL               4908   /**< Not enough slots for job. 
                                                        * Job with RMS topology
                                                        * requirements cannot reserve slots,
                                                        * be preemptive, or be a backfill job.*/
#define PEND_RMS_RLA_NO_SUCH_USER               4909   /**< User account does not
                                                        * exist on the execution host.*/
#define PEND_RMS_RLA_NO_SUCH_HOST               4910   /**< Unknown host and/or partition
                                                        * unavailable.*/
#define PEND_RMS_CHUNKJOB                       4911   /**< Cannot schedule chunk
                                                        * jobs to RMS hosts.*/
#define PEND_RLA_PROTOMISMATCH                  4912   /**< RLA protocol mismatch.*/
#define PEND_RMS_BAD_TOPOLOGY                   4913   /**< Contradictory topology
                                                        * requirements specified.*/
#define PEND_RMS_RESREQ_MCONT                   4914   /**< Not enough slots to satisfy
                                                        * manditory contiguous requirement.*/
#define PEND_RMS_RESREQ_PTILE                   4915   /**< Not enough slots to satisfy
                                                        * RMS ptile requirement.*/
#define PEND_RMS_RESREQ_NODES                   4916   /**< Not enough slots to
                                                        * satisfy RMS nodes requirement.*/
#define PEND_RMS_RESREQ_BASE                    4917   /**< Cannot satisfy RMS base
                                                        * node requirement.*/
#define PEND_RMS_RESREQ_RAILS                   4918   /**< Cannot satisfy RMS
                                                        * rails requirement.*/
#define PEND_RMS_RESREQ_RAILMASK                4919   /**< Cannot satisfy RMS
                                                        * railmask requirement.*/


/* 
 * Maui Integration Related Reasons ( 5000 - 5100)
 */
#define PEND_MAUI_UNREACH             5000   /**< Unable to communicate with external
                                              * Maui scheduler.*/
#define PEND_MAUI_FORWARD             5001   /**< Job is pending at external
                                              * Maui scheduler.*/
#define PEND_MAUI_REASON              5030   /**< External Maui scheduler sets
                                              * detail reason.*/ 

/*
 * SGI CPUSET Integration Related Reasons ( 5200 - 5299)
 */
#define PEND_CPUSET_ATTACH	      5200    /**< CPUSET attach failed. 
                                               * Job requeued */
#define PEND_CPUSET_NOT_CPUSETHOST    5201    /**< Not a cpuset host*/
#define PEND_CPUSET_TOPD_INIT         5202    /**< Topd initialization failed */
#define PEND_CPUSET_TOPD_TIME_OUT     5203    /**< Topd communication timeout */
#define PEND_CPUSET_TOPD_FAIL_ALLOC   5204    /**< Cannot satisfy the cpuset 
                                               * allocation requirement */
#define PEND_CPUSET_TOPD_BAD_REQUEST  5205    /**< Bad cpuset allocation request*/
#define PEND_CPUSET_TOPD_INTERNAL     5206    /**< Topd internal error */
#define PEND_CPUSET_TOPD_SYSAPI_ERR   5207    /**< Cpuset system API failure */
#define PEND_CPUSET_TOPD_NOSUCH_NAME  5208    /**< Specified static cpuset does 
                                               * not exist on the host */
#define PEND_CPUSET_TOPD_JOB_EXIST    5209    /**< Cpuset is already allocated 
                                               *  for this job */
#define PEND_CPUSET_TOPD_NO_MEMORY    5210    /**< Topd malloc failure */
#define PEND_CPUSET_TOPD_INVALID_USER 5211    /**< User account does not exist 
                                               *  on the cpuset host */
#define PEND_CPUSET_TOPD_PERM_DENY    5212    /**< User does not have permission 
                                               *  to run job within cpuset */
#define PEND_CPUSET_TOPD_UNREACH      5213    /**< Topd is not available */
#define PEND_CPUSET_TOPD_COMM_ERR     5214    /**< Topd communication failure */
    
#define PEND_CPUSET_PLUGIN_INTERNAL   5215    /**< CPUSET scheduler plugin internal
                                               * error */
#define PEND_CPUSET_CHUNKJOB          5216    /**< Cannot schedule chunk jobs to cpuset
                                               * hosts */
#define PEND_CPUSET_CPULIST           5217    /**< Can't satisfy CPU_LIST
                                               *   requirement */
#define PEND_CPUSET_MAXRADIUS         5218    /**< Cannot satisfy CPUSET MAX_RADIUS
                                               * requirement */

/* Bproc integration related reasons (5300 - 5320)
 */
#define PEND_NODE_ALLOC_FAIL          5300    /**< Node allocation failed */
 
/* Eagle pending reasons  (5400 - 5449) */
#define PEND_RMSRID_UNAVAIL		     5400  /**< RMS resource is not available*/

#define PEND_NO_FREE_CPUS                    5450  /**< Not enough free cpus
                                                    * to satisfy job requirements */ 
#define PEND_TOPOLOGY_UNKNOWN                5451  /**< Topology unknown or
                                                    * recently changed*/
#define PEND_BAD_TOPOLOGY                    5452  /**< Contradictory topology
                                                    * requirement specified*/
#define PEND_RLA_COMM                        5453  /**< RLA communications failure*/
#define PEND_RLA_NO_SUCH_USER                5454  /**< User account does not exist
                                                    * on execution host*/
#define PEND_RLA_INTERNAL                    5455  /**< RLA internal error*/
#define PEND_RLA_NO_SUCH_HOST                5456  /**< Unknown host and/or
                                                    * partition unavailable*/
#define PEND_RESREQ_TOOFEWSLOTS              5457  /**< Too few slots for specified
                                                    * topology requirement*/

/* PSET pending reasons (5500 - 5549) */
#define PEND_PSET_PLUGIN_INTERNAL      5500   /**< PSET scheduler plugin internal error*/
#define PEND_PSET_RESREQ_PTILE         5501   /**< Cannot satisfy PSET ptile requirement*/
#define PEND_PSET_RESREQ_CELLS         5502   /**< Cannot satisfy PSET cells requirement*/
#define PEND_PSET_CHUNKJOB             5503   /**< Cannot schedule chunk jobs to PSET hosts*/
#define PEND_PSET_NOTSUPPORT           5504   /**< Host does not support processor set
                                               * functionality*/
#define PEND_PSET_BIND_FAIL            5505   /**< PSET bind failed. Job requeued*/
#define PEND_PSET_RESREQ_CELLLIST      5506   /**< Cannot satisfy PSET CELL_LIST 
                                               * requirement*/


/* SLURM pending reasons (5550 - 5599) */
#define PEND_SLURM_PLUGIN_INTERNAL      5550  /**< SLURM scheduler plugin internal error*/
#define PEND_SLURM_RESREQ_NODES         5551  /**< Not enough resource to satisfy 
                                               * SLURM nodes requirment*/
#define PEND_SLURM_RESREQ_NODE_ATTR     5552  /**< Not enough resource to satisfy 
                                               * SLURM node attributes requirment.*/
#define PEND_SLURM_RESREQ_EXCLUDE       5553  /**< Not enough resource to satisfy
                                               * SLURM exclude requirment.*/
#define PEND_SLURM_RESREQ_NODELIST      5554  /**< Not enough resource to satisfy
                                               * SLURM nodelist requirment.*/
#define PEND_SLURM_RESREQ_CONTIGUOUS    5555  /**< Not enough resource to satisfy
                                               * SLURM contiguous requirment.*/
#define PEND_SLURM_ALLOC_UNAVAIL        5556  /**< SLURM allocation is not available.
                                               * Job requeued.*/
#define PEND_SLURM_RESREQ_BAD_CONSTRAINT 5557 /**< Invalid grammar in SLURM constraints
                                               * option, job will never run.*/

/* Cray X1 pending reasons (5600 - 5649) */
#define PEND_CRAYX1_SSP                5600   /**< Not enough SSPs for job*/
#define PEND_CRAYX1_MSP                5601   /**< Not enough MSPs for job*/
#define PEND_CRAYX1_PASS_LIMIT         5602   /**< Unable to pass limit information
                                               * to psched.*/

/* Cray Linux pending reasons (5650 - 5699) */
#define PEND_CRAYLINUX_ASSIGN_FAIL       5650   /**< Unable to reserve confirm on Cray Linux */

/* BlueGene pending reasons (5700 - 5749) */
#define PEND_BLUEGENE_PLUGIN_INTERNAL   5700  /**< BG/L: Scheduler plug-in internal error.*/
#define PEND_BLUEGENE_ALLOC_UNAVAIL     5701  /**< BG/L: Allocation is not available.
                                               * Job requeued.*/ 
#define PEND_BLUEGENE_NOFREEMIDPLANES   5702  /**< BG/L: No free base partitions
                                               * available for a full block allocation.*/
#define PEND_BLUEGENE_NOFREEQUARTERS    5703  /**< BG/L: No free quarters available
                                               * for a small block allocation.*/
#define PEND_BLUEGENE_NOFREENODECARDS   5704  /**< BG/L: No free node cards available
                                               * for a small block allocation.*/

/* resize enhancement releated pending reasons */
#define PEND_RESIZE_FIRSTHOSTUNAVAIL    5705  /**< First execution host unavailable */
#define PEND_RESIZE_MASTERSUSP          5706  /**< Master is not in the RUN state */
#define PEND_RESIZE_MASTER_SAME         5707  /**< Host is not same as for master */
#define PEND_RESIZE_SPAN_PTILE          5708  /**< Host already used by master */
#define PEND_RESIZE_SPAN_HOSTS          5709  /**< The job can only use first host */
#define PEND_RESIZE_LEASE_HOST          5710  /**< The job cannot get slots on remote hosts */

/* EGO-Enabled SLA pending reasons (5750 - 5799) */
#define PEND_PS_PLUGIN_INTERNAL   5750      /**< Host does not have enough slots
                                             * for this SLA job.*/
#define PEND_PS_MBD_SYNC          5751      /**< EGO SLA: Failed to synchronize
                                             * resource with MBD.*/

/* compound/alternative resreq related pending reasons (5800 - 5899) */
#define PEND_COMPOUND_RESREQ_OLD_LEASE_HOST 5800  /**< The job cannot get slots on 
                                                   * pre-7Update5 remote hosts */
#define PEND_COMPOUND_RESREQ_TOPLIB_HOST    5801 /**< Hosts using LSF system 
						  * integrations do not support
						  * compound resource
						  * requirements. */
#define PEND_ALTERNATIVE_RESREQ_OLD_LEASE_HOST  5802  /**< The job cannot get
						       * slots on pre-9.1
						       * remote hosts */

#define PEND_FWD_COMPOUND_RESREQ  5803 /**< The job cannot be 
					* forwarded to a
					* pre-7Update5 cluster
					*/
#define PEND_FWD_ALTERNATIVE_RESREQ 5804 /**< The job cannot be 
					  * forwarded to a
					  * pre-9.1 cluster
					  */
#define PEND_ALTERNATIVE_RESREQ_TOPLIB_HOST 5805 /**< Hosts using LSF system 
						  * integrations do not support
						  * alternative resource
						  * requirements. */

/* multi-phase resreq related pending reasons (5900) */
#define PEND_MULTIPHASE_RESREQ_OLD_LEASE_HOST 5900  /**< The job cannot get slots on 
                                                     * pre-7Update6 remote hosts */
/* Affinity related pending reasons (5901 - 5910) */
#define PEND_AFFINITY_LACK_PU            5901    /**< There is not enough PU resource
						  * on the host. */
#define PEND_AFFINITY_LACK_NUMA_MEMORY   5902    /**< Host has not enough NUMA memory
						  * for NUMA binding */
#define PEND_NONE_AFFINITY_HOST          5903    /**< Not a affinity host */
#define PEND_AFFINITY_LEASE_IN           5904    /**< Affinity job does not run on
						  * lease-in hosts. */
#define PEND_NOT_REMOTE_AFFINITY_HOST    5905    /**< Remote cluster has no numa node */
#define PEND_AFFINITY_BIND_DISABLED_HOST 5906    /**< Affinity binding
                                                  * is not enabled on the host */

/* slot pool related pending reasons (5950 - 5999) */
#define PEND_QUEUE_SLOT_POOL_LIMIT      5950        /**< MAX_SLOTS_IN_POOL is reached */

/* guarantee resource requirement reasons (6000-6400) */
#define PEND_GUARANTEE_RSRC             6000 /**< Remaining resources are reserved for
                                              * guarantees */
#define PEND_GUARANTEE_SLOTS            6300 /**< Slots on this host are reserved
                                              * for guarantees */
#define PEND_GUARANTEE_HOSTS            6301 /**< This host is reserved for
                                              * guarantees */
#define PEND_GUARANTEE_SLOTS_PER_HOST   6302 /**< Per-host slot limit is reached
                                              * for host in guaranteed slot pool */
#define PEND_GUARANTEE_NONSLA           6303 /**< Remaining slots on host are reserved for
                                              * guarantee SLA jobs */
#define PEND_GUARANTEE_SLOTS_LIMIT      6304 /**< Slot limit on host and/or
					      * host group within resource
					      * pool */
#define PEND_GUARANTEE_SLOTS_PKG        6305 /**< Slots on this host reserved
                                              * for SLAs */
#define PEND_GUARANTEE_MEM_PKG          6306 /**< Memory (mem) on this host
					      * reserved for SLAs */
#define PEND_GUARANTEE_PKG              6307 /**<  Insufficient packages
                                              * available on host */

/* license project reasons (6400-6700) */
#define PEND_GENERAL_LIMIT_LIC_PROJECT	6400   /**< Resource limit defined on
                                                * license project has been reached.*/
#define PEND_GENERAL_LIMIT_LIC_PROJECT_END 6700   /**< place holder for limit reason for license project  */
#define PEND_PREEMPT_DELAY         6701 /**< The preemptive job is allowing a grace period before preemption */
#define PEND_GLB_MIXED_MODE                6702 /**< Job requests both project mode and cluster mode licenses */ 
#define PEND_EXTSCHED_REASON               6703  /**< External scheduler sets custome pending reason.*/ 
#define PEND_PREEMPT_RESUME_DELAY          6704 /**< Resume blocked while preemption occurs for a pending job. */

/* 
 * Dynamic Cluster Related Reasons (6800 - 6900)
 */
#define PEND_AC_HOST_MISSINFO       6800  /**< Dynamic Cluster host cannot be used 
                                           * for scheduling due to missing information */
#define PEND_AC_HOST_NO_PM_TEMPLATE 6801  /**< Dynamic Cluster host cannot provide 
                                           * the job-required physical machine template */
#define PEND_AC_HOST_NO_MACHTYPE    6802  /**< Dynamic Cluster host cannot provide 
                                           * the job-required machine type */
#define PEND_AC_HOST_NOT_ACJOB      6803  /**< Cannot schedule the job on the Dynamic 
                                           * Cluster host because there is no Dynamic 
					   * Cluster job requirement */
#define PEND_AC_BAD_PROV_REQ        6804  /**< Invalid Dynamic Cluster provisioning request */
#define PEND_AC_PROV_REQ_FAIL       6805  /**< Failed to execute Dynamic Cluster job 
                                           * provisioning request */
#define PEND_AC_LESS_MAXMEM         6806  /**< Dynamic Cluster server host does not have 
                                           * enough memory to satisfy job request */
#define PEND_AC_VM_POWERINGOFF      6807  /**< Some virtual machines are shutting down on 
                                           * Dynamic Cluster host, cannot be used for 
					   * scheduling */
#define PEND_AC_UNUSE_SLOTS         6808  /**< Dynamic Cluster host does not have enough job
                                           * slots because some virtual machine jobs are 
					   * in the middle of provisioning operations */
#define PEND_HOST_NO_AC_PLUGIN      6809  /**< LSF scheduler cannot schedule jobs to use 
                                           * Dynamic Cluster hosts because the Dynamic 
					   * Cluster scheduling plugin is not configured or
					   * not loaded */
#define PEND_AC_ACJOB_PREEMPTED     6810  /**< Dynamic Cluster virtual machine image is 
                                           * saved, job is waiting for rescheduling */
#define PEND_AC_ACJOB_RESTORE_FAIL  6811  /**< Failed to restore the Dynamic Cluster virtual
                                           * machine image, job is waiting for rescheduling */
#define PEND_AC_FAIL_CREATE_REQ     6812  /**< Failed to create the Dynamic Cluster machine 
                                           * provisioning request */
#define PEND_AC_RESTOREVM_NOT_SAME_RESGROUP  6813 /**< Dynamic Cluster host does not belong
                                           * to the same resource group and cannot be used 
					   * to restore the saved virtual machine */
#define PEND_AC_RESTOREVM_NOT_ACHOST 6814 /**< Host is not a Dynamic Cluster host and cannot
                                           * be used to restore saved virtual machines */
#define PEND_AC_HOST_NO_VM_TEMPLATE  6815 /**< Dynamic Cluster host does not have virtual 
                                           * machines for the job-required template or does
					   * not allow creating virtual machines 
					   * for the specified template */
#define PEND_AC_PM_INPROVISIONING    6816 /**< Dynamic Cluster host is powering on or under
                                           * provisioning */
#define PEND_AC_HOST_PM_TEMPLATE_NOMATCH 6817 /**< Dynamic Cluster host template does not 
                                           * match job requirements */
#define PEND_AC_PM_RESERVE_TEMPLATE  6018 /**< Dynamic Cluster host has been reserved by 
                                           * a job with different physical machine templates */
#define PEND_AC_NOT_READY            6819 /**< Dynamic Cluster host is not up and running */
#define PEND_AC_HOST_NOT_EMPTY       6820 /**< Cannot re-provision the Dynamic Cluster host
                                           * because it has running jobs */
#define PEND_AC_LSF_NOT_READY        6821 /**< LSF daemons on the Dynamic Cluster host are 
                                           * not ready to accept jobs */
#define PEND_AC_TTL_NOTEXPIRED       6822 /**< Dynamic Cluster host does not have enough 
                                           * resources because the idle virtual machines 
					   * cannot be re-provisioned to meet the job requirements: 
					   * There are idle virtual machines with a time 
					   * to live that are not yet expired
                                           */
#define PEND_AC_JOB_VM_NOSUSPENDED   6823 /**< Dynamic Cluster virtual machine job cannot be
                                           * restored because it is not in a suspended 
					   * state */
#define PEND_AC_UNABLE_REPROVISIONING 6824 /**< Unable to start re-provisioning */
#define PEND_AC_VMJOB_TOO_BIG_MEMSIZE 6825 /**< The memory requirement is larger than 
                                           * the amount that Dynamic Cluster currently offers */
#define PEND_AC_HOST_INELIGIBLE_FOR_REPROVISION  6826 /**< Ineligible for re-provisioning 
                                           * due to the threshold for failed provisioning 
					   * attempts */
#define PEND_AC_TTL_NOTEXPIRED_ACHOST 6827 /**< Unable to re-provision due to the value of 
                                           * the DC_MACHINE_MIN_TTL parameter */
#define PEND_AC_RMT_TEMPLATE_NOT_FOUND 6828 /**< Dynamic Cluster template is not found 
                                           * in the remote cluster */
#define PEND_AC_NOT_HYPERVISOR       6829 /**< Host is not a Dynamic Cluster hypervisor */
#define PEND_AC_TOO_BIG_MEMSIZE_VM_ON_HV 6830 /**< Dynamic Cluster hypervisor cannot provide
                                           * a virtual machine with the job's resource (mem)
					   * requirement larger than the amount that Dynamic
					   * Cluster currently offers */
#define PEND_AC_HOST_RESREQ_COMPOUND    6831 /**< Dynamic Cluster hosts cannot be used to satisfy compound or alternative resource requirements */
#define PEND_AC_VMJOB_RESREQ_COMPOUND    6832 /**< Dynamic Cluster VM jobs cannot use compound or alternative resource requirements */
#define PEND_AC_ACJOB_RESTART          6833      /**< DC job is restarting */
#define PEND_AC_ACJOB_RESTART_FAIL     6834      /**< DC job restart failed */


/* DS pending reasons (6901- 6999 */
#define PEND_DS_NO_SPACE    6901 /**< for producing job, the host has not enough storage space*/
#define PEND_DS_NO_ACCESS   6902 /**< for consuming job, the host has no access to the data, i.e. no distance*/
#define PEND_DS_STORAGE_UNAVAIL  6903 /**< storage's status is unavailable*/
#define PEND_DS_STORAGE_CLOSE    6904 /**< storage's status is closed*/
#define PEND_DS_UNREG_DATASET 6905 /**< dataset is not unregister*/
#define PEND_DS_NO_CLOSEST_STORAGE 6906 /**< dataset is not unregister*/
#define PEND_DS_SYNTAX_ERROR 6907 /**< syntax error in -extsched */

/* Grid broker pending reason reserved (7000 - 7099), please start from next.
 */
#define PEND_GB_MASTER_NOTRUNJOB        7000
#define PEND_GB_CLUSTER_DISCONNECTED    7001
#define PEND_GB_CLUSTER_QUEUE_INVALID   7002
#define PEND_GB_REMOTE_RESOURCE         7003
#define PEND_GB_REMOTEJOB_NOT_FORWARD   7004
#define PEND_GB_MAX_FORWARD             7005
#define PEND_GB_CLUSTER_EXCLUDED        7006
#define PEND_GB_CLUSTER_USR_SPEC        7007
#define PEND_GB_CLUSTER_UNLICENSED      7008
#define PEND_GB_NO_SHARED_RESOURCE      7009
#define PEND_GB_RESOURCE_LIMIT          7010
#define PEND_GB_COMPOUND_RESREQ         7011
#define PEND_GB_ALTERNATIVE_RESREQ      7012

/*
 * Jobs Resource Reservation Related Reasons (7101 - 7400)
 * PEND_HOST_JOB_RUSAGE_THRESHOLD_MORE -
 *     PEND_HOST_JOB_RUSAGE_THRESHOLD_MORE + PEND_RES_EXTEND_SIZE -1
 */
#define PEND_HOST_JOB_RUSAGE_THRESHOLD   7101 /**< The job's requirements for resource
                                       * reservation are not satisfied.*/

#define PEND_RES_DEFAULT_SIZE 300
#define PEND_RES_EXTEND_SIZE 1000

/* Currently, PEND_HOST_JOB_RUSAGE_THRESHOLD_MORE = 9001 */
#define PEND_HOST_JOB_RUSAGE_THRESHOLD_MORE  PEND_HOST_LOAD_MORE - PEND_RES_EXTEND_SIZE

#define PEND_HOST_LOAD_MORE  PEND_HOST_QUE_RUSAGE_MORE \
                                                     - PEND_RES_EXTEND_SIZE  
#define PEND_HOST_QUE_RUSAGE_MORE      PEND_HOST_JOB_RUSAGE_MORE \
                                                                 - PEND_RES_EXTEND_SIZE 
#define PEND_HOST_JOB_RUSAGE_MORE      PEND_GENERAL_LIMIT_USER_MORE \
                                                                 - PEND_RES_EXTEND_SIZE 
#define PEND_GENERAL_LIMIT_USER_MORE   PEND_GENERAL_LIMIT_QUEUE_MORE \
                                                                 - PEND_RES_EXTEND_SIZE 
#define PEND_GENERAL_LIMIT_QUEUE_MORE  PEND_GENERAL_LIMIT_PROJECT_MORE \
                                                                 - PEND_RES_EXTEND_SIZE 
#define  PEND_GENERAL_LIMIT_PROJECT_MORE  PEND_GENERAL_LIMIT_CLUSTER_MORE \
                                                                 - PEND_RES_EXTEND_SIZE 
#define PEND_GENERAL_LIMIT_CLUSTER_MORE   PEND_GENERAL_LIMIT_HOST_MORE \
                                                                 - PEND_RES_EXTEND_SIZE 
#define PEND_GENERAL_LIMIT_HOST_MORE  PEND_GUARANTEE_RSRC_MORE \
                                                                 - PEND_RES_EXTEND_SIZE 
#define PEND_GUARANTEE_RSRC_MORE  PEND_GENERAL_LIMIT_LIC_PROJECT_MORE  \
                                                           - PEND_RES_EXTEND_SIZE
#define PEND_GENERAL_LIMIT_LIC_PROJECT_MORE  PEND_RESERVED_REASON_END \
                                                           - PEND_RES_EXTEND_SIZE + 1

#define PEND_RESERVED_REASON_END         20000


/* LSF PE integration related pending reason (7401 - 7449) */
#define PEND_NETWORK_WINDOW_LIMIT   7401    /**< Not enough windows on the network */
#define PEND_NETWORK_JOB_DEDICATED  7402    /**< Dedicated job cannot run because the network is
                                              occupied by another POE job */
#define PEND_NETWORK_WINDOW_DEDICATED 7403  /**< Network window is occupied by another dedicated POE job */
#define PEND_NETWORK_WINDOW_FAILED  7404    /**< Network cannot be used due to network failure */
#define PEND_FWD_NETWORK_REQ        7405    /**< Cannot forward job to executions clusters whose
                                              version is old and no network aware scheduling support
                                              */
#define PEND_NETWORK_EXCEED_MAX_PROTOCOL_INSTANCE 7406 /**< Job's requested instance exceed
                                                         configured MAX_PROTOCOL_INSTANCES */
#define PEND_NETWORK_CHKPNT         7407    /**< Network job cannot be checkpointable */
#define PEND_NETWORK_DISABLED       7408    /**< Netowrk aware scheduling feature is not enabled */
#define PEND_NETWORK_LEASE_IN_HOST  7409    /**< Cannot run POE job on lease-in host */


/* LSF reserves pending reason number from 1 - 20000.
 * External plugin is suggested to use LSF's reserved pending reason
 * number. However, they can use pending reason number between 20001 - 25000
 * as customer specific pending reasons. In this case, bjobs -p will only show 
 * the reason number without detailed message
 */
#define PEND_CUSTOMER_MIN		20001   /**< Customized pending reason
                                                 * number between min and max.*/
#define PEND_CUSTOMER_MAX		25000   /**< Customized pending reason
                                                 * number between min and max.*/

#define PEND_MAX_REASONS 25001  /**< The maximum number of reasons */
/*@}*/

/**
* \addtogroup suspending_reasons  suspending_reasons
* suspending_reasons is part of pending_reasons
*/
/*@{*/
/* SUSPENDING REASONS */

/* User related reasons */
#define SUSP_USER_REASON      0x00000000   /**< Virtual code. Not a reason*/
#define SUSP_USER_RESUME      0x00000001   /**< The job is waiting to be re-scheduled
                                            * after being resumed by the user. */
#define SUSP_USER_STOP        0x00000002   /**< The user suspended the job. */

/* Queue and system related reasons */
#define SUSP_QUEUE_REASON     0x00000004   /**< Virtual code. Not a reason*/
#define SUSP_QUEUE_WINDOW     0x00000008   /**< The run window of the queue is
                                            * closed. */
#define SUSP_RESCHED_PREEMPT  0x00000010   /**< Suspended after preemption. The system
                                            * needs to re-allocate CPU utilization by
job priority. */
#define SUSP_HOST_LOCK        0x00000020     /**< The LSF administrator has locked
                                              * the execution host.*/
#define SUSP_LOAD_REASON      0x00000040     /**< A load index exceeds its threshold.
                                              * The subreasons field indicates which
                                              * indices. */
#define SUSP_MBD_PREEMPT      0x00000080     /**< The job was preempted by mbatchd
                                              * because of a higher priorty job. */
#define SUSP_SBD_PREEMPT      0x00000100     /**< Preempted by sbatchd. The job limit
                                              * of the host/user has been reached. */
#define SUSP_QUE_STOP_COND    0x00000200     /**< The suspend conditions of the queue, 
                                              * as specified by the STOP_COND parameter
                                              * in lsb.queues, are true. */
#define SUSP_QUE_RESUME_COND  0x00000400     /**< The resume conditions of the queue,
                                              * as specified by the RESUME_COND
                                              * parameter in lsb.queues, are false. */
#define SUSP_PG_IT            0x00000800     /**< The job was suspended due to the paging
                                              * rate and the host is not idle yet. */
#define SUSP_REASON_RESET     0x00001000     /**< Resets the previous reason.*/
#define SUSP_LOAD_UNAVAIL     0x00002000     /**< Load information on the execution hosts
                                              * is unavailable. */
#define SUSP_ADMIN_STOP       0x00004000     /**< The job was suspened by root or the
                                              * LSF administrator. */
#define SUSP_RES_RESERVE      0x00008000     /**< The job is terminated due to resource
                                              * limit. */     
#define SUSP_MBD_LOCK         0x00010000     /**< The job is locked by the mbatchd. */
#define SUSP_RES_LIMIT        0x00020000     /**< The job's requirements for resource 
                                              * reservation are not satisfied. */
#define SUSP_SBD_STARTUP        0x00040000   /**< The job is suspended while the sbatchd
                                              * is restarting. */
#define SUSP_HOST_LOCK_MASTER   0x00080000   /**< The execution host is locked by the
                                              * master LIM. */
#define SUSP_HOST_RSVACTIVE     0x00100000   /**< An advance reservation using the
       		                              *  host is active */
#define SUSP_DETAILED_SUBREASON 0x00200000   /**< There is a detailed reason in
       		                              * the subreason field */
    /* GLB suspending reason */
#define SUSP_GLB_LICENSE_PREEMPT 0x00400000  /**< The job is preempted by glb */

  /* Cray X1 suspend reasons */
#define SUSP_CRAYX1_POSTED           0x00800000  /**< Job not placed by Cray X1
                                                  *  psched */
#define SUSP_ADVRSV_EXPIRED          0x01000000  /**< Job suspended when its advance 
                                                   * reservation expired */                                                  
#define SUSP_STOP_RELEASE_JOB_SLOT       0x02000000  /**<bstop release job slot */

/* for bstop -r jobs */
#define RELEASING_SLOT 1
#define RELEASED_SLOT 2



/*@}*/
                                                                                                
/**
* \addtogroup suspending_subreasons  suspending_subreasons
* suspending_subreasons has the following options:
*/
/*@{*/                                             
#define SUB_REASON_RUNLIMIT     0x00000001   /**< Sub reason of SUSP_RES_LIMIT: RUNLIMIT
                                              * is reached. */
#define SUB_REASON_DEADLINE     0x00000002   /**< Sub reason of SUSP_RES_LIMIT: DEADLINE
                                              * is reached. */
#define SUB_REASON_PROCESSLIMIT 0x00000004   /**< Sub reason of SUSP_RES_LIMIT: PROCESSLIMIT
                                              * is reached. */ 
#define SUB_REASON_CPULIMIT     0x00000008   /**< Sub reason of SUSP_RES_LIMIT: CPULIMIT
                                              * is reached. */ 
#define SUB_REASON_MEMLIMIT     0x00000010   /**< Sub reason of SUSP_RES_LIMIT: MEMLIMIT
                                              * is reached. */ 
#define SUB_REASON_THREADLIMIT  0x00000020   /**< Sub reason of SUSP_RES_LIMIT: THREADLIMIT
                                              * is reached. */ 
#define SUB_REASON_SWAPLIMIT    0x00000040   /**< Sub reason of SUSP_RES_LIMIT: SWAPLIMIT
                                              * is reached. */ 
#define SUB_REASON_CRAYX1_ACCOUNTID  0x00000001  /**< Account ID does not match
                                                   * those allowed by the gate */
#define SUB_REASON_CRAYX1_ATTRIBUTE  0x00000002  /**< Attribute does not match
                                                  *  those allowed by the gate */
#define SUB_REASON_CRAYX1_BLOCKED    0x00000004  /**< Blocked by one or more
                                                   * gates */
#define SUB_REASON_CRAYX1_RESTART    0x00000008  /**< Application is in the
                                                   * process of being restarted
                                                  *  and it is under the control
                                                  *  of CPR */
#define SUB_REASON_CRAYX1_DEPTH      0x00000010  /**< Depth does not match those
                                                  *  allowed by the gate */
#define SUB_REASON_CRAYX1_GID        0x00000020  /**< GID does not match those
                                                  *  allowed by the gate */
#define SUB_REASON_CRAYX1_GASID      0x00000040  /**< No GASID is available */
#define SUB_REASON_CRAYX1_HARDLABEL  0x00000080  /**< Hard label does not match
                                                  *  those allowed by the gate */
#define SUB_REASON_CRAYX1_LIMIT      0x00000100  /**< Limit exceeded in regions
                                                 *   or domains */
#define SUB_REASON_CRAYX1_MEMORY     0x00000200  /**< Memory size does not match
                                                  *  those allowed by the gate */
#define SUB_REASON_CRAYX1_SOFTLABEL  0x00000400  /**< Soft label does not match
                                                 *   those allowed by the gate */
#define SUB_REASON_CRAYX1_SIZE       0x00000800  /**< Size gate (width times
                                                  *  depth larger than gate
                                                 *  allows) */
#define SUB_REASON_CRAYX1_TIME       0x00001000  /**< Time limit does not match
                                                   * those allowed by the gate */
#define SUB_REASON_CRAYX1_UID        0x00002000  /**< UID does not match those
                                                  *  allowed by the gate */
#define SUB_REASON_CRAYX1_WIDTH      0x00004000  /**< Width does not match those
                                                   * allowed by the gate */
/*@}*/                                                   


/*
 * EXITING REASONS: currently only to indicate exited due to 
 * 1) rerunnable job being restart from last chkpnt;
 * 2) being killed while execution host is unavailable 
 */
#define EXIT_NORMAL            0x00000000  /* Job finished normally */
#define EXIT_RESTART           0x00000001  /* Rerunnable job to be restarted */
#define EXIT_ZOMBIE            0x00000002  /* Job killed while host unavailable */
#define FINISH_PEND            0x00000004  /* Job is finished and put into pend 
                                              list */
#define EXIT_KILL_ZOMBIE       0x00000008  /* The job is killed while the
                                              execution host is unreach */
#define EXIT_ZOMBIE_JOB        0x00000010  /* The job in ZOMBIE is removed */
#define EXIT_RERUN             0x00000020  /* Rerun a job without creating
                                              a ZOMBIE job */
#define EXIT_NO_MAPPING        0x00000040 /* Remote job has no mapping user name 
                                             here */
#define EXIT_REMOTE_PERMISSION 0x00000080 /* Remote job has no permission
       		     running here */
#define EXIT_INIT_ENVIRON      0x00000100 /* Remote job cannot run locally 
                                             because of environment problem */
#define EXIT_PRE_EXEC          0x00000200 /* Remote job failed in pre_exec
       		     command */
#define EXIT_REQUEUE           0x00000400 /* The job is killed and will be
       		     later requeued */
#define EXIT_REMOVE            0x00000800 /* Job could not be killed but was 
       		     removed from system, and the job is put into the zombie job list */
#define EXIT_VALUE_REQUEUE     0x00001000 /* Requeue by exit value */
#define EXIT_CANCEL            0x00002000 /* Cancel request received from
                                             remote cluster. */
#define EXIT_MED_KILLED        0x00004000 /* MED killed job on web server */
#define EXIT_REMOTE_LEASE_JOB  0x00008000 /* Remote lease job exit on execution
                                             side, return to pend on submission */
#define EXIT_CWD_NOTEXIST       0x00010000   /* Exit when cwd does not exist*/

#define EXIT_REMOVE_NO_ZOMBIE       0x00020000   /* Job could not be killed but was
                     removed from system, and the job is not put into the zombie job list */

/* Mode indicating running in batch, js, or batch-js mode */
#define LSB_MODE_BATCH    0x1
#define LSB_MODE_JS       0x2
#ifdef BSUB_RD
#define LSB_MODE_BATCH_RD 0x4
#endif /* BSUB_RD */

#if defined(hpux) || defined(__hpux)
#define RLIMIT_CPU   0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA  2
#define RLIMIT_STACK 3
#define RLIMIT_CORE  4
#define RLIMIT_RSS   5
#define RLIMIT_NOFILE 6
#if !defined(HPUX1010)
#define RLIM_INFINITY   0x7fffffff
#endif
#endif

/*
 * Error codes for lsblib calls 
 * Each error code has its corresponding error message defined in lsb.err.c
 * The code number is just the position number of its message.
 * Adding a new code here must add its message there in the corresponding
 * position.  Changing any code number here must change the position there.
 */
/* Error codes related to job */
#define    LSBE_NO_ERROR         0  /* No error at all */
#define    LSBE_NO_JOB           1  /* No matching job found */
#define    LSBE_NOT_STARTED      2  /* Job not started yet */
#define    LSBE_JOB_STARTED      3  /* Job already started */
#define    LSBE_JOB_FINISH       4  /* Job already finished */
#define    LSBE_STOP_JOB         5  /* Ask sbatchd to stop the wrong job */
#define    LSBE_DEPEND_SYNTAX    6  /* Depend_cond syntax error */
#define    LSBE_EXCLUSIVE        7  /* Queue doesn't accept EXCLUSIVE job */
#define    LSBE_ROOT             8  /* Root is not allowed to submit jobs */
#define    LSBE_MIGRATION        9  /* Job is already being migrated */
#define    LSBE_J_UNCHKPNTABLE   10 /* Job is not chkpntable */
#define    LSBE_NO_OUTPUT        11 /* Job has no output so far */
#define    LSBE_NO_JOBID         12 /* No jobId can be used now */
#define    LSBE_ONLY_INTERACTIVE 13 /* Queue only accepts bsub -I job */
#define    LSBE_NO_INTERACTIVE   14 /* Queue doesn't accept bsub -I job */

/* Error codes related to user, queue and host */
#define    LSBE_NO_USER       15    /* No user defined in lsb.users file */
#define    LSBE_BAD_USER      16    /* Bad user name */
#define    LSBE_PERMISSION    17    /* User permission denied */
#define    LSBE_BAD_QUEUE     18    /* No such queue in the system */
#define    LSBE_QUEUE_NAME    19    /* Queue name should be given */
#define    LSBE_QUEUE_CLOSED  20    /* Queue has been closed */
#define    LSBE_QUEUE_WINDOW  21    /* Queue windows are closed */
#define    LSBE_QUEUE_USE     22    /* User cannot use the queue */
#define    LSBE_BAD_HOST      23    /* Bad host name or host group name" */
#define    LSBE_PROC_NUM      24    /* Too many processors requested */
#define    LSBE_NO_HPART      25    /* No host partition in the system */
#define    LSBE_BAD_HPART     26    /* Bad host partition name */
#define    LSBE_NO_GROUP      27    /* No group defined in the system */
#define    LSBE_BAD_GROUP     28    /* Bad host/user group name */
#define    LSBE_QUEUE_HOST    29    /* Host is not used by the queue */
#define    LSBE_UJOB_LIMIT    30    /* User reach UJOB_LIMIT of the queue */
#define    LSBE_NO_HOST       31    /* No host available for migration */

#define    LSBE_BAD_CHKLOG    32    /* chklog is corrupted */
#define    LSBE_PJOB_LIMIT    33    /* User reach PJOB_LIMIT of the queue */
#define    LSBE_NOLSF_HOST    34    /* request from non LSF host rejected*/

/* Error codes related to input arguments of lsblib call */
#define    LSBE_BAD_ARG       35    /* Bad argument for lsblib call */
#define    LSBE_BAD_TIME      36    /* Bad time spec for lsblib call */
#define    LSBE_START_TIME    37    /* Start time is later than end time */
#define    LSBE_BAD_LIMIT     38    /* Bad CPU limit specification */
#define    LSBE_OVER_LIMIT    39    /* Over hard limit of queue */
#define    LSBE_BAD_CMD       40    /* Empty job (command) */
#define    LSBE_BAD_SIGNAL    41    /* Bad signal value; not supported */
#define    LSBE_BAD_JOB       42    /* Bad job name */
#define    LSBE_QJOB_LIMIT    43    /* Queue reach QJOB_LIMIT of the queue */
#define    LSBE_BAD_TERM    44    /* Expired job terminate time*/
/* 44 is reserved for future use */
                                 
/* Error codes related to lsb.events file */
#define    LSBE_UNKNOWN_EVENT 45    /* Unknown event in event log file */
#define    LSBE_EVENT_FORMAT  46    /* bad event format in event log file */
#define    LSBE_EOF           47    /* End of file */
/* 48-49 are reserved for future use */

/* Error codes related to system failure */
#define    LSBE_MBATCHD       50    /* mbatchd internal error */
#define    LSBE_SBATCHD       51    /* sbatchd internal error */
#define    LSBE_LSBLIB        52    /* lsbatch lib internal error */
#define    LSBE_LSLIB         53    /* LSLIB call fails */
#define    LSBE_SYS_CALL      54    /* System call fails */
#define    LSBE_NO_MEM        55    /* Cannot alloc memory */
#define    LSBE_SERVICE       56    /* Lsbatch service not registered */
#define    LSBE_NO_ENV        57    /* LSB_SHAREDIR not defined */
#define    LSBE_CHKPNT_CALL   58    /* chkpnt system call fail */
#define    LSBE_NO_FORK       59    /* mbatchd cannot fork */

/* Error codes related to communication between mbatchd/lsblib/sbatchd */
#define    LSBE_PROTOCOL      60    /* LSBATCH protocol error */
#define    LSBE_XDR           61    /* XDR en/decode error */
#define    LSBE_PORT          62    /* No appropriate port can be bound */
#define    LSBE_TIME_OUT      63    /* Timeout in contacting mbatchd */
#define    LSBE_CONN_TIMEOUT  64    /* Timeout on connect() call */
#define    LSBE_CONN_REFUSED  65    /* Connection refused by server */
#define    LSBE_CONN_EXIST    66    /* server connection already exists */
#define    LSBE_CONN_NONEXIST 67    /* server is not connected */
#define    LSBE_SBD_UNREACH   68    /* sbatchd cannot be reached */
#define    LSBE_OP_RETRY      69    /* Operation cannot be performed right
       	       now, op. will be retried. */
#define    LSBE_USER_JLIMIT   70    /* user has no enough job slots */
/* 71 is reserved for future use */

/* Error codes related to NQS */
#define    LSBE_NQS_BAD_PAR   72    /* Bad specification for a NQS job */ 

#define    LSBE_NO_LICENSE    73    /* Client host has no license */

/* Error codes related to calendar */
#define    LSBE_BAD_CALENDAR     74  /* Bad calendar name */
#define    LSBE_NOMATCH_CALENDAR 75  /* No calendar found */
#define    LSBE_NO_CALENDAR      76  /* No calendar in system */
#define    LSBE_BAD_TIMEEVENT    77  /* Bad calendar time events */
#define    LSBE_CAL_EXIST        78  /* Calendar exist already */
#define    LSBE_CAL_DISABLED     79  /* Calendar function is not enabled*/

/* Error codes related to modify job's parameters */
#define    LSBE_JOB_MODIFY       80  /* the job's params cannot be changed */
#define    LSBE_JOB_MODIFY_ONCE  81  /* the changed once parameters are not
                                        used */

#define    LSBE_J_UNREPETITIVE   82  /* the job is not a repetitive job */
#define    LSBE_BAD_CLUSTER      83  /* bad cluster name */

/* Error codes related jobs driven by calendar  */
#define    LSBE_PEND_CAL_JOB     84  /* Job can not be killed in pending */
#define    LSBE_RUN_CAL_JOB      85  /* This Running turn is being
       		terminated */

#define    LSBE_JOB_MODIFY_USED  86  /* Modified parameters are being used */
#define    LSBE_AFS_TOKENS       87  /* Can not get user's token */ 

/* Error codes related to event */
#define    LSBE_BAD_EVENT        88  /* Bad event name */
#define    LSBE_NOMATCH_EVENT    89  /* No event found */
#define    LSBE_NO_EVENT         90  /* No event in system */

/* Error codes related to user, queue and host */
#define    LSBE_HJOB_LIMIT       91  /* User reach HJOB_LIMIT of the queue */

/* Error codes related to bmsg */
#define    LSBE_MSG_DELIVERED    92  /* Message delivered */
#define    LSBE_NO_JOBMSG        93  /* MBD could not find the message
       		that SBD mentions about */
#define    LSBE_MSG_RETRY        94  /* x */

/* Error codes related to resource requirement */
#define    LSBE_BAD_RESREQ       95  /* Bad resource requirement */

#define    LSBE_NO_ENOUGH_HOST   96  /* No enough hosts */

/* Error codes related to configuration lsblib call */
#define    LSBE_CONF_FATAL       97  /* Fatal error in reading conf files  */
#define    LSBE_CONF_WARNING     98  /* Warning error in reading conf files  */


#define    LSBE_CAL_MODIFY       99  /* CONF used calendar cannot be modified */
#define    LSBE_JOB_CAL_MODIFY  100  /* Job created calendar cannot be modified  */
#define    LSBE_HP_FAIRSHARE_DEF    101  /* FAIRSHARE queue or  
                                           HPART defined */
#define    LSBE_NO_RESOURCE         102  /* No resource specified */
#define    LSBE_BAD_RESOURCE        103  /* Bad resource name */
#define    LSBE_INTERACTIVE_CAL     104  /* Calendar not allowed for interactive
       		    job */
#define    LSBE_INTERACTIVE_RERUN   105  /* Interactive job cannot be
       		    rerunnable */
#define    LSBE_PTY_INFILE          106  /* PTY and infile specified */
#define    LSBE_JS_DISABLED         107  /* JobScheduler is disabled */

#define    LSBE_BAD_SUBMISSION_HOST 108  /* Submission host and its host type
       		    can not be found any more */
#define    LSBE_LOCK_JOB            109  /* Lock the job so that it cann't be
       		    resume by sbatchd */
#define    LSBE_UGROUP_MEMBER       110  /* user not in the user group */
#define    LSBE_UNSUPPORTED_MC      111  /* Operation not supported for a
       		    Multicluster job */
#define    LSBE_PERMISSION_MC       112  /* Operation permission denied for a
       		    Multicluster job */
#define    LSBE_SYSCAL_EXIST        113  /* System Calendar exist already */
#define    LSBE_OVER_RUSAGE         114  /* exceed q's resource reservation */
#define    LSBE_BAD_HOST_SPEC       115  /* bad host spec of run/cpu limits */
#define    LSBE_SYNTAX_CALENDAR     116  /* calendar syntax error */
#define    LSBE_CAL_USED            117  /* delete a used calendar */
#define    LSBE_CAL_CYC             118  /* cyclic calednar dependence */
#define    LSBE_BAD_UGROUP          119  /* bad user group name */
#define    LSBE_ESUB_ABORT          120  /* esub aborted request */     
#define    LSBE_EXCEPT_SYNTAX       121  /* Bad exception handler syntax */  
#define    LSBE_EXCEPT_COND         122  /* Bad exception condition
       		   specification */
#define    LSBE_EXCEPT_ACTION       123  /* Bad or invalid action
       		    specification */     
#define    LSBE_JOB_DEP             124  /* job dependence, not deleted immed */
/* error codes for job group */
#define    LSBE_JGRP_EXIST          125  /* the job group exists */
#define    LSBE_JGRP_NULL           126  /* the job group doesn't exist */
#define    LSBE_JGRP_HASJOB         127  /* the group contains jobs */
#define    LSBE_JGRP_CTRL_UNKWN     128  /* the unknown group control signal */
#define    LSBE_JGRP_BAD            129  /* Bad Job Group name */
#define    LSBE_JOB_ARRAY           130  /* Job Array */
#define    LSBE_JOB_SUSP            131  /* Suspended job not supported */
#define    LSBE_JOB_FORW            132  /* Forwarded job not suported */
#define    LSBE_JGRP_HOLD           133  /* parent group is held */
#define    LSBE_BAD_IDX             134  /* bad index */
#define    LSBE_BIG_IDX             135  /* index too big */
#define    LSBE_ARRAY_NULL          136  /* job array not exist*/
#define    LSBE_CAL_VOID            137  /* Void calendar */
#define    LSBE_JOB_EXIST           138  /* the job exists */
#define    LSBE_JOB_ELEMENT         139  /* Job Element fail */
#define    LSBE_BAD_JOBID           140  /* Bad jobId */
#define    LSBE_MOD_JOB_NAME        141  /* cannot change job name */

/* error codes for frame job */
#define    LSBE_BAD_FRAME           142  /* Bad frame expression */
#define    LSBE_FRAME_BIG_IDX       143  /* Frame index too long */
#define    LSBE_FRAME_BAD_IDX       144  /* Frame index syntax error */

#define    LSBE_PREMATURE           145  /* child process died */

/* error code for user not in project group */
#define    LSBE_BAD_PROJECT_GROUP   146  /* Invoker is not in project group */

/* error code for user group / host group */
#define    LSBE_NO_HOST_GROUP       147  /* No host group defined in the system */
#define    LSBE_NO_USER_GROUP       148  /* No user group defined in the system */
#define    LSBE_INDEX_FORMAT        149   /* Bad jobid index file format */

/* error codes for IO_SPOOL facility */
#define    LSBE_SP_SRC_NOT_SEEN     150  /* source file does not exist */
#define    LSBE_SP_FAILED_HOSTS_LIM 151  /* Number of failed spool hosts reached max */
#define    LSBE_SP_COPY_FAILED      152  /* spool copy failed for this host*/
#define    LSBE_SP_FORK_FAILED      153  /* fork failed */
#define    LSBE_SP_CHILD_DIES       154  /* status of child is not available */
#define    LSBE_SP_CHILD_FAILED     155  /* child terminated with failure */
#define    LSBE_SP_FIND_HOST_FAILED 156  /* Unable to find a host for spooling */
#define    LSBE_SP_SPOOLDIR_FAILED  157  /* Cannot get $JOB_SPOOLDIR for this host */
#define    LSBE_SP_DELETE_FAILED    158  /* Cannot delete spool file for this host */
 
#define    LSBE_BAD_USER_PRIORITY   159  /* Bad user priority */
#define    LSBE_NO_JOB_PRIORITY     160  /* Job priority control undefined */
#define    LSBE_JOB_REQUEUED        161  /* Job has been killed & requeued */
#define    LSBE_JOB_REQUEUE_REMOTE  162  /* Remote job cannot kill-requeued */
#define    LSBE_NQS_NO_ARRJOB       163  /* Cannot submit job array to a NQS queue */  

/* error codes for EXT_JOB_STATUS */
#define    LSBE_BAD_EXT_MSGID       164  /* No message available */ 
#define    LSBE_NO_IFREG	    165  /* Not a regular file */
#define    LSBE_BAD_ATTA_DIR  	    166  /* MBD fail to create files in the directory*/ 
#define    LSBE_COPY_DATA           167  /* Fail to transfer data */  
#define    LSBE_JOB_ATTA_LIMIT      168  /* exceed the limit on data transferring of a msg*/  
#define    LSBE_CHUNK_JOB           169  /* cannot resize a chunk job, cannot bswitch a run/wait job */

/* Error code used in communications with dlogd */

#define    LSBE_DLOGD_ISCONN        170  /* dlogd is already connected */ 

/* Error code for LANL3_1ST_HOST */
#define    LSBE_MULTI_FIRST_HOST    171  /* Multiple first execution host */
#define    LSBE_HG_FIRST_HOST       172  /* Host group as first execution host */
#define    LSBE_HP_FIRST_HOST       173  /* Host partition as first execution host */
#define    LSBE_OTHERS_FIRST_HOST   174  /* "others" as first execution host */

/* error code for multi-cluster: remote only queue */
#define    LSBE_MC_HOST             175  /* cannot specify exec host */
#define    LSBE_MC_REPETITIVE       176  /* cannot specify repetitive job */
#define    LSBE_MC_CHKPNT           177  /* cannot be a chkpnt job */
#define    LSBE_MC_EXCEPTION        178  /* cannot specify exception */
#define    LSBE_MC_TIMEEVENT        179  /* cannot specify time event */
#define    LSBE_PROC_LESS     	    180  /* Too few processors requested */
#define    LSBE_MOD_MIX_OPTS        181  /* bmod pending options and running
       		    options together towards running job */
#define    LSBE_MOD_REMOTE          182  /* cannot bmod remote running job */
#define    LSBE_MOD_CPULIMIT        183  /* cannot bmod cpulimit without
LSB_JOB_CPULIMIT defined */
#define    LSBE_MOD_MEMLIMIT        184  /* cannot bmod memlimit without
LSB_JOB_MEMLIMIT defined */
#define    LSBE_MOD_ERRFILE         185  /* cannot bmod err file name */
#define    LSBE_LOCKED_MASTER       186  /* host is locked by master LIM*/
#define    LSBE_WARNING_INVALID_TIME_PERIOD 187 /* warning time period is 
       		           invalid */
#define    LSBE_WARNING_MISSING     188  /* either warning time period or
       		    warning action is not specified */
#define    LSBE_DEP_ARRAY_SIZE      189  /* The job arrays involved in 
       		  * one to one dependency do not
       		  *  have the same size.
       		  */ 
#define    LSBE_FEWER_PROCS         190  /* Not enough processors to be reserved (lsb_addreservation()) */
#define    LSBE_BAD_RSVID           191  /* Bad reservation ID */
#define    LSBE_NO_RSVID            192  /* No more reservation IDs can be used now */
#define    LSBE_NO_EXPORT_HOST      193  /* No hosts are exported */
#define    LSBE_REMOTE_HOST_CONTROL   194  /* Trying to control remote hosts*/
#define    LSBE_REMOTE_CLOSED       195   /*Can't open a remote host closed by the remote cluster admin */
#define    LSBE_USER_SUSPENDED     196    /* User suspended job */
#define    LSBE_ADMIN_SUSPENDED    197    /* Admin suspended job */
#define    LSBE_NOT_LOCAL_HOST     198    /* Not a local host name in 
       		   * bhost -e command 
       		   */
#define    LSBE_LEASE_INACTIVE	    199	  /* The host's lease is not active. */
#define    LSBE_QUEUE_ADRSV	    200	  /* The advance reserved host is not on queue. */
#define    LSBE_HOST_NOT_EXPORTED   201   /* The specified host(s) is not exported. */
#define    LSBE_HOST_ADRSV	    202	  /* The user specified host is not inn advance reservation */
#define    LSBE_MC_CONN_NONEXIST    203    /* The remote cluster is not connected */
#define    LSBE_RL_BREAK	    204  /* The general resource limit broken */

/* ---- The following RMS errors are obsoleted in Eagle */
#define    LSBE_LSF2TP_PREEMPT      205 /* cannot submit a job with special
       		 * topology requirement to a
       		 * preemptive queue
       		 */
#define    LSBE_LSF2TP_RESERVE      206 /* cannot submit a job with special
       		 * topology requirement to a queue
       		 * with slot reservation
       		 */
#define    LSBE_LSF2TP_BACKFILL     207 /* cannot submit a job with special
       		 * topology requirement to a queue
       		 * with backill
       		 */      
/* ---- The above RMS errors are obsoleted in Eagle */
#define     LSBE_RSV_POLICY_NAME_BAD     208 /* none existed policy name */
#define     LSBE_RSV_POLICY_PERMISSION_DENIED        209 /* All normal user has no privilege */
#define     LSBE_RSV_POLICY_USER        210 /* user has no privilege */
#define     LSBE_RSV_POLICY_HOST       211 /* user has no privilege to create reservation on host */
#define     LSBE_RSV_POLICY_TIMEWINDOW        212 /* time window is not allowed by policy */
#define     LSBE_RSV_POLICY_DISABLED   213 /* the feature is disabled */
  /* the general limit related errors */
#define LSBE_LIM_NO_GENERAL_LIMIT 	214 /* There are no general limit defined */
#define LSBE_LIM_NO_RSRC_USAGE 215  /* There are no resource usage */
#define LSBE_LIM_CONVERT_ERROR 216  /* Convert data error */
#define LSBE_RSV_NO_HOST       217  /* There are no qualified host found in cluster*/
#define LSBE_MOD_JGRP_ARRAY    218  /* Cannot modify job group on element of job array */
#define LSBE_MOD_MIX           219  /* Cannot combine modify job group or service class option with others */
#define LSBE_SLA_NULL          220  /* the service class doesn't exist */
#define LSBE_MOD_JGRP_SLA      221  /* Modify job group for job in service class is not supported*/
#define LSBE_SLA_MEMBER        222  /* User or user group is not a member of the specified service class */ 
#define LSBE_NO_EXCEPTIONAL_HOST 223 /* There is no exceptional host found */
#define LSBE_WARNING_INVALID_ACTION 224 /* warning action (signal) is invalid */
  
#define LSBE_EXTSCHED_SYNTAX    225 /* Extsched option syntax error */ 
#define LSBE_SLA_RMT_ONLY_QUEUE 226 /* SLA doesn't work with remote only queues  */
#define LSBE_MOD_SLA_ARRAY      227 /* Cannot modify service class on element of job array */
#define LSBE_MOD_SLA_JGRP       228 /* Modify service class for job in job group is not supported*/
#define LSBE_MAX_PEND           229 /* Max. Pending job error */
#define LSBE_CONCURRENT         230 /* System concurrent query exceeded */
#define LSBE_FEATURE_NULL       231 /* Requested feature not enabled */

#define LSBE_DYNGRP_MEMBER      232 /* Host is already member of group */
#define LSBE_BAD_DYN_HOST       233 /* Host is not a dynamic host */
#define LSBE_NO_GRP_MEMBER      234 /* Host was not added with badmin hghostadd */
#define LSBE_JOB_INFO_FILE      235 /* Cannot create job info file */
#define LSBE_MOD_OR_RUSAGE      236 /* Cannot modify rusage to a new || (or) expression after the job is dispatched */
#define LSBE_BAD_GROUP_NAME     237 /* Bad host group name */
#define LSBE_BAD_HOST_NAME      238 /* Bad host name */
#define LSBE_DT_BSUB            239 /* Bsub is not permitted on DT cluster */

#define LSBE_PARENT_SYM_JOB	240 /* The parent symphony job/group was 
                                     * gone when submitting jobs
                                  */
#define LSBE_PARTITION_NO_CPU   241 /* The partition has no cpu alllocated */
#define LSBE_PARTITION_BATCH	242 /* batch partition does not accept online jobs: obsolete */
#define LSBE_PARTITION_ONLINE   243 /* online partition does not accept batch jobs */
#define LSBE_NOLICENSE_BATCH    244 /* no batch licenses */
#define LSBE_NOLICENSE_ONLINE   245 /* no online licenses */
#define LSBE_SIGNAL_SRVJOB      246 /* signal is not supported for service job */
#define LSBE_BEGIN_TIME_INVALID 247 /* the begin time is not later than current time. */
#define LSBE_END_TIME_INVALID   248 /* the end time is not later than current time. */
#define LSBE_BAD_REG_EXPR       249 /* Bad regular expression */

#define LSBE_GRP_REG_EXPR       250 /* Host group has regular expression */
#define LSBE_GRP_HAVE_NO_MEMB   251 /* Host group have no member */ 
#define LSBE_APP_NULL           252 /* the application doesn't exist */
#define LSBE_PROC_JOB_APP       253 /* job's proclimit rejected by App */ 
#define LSBE_PROC_APP_QUE       254 /* app's proclimit rejected by Queue */ 
#define LSBE_BAD_APPNAME        255 /* application name is too long  */ 
#define LSBE_APP_OVER_LIMIT     256 /* Over hard limit of queue */
#define LSBE_REMOVE_DEF_APP     257 /* Cannot remove default application */
#define LSBE_EGO_DISABLED       258 /* Host is disabled by EGO */
#define LSBE_REMOTE_HOST        259 /* Host is a remote host. Remote hosts cannot be added to a local host group. */
#define LSBE_SLA_EXCLUSIVE      260 /* SLA is exclusive, only accept exclusive job. */
#define LSBE_SLA_NONEXCLUSIVE   261 /* SLA is non-exclusive, only accept non-exclusive job */
#define LSBE_PERFMON_STARTED	262 /* The feature has already been started */
#define LSBE_PERFMON_STOPED	263 /* The Featurn has already been turn down */
#define LSBE_PERFMON_PERIOD_SET 264 /* Current sampling period is already set to %%s,seconds. Ignored*/
#define LSBE_DEFAULT_SPOOL_DIR_DISABLED 265 /* Default spool dir is disabled */
#define LSBE_APS_QUEUE_JOB      266 /* job belongs to an APS queue and cannot be moved */
#define LSBE_BAD_APS_JOB        267 /* job is not in an absolute priority enabled queue */
#define LSBE_BAD_APS_VAL        268 /* Wrong aps admin value */
#define LSBE_APS_STRING_UNDEF   269 /* Trying to delete a non-existent APS string */
#define LSBE_SLA_JOB_APS_QUEUE  270 /* A job cannot be assigned an SLA and an APS queue with factor FS */
#define LSBE_MOD_MIX_APS        271 /* bmod -aps | -apsn option cannot be mixed with other option */
#define LSBE_APS_RANGE          272 /* specified ADMIN factor/system APS value out of range */
#define LSBE_APS_ZERO          273 /* specified ADMIN factor/system APS value is zero */

#define LSBE_DJOB_RES_PORT_UNKNOWN         274 /* res port is unknown */
#define LSBE_DJOB_RES_TIMEOUT              275 /* timeout on res communication */
#define LSBE_DJOB_RES_IOERR                276 /* I/O error on remote stream */
#define LSBE_DJOB_RES_INTERNAL_FAILURE     277 /* res internal failure */

#define LSBE_DJOB_CAN_NOT_RUN              278 /* can not run outside LSF */
#define LSBE_DJOB_VALIDATION_BAD_JOBID     279 /* distributed job's validation failed due to incorrect job ID or index */
#define LSBE_DJOB_VALIDATION_BAD_HOST      280 /* distributed job's validation failed due to incorrect host selection */ 
#define LSBE_DJOB_VALIDATION_BAD_USER      281 /* distributed job's validation failed due to incorrect user */
#define LSBE_DJOB_EXECUTE_TASK             282 /* failed while executing tasks */
#define LSBE_DJOB_WAIT_TASK                283 /* failed while waiting for tasks to finish*/

#define LSBE_APS_HPC      284 /* Obsoleted error: HPC License not exist */
#define LSBE_DIGEST_CHECK_BSUB  285 /* Integrity check of bsub command failed */
#define LSBE_DJOB_DISABLED  286 /* Distributed Application Framework disabled */

/* Error codes related to runtime estimation and cwd */
#define  LSBE_BAD_RUNTIME     287    /* Bad runtime specification */
#define  LSBE_BAD_RUNLIMIT    288    /* RUNLIMIT: Cannot exceed RUNTIME*JOB_RUNLIMIT_RATIO */
#define  LSBE_OVER_QUEUE_LIMIT     289    /* RUNTIME: Cannot exceed the hard runtime limit in the queue */
#define  LSBE_SET_BY_RATIO     290  /* RUNLIMIT: Is not set by command line */
#define  LSBE_BAD_CWD             291  /* current working directory name too long */

#define LSBE_JGRP_LIMIT_GRTR_THAN_PARENT 292 /* Job group limit is greater than its parent group */
#define LSBE_JGRP_LIMIT_LESS_THAN_CHILDREN 293 /* Job group limit is less than its children groups */
#define LSBE_NO_ARRAY_END_INDEX 294 /* Job Array end index should be specified explicitly */
#define LSBE_MOD_RUNTIME        295 /* cannot bmod runtime without LSB_MOD_ALL_JOBS=y defined */

/* EP3 */
#define LSBE_BAD_SUCCESS_EXIT_VALUES 296
#define LSBE_DUP_SUCCESS_EXIT_VALUES 297
#define LSBE_NO_SUCCESS_EXIT_VALUES 298

#define LSBE_JOB_REQUEUE_BADARG 299
#define LSBE_JOB_REQUEUE_DUPLICATED 300
#define LSBE_JOB_REQUEUE_INVALID_DIGIT 301 /* "all" with number */
#define LSBE_JOB_REQUEUE_INVALID_TILDE 302 /* ~digit without "all" */
#define LSBE_JOB_REQUEUE_NOVALID 303

#define LSBE_NO_JGRP            304 /* No matching job group found */
#define LSBE_NOT_CONSUMABLE     305

/* AR pre/post */
#define LSBE_RSV_BAD_EXEC       306 /* Cannot parse an Advance Reservation -exec string */
#define LSBE_RSV_EVENTTYPE      307 /* Unknown AR event type */
#define LSBE_RSV_SHIFT          308 /* pre/post cannot have postive offset */
#define LSBE_RSV_USHIFT         309 /* pre-AR command cannot have offset < 0 in user-created AR */
#define LSBE_RSV_NUMEVENTS      310 /* only one pre- and one post- cmd permitted per AR */

/*Error codes related to AR Modification*/
#define LSBE_ADRSV_ID_VALID          311 /* ID does not correspond to a known AR. */
#define LSBE_ADRSV_DISABLE_NONRECUR  312 /* disable non-recurrent AR. */
#define LSBE_ADRSV_MOD_ACTINSTANCE   313 /* modification is rejected because AR is activated. */
#define LSBE_ADRSV_HOST_NOTAVAIL     314 /* modification is rejected because host slots is not available. */
#define LSBE_ADRSV_TIME_MOD_FAIL     315 /* the  time of the AR cannot be modified since resource is not available. */
#define LSBE_ADRSV_R_AND_N           316 /* resource requirement (-R) must be followed a slot requirment (-n) */
#define LSBE_ADRSV_EMPTY             317 /*modification is rejected because trying to empty the AR. */
#define LSBE_ADRSV_SWITCHTYPE        318 /*modification is rejected because switching AR type. */
#define LSBE_ADRSV_SYS_N             319 /*modification is rejected because specifying -n for system AR. */
#define LSBE_ADRSV_DISABLE           320 /* disable string is not valid. */
#define LSBE_ADRSV_ID_UNIQUE         321 /* Unique AR ID required */
#define LSBE_BAD_RSVNAME             322 /* Bad reservation name */
#define LSBE_ADVRSV_ACTIVESTART      323 /* Cannot change the start time of an active reservation. */
#define LSBE_ADRSV_ID_USED           324 /* AR ID is refernced by a job */
#define LSBE_ADRSV_PREVDISABLED      325 /* the disable period has already been disabled */
#define LSBE_ADRSV_DISABLECURR       326 /* an active period of a recurring reservation cannot be disabled */
#define LSBE_ADRSV_NOT_RSV_HOST      327 /* modification is rejected because specified hosts or host groups do not belong to the reservation */

/*new parser */
#define LSBE_RESREQ_OK               328 /*checking resreq return ok */
#define LSBE_RESREQ_ERR              329 /*checking resreq return error */

#define LSBE_ADRSV_HOST_USED         330 /* modification is rejected because reservation has running jobs on the specified hosts or host groups */

#define LSBE_BAD_CHKPNTDIR           331 /* The checkpoint directory is too long */
#define LSBE_ADRSV_MOD_REMOTE        332 /* trying to modify in a remote cluster */
#define LSBE_JOB_REQUEUE_BADEXCLUDE  333
#define LSBE_ADRSV_DISABLE_DATE      334 /* trying to disable for a date in the past */
#define LSBE_ADRSV_DETACH_MIX        335 /* cannot mix the -Un option with others for started jobs */
#define LSBE_ADRSV_DETACH_ACTIVE     336 /* cannot detach a started job when the reservation is active */
#define LSBE_MISSING_START_END_TIME  337 /* invalid time expression: must specify day for both start and end time */
#define LSBE_JOB_RUSAGE_EXCEED_LIMIT 338 /* Queue level limitation */
#define LSBE_APP_RUSAGE_EXCEED_LIMIT 339 /* Queue level limitation */
#define LSBE_CANDIDATE_HOST_EMPTY    340 /* Hosts and host groups specified by -m are not used by the queue */
#define LSBE_HS_BAD_AFTER_BRACKT 341 /* An int must follow an open bracket */
#define LSBE_HS_NO_END_INDEX 342 /* An end index must follow a dash */
#define LSBE_HS_BAD_COMMA 343 /* Integers must come before and after the comma */
#define LSBE_HS_BAD_FORMAT 344 /* Incorrect condensed host specification */
#define LSBE_HS_BAD_ORDER 345 /* The start index must be less than end index */
#define LSBE_HS_BAD_MANY_DIGITS 346 /* The end index must be less than 10 digits */
#define LSBE_HS_BAD_NUM_DIGITS 347 /* Number of digits in the start index must be less than that of end index */
#define LSBE_HS_BAD_END_INDEX 348 /* The end index cannot start with zero (0) */
#define LSBE_HS_BAD_INDEX 349 /* Index must be an integer or a range */

/* host group admin*/
#define LSBE_COMMENTS                350 /* When a Host Group Admin (badmin hclose or hopen) closes or opens a host, 
                                          * the usage of the -C "message" option must be compulsory, as is the logging 
                                          * of the name of the person performing the action. */

#define LSBE_FIRST_HOSTS_NOT_IN_QUEUE 351 /* First hosts specified by -m are not used by the queue */

#define LSBE_JOB_NOTSTART          352  /* The job is not started */
#define LSBE_RUNTIME_INVAL          353  /* Accumulated runtime of the job is not available */
#define LSBE_SSH_NOT_INTERACTIVE   354  /* SSH feature can only be used for interactive job */
#define LSBE_LESS_RUNTIME  355 /* Run time specification is less than the accumulated run time */
#define LSBE_RESIZE_NOTIFY_CMD_LEN   356 /* Resize job notification command */
#define LSBE_JOB_RESIZABLE	      357 /* Job is not resizable */
#define LSBE_RESIZE_RELEASE_HOSTSPEC  358 /* Bad bresize release host spec */
#define LSBE_NO_RESIZE_NOTIFY         359 /* no resize notify matches in mbatchd*/
#define LSBE_RESIZE_RELEASE_FRISTHOST 360  /* Can't release first exec host */
#define LSBE_RESIZE_EVENT_INPROGRESS  361  /* resize event in progress */
#define LSBE_RESIZE_BAD_SLOTS         362   /* too few or too many slots */
#define LSBE_RESIZE_NO_ACTIVE_REQUEST 363  /* No active resize request */
#define LSBE_HOST_NOT_IN_ALLOC        364  /* specified host not part of the
       		   * job's allocation
       		   */
#define LSBE_RESIZE_RELEASE_NOOP      365  /* nothing released */
#define LSBE_RESIZE_URGENT_JOB	      366  /* Can't resize a brun job */
#define LSBE_RESIZE_EGO_SLA_COEXIST   367
#define LSBE_HOST_NOT_SUPPORT_RESIZE  368  /* hpc jobs can't be resized */
#define LSBE_APP_RESIZABLE            369  /* Application doesn't allow resizable */
#define LSBE_RESIZE_LOST_AND_FOUND    370  /* can't operate on lost & found
       		    * hosts
       		 */
#define LSBE_RESIZE_FIRSTHOST_LOST_AND_FOUND 371 /* can't resize while the
                * first host is lost
                * & found
             */
#define LSBE_RESIZE_BAD_HOST          372 /* bad host name (for resize) */ 
#define LSBE_AUTORESIZE_APP           373 /* proper app is required by an auto-resizable job */
#define LSBE_RESIZE_PENDING_REQUEST   374 /* cannot resize job because there is a pedning resize request */
#define LSBE_ASKED_HOSTS_NUMBER       375 /* number of hosts specified by -m exceeding configuration */
#define LSBE_AR_HOST_EMPTY            376 /* All hosts reserved by advanced reservation are invalid in intersected hosts */
#define LSBE_AR_FIRST_HOST_EMPTY      377 /* First hosts specified by -m are not used by advanced reservation */    
#define LSBE_JB                       378 /* Internal jobbroker error */
#define LSBE_JB_DBLIB                 379 /* Internal jobbroker database library error */
#define LSBE_JB_DB_UNREACH            380 /* Jobbroker cannot reach database */
#define LSBE_JB_MBD_UNREACH           381 /* Jobbroker cannot reach mbatchd */
#define LSBE_JB_BES                   382 /* BES server returned an error */
#define LSBE_JB_BES_UNSUPPORTED_OP    383 /* Unsupported BES operation */
#define LSBE_LS_PROJECT_NAME    384  /* invalid LS project name*/
#define LSBE_END_TIME_INVALID_COMPARE_START 385/* the end time is not later than start  time. */
#define LSBE_HP_REDUNDANT_HOST        386 /* one host cannot be defined in more than one host partition.*/
#define LSBE_COMPOUND_APP_SLOTS       387 /* The application level compound resreq causes slots requirements conflict */
#define LSBE_COMPOUND_QUEUE_SLOTS     388 /* The queue level compound resreq causes slots requirements conflict */
#define LSBE_ADV_RSRCREQ_RESIZE       389 /* Resizable job cannot work with
					   * compound or alternative resreq
					   */
#define LSBE_COMPOUND_RESIZE          LSBE_ADV_RSRCREQ_RESIZE /* obsolete */
/* compute unit support */
#define LSBE_CU_OVERLAPPING_HOST      390 /* Compute units cannot have overlapping hosts */
#define LSBE_CU_BAD_HOST              391 /* The compute unit cannot contain other compute units */
#define LSBE_CU_HOST_NOT_ALLOWED      392 /* The compute unit cannot contain host or host group as a member */
#define LSBE_CU_NOT_LOWEST_LEVEL      393 /* Only lowest level compute units are allowed to add hosts as a member */
#define LSBE_CU_MOD_RESREQ            394 /* You cannot modify a compute unit resource requirement when a job is already running */
#define LSBE_CU_AUTORESIZE            395 /* A compute unit resource requirement cannot be specified for auto resizable jobs */
#define LSBE_NO_COMPUTE_UNIT_TYPES    396 /* No COMPUTE_UNIT_TYPES are specified in lsb.params */
#define LSBE_NO_COMPUTE_UNIT          397 /* No compute unit defined in the system */
#define LSBE_BAD_COMPUTE_UNIT         398 /* No such compute unit defined in the system */
#define LSBE_CU_EXCLUSIVE             399 /* The queue is not configured to accept exclusive compute unit jobs */
#define LSBE_CU_EXCLUSIVE_LEVEL       400 /* The queue is not configured to accept higher level of exclusive compute unit jobs */
#define LSBE_CU_SWITCH                401 /* Job cannot be switched due to the exclusive compute unit reqirement */
#define LSBE_COMPOUND_JOB_SLOTS       402 /* Job level compound resreq causes slots requirements conflict */
#define LSBE_COMPOUND_QUEUE_RUSAGE_OR 403 /* "||" used in rusage[] of queue resource requirement. It's conflict with job level compound resource requirement  */
#define LSBE_CU_BALANCE_USABLECUSLOTS 404 /* balance and usablecuslots cannot both be used in a compute unit resource requirement */
#define LSBE_COMPOUND_TSJOB_APP       405 /* TS jobs cannot use compound resource requirement (application level) */
#define LSBE_COMPOUND_TSJOB_QUEUE     406 /* TS jobs cannot use compound resource requirement (queue level) */
#define LSBE_EXCEED_MAX_JOB_NAME_DEP  407 /* Job dependency conditions using a job name or job name wild-card
                                             exceed limitation set by MAX_JOB_NAME_DEP in lsb.params */
#define LSBE_WAIT_FOR_MC_SYNC 408 /* "is waiting for the remote cluster to synchronize." */
#define LSBE_RUSAGE_EXCEED_RESRSV_LIMIT 409 /* Job cannot exceed queue level RESRSV_LIMIT limitation */
#define LSBE_JOB_DESCRIPTION_LEN 410      /* job description too long */
#define LSBE_NOT_IN_SIMMODE           411 /* Cannot use simulation options */
#define LSBE_SIM_OPT_RUNTIME          412 /* Value of runtime simulation is incorrect */
#define LSBE_SIM_OPT_CPUTIME          413 /* Value of cputime simulation is incorrect */
#define LSBE_SIM_OPT_MAXMEM           414 /* Incorrect maxmem simulation opt */
#define LSBE_SIM_OPT_EXITSTATUS       415 /* Incorrect job exitstatus simulation opt */
#define LSBE_SIM_OPT_SYNTAX           416 /* Incorrect job simulation option syntax */
#define LSBE_SIM_BSUB                 417/*Can not submission job withot -sim*/
#define LSBE_MAX_SLOTS_IN_POOL        418 /* MAX_SLOTS_IN_POOL is reached. */

#define LSBE_BAD_GUAR_RESOURCE_NAME   419 /* Guaranteed Resource pool
					   * name does not exist
					   */
#define LSBE_GUAR_SLA_USER_NOT_ALLOWED   420 /* User and guarantee sla
					      * mismatch
					      */
#define LSBE_GUAR_SLA_QUEUE_NOT_ALLOWED  421 /* Queue and guarantee sla
					      * mismatch
					      */
#define LSBE_GUAR_SLA_APP_NOT_ALLOWED    422 /* App profile and guarantee sla
					      * mismatch
					      */ 
#define LSBE_GUAR_SLA_PROJECT_NOT_ALLOWED  423 /* Project and guarantee sla
						* mismatch
						*/
#define LSBE_GUAR_SLA_GROUP_NOT_ALLOWED  424 /* User fairshare group and
					      * guarantee sla mismatch
					      */
#define LSBE_GUAR_SLA_GROUP_QUEUE_MISMATCH 425 /* Group queue fairshare
						* problem
						*/
#define LSBE_SLA_NOT_GUAR_SLA            426 /* Specified sla not a 
					      * guarantee sla
					      */
#define LSBE_SLA_ACCESS_CONTROL          427 /* Does not satisfy guarantee
					      * sla access control
					      */
#define LSBE_GUAR_SLA_JOB_STARTED        428 /* Can't mod started job belonging
					      * to guarantee SLA
					      */
#define LSBE_GUAR_SLA_INVALID_OP         429 /* Operation not allowed for
					      * guarantee type SLAs
					      */

#define LSBE_LIVECONF_MBD_RETERR      430      /* Live Reconfig got error message from mbatchd*/
#define LSBE_LIVECONF_MBD_REJECT      431      /* Live Reconfig got error message from mbatchd*/
#define LSBE_LIVECONF_MBD_INFO        432      /* Live Reconfig got info message from mbatchd*/
#define LSBE_EXCEED_JOBSPERPACK_LIMITATION 433 /* The amount of job requests in one pack exceeds 
                                             the limit LSB_MAX_PACK_JOBS defined in lsf.conf */
#define LSBE_PACK_SUBMISSION_DISABLED 434 /* Pack submission is disabled by defining LSB_MAX_PACK_JOBS=0 in lsf.conf */
#define LSBE_GUAR_SLA_LP_NOT_ALLOWED  435 /* License project and guarantee sla mismatch */
#define LSBE_GLB_MIX_MODE             436 /* Job requests both cluster mode and a project mode License Scheduler resources */
#define LSBE_PERFLOG_ENABLED_ALREADY  437 /* Performance metrics logging is already enabled */

#define LSBE_GE_UNSUPPORT_CMD        439 /* This command is not supported on grid execution cluster */
#define LSBE_WRONG_DEST_QUEUE        440 /* The destination queue does not include all send queue of source queue */
#define LSBE_MULTI_HOST              441 /* Multiple hosts with same hostname exist */
#define LSBE_MULTI_HOST_SPEC         442 /* Multiple hosts with same hostname exist in hostSpec */
#define LSBE_RUN_JOB_PROCESSING      443 /* brun job is in processing, block other job control cmd*/
#define LSBE_GBRUNJOB_LEASEHOST      444  /* cannot brun job on lease host in gb mode*/
#define LSBE_GBRUNJOB_BAD_HOST       445 /* The specified hosts in brun should be all local hosts or all remote hosts in one cluster */
#define LSBE_LOCAL_CLUSTER           446 /* bsub -m localcluster */
#define LSBE_OTHER_HOST              447 /* bsub -m others or bsub -m "rmthost others" */
#define LSBE_GBRUNJOB_CLUSTER_NOTSUPPORT 448 /* brun -m clustername is not supported. */
#define LSBE_MOD_BAD_CLUSTER_HOST    449 /* Fore forwarded job, all bmod -m hosts don't belong execution cluster of the job */
#define LSBE_NO_LSFXL_ENTITLEMENT    450  /* XL requires LSF Advanced Edition */
#define LSBE_CLUSTER_FIRST_HOST      451 /* "cluster" as first execution host */
#define LSBE_HOST_FORMAT             452 /* bad askedHost format */
#define LSBE_MBD_IN_RESTART          453  /* mbatchd is in process of restart */
#define LSBE_MBD_PRESTART_DUPLOG     454  /* Does not support parallel restart under dup event log*/
#define LSBE_EOB                     455  /* encounter boundary */
#define LSBE_WIN_PRESTART_NOT_SUPPORT 456 /* do not support parallel restart */
#define LSBE_QUEUE_HOST2              457 /* host, hostgroup or cluster not use by queue */
#define LSBE_MOD_PROCESSOR            458 /* cannot change processor num of forwarded job */
#define LSBE_HOST_HOSTGROUP_INVALID   459 /* Host name or host group name is not valid */
#define LSBE_BQUEUES_HOST_NOT_SUPPORT 460 /* Remote host or hostgroup cannot be used in bqueues -m option */
#define LSBE_XL_SAME_CLUSTER        461  /* For brun forward-only case, cannot
                                            brun the forwarded job to the same
                                            cluster on which it is pend */
#define LSBE_RUN_FORWARD_MIX        462  /* -m cluster_name cannot be used with
                                            other brun optons */
#define LSBE_JOBSPECIFY_ERROR         463 /* when the jobs -sum a invalid  */
#define LSBE_SND_RCV_QUEUE            464 /* in XL brun force-forward, when the
                                             send-recv queue configuration is
                                             not correct */
#define LSBE_BRUN_DEST_CLUSTER        465 /* in XL brun force-forward, when
                                             job's asked hosts not belong to the
                                             specified cluster */
#define LSBE_JOB_FORWARD_CANCELLING   466 /* Job is during unforward operation, some other operation should be blocked. */
#define LSBE_BRUN_COMPOUND_RESREQ     467 /* In XL, cannot brun a job with compound resource
                                           * requirement to remote cluster. */
#define LSBE_JOB_REQUEUING            468 /* Job is during brequeue operation, some other operation should be blocked. */
#define LSBE_BRUN_ONE_CLUSTER         469 /* Only one name is allowed if specify cluster name */
#define LSBE_QUERY_DIAGNOSE_DIR_ERROR 470 /* The directory for logdir does not exist or has no write permission. */
#define LSBE_QUERY_DIAGNOSE_DURATION_ERROR 471 /* The duration for query diagnosis is not valid. */
#define LSBE_JOB_BEING_KILL         472 /* job is "bkill", but not receive the
                                           status update from execution cluster,
                                           block brun*/
#define LSBE_EMB_BSUB              473  /* This command cannot be embedded within bsub. */
#define LSBE_QUERY_UPD_JOBCREATE        474 /* Create new job failed in query MBD */
#define LSBE_QUERY_UPD_NOTENABLE        475 /* new job updating thread is not enabled */
#define LSBE_REMOTE_HOST_UNSUPPORT    476 /* Remote host is not supported */
#define LSBE_AC_RESREQ_COMPOUND       477 /* Dynamic Cluster job cannot use compound or alternative resource requirement */
#define LSBE_AC_RESREQ_MULTIPHASE     478 /* Dynamic Cluster job cannot use multiphase memory reservation */
#define LSBE_AC_RESREQ_DURATION       479 /* Dynamic Cluster job cannot use duration in memory reservation */
#define LSBE_AC_BAD_HOST              480 /* The hypervisor hosts you specified do not exist. */
#define LSBE_AC_NO_HOST               481 /* No hosts in the cluster. */
#define LSBE_AC_BAD_PROVISION_ID      482 /* Incorrect provision ID */
#define LSBE_AC_BAD_PROVISION_JOBID   483 /* Incorrect provision Job ID */
#define LSBE_AC_NO_PROVISION          484 /* No provision requests in the cluster */
#define LSBE_AC_NO_VM                 485 /* No hosts in the cluster */
#define LSBE_AC_BAD_VM                486 /* The virtual machines you specified do not exist */
#define LSBE_AC_NO_TEMPLATE           487 /* No Dynamic Cluster template requests found */
#define LSBE_AC_NO_SUIT_TEMPLATE      488 /* No Dynamic Cluster templates found in the cluster */
#define LSBE_AC_TEMPLATE_NOT_FOUND    489 /* The Dynamic Cluster template you specified does not exist */
#define LSBE_AC_NO_PARAMETERS         490 /* Incorrect parameter */
#define LSBE_AC_HOST_IN_PROVISIONING  491 /* Cannot operate on hosts in the Provisioning state */
#define LSBE_AC_HOST_IN_SAVED         492 /* Cannot operate on hosts in the Saved state */
#define LSBE_AC_HOST_SBD_STARTING_UP  493 /* Cannot operate on hosts waiting for the slave batch daemon to start up */
#define LSBE_AC_RESIZABLE             494 /* Dynamic Cluster jobs cannot be resizable jobs */
#define LSBE_AC_VIRTUAL_MACHINE       495 /* Cannot specify virtual machines */
#define LSBE_AC_CHUNK_JOB             496 /* Dynamic Cluster jobs cannot be chunk jobs */
#define LSBE_AC_VMACTION              497 /* Cannot combine dc_vmaction with Dynamic Cluster physical machine jobs */
#define LSBE_AC_MODIFY_RUNNING_VMJOB  498 /* Cannot modify the Dynamic Cluster job because it is running on a virtual machine */
#define LSBE_AC_MODIFY_VMJOB_IN_SAVED 499 /* Cannot modify the Dynamic Cluster jobs because the job is waiting for a virtual machine to be restored */
#define LSBE_AC_MODIFY_TO_NON_ACJOB   500 /* Cannot modify the Dynamic Cluster job to be a non-Dynamic Cluster job after the job is running */
#define LSBE_AC_JOB_RUSAGE_EXCEED_LIMIT 501 /* Error in the rusage section: The job-level resource requirement (mem) round-up value exceeds the limit set by the queue-level resource requirement (mem) value */
#define LSBE_AC_APP_RUSAGE_EXCEED_LIMIT 502 /* Error in the rusage section: The application-level resource requirement (mem) round-up value exceeds the limit set by the queue-level resource requirement (mem) value */
#define LSBE_AC_DEFAULT_RUSAGE_EXCEED_RESRSV_LIMIT 503 /* The default memory requirement value (DC_DEFAULT_JOB_MEMSIZE) is outside the range set by RESRSV_LIMIT defined in lsb.queues */
#define LSBE_AC_JOB_RUSAGE_EXCEED_LARGEST_MEMSIZE 504 /* Error in the rusage section: The job-level resource requirement (mem) value exceeds the largest memory size in DC_VM_MEMSIZE_DEFINED */
#define LSBE_AC_APP_RUSAGE_EXCEED_LARGEST_MEMSIZE 505 /* Error in the rusage section: The application-level resource requirement (mem) value exceeds the largest memory size in DC_VM_MEMSIZE_DEFINED */
#define LSBE_AC_QUE_RUSAGE_EXCEED_LARGEST_MEMSIZE 506 /* Error in the rusage section: The queue-level resource requirement (mem) value exceeds the largest memory size in DC_VM_MEMSIZE_DEFINED */
#define LSBE_AC_NOT_AN_AC_JOB         507 /* Not a Dynamic Cluster job */
#define LSBE_AC_NO_TEMPLATE_SPECIFIED 508 /* No template specified */
#define LSBE_AC_MODIFY_RUNNING_JOB_TO_ACJOB   509 /* Cannot modify the non-Dynamic Cluster job to be a Dynamic Cluster job after the job is running */
#define LSBE_AC_MODIFY_OPTIONS_RUNNING_ACJOB  510 /* Cannot modify Dynamic Cluster job options after the job is running */
#define LSBE_AC_MODIFY_OPTIONS_WITHOUT_AC_TMPL  511 /* Dynamic Cluster template name was not requested at the job level */
#define LSBE_AC_NOT_AN_AC_VMJOB       512 /* Not a Dynamic Cluster vm job */
#define LSBE_AC_FEATURE_NOT_ENABLE 513 /* Dynamic Cluster is not enabled */

/* brun under Dynamic Cluster */
#define LSBE_AC_ONE_VM_ONLY_ALLOWED   514 /* The virtual machines cannot be specified with other machine types */
#define LSBE_AC_ONE_HV_ONLY_ALLOWED   515 /* The pure hypervisors cannot be specified with other machine types */
#define LSBE_AC_JOB_IS_SAVED          516 /* Cannot operate on the job because it is already saved into a disk image */
#define LSBE_AC_NON_ACHOST            517 /* Cannot operate on non-Dynamic Cluster hosts */
#define LSBE_AC_RESOTRE_ON_NON_HV     518 /* Cannot restore the virtual machine on a non-hypervisor host */
#define LSBE_AC_RESOTRE_ON_RESGROUP   519 /* Cannot restore the virtual machine due to different resource group */
#define LSBE_AC_INSUFFICIENT_HOST     520 /* Cannot restore the virtual machine due to insufficient memory or number of CPUs */
#define LSBE_AC_RESTORE_VM_NOT_FOUND  521 /* Cannot restore the virtual machine because the virtual machine is not found */
#define LSBE_AC_UNABLE_TO_RESTORE_VM  522 /* Unable to restore the virtual machine */
#define LSBE_AC_VM_HAS_WORKINFO       523 /* Other workload is on the virtual machine */
#define LSBE_AC_PURE_HYPERVISOR       524 /* Cannot specify the pure hypervisor */
#define LSBE_AC_UNABLE_PROVISION      525 /* Unable to provision the physical or virtual machine */
#define LSBE_AC_HOSTS_MIXED           526 /* Cannot specify Dynamic Cluster hosts together with non-Dynamic Cluster hosts */

#define LSBE_QUERY_INVALID_EAUTH          527 /* Eauth is invalid for query mbatchd, wait new query mbatchd startup*/
#define LSBE_NOT_REQUEUE_LPJ        528 /* The reply code is describe that this job is local pend state and is not allowed to be requeued. */
#define LSBE_UNSUPPORTED_MC_CHKPNTABLE      529  /* Operation not supported for a Multicluster checkpointable job */
#define LSBE_AC_BAD_VMACTION          530 /* Incorrect virtual machine preemption action syntax */
#define LSBE_AC_SWITCH_MIGRATING_JOB  531 /* Dynamic Cluster jobs cannot be switched while being migrated */
#define LSBE_MOD_COMPOUND_RESREQ      532 /* In XL, cannot modify a forwarded
					   * job's resource requirement to
					   * compound resource requirement.
					   */
#define LSBE_ALTERNATIVE_JOB_SLOTS    533 /* Job level alternative resreq
					   * causes slots requirements conflict
					   */
#define LSBE_ALTERNATIVE_APP_SLOTS    534 /* The application level alternative
					   * resreq causes slots requirements
					   * conflict
					   */
#define LSBE_ALTERNATIVE_QUEUE_SLOTS  535 /* The queue level alternative resreq
					   * causes slots requirements conflict
					   */
#define LSBE_ALTERNATIVE_TSJOB_APP    536 /* TS jobs cannot use alternative
					   * resource requirement
					   * (application level)
					   */
#define LSBE_ALTERNATIVE_TSJOB_QUEUE  537 /* TS jobs cannot use alternative
					   * resource requirement (queue level)
					   */
#define LSBE_ALTERNATIVE_QUEUE_RUSAGE_OR 538 /* "||" used in rusage[] of queue
					      * resource requirement. It's
					      * conflict with job level
					      * alternative resource
					      * requirement
					      */
#define LSBE_BRUN_ALTERNATIVE_RESREQ     539 /* In XL, cannot brun a job with
					      * alternative resource
					      * requirement to remote cluster
					      */
#define LSBE_RESIZE_QUEUE_ADV_RSRCREQ    540 /* Resizable job can't be sent
					      * to a queue with compound
					      * or alternative rsrcreq
					      */
#define LSBE_JOB_BEING_MODIFIED     	541 /* Job is locked while being modified, other job control
                                             * command cannot be performed */
#define LSBE_MC_BMOD_NOT_SUPPORT    	542 /* Remote cluster doesn't support MC bmod */
#define LSBE_MC_BMOD_PARAMETER_DISALLOW 543 /* Parameter is not allowed to be modified on forwarded
                                             * job */
#define LSBE_MC_REMOTE_HOST_SPEC   	544 /* Cannot get host spec for remote host */

#define LSBE_EXCLUDED_PRIO              545 /* specified '~' with preference */
#define LSBE_ALL_OTHERS                 546 /* specified "all" with "others" */
#define LSBE_EXCLUDED_NO_ALL            547 /* specified '~' without "all" */
#define LSBE_ASKED_CLUSTER_NULL         548 /* no valid cluster in specified clusters */
#define LSBE_M_CLUSTER         		549 /* specified -m and -cluster */
#define LSBE_DUPLICATE_CLUSTER          550 /* specified duplicated cluster names */
#define LSBE_CLUSTER_ADRSV         	551 /* AR not in specified cluster*/
#define LSBE_BAD_ASKED_CLUSTER       	552 /* Bad asked cluster */
#define LSBE_QUEUE_ASKED_CLUSTER        553 /* Asked cluster is not used by the queue */
#define LSBE_OTHERS_ALONE               554 /* others can not be used alone */
#define LSBE_GB_ASKED_CLUSTER           555 /* XL mode do not support -cluster option */
#define LSBE_BMOD_ASKED_CLUSTER_DISALLOW 556 /* bmod -cluster/clustern only support local pending job */
#define LSBE_LOCAL_CLUSTER_NONE 	557 /* only specify local cluster and HOSTS = none */

#define LSBE_EFFECTIVE_MODIFY_COMPOUND	558 /* Cannot modify job effective 
                          		     * resource requirement to 
                                             * compound resource requirement
                                             */
#define LSBE_EFFECTIVE_MODIFY_ALTERNATIVE 559 /* Cannot modify job effective 
                                               * resource requirement to alternative 
                                               * resource requirement 
                                               */
#define LSBE_EFFECTIVE_MODIFY_COMPOUND_TO_SIMPLE 560 
					     /* Cannot modify job compound 
                          		      * effective rsrcreq to simple 
                                              * resource requirement 
                                              */
#define LSBE_MC_MOD_ASKED_HOST  	561 /* Cannot modify execuiton host specification for forwarded job */
#define LSBE_MC_MOD_QUEUE   		562 /* Cannot modify queue for a forwarded job. */
#define LSBE_MC_MOD_DEPEND_COND 	563 /* Cannot modify dependency condition for a forwarded job. */
#define LSBE_MC_MOD_SLA 		564 /* Cannot modify service class for a forwarded job. */
#define LSBE_LS_NO_RESREQ               565 /* There is no LS resource sepcified
                                             * in rusage
					     */
#define LSBE_GBRUNJOB_MASTERCANDIDATE   566    /* XL master or master candidate host cannot run job.
                                                  */
#define LSBE_GB_BRUN_REMOTE_QUEUE 567 /* remote queue cannot receive job */
#define LSBE_GLB_MIX_PROJECT_MODE 568 /* Job requests both fast dispatch project mode and a project mode License Scheduler resources */
#define LSBE_MC_REMOTE_CLUSTER   569 /* cannot specify remote cluster name with -m option in Multi-Cluster or Single-Cluster mode */
#define LSBE_EXCEED_QUEUE_RUSAGE  570 /* Resource requirement values for the job must be within the limits set by the queue-level resource requirement values */
#define LSBE_EXCEED_RESRSV_LIMIT  571 /* The rusage value for the job is outside the range set by RESRSV_LIMIT in lsb.queues */
#define LSBE_AC_SPAN_MANY_VMS     572 /* More than one virtual machine cannot be requested */
#define LSBE_AC_NONE_COEXIST      573 /* Cannot specify the keyword "none" with Dynamic Cluster template names */
#define LSBE_AC_NO_TEMPLATE_VM    574 /* The Dynamic Cluster templates you specified cannot provision the virtual machines */
#define LSBE_AC_NO_TEMPLATE_PM    575 /* The Dynamic Cluster templates you specified cannot provision the physical machines */
#define LSBE_AC_ANY_TMPL_COEXIST      576 /* Cannot specify the keyword "any" with Dynamic Cluster template names */
#define LSBE_AC_NONE_TMPL_WITH_OTHERS 577 /* Other Dynamic Cluster options cannot be combined when the Dynamic Cluster template name "none" is specified */
#define LSBE_AC_TMPL_BEING_REMOVED    578 /* Other Dynamic Cluster options cannot be combined when the Dynamic Cluster template name is being removed */
#define LSBE_AC_JOB_NO_TEMPLATE_VM    579 /* The Dynamic Cluster job's templates cannot provision the virtual machines */
#define LSBE_AC_JOB_NO_TEMPLATE_PM    580 /* The Dynamic Cluster job's templates cannot provision the physical machines */
#define LSBE_AC_JOB_NO_VMACTION       581 /* The preemption action for the virtual machine was not requested at the job level */
#define LSBE_AC_MODIFY_MEM_RUSAGE     582 /* Cannot modify the Dynamic Cluster job resource requirement after the job is running or the job is saved */
#define LSBE_NETWORK_PE_DISABLED 583 /* LSF_PE_NETWORK_NUM is not configured */
#define LSBE_NETWORK_SYNTAX  584 /* Bad network requirement syntax */
#define LSBE_NETWORK_INVALID_TYPE   585 /* Invalid type in network requirement */
#define LSBE_NETWORK_INVALID_PROTOCOL   586 /* Invaid protocol name in network requirement */
#define LSBE_NETWORK_IP_INSTANCE    587 /* Cannot specify instance with POE job in IP mode*/
#define LSBE_NETWORK_INVALID_USAGE  588 /* Network usage is not valid in network requirement */
#define LSBE_NETWORK_INSTANCE   589 /* Window instance is not valid in network requirement */
#define LSBE_NETWORK_BAD_KEYWORD    590 /* Bad keyword in network requirement */
#define LSBE_EXCEED_MAX_PROTOCOL_CNT 591 /* Specified protocol count for network requirement exceeds
                                            the maximum value of 128 */
#define LSBE_NETWORK_INVALID_MODE   592 /* Mode is not valid in network requirement */
#define LSBE_NETWORK_IP_DEDICATED   593 /* Network dedicated usage cannot be used together with IP mode */
#define LSBE_BRUN_NETWORK_JOB       594 /* Cannot force a job with network requirement to run */
#define LSBE_NETWORK_CHANGE_RUNNING_JOB 595 /* Cannot change running job's network requirement */
#define LSBE_NETWORK_JOB_RESIZE     596  /* cannot resize job with network requirement */
#define LSBE_NETWORK_EXCEED_QUEUE_MAX_PROTOCOL_INSTANCE 597   /* Cannot exceed MAX_PROTOCOL_INSTANCES */
#define LSBE_EXCLUSIVE_AFFINITY    598 /* Queue doesn't accept "exclusive=(...,alljobs..." affinity job */
#define LSBE_RESIZE_AFFINITY_JOB_BAD_HOST 599  /* Invalid bresize request for affinity job */
#define LSBE_NETWORK_CHKPNT     600 /* POE job cannot be checkpointable */
#define LSBE_NETWORK_EXCEED_CLUSTER_MAX_PROTOCOL_INSTANCE   601 /* cannt exceed cluster MAX_PROTOCOL_INSTANCES */

#define LSBE_BAD_GLOBAL_FAIRSHARE_NAME  602 /* Global fairshare
                                             * name does not exist
                                             */
#define LSBE_BAD_GLOBAL_FAIRSHARE_CLUSTER  603 /* Global fairshare participant
                                                * cluster does not exist
                                                */
#define LSBE_NO_GLOBAL_FAIRSHARE      604   /* No global fairshare defined in lsb.globalpolicies file */
#define LSBE_NOT_GPD_CLUSTER          605 /* The cluster is not GPD cluster */
#define LSBE_NOT_MASTER_CANDIDATE     606 /* The host is not a master candidate host */
#define LSBE_BAD_GPD_CLUSTER          607 /* Unknown GPD cluster name */

/* RFC1503&2753: Sick Host Part B */
#define LSBE_ZOMBIE_JOB               608 /* Job can be found but only in the zombie list*/
#define LSBE_AC_CHKPNT_NOT_RERUNNABLE         609 /* DC virtual machine checkpoint job must be rerunnable*/
#define LSBE_AC_CHKPNT_LSFCHKPNT              610 /* DC virtual machine checkpoint job can not be regular checkpointable*/
#define LSBE_AC_CHKPNT_NOT_VMJOB              611 /* DC virtual machine checkpoint job must be VM job*/
#define LSBE_AC_CHKPNT_BAD_ARGUMENT           612 /* Bad DC checkpoint period argument*/
#define LSBE_PE_INVALID_PROTOCOL_COUNT        613 /* Protocol count is not valid in network requirement */
#define LSBE_NUM_ERR                  614 /* Number of the above error codes */


										      
/*********************************************************/

/* op codes for hand shake protocol between client/server */
#define PREPARE_FOR_OP          1024
#define READY_FOR_OP            1023

/*
 * Data structures for lsblib interface
 */


/**
 * \addtogroup lsb_submit_options lsb_submit_options
 * define statements used by lsb_submit.
 */
/*@{*/

/* lsb_submit() options */
#define  SUB_JOB_NAME       0x01    
               /**< Flag to indicate jobName parameter
       		* has data. Equivalent to bsub -J command line option
       		* existence. */
#define  SUB_QUEUE          0x02    
               /**< Flag to indicate queue parameter has
       		* data. Equivalent to bsub -q command line option
       		* existence. */
#define  SUB_HOST           0x04    
               /**< Flag to indicate numAskedHosts
       		* parameter has data. Equivalent to bsub -m command line
       		* option existence.*/
#define  SUB_IN_FILE        0x08    
               /**< Flag to indicate inFile parameter
       		* has data. Equivalent to bsub -i command line option
       		* existence. */
#define  SUB_OUT_FILE       0x10    
               /**< Flag to indicate outFile parameter
       		* has data. Equivalent to bsub -o command line option
       		* existence. */
#define  SUB_ERR_FILE       0x20    
               /**< Flag to indicate errFile parameter
       		* has data. Equivalent to bsub -e command line option
       		* existence. */
#define  SUB_EXCLUSIVE      0x40    
               /**< Flag to indicate execution of a job
       		* on a host by itself requested. Equivalent to bsub -x
       		* command line option existence. */
#define  SUB_NOTIFY_END     0x80    
               /**< Flag to indicate whether to send mail
       		* to the user when the job finishes. Equivalent to bsub -N
       		* command line option existence. */
#define  SUB_NOTIFY_BEGIN   0x100   
               /**< Flag to indicate whether to send mail
       		* to the user when the job is dispatched. Equivalent to
       		* bsub -B command line option existence.  */
#define  SUB_USER_GROUP	    0x200   
               /**< Flag to indicate userGroup name
       		* parameter has data. Equivalent to bsub -G command line
       		* option existence. */
#define  SUB_CHKPNT_PERIOD  0x400   
               /**< Flag to indicatechkpntPeriod
       		* parameter has data . Equivalent to bsub -k command line
       		* option existence. */
#define  SUB_CHKPNT_DIR     0x800   
               /**< Flag to indicate chkpntDir parameter
       		* has data. Equivalent to bsub -k command line option
       		* existence. */
#define  SUB_CHKPNTABLE     SUB_CHKPNT_DIR 
               /**< Indicates the job is
       		* checkpointable. Equivalent to bsub -k command
       		* line option.  */
#define  SUB_RESTART_FORCE  0x1000  
               /**< Flag to indicate whether to force
       		* the job to restart even if non-restartable conditions
       		* exist. These conditions are operating system specific.
       		* Equivalent to brestart() -f command line option
       		* existence.  */
#define  SUB_RESTART        0x2000  
               /**< Flag to indicate restart of a
		* checkpointed job. Only jobs that have been successfully checkpointed
		* can be restarted. Jobs are re-submitted and assigned a new job ID.
		* By default, jobs are restarted with the same output file, file
		* transfer specifications, job name, window signal value, checkpoint
		* directory and period, and rerun options as the original job. To
		* restart a job on another host, both hosts must be binary compatible,
		* run the same OS version, have access to the executable, have access
		* to all open files (LSF must locate them with an absolute path name),
		* and have access to the checkpoint directory. Equivalent to bsub -k
		* command line option existence. */
#define  SUB_RERUNNABLE     0x4000  
               /**< Indicates the job is re-runnable.
		* If the execution host of the job is considered down, the batch
		* system will re-queue this job in the same job queue, and re-run
		* it from the beginning when a suitable host is found. Everything
		* will be as if it were submitted as a new job, and a new job ID will
		* be assigned. The user who submitted the failed job will receive a
		* mail notice of the job failure, requeueing of the job, and the
		* new job ID.
		*
		* For a job that was checkpointed before the execution host went down,
		* the job will be restarted from the last checkpoint. Equivalent to
		* bsub -r command line option existence. */
#define  SUB_WINDOW_SIG     0x8000  
               /**< Flag to indicate sigValue parameter
		* has data. Sends a signal as the queue window closes. */
#define  SUB_HOST_SPEC      0x10000 
               /**< Flag to indicate hostSpec parameter
		* has data.  */
#define  SUB_DEPEND_COND    0x20000 
               /**< Flag to indicate dependCond parameter
		* has data. Equivalent to bsub -w command line option existence. */
#define  SUB_RES_REQ        0x40000 
               /**< Flag to indicate resReq parameter
		* has data. Equivalent to bsub -R command line option existence. */
#define  SUB_OTHER_FILES    0x80000 
               /**< Flag to indicate nxf parameter
                * and structure xf have data. */
#define  SUB_PRE_EXEC	    0x100000 
               /**< Flag to indicate preExecCmd
		* parameter has data. Equivalent to bsub -E command line option
		* existence. */
#define  SUB_LOGIN_SHELL    0x200000 
               /**< Equivalent to bsub -L command
                * line option existence. */
#define  SUB_MAIL_USER 	    0x400000 
               /**< Flag to indicate mailUser parameter
                * has data. */
#define  SUB_MODIFY         0x800000 
               /**< Flag to indicate newCommand
                * parameter has data. Equivalent to
                * bmod bsub_options existence. */
#define  SUB_MODIFY_ONCE    0x1000000 
               /**< Flag to indicate modify
                * option once. */
#define  SUB_PROJECT_NAME   0x2000000 
               /**< Flag to indicate ProjectName
		* parameter has data . Equivalent to bsub -P command line option
		* existence. */
#define  SUB_INTERACTIVE    0x4000000 
               /**< Indicates that the job is submitted
		* as a batch interactive job. When this flag is given, \ref lsb_submit
		* does not return unless an error occurs during the submission process.
		* When the job is started, the user can interact with the job's
		* standard input and output via the terminal. See the -I option
		* in bsub for the description of a batch interactive job. Unless
		* the SUB_PTY flag is specified, the job will run without a
		* pseudo-terminal. Equivalent to bsub -I command line option. */
#define  SUB_PTY            0x8000000 
               /**< Requests pseudo-terminal support
		* for a job submitted with the SUB_INTERACTIVE flag. This flag is
		* ignored if SUB_INTERACTIVE is not specified. A pseudo-terminal
		* is required to run some applications (such as: vi). Equivalent to
		* bsub -Ip command line option.  */
#define  SUB_PTY_SHELL      0x10000000 
               /**< Requests pseudo-terminal shell
		* mode support for a job submitted with the SUB_INTERACTIVE and
		* SUB_PTY flags. This flag is ignored if SUB_INTERACTIVE and SUB_PTY
		* are not specified. This flag should be specified for submitting
		* interactive shells, or applications which redefine the ctrl-C and
		* ctrl-Z keys (such as: jove). Equivalent to bsub -Is
		* command line option.  */
#define  SUB_EXCEPT         0x20000000      /**< Exception handler for job. */
#define  SUB_TIME_EVENT     0x40000000      /**< Specifies time_event. */
/* the last bit 0x80000000 is reserved for internal use */
/*@}*/

/**
 * \addtogroup lsb_submit_options2 lsb_submit_options2
 * define statements used by \ref lsb_submit.
 */
/*@{*/
   
#define  SUB2_HOLD                0x01      /**< Hold the job after it is
		* submitted. The job will be in PSUSP status. Equivalent to
		* bsub -H command line option.  */
#define  SUB2_MODIFY_CMD          0x02      /**< New cmd for bmod. */
#define  SUB2_BSUB_BLOCK          0x04      
               /**< Submit a job in a synchronous
		* mode so that submission does not return until the job terminates.
		* Note once this flag is set, the \ref lsb_submit will never return if
		* the job is accepted by LSF. Programs that wishes to know the status
		* of the submission needs to fork, with the child process invoking the
		* API call in the blocking mode and the parent process wait on the
		* child process (see wait() for details.  */
#define  SUB2_HOST_NT             0x08      /**< Submit from NT. */
#define  SUB2_HOST_UX             0x10      /**< Submit fom UNIX.  */
#define  SUB2_QUEUE_CHKPNT        0x20      /**< Submit to a chkpntable queue. */
#define  SUB2_QUEUE_RERUNNABLE    0x40      /**< Submit to a rerunnable queue. */
#define  SUB2_IN_FILE_SPOOL       0x80      /**< Spool job command. */
#define  SUB2_JOB_CMD_SPOOL       0x100     /**< Inputs the specified file
                     		             * with spooling */
#define  SUB2_JOB_PRIORITY        0x200     /**< Submits job with priority. */
#define  SUB2_USE_DEF_PROCLIMIT   0x400     /**< Job submitted without -n,
                     		             * use queue's default proclimit */
#define  SUB2_MODIFY_RUN_JOB      0x800     /**< bmod -c/-M/-W/-o/-e/-v */
#define  SUB2_MODIFY_PEND_JOB     0x1000    /**< bmod options only to
                     		             * pending jobs */
#define  SUB2_WARNING_TIME_PERIOD 0x2000    /**< Job action warning time.
                     	                     * Equivalent to bsub or bmod -wt. */
#define  SUB2_WARNING_ACTION      0x4000    /**< Job action to be taken
                     	                     * before a job control action occurs.
                     	                     * Equivalent to bsub or bmod -wa. */
#define  SUB2_USE_RSV             0x8000    /**< Use an advance reservation
                     	                     * created with the brsvadd command.
                     	                     * Equivalent to bsub -U. */
#define  SUB2_TSJOB               0x10000   /**< Windows Terminal
                     		             * Services job  */

/* SUB2_LSF2TP is obsolete in Eagle. We keep it here for backward
 * compatibility */
#define  SUB2_LSF2TP 0x20000                /**< Parameter is deprecated */
#define  SUB2_JOB_GROUP           0x40000   /**< Submit into a job group */
#define  SUB2_SLA                 0x80000   /**< Submit into a service class */
#define  SUB2_EXTSCHED            0x100000  /**< Submit with -extsched options*/
#define  SUB2_LICENSE_PROJECT     0x200000  /**< License Scheduler project */
#define  SUB2_OVERWRITE_OUT_FILE  0x400000  /**< Overwrite the standard output
                                             * of the job. Equivalent to bsub -oo. */
#define  SUB2_OVERWRITE_ERR_FILE  0x800000  /**< Overwrites the standard error
                                             * output of the job. Equivalent to
                                             * bsub -eo. */

/* Following are for symphony submission definition.
 * Note that SYM_GRP is an LSF job, which represents a symphony group.
 */
#define  SUB2_SSM_JOB             0x1000000   /**< (symphony) session job */
#define  SUB2_SYM_JOB             0x2000000   /**< (symphony) symphony job */
#define  SUB2_SRV_JOB             0x4000000   /**< (symphony) service(LSF) job */
#define  SUB2_SYM_GRP             0x8000000   /**< (symphony) "group" job */
#define  SUB2_SYM_JOB_PARENT      0x10000000  /**< (symphony) symphony job has
                                               * child symphony job */
#define  SUB2_SYM_JOB_REALTIME    0x20000000  /**< (symphony) symphony job has
                                               * real time feature */
#define  SUB2_SYM_JOB_PERSIST_SRV 0x40000000  /**< (symphony) symphony job has
                                               * dummy feature to hold all
       		                               * persistent service jobs. */
#define  SUB2_SSM_JOB_PERSIST     0x80000000   /**< Persistent session job */
/*@}*/

/**
 * \addtogroup lsb_submit_options3 lsb_submit_options3
 * define statements used by \ref lsb_submit.
 */
/*@{*/ 
#define  SUB3_APP                 0x01        /**< Application profile name.
                     		               * Equivalent to bsub -app. */
#define  SUB3_APP_RERUNNABLE      0x02        /**< Job rerunable because of
                     		               * application profile  */
#define  SUB3_ABSOLUTE_PRIORITY   0x04        /**< Job modified with absolute priority.
                     		               * Equivalent to bmod -aps.  */
#define  SUB3_DEFAULT_JOBGROUP    0x08        /**< Submit into a default job group.
                     		               * Equivalent to bsub -g. */
#define  SUB3_POST_EXEC    0x10               /**< Run the specified post-execution
                                               * command on the execution host
                                               * after the job finishes. Equivalent
                                               * to bsub -Ep.*/
#define  SUB3_USER_SHELL_LIMITS    0x20    
         /**< Pass user shell limits to
          * execution host. Equivalent to bsub -ul. */
#define  SUB3_CWD     0x40    
      /**< Current working directory specified on the
       * command line with bsub -cwd */
#define  SUB3_RUNTIME_ESTIMATION 0x80   
      /**< Runtime estimate. Equivalent to bsub -We.
       * Use in conjunction with SUB3_RUNTIME_ESTIMATION_ACC and
       * SUB3_RUNTIME_ESTIMATION_PERC. */
#define  SUB3_NOT_RERUNNABLE        0x100     /**< Job is not rerunnable.
                     		               * Equivalent to bsub -rn. */
#define  SUB3_JOB_REQUEUE        0x200        /**< Job level requeue exit values. */
#define  SUB3_INIT_CHKPNT_PERIOD    0x400     
      /**< Initial checkpoint period.
       * Equivalent to bsub -k initial_checkpoint_period. */
#define  SUB3_MIG_THRESHOLD         0x800   
      /**< Job migration threshold.
       * Equivalent to bsub -mig migration_threshold. */
#define  SUB3_APP_CHKPNT_DIR        0x1000    /**< Checkpoint dir was set by
                     		               * application profile */
#define  SUB3_BSUB_CHK_RESREQ        0x2000   
      /**< Value of BSUB_CHK_RESREQ
       * environment variable, used for select section resource requirement
       * string syntax checking with bsub -R. bsub only checks the
       * resreq syntax. */
#define  SUB3_RUNTIME_ESTIMATION_ACC 0x4000   
      /**< Runtime estimate that is
       * the accumulated run time plus the runtime estimate. Equivalent
       * to bmod -We+. Use in conjunction with SUB3_RUNTIME_ESTIMATION. */
#define  SUB3_RUNTIME_ESTIMATION_PERC 0x8000   
      /**< Runtime estimate in
       * percentage of completion. Equivalent to bmod -Wep. Two digits
       * after the decimal point are suported. The highest eight bits
       * of runtimeEstimation in the submit structure are used for the
       * integer; the remaining bits are used for the fraction. Use in
       * conjunction with SUB3_RUNTIME_ESTIMATION.  */
#define  SUB3_INTERACTIVE_SSH      0x10000  
       /**< Protects the sessions of
       	* interactive jobs with SSH encryption. Equivalent to
       	* bsub -IS|-ISp|-ISs.*/
#define  SUB3_XJOB_SSH             0x20000  
       /**< Protect the sessions of
       	* interactive x-window job with SSH encryption. Equivalent
       	* to bsub -IX.*/
#define  SUB3_AUTO_RESIZE           0x40000  /**< If set the submitted job
       		                              * is auto-resizable*/
#define  SUB3_RESIZE_NOTIFY_CMD     0x80000  /**< If set, the resize notify cmd
                                             * specified */

#define  SUB3_BULK_SUBMIT           0x100000   /**< Job broker bulk submit */
#define  SUB3_INTERACTIVE_TTY        0x200000  /**< tty mode for interactive job*/
#define  SUB3_FLOATING_CLIENT       0x400000   /**< Job submitted from floating client */
#define  SUB3_XFJOB                 0x800000  /**< ssh X11 forwarding (bsub -XF) */
#define  SUB3_XFJOB_EXCLUSIVE       0x1000000  /**< ssh X11 forwarding (bsub -XF) without bsub -I... */
#define  SUB3_JOB_DESCRIPTION       0x2000000  /**< Job description. */
#define  SUB3_SIMULATION            0x4000000  /**< Job submitted from floating client */

#define  SUB3_BRUN_FORWARD          0x8000000   /**< used by forward decision in brun */
#define  SUB3_RESREQ_ANYTYPE        0x10000000   /* used by forward decision when
                                                specified -m clustername */

#define SUB3_BRUN_FORWARD_ONLY      0x20000000  /* for brun case GB_TO_EXEC and
                                                   EXEC_TO_DIFF_EXEC, we only
                                                   forward but not issue brun
                                                   request*/

#define SUB3_CREATE_CWD_DIRECTORY   0x40000000  /**< Create CWD directory or not */
#define SUB3_BMOD_FROM_LS          0x80000000 /**< Bmod is triggered by License Scheduler */

/*@}*/

/**
 * \addtogroup lsb_submit_options4 lsb_submit_options4
 * define statements used by \ref lsb_submit.
 */
/*@{*/ 
#define  SUB4_AC_TEMPLATE_NONE     0x00000001 /**< Dynamic Cluster template 'none' specified */
#define  SUB4_AC_TEMPLATE          0x00000002 /**< Job submitted with a Dynamic Cluster template specified */
#define  SUB4_AC_MTYPE_VM          0x00000004  /**< a virtual machine requested */
#define  SUB4_AC_MTYPE_PM          0x00000008  /**< physical machines requested */
#if 0
#define  SUB4_AC_SAVEVM            0x00000010  /**< VM job action "savevm" when being preemted */
#define  SUB4_AC_LIVEMIGRATEVM     0x00000020  /**< VM job action "livemig" when being preemted */
#endif
#define  SUB4_AC_PREEMPTACTION_VM  0x00000010  /**< Job action when VM is being preempted */
#define  SUB4_AC_MTYPE_ANY         0x00000020  /**< 'any' machines requested */
#define  SUB4_OUTDIR               0x00000040  /**< output directory */

#define  SUB4_CLUSTER              0x00000080
               /**< Flat to indicate numAskedClusters
                * parameter has data. Equivalent to bsub -cluster command line
                * option existence.*/
#define SUB4_USERTGT                0x00000100  /*RFC#3633, krb5 integration*/
#define SUB4_NETWORK              0x200  /**< Job carry network requirement */
#define SUB4_AC_CHECKPOINT          0x00000400  /*DC check point*/

/*@}*/

/* Check whether a job is symphony job. These macros should be used by all
 * components, including ("submit" struct actually):
 *   - mbatchd: jData->submitReq
 *   - sbatchd: jobCard->jobSpecs
 *   - API: lsb_submit() and lsb_readjobinfo()
 */
#define IS_SSM_JOB(option) ((option) & SUB2_SSM_JOB)
#define IS_SSM_JOB_PERSIST(option) ((option) & SUB2_SSM_JOB_PERSIST)
#define IS_SYM_JOB(option) ((option) & SUB2_SYM_JOB)
#define IS_SYM_JOB_PARENT(option) ((option) & SUB2_SYM_JOB_PARENT)
#define IS_SYM_JOB_REALTIME(option) ((option) & SUB2_SYM_JOB_REALTIME)
#define IS_SYM_JOB_PERSIST_SRV(option) ((option) & SUB2_SYM_JOB_PERSIST_SRV)
#define IS_SRV_JOB(option) ((option) & SUB2_SRV_JOB)
#define IS_SYM_GRP(option) ((option) & SUB2_SYM_GRP)
#define IS_SYM_JOB_OR_SYM_GRP(option) (IS_SYM_JOB(option)\
            || IS_SYM_GRP(option))
/* symphony job for which resource usage should be collected */
#define IS_REAL_SYM_JOB(option) (IS_SYM_JOB(option) && !IS_SYM_JOB_PERSIST_SRV(option))

#define IS_WLM_JOB(option) (IS_SSM_JOB(option) || IS_SYM_JOB(option)\
            || IS_SRV_JOB(option) || IS_SYM_GRP(option))
#define IS_BATCH_JOB(option) (!IS_WLM_JOB(option))
/* job for which resource usage should be collected */
#define IS_JOB_FOR_ACCT(option) (IS_REAL_SYM_JOB(option) || IS_BATCH_JOB(option))

#define IS_JOB_FOR_SYM(option) (IS_SYM_JOB(option)\
            || IS_SRV_JOB(option) || IS_SYM_GRP(option))

/* Don't send IS_SYM_JOB/IS_SYM_GRP jobs to scheduler;  
 * neither publish events nor brun the job allowed. 
 */
#define IS_SYM_JOB_OR_GRP(jp) \
            (   (jp) != NULL && (jp)->shared != NULL\
             && (  IS_SYM_JOB((jp)->shared->jobBill.options2)\
	         ||IS_SYM_GRP((jp)->shared->jobBill.options2)))

/* name of the lost and find queue and host */
#define  LOST_AND_FOUND      "lost_and_found"

#define  DELETE_NUMBER     -2
#define  DEL_NUMPRO        INFINIT_INT
#define  DEFAULT_NUMPRO    INFINIT_INT -1 
/**
 * \addtogroup calendar_command  calendar_command
 * options  for user calendar commands 
 */
/*@{*/
#define CALADD            1       /**< Add calenda*/
#define CALMOD            2       /**< Modify calenda*/
#define CALDEL            3       /**< Delete calenda*/
#define CALUNDEL          4       /**< Undelete calenda*/
#define CALOCCS           5       /**< Calenda occs*/
/*@}*/

/* for user event commands */
#define EVEADD            1
#define EVEMOD            2
#define EVEDEL            3

/* cpuset allocation return code */
#if defined(SOLARIS) || defined(__irix658)
#define PLUGIN_REQUEUE 126
#define PLUGIN_EXIT  125
#endif

/**
 * \brief  xFile 
 */
struct xFile {
    char *subFn;  /**< Pathname at submission host */
    char *execFn; /**< Pathname at execution host */
/**
 * \addtogroup defs_lsb_XF_OP defs_lsb_XF_OP
 * options  xFile operation 
 */
/*@{*/	
#define  XF_OP_SUB2EXEC         0x1  	/**< Transfer files from submit peer to 
                                         * execution peer */
#define  XF_OP_EXEC2SUB         0x2     /**< Transfer files from execution peer to 
                                         * submit peer */
#define  XF_OP_SUB2EXEC_APPEND  0x4	/**< Transfer files from submit peer to 
                                         * execution peer with appending mode */
#define  XF_OP_EXEC2SUB_APPEND  0x8    	/**< Transfer files from execution peer to 
                                         * submit peer with appending mode */
#define  XF_OP_URL_SOURCE       0x10  
/*@}*/
    int options;  /**< Defined in \ref defs_lsb_XF_OP*/
};

  /* For NQS */
#define  NQS_ROUTE              0x1
#define  NQS_SIG                0x2
#define  NQS_SERVER             0x4


#if defined(WIN32)
/* For Regular expression. Used by reg_comp and reg_exec */

#define MAXNFA  1024
#define MAXTAG  10

#define OKP     1
#define NOP     0

#define CHR     1
#define ANY     2
#define CCL     3
#define BOL     4
#define EOL     5
#define BOT     6
#define EOT     7
#define BOW	8
#define EOW	9
#define REF     10
#define CLO     11

#define END     0

/*
 * The following defines are not meant to be changeable.
 * They are for readability only.
 */

#define MAXCHR	128
#define CHRBIT	8
#define BITBLK	MAXCHR/CHRBIT
#define BLKIND	0170
#define BITIND	07

#define ASCIIB	0177

/*
 * char classification table for word boundary operators BOW
 * and EOW. the reason for not using ctype macros is that we can
 * let the user add into our own table. see re_modw. This table
 * is not in the bitset form, since we may wish to extend it in the
 * future for other char classifications. 
 *
 *	TRUE for 0-9 A-Z a-z _
 */

static unsigned char chrtyp[MAXCHR] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 0, 0, 0, 0
};

#define inascii(x)	(0177&(x))
#define iswordc(x) 	chrtyp[inascii(x)]

/*
 * skip values for CLO XXX to skip past the closure
 */

#define ANYSKIP	2 	/* [CLO] ANY END ...	  */
#define CHRSKIP	3	/* [CLO] CHR chr END ...  */
#define CCLSKIP 18	/* [CLO] CCL 16bytes END ... */

#endif

/*  In LSF7.0.6, we introduce submit_ext structure to support 
 *  extended fields for furture added submit options.
 *  Each new options should have a unique key defined here.
 *  The new defined key should be bigger than 1000. 
 *  Keys below 1000 are used for internal use.
 */
#define JDATA_EXT_TEST         1001  /* submit_ext test */
#define JDATA_EXT_SIMREQ    1002  /* LSF simulator: simReq */
#define JDATA_EXT_JOBIDS     1003  /* jobInfoReq: jobIds */ 
#define JDATA_EXT_PACKJOB_ESUB 1004 /* additional esub info */
#define JDATA_EXT_SOURCECLUSTERS 1005 /* jobInfoReq: sourceClusterNames */
#define JDATA_EXT_JOB_CWD_TTL  1006 /* job cwd time-to-live parameter */
#define JDATA_EXT_AC_TEMPLATE  1007  /*  Dynamic Cluster template name */
#define JDATA_EXT_AC_PREEMPTOPT_VM  1008 /*  Dynamic Cluster VM job preempting options (string)*/
#define JDATA_EXT_AFFINITYINFO      1009 /* notify mbatchd to send affinity allocation info */
#define JDATA_EXT_AC_CHKPNTVM_PERIOD  1010 /* dc chkpnt period*/
#define JDATA_EXT_JOBMODIFY_FROM_LS   1011 /* LS trigger job modification */


/* structure for lsb_submit() call */

/**
 * \brief extend submit data structure
 */
struct submit_ext {
   int num;            /**< number of key value pairs. */
   int *keys;          /**< Array of keys of the extended fields. */
   char **values;      /**< Array of values of the extended fields */
};

/**
 * \brief  submit request structure.
 */
struct submit {
    int     options;        /**<  <lsf/lsbatch.h> defines the flags in \ref lsb_submit_options constructed from bits.
                             * These flags correspond to some of the options of the bsub command line.
	                     * Use the bitwise OR to set more than one flag.*/

    int     options2;       /**< Extended bitwise inclusive OR of some of the flags in \ref lsb_submit_options2. */


    char    *jobName;       /**< The job name. If jobName is
                             * NULL, command is used as the job name.*/
    char    *queue;         /**< Submit the job to this queue. If queue is
                             * NULL, submit the job to a system default
                             * queue. */
    int     numAskedHosts;  /**< The number of invoker specified candidate
                             * hosts for running the job. If numAskedHosts
                             * is 0, all qualified hosts will be considered. */
    char    **askedHosts;   
                  /**< The array of names of invoker specified candidate hosts.
                   *  The number of hosts is given by numAskedHosts. */
    char    *resReq;        
                  /**< The resource requirements of the job.
                   * If resReq is NULL, the batch system will try to obtain
                   * resource requirements for command from the remote task
                   * lists (see \ref ls_task ). If the task does not appear in the
                   * remote task lists, then the default resource requirement
                   * is to run on host() of the same type.  */
    int     rLimits[LSF_RLIM_NLIMITS];  
                  /**< Limits on the consumption of
                   * system resources by all processes belonging to this job.
                   * See getrlimit() for details. If an element of the array
                   * is -1, there is no limit for that resource. For the
                   * constants used to index the array, see \ref lsb_queueinfo . */
    char    *hostSpec;      
                  /**< Specify the host model to use
                   * for scaling rLimits[LSF_RLIMIT_CPU] and
                   * rLimits[LSF_RLIMIT_RUN]. (See \ref lsb_queueinfo).
                   * If hostSpec is NULL, the local host is assumed. */
    int     numProcessors;  /**<  The initial number of processors
                             * needed by a (parallel) job. The default is 1. */
    char    *dependCond;    /**< The job dependency condition. */
    char    *timeEvent;     /**<  Time event string */
    time_t  beginTime;      
                  /**<  Dispatch the job on or after
                   * beginTime, where beginTime is the number of seconds since
                   * 00:00:00 GMT, Jan. 1, 1970 (See time(), ctime()). If
                   * beginTime is 0, start the job as soon as possible. */
    time_t  termTime; 
                  /**<  The job termination deadline.
                   * If the job is still running at termTime, it will be sent
                   * a USR2 signal. If the job does not terminate within 10
                   * minutes after being sent this signal, it will be ended.
                   * termTime has the same representation as beginTime. If
                   * termTime is 0, allow the job to run until it reaches
                   * a resource limit. */
    int     sigValue; 
                  /**< Applies to jobs submitted to a queue that has a run
                   * window (See \ref lsb_queueinfo). Send signal sigValue to
                   * the job 10 minutes before the run window is going to
                   * close. This allows the job to clean up or checkpoint
                   * itself, if desired. If the job does not terminate 10
                   * minutes after being sent this signal, it will be
                   * suspended. */
    char    *inFile;  
                  /**< The path name of the job's standard input file. If
                   * inFile is NULL, use /dev/null as the default. */
    char    *outFile; 
                  /**< The path name of the job's standard output
                   * file. If outFile is NULL, the job's output will be mailed
                   * to the submitter */
    char    *errFile; 
                 /**< The path name of the job's standard
                  * error output file. If errFile is NULL, the standard error
                  * output will be merged with the standard output of the job.*/
    char    *command;       /**< When submitting a job, the command line of the job.  
    			     * When modifying a job, a mandatory parameter that 
			     * should be set to jobId in string format.*/
    char    *newCommand;    /**< New command line for bmod. */
    time_t  chkpntPeriod;   /**< The job is checkpointable with a period of
                             * chkpntPeriod seconds. The value 0 disables
                             * periodic checkpointing.  */
    char    *chkpntDir; 
                   /**< The directory where the chk
                    * directory for this job checkpoint files will be created.
                    * When a job is checkpointed, its checkpoint files are placed
                    * in chkpntDir/chk. chkpntDir can be a relative or absolute
                    * path name.  */
    int     nxf;            /**< The number of files to transfer. */
    struct xFile *xf;       /**< The array of file transfer
                             * specifications. (The xFile structure is defined
                             * in <lsf/lsbatch.h>.) */
    char    *preExecCmd;    /**< The job pre-execution command. */
    char    *mailUser;      /**< The user that results are mailed to.*/
    int    delOptions;      /**< Delete options in options field. */
    int    delOptions2;     /**< Extended delete options in options2 field. */
    char   *projectName;    /**< The name of the project the job will
                             * be charged to. */
    int    maxNumProcessors; /**< Maximum number of processors required
                              * to run the job. */
    char   *loginShell;     /**< Specified login shell used to initialize
                             * the execution environment for the job (see the
                             * -L option of bsub).  */
    char   *userGroup;      /**< The name of the LSF user group
                             * (see lsb.users) to which the job will belong.
                             * (see the -G option of bsub) */
    char   *exceptList;  
            /**< Passes the exception handlers to
             * mbatchd during a job. (see the -X option of bsub).
             * Specifies execption handlers that tell the system
             * how to respond to an exceptional condition for a job.
             * An action is performed when any one of the following
             * exceptions is detected:
             *
       	     * - \b missched - A job has not been scheduled within the time
       	     * event specified in the -T option.
       	     * - \b overrun - A job did not finish in its maximum time (maxtime).
       	     * - \b underrun - A job finished before it reaches its minimum
       	     * running time (mintime).
       	     * - \b abend - A job terminated abnormally. Test an exit code that
       	     * is one value, two or more comma separated values, or a
       	     * range of values (two values separated by a `-' to indivate
       	     * a range). If the job exits with one of the tested values,
       	     * the abend condition is detected.
       	     * - \b startfail - A job did not start due to insufficient system
       	     * resources.
       	     * - \b cantrun - A job did not start because a dependency condition
       	     * (see the -w option of bsub) is invalid, or a startfail
       	     * exception occurs 20 times in a row and the job is suspended.
       	     * For jobs submitted with a time event (see the -T option of bsub),
       	     * the cantrun exception condition can be detected once in each
       	     * time event.
       	     * - \b hostfail - The host running a job becomes unavailable.
       	     *
       	     * When one or more of the above exceptions is detected, you
       	     * can specify one of the following actions to be taken:
       	     * - \b alarm - Triggers an alarm incident (see balarms(1)).
       	     * The alarm can be viewed, acknowledged and resolved.
       	     * - \b setexcept - Causes the exception event event_name to be set.
       	     * Other jobs waiting on the exception event event_name
       	     * specified through the -w option can be triggered. event_name
       	     * is an arbitrary string.
       	     * - \b rerun - Causes the job to be rescheduled for execution.
       	     * Any dependencies associated with the job must be satisfied
       	     * before re-execution takes place. The rerun action can only
       	     * be specified for the abend and hostfail exception conditions.
       	     * The startfail exception condition automatically triggers
       	     * the rerun action.
       	     * - \b kill - Causes the current execution of the job to be
       	     * terminated. This action can only be specified for the
       	     * overrun exception condition.
	     */

    int    userPriority; /**< User priority for fairshare scheduling. */
    char   *rsvId;          /**< Reservation ID for advance reservation. */
    char   *jobGroup;  	    /**< Job group under which the job runs. */
    char   *sla;            /**< SLA under which the job runs. */
    char   *extsched;       /**< External scheduler options. */
    int    warningTimePeriod;  /**< Warning time period in seconds,
                                * -1 if unspecified. */
    char   *warningAction;  /**< Warning action, SIGNAL | CHKPNT |
                             * command, NULL if unspecified. */
    char   *licenseProject; /**< License Scheduler project name. */
    int    options3;        /**< Extended bitwise inclusive OR of options flags in \ref lsb_submit_options3. */
    int    delOptions3;     /**< Extended delete options in options3 field. */
    char   *app;            /**< Application profile under which the job runs. */
    int  jsdlFlag;          /**< -1 if no -jsdl and -jsdl_strict options.
                             * - 0 -jsdl_strict option
                             * - 1 -jsdl option */
    char *jsdlDoc;          /**< JSDL filename*/
    void   *correlator;     /**< ARM correlator */
    char *apsString;        /**<  Absolute priority scheduling string set by
                             * administrators to denote static system APS
                             * value or ADMIN factor APS value. This field
                             * is ignored by \ref lsb_submit. */
    char  *postExecCmd;     /**< Post-execution commands specified
    		             * by -Ep option of bsub and bmod. */
    char   *cwd;            /**< Current working directory
    		             * specified by -cwd option of bsub and bmod. */
    int      runtimeEstimation;   /**< Runtime estimate specified by -We
                                   * option of bsub and bmod.  */
    char *requeueEValues;   /**< Job-level requeue exit values specified
    	                     * by -Q option of bsub and bmod. */
    int     initChkpntPeriod;  /**< Initial checkpoint period specified
                                * by -k option of bsub and bmod. */
    int     migThreshold;      /**< Job migration threshold specified
                                * by -mig option of bsub and bmod. */
    char *notifyCmd;           /**< Job resize notification command to be
                                * invoked on the first execution host when
                                * a resize request has been satisfied. */
    char *jobDescription;      /**< Job description. */
#if defined(LSF_SIMULATOR)
    char *simReq;              /**< simulation related options */
#endif
    struct submit_ext *submitExt; /**< For new options in future  */
    int    options4;        /**< Extended bitwise inclusive OR of options flags in \ref lsb_submit_options4. */
    int    delOptions4;     /**< Extended delete options in options4 field. */
    int     numAskedClusters;  /**< The number of invoker specified candidate
                             * clusters for running the job. */
    char    **askedClusters;
                  /**< The array of names of invoker specified candidate clusters.
                   *  The number of clusters is given by numAskedClusters. */
	char*  flow_id;         /** < Internal option, indicate the process manager flow id. no usage. */
	char*  outdir;         /**< Output directory
                             * specified by -outdir option of bsub and bmod. */
    char *dcTmpls;         /**< Dynamic Cluster templates */
    char *dcVmActions;     /**< Dynamic Cluster vm actions */
    char   *networkReq;        /**< PE network requirement, specified by -network. */
};


/* Maximum supported PE networks */
#define MAX_PE_NETWORK_NUM 8
#define DEF_PE_NETWORK_NUM 0

/** 
 * \for PE network requirement
 */
struct networkReq {
    int options;  /* network options */
    int nInstance; /* number of nInstance */
    int nProtocol;    /* number of protocols */
    char *protocols; /* network protocols list string. Two protocol names are seperated by ',' */
};

struct networkAlloc {
    char *networkID;  /* allocated networkID array */
    int  num_window; /* allocated windwos number array */
};

struct networkInfoEnt {
    char *networkID;
    int  status;
    int  numReservedWindow;
    int  numTotalWindow;
};

/**
 * \ PE network options
 */
#define PE_NETWORK_TYPE_SN_ALL       0x001
#define PE_NETWORK_MODE_US           0x002
#define PE_NETWORK_USAGE_SHARED      0x004
#define PE_NETWORK_TYPE_UNSPEC       0x008

/**
  * \ PE network status
  */
#define PE_NETWORK_STAT_OK           0x0
#define PE_NETWORK_STAT_FULL         0x01
#define PE_NETWORK_STAT_EXCLUSIVE    0x02
#define PE_NETWORK_STAT_UNAVAIL      0x04

#if defined(COMPAT32) && defined(INT32JOBID)

/* a LSF32 version of the struct submit, which does not contain
 *     (1) int delOptions2;
 *     (2) int userPriority;
 * which are defined in LSF40 version of struct submit.
 *
 * This structure is used by the wrapper function of lsb_submit()
 * to obtain the correct structure from the function caller. Otherwise
 * the library code may access the wrong fields/memory in the structure.
 *
 */
struct submit32 {
    int     options;
    int     options2;
    char    *jobName;
    char    *queue;
    int     numAskedHosts;              /* num of user specified hosts */
    char    **askedHosts;
    char    *resReq;
    int     rLimits[LSF_RLIM_NLIMITS];  /* resource limits (soft) */
    char    *hostSpec;
    int     numProcessors;              /* min num of proc for the job */
    char    *dependCond;                /* depend_cond string */
    char    *timeEvent;                 /* time event string */
    time_t  beginTime;
    time_t  termTime;
    int     sigValue;
    char    *inFile;                    /* Submission input file */ 
    char    *outFile;                   /* Submission output file */
    char    *errFile;                   /* Submission error file */
    char    *command;
    char    *newCommand;                /* New command for bmod */
    time_t  chkpntPeriod;               /* chkpnt period */
    char    *chkpntDir;                 /* chkpnt directory */
    int     nxf;                        /* Number of files to transfer */
    struct xFile *xf;                   /* Files to transfer */
    char    *preExecCmd;                /* Command string (+arguments) to be
                                           pre_executed */
    char    *mailUser;                  /* The specified user that results are
                                           mailed to */
    int    delOptions;
    char   *projectName;                /* Project name */
    int    maxNumProcessors;            /* max num of processors for the job */
    char   *loginShell;                 /* login shell specified by -L */
    char   *userGroup;                  /* User group */
    char   *exceptList;                 /* List of exception handlers */
};
#endif /* COMPAT32 */

/**
 * \brief submit reply.
 */
struct submitReply {
     char    *queue;	     /**< The queue the job was submitted to. */
     LS_LONG_INT  badJobId;  /**< DependCond contained badJobId but badJobId
     		 	      * does not exist in the system. */
     char    *badJobName;	
               /**< DependCond contained badJobName but badJobName
	        * does not exist in the system.
	        *
	        * If the environment variable BSUB_CHK_RESREQ is set, the value
	        * of lsberrno is either LSBE_RESREQ_OK or LSBE_RESREQ_ERR,
	        * depending on the result of resource requirement string
	        * checking. The badJobName field contains the detailed
	        * error message. */
     int     badReqIndx;
               /**< If lsberrno is LSBE_BAD_HOST,
		* (**askedHosts)[badReqIndx] is not a host known to the system.
		* If lsberrno is LSBE_QUEUE_HOST, (**askedHosts)[badReqIndx]
		* is not a host used by the specified queue. If lsberrno is
		* LSBE_OVER_LIMIT, (*rLimits)[badReqIndx] exceeds the queue's
		* limit for the resource. */
};

/**
 * \brief pack submission reply.
 */
 struct packSubmitReply {
    int num;    /**< the number of submit replies. */
    int *lserrno; /**< lserrno returned for each submission, 0 on success. */
    struct submitReply **submitReplies; /**< submission result for each job in the request pack. */
};

/** 
 * \brief pack submission request.
 */
struct packSubmit {
    int num;  /**< the number of job submission requests. */
    struct submit **reqs; /**< job submission requests. */
};

/**
 * \brief  submit migration request.
 */ 
struct submig {
    LS_LONG_INT jobId;    /**< The job ID of the job to be migrated.*/
    int  options;         /**< Please refer to \ref lsb_submit_options.*/
    int  numAskedHosts;   /**< The number of hosts supplied as candidates 
                           * for migration.*/
    char **askedHosts;    /**< An array of pointers to the names of candidate
                           * hosts for migration.*/
    int acAction; /**< Dynamic Cluster provision action */
};

/* structure for lsb_addjgrp() call */
struct jgrpAdd {
    char    *groupSpec;
    char    *timeEvent;
    char    *depCond;
    char    *sla;
    int        maxJLimit;
};

/* structure for lsb_modjgrp() call */
struct jgrpMod {
    char   *destSpec;
    struct jgrpAdd  jgrp;
};

/* structure for lsb_addjgrp() and lsb_modjgrp() call reply */
struct jgrpReply {
	char    *badJgrpName;
        int     num;
        char    **delJgrpList;
};

/**
 * \brief Signal a group of jobs.
 */
struct signalBulkJobs {
    int           signal;       /**< Signal type */
    int           njobs;        /**< Number of jobs */
    LS_LONG_INT   *jobs;        /**< Jobids list */
    int           flags;        /**< Flags */
};

/* structure for lsb_ctrljgrp() call */
struct jgrpCtrl{
    char   *groupSpec;
    char   *userSpec;
    int    options;
    int    ctrlOp;  /* JGRP_RELEASE, JGRP_HOLD, JGRP_DEL */
};

/* Indicate no change in chkpnt period for lsb_chkpntjob() */
#define LSB_CHKPERIOD_NOCHNG -1

/**
 * \addtogroup chkpnt_job_option  chkpnt_job_option 
 * checkpoint job options()
 */
/*@{*/
#define LSB_CHKPNT_KILL  0x1  /**< Kill process if successfully chkpnted */
#define LSB_CHKPNT_FORCE 0x2  /**< Force chkpnt even if non-chkpntable
			       * conditions exist. */
#define LSB_CHKPNT_COPY  0x3  /**< Copy all regular files in use by the 
                               * checkpointed process to the checkpoint 
                               *directory. */ 
#define LSB_CHKPNT_MIG   0x4  /**< Chkpnt for the purpose of migration */    
#define LSB_CHKPNT_STOP  0x8  /**< Stop  process if successfully chkpnted */
/*@}*/

/**
 * \addtogroup kill_requeue  kill_requeue 
 * kill and requeue a job options()
 */
/*@{*/
#define LSB_KILL_REQUEUE 0x10 /**< Kill then re-queue a job */
/*@}*/

#define LSB_RMT_JOB_MOD  0x20 /* remote job control action, job modify */
#define LSB_RMT_JOB_SWITCH  0x40 /* remote job control action, job switch */
#define LSB_RMT_JOB_MOVE  0x80 /* remote job control action, job top */
#define LSB_RMT_JOB_RUN  0x100 /* remote job control action, job brun */

#define IS_RMT_JOB_CTRL(flags) ((flags) == LSB_RMT_JOB_MOD || \
                            (flags) == LSB_RMT_JOB_SWITCH || \
                            (flags) == LSB_RMT_JOB_MOVE || \
                            (flags) == LSB_RMT_JOB_RUN )

#define LSB_RMT_JOBCTRL_NONE    -1      /**< Indicates this is not a remote job control operation */
#define LSB_RMT_JOBCTRL_INIT    0       /**< Remote job control operation starts */
#define LSB_RMT_JOBCTRL_REQ_SENT    1   /**< Job control request has been sent to remote cluster */
#define LSB_RMT_JOBCTRL_REQ_RCVD    2   /**< Received a job control request from remote cluster */
#define LSB_RMT_JOBCTRL_REPLY_RCVD  3   /**< Received reply for the job control request */
#define LSB_RMT_JOBCTRL_ACK_RCVD    4   /**< Reply has been sent and the acknowledge for the reply received */

struct rmtJobCtrlRecord {
    char *rmtCluster;       /**< Destination cluster to which the operated remote jobs belong */
    char *rmtJobCtrlId;     /**< The identifier of this remote control session */
    int  numSuccJobId;      /**< The number for the successfully operated job ID */
    LS_LONG_INT *succJobIdArray;    /**< Successfully operated job ID array */
    int  numFailJobId;      /**< The number of the job ID that failed */
    LS_LONG_INT *failJobIdArray;    /** Job ID array contains those jobs that failed */
    int  *failReason;       /**< For each job ID that failed, its failure reason is recorded in this array */
};

/* options for lsb_openjobinfo() */

/**
 * \addtogroup defs_lsb_openjobinfo  defs_lsb_openjobinfo 
 * Information options about job.
 */
/*@{*/
#define ALL_USERS       "all"         /**< Reserved user name */
/**
 * \defgroup defs_lsb_openjobinfo_a defs_lsb_openjobinfo_a
 * defs_lsb_openjobinfo_a is part of defs_lsb_openjobinfo
 */
/*@{*/
#define ALL_JOB         0x0001 
         /**< Information about all jobs, including unfinished jobs (pending,
          * running or suspended) and recently finished jobs. LSF remembers
          * jobs finished within the preceding period. This period is set by
          * the parameter CLEAN_PERIOD in the lsb.params file. The default is
          * 3600 seconds (1 hour). (See lsb.params). The command line equivalent
          * is bjobs -a.
          */   
#define DONE_JOB        0x0002   /**< Information about recently finished jobs.*/
#define PEND_JOB        0x0004   /**< Information about pending jobs.*/
#define SUSP_JOB        0x0008   /**< Information about suspended jobs.*/
#define CUR_JOB         0x0010   /**< Information about all unfinished jobs.*/
#define LAST_JOB        0x0020   /**< Information about the last submitted job.*/
/*@}*/                           
#define RUN_JOB         0x0040   /**< Information about all running jobs*/
#define JOBID_ONLY      0x0080   /**< Information about JobId only.*/
#define HOST_NAME       0x0100   /**< Internal use only.*/
#define NO_PEND_REASONS 0x0200   /**< Exclude pending jobs.*/
#define JGRP_INFO       0x0400   /**< Return group info structures */
#define JGRP_RECURSIVE  0x0800   /**< Recursively search job group tree */
#define JGRP_ARRAY_INFO 0x1000   /**< Return job array info structures */
#define JOBID_ONLY_ALL  0x02000  /**< All jobs in the core */
#define ZOMBIE_JOB      0x04000  /**< All zombie jobs */
#define TRANSPARENT_MC  0x08000  /**< Display remote jobs by their
                                  * submission jobid. */
#define EXCEPT_JOB      0x10000  /**< Exceptional jobs */
#define MUREX_JOB       0x20000  /**< Display for murex jobs */
#define FORWARDED_JOB   0x40000 /**< Grid broker -f and -fwd, show forwarded jobs */
#define SYM_TOP_LEVEL_ONLY   0x80000    /**< Only show top-level symphony job */
#define JGRP_NAME            0x100000   /**< For internal use only */
#define COND_HOSTNAME        0x200000   /**< Condensed host group */
#define FROM_BJOBSCMD        0x400000   /**< Called from command,
                                         * for internal use only */
#define WITH_LOPTION         0x800000   /**< -l in command parameter,
                                         * for internal use only */
#define APS_JOB              0x1000000  /**< Jobs submitted to aps queue */
#define UGRP_INFO            0x2000000  /**< Information about user group.*/
                                        /** RFC#1531: -G option support*/
#define TIME_LEFT            0x4000000  /**< -WL*/
       		          /**< Estimated time remaining based on the runtime
       		           * estimate or runlimit.*/
#define FINISH_TIME            0x8000000  /* -WF*/
       		/**< Estimated finish time based on the runtime estimate
       		 * or runlimit.*/
#define COM_PERCENTAGE            0x10000000  /* -WP*/
            /**< Estimated completion percentage based on the runtime
             * estimate or runlimit. If options is 0, default to CUR_JOB.*/
#define SSCHED_JOB           0x20000000  /**< -ss option */


#define KILL_JGRP_RECURSIVE  0x40000000  /**< -G option */
#define REMOTE_PENDING_JOB   0x80000000  /* brequeue -p & -a option,
                                          * is used for select remote pending job */

/*@}*//* don't define anything after REMOTE_PENDING_JOB, reached 32 bit limit */

/**
 * \addtogroup group_nodetypes group_nodetypes
 * define statements group node types.
 */
/*@{*/
#define    JGRP_NODE_JOB     1       /**<  Job*/
#define    JGRP_NODE_GROUP   2	     /**<  Group*/
#define    JGRP_NODE_ARRAY   3	     /**<  Array*/
#define    JGRP_NODE_SLA     4       /**<  SLA*/
/*@}*/

/* jobId macros */
#ifndef INT32JOBID
#define LSB_MAX_ARRAY_JOBID	0x0FFFFFFFF
#define LSB_MAX_ARRAY_IDX	0x07FFFFFFF
#define LSB_MAX_SEDJOB_RUNID    (0x0F)
#define LSB_JOBID(array_jobId, array_idx)  \
                 (((LS_UNS_LONG_INT)array_idx << 32) | array_jobId)
#define LSB_ARRAY_IDX(jobId) \
        (((jobId) == -1) ? (0) : (int)(((LS_UNS_LONG_INT)jobId >> 32) \
                                                    & LSB_MAX_ARRAY_IDX))
#else
#define LSB_MAX_ARRAY_JOBID	0x0FFFFF
#define LSB_MAX_ARRAY_IDX	0x07FF
#define LSB_JOBID(array_jobId, array_idx)  \
                 (((unsigned int)array_idx << 20) | array_jobId)
#define LSB_ARRAY_IDX(jobId) \
                 (((jobId) == -1) ? (0) : ((unsigned int)jobId >> 20))
#endif /* INT32JOBID */
#define LSB_ARRAY_JOBID(jobId)\
                 (((jobId) == -1) ? (-1) : (int)(jobId & LSB_MAX_ARRAY_JOBID))

/* Status of a job group */

#define    JGRP_INACTIVE      0
#define    JGRP_ACTIVE        1
#define    JGRP_HOLD          2
#define    JGRP_UNDEFINED     -1

/**
 * \addtogroup jobgroup_controltypes jobgroup_controltypes
 * define statements job group control types.
 */
/*@{*/

#define  JGRP_RELEASE        1  /**< bgrelease */
#define  JGRP_HOLD           2  /**< bghold */
#define  JGRP_DEL            3  /**< bgdel */
/*@}*/

/**
 * \addtogroup jobgroup_counterIndex jobgroup_counterIndex
 *  Following can be used to index  into 'counters' array.
 */
/*@{*/
#define     JGRP_COUNT_NJOBS   0       	/**< Total jobs in the array*/
#define     JGRP_COUNT_PEND    1       	/**< Number of pending jobs in the array*/
#define     JGRP_COUNT_NPSUSP  2       	/**< Number of held jobs in the array*/
#define     JGRP_COUNT_NRUN    3       	/**< Number of running jobs in the array*/
#define     JGRP_COUNT_NSSUSP  4       	/**< Number of jobs suspended by the system
                                         * in the array*/
#define     JGRP_COUNT_NUSUSP  5       	/**< Number of jobs suspended by the user
                                         * in the array*/
#define     JGRP_COUNT_NEXIT   6       	/**< Number of exited jobs in the array*/
#define     JGRP_COUNT_NDONE   7       	/**< Number of successfully completed jobs*/
#define     JGRP_COUNT_NJOBS_SLOTS   8  /**< Total slots in the array*/
#define     JGRP_COUNT_PEND_SLOTS  9    /**< Number of pending slots in the array*/
#define     JGRP_COUNT_RUN_SLOTS    10  /**< Number of running slots in the array*/
#define     JGRP_COUNT_SSUSP_SLOTS  11  /**< Number of slots suspended by the system
                                         * in the array*/
#define     JGRP_COUNT_USUSP_SLOTS  12  /**< Number of slots suspended by the user
                                         * in the array*/
#define     JGRP_COUNT_RESV_SLOTS  13   /**< Number of reserverd slots in the array*/
/*@}*/

/* job group modification types */
#define JGRP_MOD_LIMIT 0x1

/*the number of counters of job group
* based on job level
*/
#define    NUM_JGRP_JOB_COUNTERS 8
/* the number of all counters of job group,
* including job level and slot level
*/
#define    NUM_JGRP_COUNTERS 14  /* {njobs, npend, 
       	    npsusp, nrun, 
       	    nssusp nususp,
       	    nexit, ndone} */
#define    JGRP_CREATE_EXP   0x01 /* job group is created explicitly */
#define    JGRP_CREATE_IMP   0x02 /* job group is created implicitly */
/* The LSF job group.
 */
struct jgrp {
    char   *name;
    char   *path;
    char   *user;
    char   *sla;
    int    counters[NUM_JGRP_COUNTERS];
    int    maxJLimit;
};

/* Structure for lsb_setjobattr() call */
struct jobAttrInfoEnt {
    LS_LONG_INT jobId;                   /* id of the job */
    u_short   port;                      /* port number of the job */
    char      hostname[MAXHOSTNAMELEN];  /* first executing host of the job */
};

/**
 * \brief  job attribute setting log.
 */
struct jobAttrSetLog {
    int       jobId;         /**< The unique ID for the job */
    int       idx;           /**< Job array index; must be 0 in JOB_NEW */
    int       uid;           /**< The user who requested the action */
    int       port;          /**< Job attributes */
    char      *hostname;     /**< Name of the host */
};

/**
 * \brief  job information head.
 */
struct jobInfoHead {
    int   numJobs;          /**< The number of jobs in the connection*/
    LS_LONG_INT *jobIds;    /**< An array of job identification numbers in the conection */
    int   numHosts;	    /**< The number of hosts in the connection */
    char  **hostNames;      /**< An array of host names in the connection */
    int   numClusters;      /**< The number of clusters in the connection */
    char  **clusterNames;   /**< An array of cluster names in the connection */
    int   *numRemoteHosts;  /**< The number of remoteHosts in the connection*/
    char  ***remoteHosts;   /**< An array of remoteHost names in the connection */
};

/**
 * \brief job Information head extent
 */
struct jobInfoHeadExt {
    struct jobInfoHead  *jobInfoHead;  /**<  Job Information header */
    struct groupInfoReply *groupInfo;  /**<  Group Information returned */
};

/**
 * \brief structure reserveItem 
 */
struct reserveItem {
    char    *resName;   /**< Name of the resource to reserve. */
    int     nHost;     	/**< The number of hosts to reserve this resource.*/
    float   *value;    	/**< Amount of reservation is made on each host. Some
                         * hosts may reserve 0. */
    int     shared;    	/**< Flag of shared or host-base resource */
};

#define  AC_JOB_INFO_PRE_DISPATCH    0x01
#define  AC_JOB_INFO_VMJOB           0x02
#define  AC_JOB_INFO_SAVING_MACHINES 0x04
#define  AC_JOB_INFO_MACHINE_SAVED   0x08
#define  AC_JOB_INFO_RESTORING_MACHINES 0x10
#define  AC_JOB_INFO_PROV_DONE          0x20
#define  AC_JOB_INFO_MIGRATING        0x80
#define  AC_JOB_INFO_CHECKPOINTING_MACHINES    0x200
#define  AC_JOB_INFO_RESTARTING_MACHINES 0x800
#define  AC_JOB_INFO_SHUTTINGDOWN_MACHINES 0x2000




struct acInfo {
    int     flag;       /**< Dynamic Cluster info flag */
    time_t  provStartTime; /**< provision start time */
    time_t  provEndTime; /**<provision end time */
    char    *reqId;     /**< provision request id */
    int     numVmHosts; /**< number of virtual machines */
    char    **vmHosts;  /**< virtual machine host names */
    int     numVmHostTypes; /**< number of (vm) host types */
    char    **vmHostTypes;  /**< (vm) host types */
};

typedef struct affinityTaskInfo {
    PU_t            pu_type;           /**< allocated pu type */
    char           *pu_list;           /**< allocated pu list */
    PU_t            expu_type;         /**< exclusive pu type */
    memBindPolicy_t mem_bind_policy;   /**< memory bind policy */
    int             mem_node_id;       /**< memory node id */
    PU_t            cpu_bind_level;    /**< cpu bind level */
} affinityTaskInfo_t;

typedef struct affinityHostInfo {
    char           *hostname;          /**< host name */
    int             num_task;          /**< number of tasks */
    float           mem_per_task;      /**< memory per task */
    affinityTaskInfo_t  *taffinity;    /**< affinity info for each task */
} affinityHostInfo_t;

/**
 * \brief  job information entry. 
 */
struct jobInfoEnt {
    LS_LONG_INT jobId;     /**< The job ID that the LSF system assigned to the job.*/
    char    *user;         /**< The name of the user who submitted the job.*/
    int     status;        /**< The current status of the job.Possible values
                            * areshown in job_states*/
    int     *reasonTb;     /**< Pending or suspending reasons of the job */
    int     numReasons;    /**< Length of reasonTb[] */
    int     reasons;       /**< The reason a job is pending or suspended. */
    int     subreasons;    /**< The reason a job is pending or suspended. If status
                            * is JOB_STAT_PEND, the values of reasons and subreasons
                            * are explained by \ref lsb_pendreason. If status is
                     	    * JOB_STAT_PSUSP, the values of reasons and subreasons
                     	    * are explained by \ref lsb_suspreason.
                     	    *  
                     	    * When reasons is PEND_HOST_LOAD or SUSP_LOAD_REASON, 
                     	    * subreasons indicates the load indices that are out
                     	    * of bounds. If reasons is PEND_HOST_LOAD, subreasons
                     	    * is the same as busySched in the hostInfoEnt structure;
                     	    * if reasons is SUSP_LOAD_REASON, subreasons is the same
                     	    * as busyStop in the hostInfoEnt structure.
                     	    * (See \ref lsb_hostinfo) */
    int     jobPid;        /**< The job process ID. */
    time_t  submitTime;    /**< The time the job was submitted, in seconds since
                            * 00:00:00 GMT, Jan. 1, 1970. */
    time_t  reserveTime;   /**< Time when job slots are reserved */
    time_t  startTime;     /**< The time that the job started running, if it has
                            * been dispatched.*/
    time_t  predictedStartTime;   /**< Job's predicted start time */
    time_t  endTime;              /**< The termination time of the job, if it
                                   * has completed.*/
    time_t  lastEvent;     /**< Last time event */
    time_t  nextEvent;     /**< Next time event */
    int     duration;      /**< Duration time (minutes) */
    float   cpuTime;       /**< CPU time consumed by the job */
    int     umask;         /**< The file creation mask when the job was submitted.*/
    char    *cwd;          /**< The current working directory when the job
                            * was submitted.*/
    char    *subHomeDir;   /**< Home directory on submission host.*/    
    char    *fromHost;     /**< The name of the host from which the job was 
                            * submitted. */
    char    **exHosts;     /**< The array of names of hosts on which the job
                            * executes. */
    int     numExHosts;    /**< The number of hosts on which the job executes. */
    float   cpuFactor;     /**< The CPU factor for normalizing CPU and wall clock
                            * time limits.*/
    int     nIdx;          /**< The number of load indices in the loadSched and
                            * loadStop arrays.*/
    float   *loadSched;    /**< The values in the loadSched array specify
                            * the thresholds for the corresponding load indices.
                            * Only if the current values of all specified load
                            * indices of a host are within (below or above, 
                            * depending on the meaning of the load index) their
                            * corresponding thresholds may the suspended job be
                            * resumed on this host. 
                     	    *
                     	    * For an explanation of the entries in the loadSched,
                     	    * see \ref lsb_hostinfo.
                     	    */
    float   *loadStop;     /**< The values in the loadStop array specify the
                            * thresholds for job suspension; if any of the current
                            * load index values of the host crosses its threshold,
                     	    * the job will be suspended. 
                     	    *
                     	    * For an explanation of the entries in the loadStop,
                     	    * see \ref lsb_hostinfo.
                     	    */
    struct  submit submit;  /**< Structure for \ref lsb_submit call.*/
    int     exitStatus;     /**< Job exit status.*/
    int     execUid;        /**< Mapped UNIX user ID on the execution host.*/
    char    *execHome;      /**< Home directory for the job on the execution host.*/
    char    *execCwd;       /**< Current working directory for the job on the
                             * execution host.*/
    char    *execUsername;  /**< Mapped user name on the execution host.*/
    time_t  jRusageUpdateTime; /**< Time of the last job resource usage update.*/
    struct  jRusage runRusage; /**< Contains resource usage information for the job.*/
    int     jType;          /**< Job type.N_JOB, N_GROUP, N_HEAD */
    char    *parentGroup;   /**< The parent job group of a job or job group. */
    char    *jName;         /**< If jType is JGRP_NODE_GROUP, then it is the job
                             * group name. Otherwise, it is the
                     	     *job name. */
    int     counter[NUM_JGRP_COUNTERS];  /**< Index into the counter array, only
                                          * used for job arrays. Possible index values are
                                          * shown in \ref jobgroup_counterIndex*/
    u_short port;           /**< Service port of the job. */
    int     jobPriority;    /**< Job dynamic priority */
    int numExternalMsg;     /**< The number of external messages in the job. */
    struct jobExternalMsgReply **externalMsg; /**< This structure contains the
                                               * information required to define
                                               * an external message reply.*/
    int     clusterId;      /**< MultiCluster cluster ID. If clusterId is greater
                             * than or equal to 0, the job is a pending remote job,
                             * and \ref lsb_readjobinfo checks for host_name\@cluster_name.
                             * If host name is needed, it should be found in 
                             * jInfoH->remoteHosts. If the remote host name is not
                             * available, the constant string remoteHost is used.*/
    char   *detailReason;   /**<  Detail reason field */
    float   idleFactor;     /**< Idle factor for job exception handling. If the job
                             * idle factor is less than the specified threshold, LSF
                             * invokes LSF_SERVERDIR/eadmin to trigger the action for
                             * a job idle exception.*/
    int     exceptMask;     /**< Job exception handling mask */

    char   *additionalInfo; /**< Placement information of LSF jobs. Arbitrary information of
                             * a job stored as a string */
    int     exitInfo;       /**< Job termination reason. See lsbatch.h.*/
    int    warningTimePeriod; /**< Job warning time period in seconds; -1 if unspecified. */
    char   *warningAction;  /**< Warning action, SIGNAL | CHKPNT |
                             *command, NULL if unspecified */
    char   *chargedSAAP;    /**< SAAP charged for job */
    char   *execRusage;     /**< The rusage satisfied at job runtime */
    time_t rsvInActive;     /**< The time when advance reservation expired or was deleted. */
    int    numLicense;      /**< The number of licenses reported from License Scheduler. */
    char   **licenseNames;  /**< License Scheduler license names.*/
    float  aps;             /**< Absolute priority scheduling (APS) priority value.*/
    float  adminAps;        /**< Absolute priority scheduling (APS) string set by
                             * administrators to denote static system APS value */
    int    runTime;         /**< The real runtime on the execution host. */
    int reserveCnt;         /**< How many kinds of resource are reserved by this job*/
    struct reserveItem *items; /**< Detail reservation information for each
                                * kind of resource*/
    float  adminFactorVal;  /**< Absolute priority scheduling (APS) string set by
                             * administrators to denote ADMIN
                     	     * factor APS value. */
    int    resizeMin;       /**< Pending resize min. 0, if no resize pending. */	
    int    resizeMax;       /**< Pending resize max. 0, if no resize pending */	
    time_t resizeReqTime;   /**< Time when pending request was issued */
    int    jStartNumExHosts; /**< Number of hosts when job starts */
    char   **jStartExHosts;  /**< Host list when job starts */
    time_t lastResizeTime;   /**< Last time when job allocation changed */
    int    numhRusages;         /**< The number of host-based rusage entries in the list hostRusage */
    struct hRusage * hostRusage; /**< Host based rusage list, one per host */
    int maxMem;  /**< job maximum memory usage*/
    int avgMem;  /**< job average memory usage */
    time_t fwdTime;             /**< Time when job forwarded */
    char*  srcCluster;         /**< Cluster name which job accepted */
    LS_LONG_INT srcJobId;        /**< The job Id assigned by the cluster which job accepted */
    char*  dstCluster;          /**< Cluster name which job forwarded */
    LS_LONG_INT dstJobId;        /**< The job Id assigned by the cluster which job forwarded */
 
    time_t brunJobTime;          /**< Time when job is being brun */
    char*  brunJobUser;          /**< who issue brun */
    char*  appResReq;            /**< The resreq for the application to which the job was submitted. */
    char*  qResReq;              /**< The resreq for the queue to which the job was submitted. */
    char*  combinedResReq;       /**< The result of mbd merging job/app/queue level resreqs for each job. */
    char*  effectiveResReq;      /**< Resource requirements used by Scheduler to dispatch jobs. */
    struct acInfo  *acinfo; /**< Dynamic Cluster information */
    char*  outdir;          /**< The output directory */
    int    isInProvisioning;     /**< Is the job in provisioning except live migration*/
    int    acJobWaitTime;        /**< Dynamic Cluster VM job wait time, include provision time */
    int    totalProvisionTime;   /**< Dynamic Cluster VM job total provision time */
    char*  subcwd;               /**< The submission directory */
    int    num_network;     /**< number of PE network allocation */
    struct networkAlloc *networkAlloc; /**< PE network allocation */
    int    numhostAffinity;      /**< number of hostAffinity */ 
    affinityHostInfo_t *hostAffinity; /**< affinity allocation on each host */
};
/* the bit set for jobInfoEnt->exceptMask */
#define J_EXCEPT_OVERRUN   0x02
#define J_EXCEPT_UNDERUN   0x04
#define J_EXCEPT_IDLE      0x80
#define J_EXCEPT_RUNTIME_EST_EXCEEDED 0x100

/* exception showed by bjobs -l and bacct -l*/
#define OVERRUN  "overrun"
#define UNDERRUN "underrun"
#define IDLE     "idle"
#define SPACE    "  "
#define RUNTIME_EST_EXCEEDED "runtime_est_exceeded"

#if defined(COMPAT32) && defined(INT32JOBID)

/* a LSF32 version of the struct jobInfoEnt, which does not contain
 *     (1) int jobPriority;
 *     (2) int numExternalMsg;
 *     (3) struct jobExternalMsgReply **externalMsg;
 * which are defined in LSF40 version of struct jobInfoEnt. Also, the
 * struct submit in the structure is also different.
 *
 * This structure is used by the wrapper function of \ref lsb_readjobinfo
 * to return the correct structure back to the function caller. Otherwise
 * the caller may access the wrong fields/memory in the structure.
 *
 */
struct jobInfoEnt32 {
    int     jobId;
    char    *user;
    int     status;
    int     *reasonTb;   /* pending or suspending reasons of the job */
    int     numReasons;  /* length of reasonTb[] */
    int     reasons;     /* reserved for future use */
    int     subreasons;  /* reserved for future use */
    int     jobPid;      /* process Id of the job */
    time_t  submitTime;  /* time when the job is submitted */
    time_t  reserveTime; /* time when some slots are reserved for the job */
    time_t  startTime;   /* time when job is actually started */
    time_t  predictedStartTime;    /* job's predicted start time */
    time_t  endTime;     /* time when the job finishes */
    time_t  lastEvent;   /* last time event */
    time_t  nextEvent;   /* next time event */
    int     duration;    /* duration time (minutes) */
    float   cpuTime;     /* CPU time consumed by the job */
    int     umask;
    char    *cwd;
    char    *subHomeDir;
    char    *fromHost;   /* host from which the job is submitted */
    char    **exHosts;   /* host(s) on which the job executes */
    int     numExHosts;  /* number of execution hosts */
    float   cpuFactor;
    int     nIdx;
    float   *loadSched;  /* Stop scheduling new jobs if over */
    float   *loadStop;   /* Stop jobs if over this load */
    struct  submit32 submit;
    int     exitStatus;
    int     execUid;
    char    *execHome;
    char    *execCwd;
    char    *execUsername;
    time_t  jRusageUpdateTime;
    struct  jRusage runRusage;
    int     jType;                      /* N_JOB, N_GROUP, N_HEAD */
    char    *parentGroup;               /* the group or job's parent spec */
    char    *jName;                     /* the group or job's name */
    int     counter[NUM_JGRP_COUNTERS]; /* {njobs, npend, npsusp, nrun, nssusp
                                            nususp, nexit, ndone} */
    u_short port;        /* service port of the job */
};
#endif /* COMPAT32 */

/* LSF7.0 moved jobInfoReq structure definition from 
 * daemonout.h to lsbatch.h. This structure will work
 * with new API \ref lsb_openjobinfo_req
 */
 
 /**
  * \brief  job Information Request 
  */
struct jobInfoReq {
    int    options;        /**< Options defined in \ref defs_lsb_openjobinfo */
    char   *userName;      /**< Name of user whose jobs to be checked */
    LS_LONG_INT jobId;     /**< Job id, 0 means all jobs or more than one job specified     */
    char   *jobName;       /**< Job name                     */
    char   *queue;         /**< Queue name                   */
    char   *host;          /**< Check jobs running on this host */
    char   *app;           /**< Job application              */
    char   *jobDescription;/**< Job description              */
    char   *jobGroupName;       /**< Job group name          */
    char   *projectName;        /**< project name            */
    char   *licenseProjectName; /**< licnese project name    */
    char   *serviceClassName;   /**< service class name      */

   
    struct submit_ext *submitExt; /**< For new options in future  */
    char   *sourceClusterName; /**<Source Cluster Name, NULL means more than one job specified */
};


/* PKVPs key */
/* For jobQueryCounters */
#define JOBCOUNT_NUM_SLOTS_RUN          "Slots of run jobs"
#define JOBCOUNT_NUM_SLOTS_SSUPS        "Slots of SSUSP jobs"
#define JOBCOUNT_NUM_SLOTS_USUSP        "Slots of USUSP jobs"
#define JOBCOUNT_NUM_SLOTS_UNKNOWN      "Slots of UNKNOWN jobs"
#define JOBCOUNT_NUM_SLOTS_PEND         "Slots of PEND jobs"
#define JOBCOUNT_NUM_SLOTS_FWDPEND      "Slots of FWDPEND jobs"
#define JOBCOUNT_NUM_SLOTS_PROV         "Slots of PROV jobs"









struct jobQueryCounters
{
    int runNum;                 /* Number of RUN job slots */
    int ssuspNum;               /* Number of SSUSP job slots */
    int ususpNum;               /* Number of USUSP job slots */
    int unknownNum;             /* Number of UNKNOWN job slots */
    int pendNum;                /* Number of PEND job slots */
    int fowardPendNum;          /* Number of FOWARDPEND job slots */
    int provNum;                /* Number of PROV job slots */
};


/* PKVPs key */
/* For queryInfo */
#define QUERYINFO_JOB_LIST_NUM              "jobListNum"
#define QUERYINFO_JOB_LIST                  "jobList"
#define QUERYINFO_OPTIONS                   "options"
#define QUERYINFO_USER_NAME                 "userName"
#define QUERYINFO_JOB_NAME                  "jobName"
#define QUERYINFO_QUEUE_NAME                "queueName"
#define QUERYINFO_HOST_NAME                 "hostName"
#define QUERYINFO_APP_NAME                  "appName"
#define QUERYINFO_JOB_GROUP_NAME            "jobGroupName"
#define QUERYINFO_PROJECT_NAME              "projectName"
#define QUERYINFO_LICENSE_PROJECT_NAME      "licenseProjectName"
#define QUERYINFO_SERVICE_CLASS_NAME        "serviceClassName"
#define QUERYINFO_JOB_DESCRIPTION           "jobDescription"
#define QUERYINFO_SOURCE_CLUSTER_NAME       "sourceClusterName"

struct queryInfo
{
    int                 jobListNum;         /* num of jobs in joblist */       
    LS_LONG_INT*        joblist;            
    int                 options;            
    char                *userName;          
    char                *jobName;          
    char                *queueName;         
    char                *hostName;           
    char                *appName;           
    char                *jobGroupName;
    char                *projectName;
    char                *licenseProjectName;
    char                *serviceClassName;
    char                *jobDescription;
    char                **sourceClusterName; 
};


struct jobInfoQuery {
    int   nCols;          /* # of columns of job information which will returned */
    long  *colIndexs;     /* job information column index */
    char  *query;         /* Query condition, should be a valid TCL expression */

    struct submit_ext *submitExt; /**< For new options in future  */
};




/**
 * \brief  user information entry. 
 */
struct userInfoEnt {
    char   *user;         /**< Name of the user or user group */
    float  procJobLimit;  /**< The maximum number of job slots the user
                           * or user group can use on each processor. The job
                           * slots can be used by started jobs or reserved for
                           * PEND jobs. */
    int    maxJobs;       /**< The maximum number of job slots that the user
                           * or user group can use simultaneously in the local
                           * LSF cluster. The job slots can be used by started
                           * jobs or reserved for PEND jobs. */
    int    numStartJobs;  /**< The current number of job slots used by running
                           * and suspended jobs belonging to the user or user
                           * group.*/
    int    numJobs;       /**< The total number of job slots in the LSF cluster
                           * for the jobs submitted by the user or user group.
                           */
    int    numPEND;       /**< The number of job slots the user or user group
                           * has for pending jobs. */
    int    numRUN;        /**< The number of job slots the user or user group
                           * has for running jobs.*/
    int    numSSUSP;      /**< The number of job slots for the jobs belonging
                           * to the user or user group that have been suspended
                           * by the system.*/
    int    numUSUSP;      /**< The number of job slots for the jobs belonging
                           * to the user or user group that have been suspended
                           * by the user or the LSF system administrator. */
    int    numRESERVE;    /**< The number of job slots reserved for the pending
                           * jobs belonging to the user or user group. */
    int    maxPendJobs;   /**< The maximum number of pending jobs allowed. */
};

/* UserEquivalent info */
struct userEquivalentInfoEnt {
    char *equivalentUsers;
};

/* UserMapping info */
struct userMappingInfoEnt {
    char *localUsers;            /* Users in the local cluster */
    char *remoteUsers;           /* Users in remote clusters */
    char *direction;             /* "export" or "import" */
};


/* APS structures used for mapping between factors */
/**
 * \brief  APS structures used for mapping between factors
 */
struct apsFactorMap {
    char   *factorName;         /**< Name of factor. */
    char   *subFactorNames;     /**< SubFactor names. */
};
/**
 * \brief  APS structures used for mapping between factors
 */
struct apsLongNameMap {
    char    *shortName;   /**< Short name */
    char    *longName;    /**< Long name */
};
	


/* some values should not
 * conflict with the option values for lsb_usergrpinfo() and lsb_hostinfo_ex()
 * since they share the same xdr_infoReq() 
 */
#define ALL_QUEUE       0x01      /* for compatibility for 2.0 */
#define DFT_QUEUE       0x02      /* for compatibility for 2.0 */
#define CHECK_HOST      0x80
#define CHECK_USER      0x100
#define SORT_HOST       0x200
#define QUEUE_SHORT_FORMAT  0x400 /* not bqueues -l or -r */
#define EXPAND_HOSTNAME 0x800     /* expand hostname into official hostname */
#define RETRIEVE_BATCH  0x1000    /* only retrieve batch partitions */
#define GLOBAL_FAIRSHARE_LOAD   0x2000    /* used with bqueues -l or -r */

/* Signal number in each version LSB_SIG_NUM must be equal to 
 * signal number in the latest version.
 */
#define LSB_SIG_NUM_40            25
#define LSB_SIG_NUM_41            26 
#define LSB_SIG_NUM_51            30 /* Solutions #38347 */
#define LSB_SIG_NUM_60            30 
#define LSB_SIG_NUM               30 

/* Dynamic CPU provision 
 * to indicate whether a SP can lend or borrow hosts
 */
#define DCP_LEND_HOSTS                0x0001
#define DCP_BORROW_HOSTS              0x0002

/* status to indicate the current situation of Dynamic CPU provision 
 * DCP_UNDER_ALLOC_AND_STARVING means a partition is under allocation 
 * of dynamic cpu and its pending jobs are starving for more cpus.
 */
#define DCP_ALLOC_CPU_OK                     0x0
#define DCP_UNDER_ALLOC_CPU                  0x0001
#define DCP_JOB_WAIT_FOR_CPU                 0x0002
#define DCP_ALLOC_CPU_BUSY                   0x0004

/*
 * Queue level fairshare factors
 */
struct fsFactors {
    float   runTimeFactor; /* Run time weighting factor for fairshare 
                            * scheduling */
    float   cpuTimeFactor; /* CPU time weighting factor for fairshare 
                            * scheduling */
    float   runJobFactor;  /* Job slots weighting factor for fairshare 
                            * scheduling */
    float   histHours;     /* HIST_HOURS: The number of hours of resource 
                            * consumption history used for fair share 
                            * scheduling */
    float   committedRunTimeFactor; /* committed runtime weighting factor 
                                     * for fairshare scheduling */
    float   fairAdjustFactor;       /* Fairshare adjustment weighting factor */
    int     enableHistRunTime;      /* Enable or disable the decay of 
                                     * historical run time in the calculation 
                                     * of fairshare scheduling priority */
    int     enableRunTimeDecay;     /* Enable or disable the decay of run time 
                                     * in the calculation of fairshare 
                                     * scheduling priority */
    float   rmtPendJobFactor;  /* Remote pend job number weighting facotr for
                                 fairshare scheduling */
};


/* Structure for lsb_queueinfo() call */
/* !!! IMPORTANT !!!
 * If you change queueInfoEnt, you have to change Intlib/ade.lsbatch.h too!
 */
/**
* queueInfoEnt  queue information entry.
*/
struct queueInfoEnt {
    char   *queue;       /**< The name of the queue. */
    char   *description; /**< Describes the typical use of the queue. */
    int    priority;	 /**< Defines the priority of the queue. This
		           * determines the order in which the job queues
		           * are searched at job dispatch time: queues with
		           * higher priority values are searched first. (This
		           * is contrary to UNIX process priority ordering.) */
    short  nice;         /**< Defines the nice value at which jobs in this
                          * queue will be run. */
    char   *userList;    /**< A blank-separated list of names of users
    			   * allowed to submit jobs to this queue. */
    char   *hostList;    /**< A blank-separated list of names of hosts to
    			   * which jobs in this queue may be dispatched. */
    char   *hostStr;     /**< Original HOSTS string in case "-" is used. */
    int    nIdx;         /**< The number of load indices in the loadSched
    			   * and loadStop arrays. */
    float  *loadSched;	 /**< The queue and host loadSched and loadStop
		           * arrays control batch job dispatch, suspension, and
		           * resumption.
		           *
		           * The values in the loadSched array specify
		           * thresholds for the corresponding load indices.
		           * Only if the current values of all specified load
		           * indices of a host are within (below or above,
		           * depending on the meaning of the load index) the
		           * corresponding thresholds of this queue, will jobs
		           * in this queue be dispatched to the host. The same
		           * conditions are used to resume jobs dispatched from
		           * this queue that have been suspended on the host. */
    float  *loadStop;	 /**< The values in the loadStop array specify the
		           * thresholds for job suspension. If any of the
		           * current load index values of a host goes beyond
		           * a queue's threshold, jobs from the queue will
		           * be suspended.
		           *
		           * For an explanation of the fields in the loadSched
		           * and loadStop arrays, see \ref lsb_hostinfo. */
    int    userJobLimit;  /**< Per-user limit on the number of jobs
                           * that can be dispatched from this queue and executed
                           * concurrently. */
    float  procJobLimit;  /**< Per-processor limit on the number of jobs
                           * that can be dispatched from this queue and executed
                           * concurrently. */
    char   *windows;      /**< A blank-separated list of time windows
                           * describing the run window of the queue. When a
                           * queue's run window is closed, no job from this
                           * queue will be dispatched. When the run window
                           * closes, any running jobs from this queue will
                           * be suspended until the run window reopens, when
                           * they will be resumed. The default is no restriction,
                           * or always open (i.e., 24 hours a day, seven days
                           * a week).
                           *
                           * A time window has the format begin_time-end_time.
                           * Time is specified in the format [day:]hour[:minute],
                           * where all fields are numbers in their respective
                           * legal ranges: 0(Sunday)-6 for day, 0-23 for hour,
                           * and 0-59 for minute. The default value for minute
                           * is 0 (on the hour); the default value for day is
                           * every day of the week. The begin_time and end_time
                           * of a window are separated by `-', with no white
                           * space (i.e., blank or TAB) in between. Both
                           * begin_time and end_time must be present for a time
                           * window.
                           *
                           * Note that this run window only applies to batch
                           * jobs;interactive jobs scheduled by the LSF Load
                           * Information Manager (LIM) are controlled by another
                           * set of run windows. */
    int    rLimits[LSF_RLIM_NLIMITS];  
          /**< The per-process UNIX hard resource
           * limits for all jobs submitted to this queue (see getrlimit()
           * and lsb.queues). The default values for the resource limits
           * are unlimited, indicated by -1. The constants used to index
           * the rLimits array and the corresponding resource limits are
           * listed below.
           * <br> LSF_RLIMIT_CPU (CPULIMIT)
           * <br> LSF_RLIMIT_FSIZE (FILELIMIT)
           * <br> LSF_RLIMIT_DATA (DATALIMIT)
           * <br> LSF_RLIMIT_STACK    (STACKLIMIT)
           * <br> LSF_RLIMIT_CORE     (CORELIMIT)
           * <br> LSF_RLIMIT_RSS      (MEMLIMIT)
           * <br> LSF_RLIMIT_RUN      (RUNLIMIT)
           * <br> LSF_RLIMIT_PROCESS     (PROCESSLIMIT)
           * <br> LSF_RLIMIT_SWAP     (SWAPLIMIT)
           * <br> LSF_RLIMIT_THREAD
           * <br> LSF_RLIMIT_NOFILE
           * <br> LSF_RLIMIT_OPENMAX
           * <br> LSF_RLIMIT_VMEM */
    char   *hostSpec;	
          /**< A host name or host model name. If the queue
           * CPULIMIT or RUNLIMIT gives a host specification, hostSpec
           * will be that specification. Otherwise, if defaultHostSpec
           * (see below) is not NULL, hostSpec will be defaultHostSpec.
           * Otherwise, if DEFAULT_HOST_SPEC is defined in the
           * lsb.params file, (see lsb.params), hostSpec will be this
           * value. Otherwise, hostSpec will be the name of the host with
           * the largest CPU factor in the cluster. */
    int    qAttrib;     /**< The attributes of the queue. */
    int    qStatus;     /**< The status of the queue. */
    int    maxJobs;     /**< The maximum number of jobs dispatched by the
    	                 * queue and not yet finished. */
    int    numJobs;     /**< Number of jobs in the queue, including pending,
    	                 * running, and suspended jobs. */
    int    numPEND;     /**< Number of pending jobs in the queue.*/
    int    numRUN;	/**< Number of running jobs in the queue.*/
    int    numSSUSP;	/**< Number of system suspended jobs in the queue.*/
    int    numUSUSP;	/**< Number of user suspended jobs in the queue.*/
    int    mig;         /**< The queue migration threshold in minutes. */
    int    schedDelay;  /**< The number of seconds that a new job waits,
                         * before being scheduled. A value of zero (0) means
                         * the job is scheduled without any delay. */
    int    acceptIntvl; /**< The number of seconds for a host to wait after
                         * dispatching a job to a host, before accepting
                         * a second job to dispatch to the same host. */
    char   *windowsD;   /**< A blank-separated list of time windows describing
                         * the dispatch window of the queue. When a queue's
                         * dispatch window is closed, no job from this queue
                         * will be dispatched.The default is no restriction,
                         * or always open (i.e., 24 hours a day, seven days
                         * a week).
                         *
                         * For the time window format, see windows (above).*/
    char   *nqsQueues;	 
          /**< A blank-separated list of queue specifiers.
           * Each queue specifier is of the form queue\@host where host
           * is an NQS host name and queue is the name of a queue on
           * that host. */
    char   *userShares;  
          /**< A blank-separated list of user shares. Each share
           * is of the form [user, share] where user is a user name,
           * a user group name, the reserved word default or the reserved
           * word others, and share is the number of shares the user gets. */
    char   *defaultHostSpec;  
          /**< The value of DEFAULT_HOST_SPEC in the Queue
           * section for this queue in the lsb.queues file. */
    int    procLimit;         
          /**< An LSF resource limit used to limit the
           * number of job slots (processors) a (parallel) job in the
           * queue will use. A job submitted to this queue must specify
           * a number of processors not greater than this limit. */
    char   *admins;		
          /**< A list of administrators of the queue. The users
           * whose names are here are allowed to operate on the jobs in
           * the queue and on the queue itself. */
    char   *preCmd;      
          /**< Queue's pre-exec command. The command is executed
           * before the real batch job is run on the execution host (or on
           * the first host selected for a parallel batch job).  */
    char   *postCmd;     
          /**< Queue's post-exec command. The command is run
    	   * when a job terminates. */
    char   *requeueEValues;   
          /**< Jobs that exit with these values are
           * automatically requeued. */
    int    hostJobLimit;      
          /**< The maximum number of job slots a host can
           * process from this queue, including job slots of dispatched
           * jobs which have not finished yet and reserved slots for some
           * PEND jobs. This limit controls the number of jobs sent to
           * each host, regardless of a uniprocessor host or multiprocessor
           * host. Default value for this limit is infinity. */
    char   *resReq;     
         /**< Resource requirement string used to determine
    	  * eligible hosts for a job. */
    int    numRESERVE;    /**< Number of reserved job slots for pending jobs. */
    int    slotHoldTime;  /**< The time used to hold the reserved job slots
    		           * for a PEND job in this queue. */
    char   *sndJobsTo;	  /**< Remote MultiCluster send-jobs queues to
    		           * forward jobs to. */
    char   *rcvJobsFrom;  /**< Remote MultiCluster receive-jobs queues that
    		           * can forward to this queue. */
    char   *resumeCond;   /**< Resume threshold conditions for a suspended
    		           * job in this queue. */
    char   *stopCond;     /**< Stop threshold conditions for a running job
    		           * in this queue. */
    char   *jobStarter;     /**< Job starter command for a running job in
    		             * this queue */
    char   *suspendActCmd;  /**< Command configured for the SUSPEND action. */
    char   *resumeActCmd;      /**< Command configured for the RESUME action. */
    char   *terminateActCmd;   /**< Command configured for the TERMINATE action. */
    int    sigMap[LSB_SIG_NUM];  /**< Configurable signal mapping */
    char   *preemption;       /**< Preemptive scheduling and preemption
                               * policy specified for the queue. */
    int    maxRschedTime;      
           /**< Time period for a remote cluster to
            * schedule a job.
            *
            * MultiCluster job forwarding model only. Determines how long
            * a MultiCluster job stays pending in the execution cluster
            * before returning to the submission cluster. The remote
            * timeout limit in seconds is:
            * \li MAX_RSCHED_TIME * MBD_SLEEP_TIME=timeout */


    int    numOfSAccts;           /**< Number of share accounts in the queue. */
    struct shareAcctInfoEnt*  shareAccts;  
          /**< (Only used for queues with
           * fairshare policy) a share account vector capturing the
           * fairshare information of the users using the queue.
           *
           * The storage for the array of queueInfoEnt structures will be
           * reused by the next call. */
    char   *chkpntDir;            /**< The directory where the checkpoint
                                   * files are created. */
    int    chkpntPeriod;          /**< The checkpoint period in minutes.  */
    int    imptJobBklg;           
          /**< MultiCluster job forwarding model only.
           * Specifies the MultiCluster pending job limit for a
           * receive-jobs queue. This represents the maximum number of
           * MultiCluster import jobs that can be pending in the queue;
           * once the limit has been reached, the queue stops accepting
           * jobs from remote clusters. */
    int    defLimits[LSF_RLIM_NLIMITS];  
          /**< The default (soft) resource
           * limits for all jobs submitted to this queue (see getrlimit()
           * and lsb.queues). */
    int    chunkJobSize;	  
          /**< The maximum number of jobs allowed to
           * be dispatched together in one job chunk. Must be a positive
           * integer greater than 1. */
    int    minProcLimit;          
          /**< The minimum number of job slots
           * (processors) that a job in the queue will use. */
    int    defProcLimit;          
          /**< The default (soft) limit on the number of job slots (processors)
           * that a job in the queue will use. */
    char   *fairshareQueues;  /**< The list of queues for cross-queue fairshare. */
    char   *defExtSched;      /**< Default external scheduling for the queue. */
    char   *mandExtSched;     /**< Mandatory external scheduling options
    		               * for the queue. */
    int    slotShare;         
          /**< Share of job slots for queue-based fairshare.
           * Represents the percentage of running jobs (job slots) in use
           * from the queue. SLOT_SHARE must be greater than zero (0) and
           * less than or equal to 100.
           *
           * The sum of SLOT_SHARE for all queues in the pool does not need
           * to be 100%. It can be more or less, depending on your needs. */
    char   *slotPool;         
          /**< Name of the pool of job slots the queue
           * belongs to for queue-based fairshare. A queue can only belong
           * to one pool. All queues in the pool must share the same set
           * of hosts.
           *
           * Specify any ASCII string up to 60 chars long. You can use
           * letters, digits, underscores (_) or dashes (-). You cannot use
           * blank spaces. */
    int    underRCond;	
          /**< Specifies a threshold for job underrun exception
           * handling. If a job exits before the specified number of
           * minutes, LSF invokes LSF_SERVERDIR/eadmin to trigger the
           * action for a job underrun exception. */
    int    overRCond;	
          /**< Specifies a threshold for job overrun exception
           * handling. If a job runs longer than the specified run time,
           * LSF invokes LSF_SERVERDIR/eadmin to trigger the action for
           * a job overrun exception. */
    float  idleCond;	
          /**< Specifies a threshold for idle job exception
           * handling. The value should be a number between 0.0 and 1.0
           * representing CPU time/runtime. If the job idle factor is less
           * than the specified threshold, LSF invokes
           * LSF_SERVERDIR/eadmin to trigger the action for a job idle
           * exception. */
    int    underRJobs;	/**< The number of underrun jobs in the queue. */
    int    overRJobs;	/**< The number of overrun jobs in the queue. */
    int    idleJobs;	/**< The number of idle jobs in the queue. */
    int    warningTimePeriod;  
          /**< Specifies the amount of time before a job
           * control action occurs that a job warning action is to be
           * taken. For example, 2 minutes before the job reaches run
           * time limit or termination deadline, or the queue's run window
           * is closed, an URG signal is sent to the job.
           *
           * Job action warning time is not normalized.
           *
           * A job action warning time must be specified with a job
           * warning action in order for job warning to take effect. */
    char   *warningAction;      
          /**< Specifies the job action to be taken
           * before a job control action occurs. For example, 2 minutes
           * before the job reaches run time limit or termination
           * deadline, or the queue's run window is closed, an URG signal
           * is sent to the job.
           *
           * A job warning action must be specified with a job action
           * warning time in order for job warning to take effect.
           *
           * If specified, LSF sends the warning action to the job before
           * the actual control action is taken. This allows the job time
           * to save its result before being terminated by the job
           * control action.
           *
           * You can specify actions similar to the JOB_CONTROLS queue
           * level parameter: send a signal, invoke a command, or
           * checkpoint the job. */
    char   *qCtrlMsg;           /**< AdminAction - queue control message*/
    char   *acResReq;    /**< Acept resource request. */
    int	   symJobLimit;	 /**< Limit of running session scheduler jobs. */
    char   *cpuReq;      /**< cpu_req for service partition of session
                          * scheduler */
    int    proAttr;      /**< Indicate whether it would be willing to
                          * donate/borrow. */
    int    lendLimit;    /**< The maximum number of hosts to lend. */
    int    hostReallocInterval;  /**< The grace period to lend/return idle hosts. */
    int    numCPURequired;   /**< Number of CPUs required by CPU provision. */
    int    numCPUAllocated;  /**< Number of CPUs actually allocated. */
    int    numCPUBorrowed;   /**< Number of CPUs borrowed. */
    int    numCPULent;       /**< Number of CPUs lent.*/
    /* the number of reserved cpu(numCPUReserved) = numCPUAllocated - numCPUBorrowed + numCPULent */


    /* the following fields are for real-time app(ex. murex) of symphony */
    int    schGranularity;        /**< Scheduling granularity. in milliseconds. */
    int    symTaskGracePeriod;    /**< The grace period for stopping session
                                   * scheduler tasks. */
    int    minOfSsm;              /**< Minimum number of SSMs. */
    int    maxOfSsm;              /**< Maximum number of SSMs. */
    int    numOfAllocSlots;       /**< Number of allocated slots. */
    char *servicePreemption;      /**< Service preemptin policy. */

    int    provisionStatus;       /**< Dynamic cpu provision status. */
    int    minTimeSlice;          /**< The minimum time for preemption and
                                   * backfill, in seconds. */
    char   *queueGroup;           /**< List of queues defined in a queue group
                                   * for absolute priority scheduling (APS)
                                   * across multiple queues. */
    int    numApsFactors;	  /**< The number of calculation factors for
                                   * absolute priority scheduling (APS). */
    struct apsFactorInfo *apsFactorInfoList;	
          /**< List of calculation factors
           * for absolute priority scheduling (APS) */
    struct apsFactorMap  *apsFactorMaps;  
          /**< The mapping of factors to
           * subfactors for absolute priority scheduling (APS). */
    struct apsLongNameMap *apsLongNames;  
          /**< The mapping of factors to their
           * long names for absolute priority scheduling (APS). */
    int    maxJobPreempt;        /**< Maximum number of job preempted times. */
    int    maxPreExecRetry;      /**< Maximum number of pre-exec retry times. */
    int    localMaxPreExecRetry;  /**< Maximum number of pre-exec retry times for local cluster */
    int    maxJobRequeue;        /**< Maximum number of job re-queue times. */
    int    usePam;               /**< Use Linux-PAM */
    /* compute unit exclusive */
    int    cu_type_exclusive; /**< Compute unit type */
    char  *cu_str_exclusive;  /**< A string specified in EXCLUSIVE=CU[\<string>] */
    char  *resRsvLimit;       /**< Resource reservation limit */
    
    struct fsFactors fairFactors; /**< Fairshare factors */
    int    maxSlotsInPool;      /**< Used together with automatic scanning through the list
                                 *   of hosts to calculate total usable slots for one slot pool. The
                                 *   minimum value of both will take effect. And eventually, the
                                 *   value will be used for slot distribution calculation */
    int    usePriorityInPool;   /**< When enabled, LSF dispatch jobs strictly force defined rate
                                 *   for each queue in one slot pool first, then dispatch more jobs
                                 *   with FCFS policy if there are slots left in the slot pool */
    int    noPreemptInterval; /**< Uninterrupted running time (minutes) before job can be preempted. */     
    int    maxTotalTimePreempt; /**< Maximum accumulated preemption time (minutes). */

    int   qAttrib2;           /**< 2nd queue attributes field */
#if defined(LSF_8_0_MOCK_PARAM)
    int    mockParamA;
    char   *mockParamB;
    float mockParamC;
#endif /* LSF_8_0_MOCK_PARAM */
    int preemptDelayTime; /**< The grace period before preemption*/

    int    acp;                        /* absolute cluster preference */
    int    maxWaitTime;         /* time period for re-dispatch job to same remote cluster */

    int    imptSlotBklg;
          /**< This variable has similar meaning to imptJobBklg only that the 
            * limit is posed on the number of pending slots other than
            * the number of pending jobs.
           */
    int    maxSlotsPreempted;   /**< Maximum number of slots that can be preempted. */
    char   *successEValues;     /**< Jobs with these exit values are treated as done successfully. */
    int    nDiscreteProcLimits; /**< Store the count of discrete proclimit vlaues specified in queue */
    int    *discreteProcLimits; /**< Store discrete proclimit values specified in queue */
    int     maxProtocolInstance;   /**< maximum number of window instances allowed for job */
    char    *networkReq;        /**< POE job network requirement string */
    char   *globalFairshare;    /**< If the queue participates into a global fairshare
                                 *   policy, this variable is the name of the
                                 *   global fairshare policy. Otherwise this
                                 *   variable is NULL.
                                 */
};

/**
 * \addtogroup signal_action signal_action
 * define status for signal action
 */
/*@{*/
#define ACT_NO              0       /**<  No action*/
#define ACT_START           1	    /**<  Start*/
#define ACT_PREEMPT         2	    /**<  Preempt*/
#define ACT_DONE            3 	    /**<  Done*/
#define ACT_FAIL            4 	    /**<  Fail*/
/*@}*/

/**
 * \brief  host information entry. 
 */
struct hostInfoEnt {
    char   *host;            /**< The name of the host.*/
    int    hStatus;          /**< The status of the host. It is the bitwise 
                              * inclusive OR.  see \ref host_status */
    int    *busySched;       /**< Indicate host loadSched busy reason */
    int    *busyStop;        /**< Indicate host loadStop  busy reason.*/
    float  cpuFactor;        /**< The host CPU factor used to scale CPU load
                              * values to account for differences in CPU speeds.
                              * The faster the CPU, the larger the CPU factor.*/
    int    nIdx;             /**< The number of load indices in the load,
                              * loadSched and loadStop arrays. */
    float  *load;            /**< Load information array on a host. This array
                              * gives the load information that is used for 
                              * scheduling batch jobs. This load information 
                              * is the effective load information from \ref ls_loadofhosts
                              * (see \ref ls_loadofhosts) plus the load reserved for
                              * running jobs (see lsb.queues for details on
                              * resource reservation). The load array
                              * is indexed the same as loadSched and loadStop 
                              * (see loadSched and loadStop below). */
    float  *loadSched;       /**< Stop scheduling new jobs if over */
    float  *loadStop;        /**< Stop jobs if over this load.
                              * The loadSched and loadStop arrays control batch
                              * job scheduling, suspension, and resumption.
                              * The values in the loadSched array specify the
                              * scheduling thresholds for the corresponding load
                              * indices. Only if the current values of all
                              * specified load indices of this host are within
                              * (below or above, depending on the meaning of the
                              * load index) the corresponding thresholds of this
                              * host, will jobs be scheduled to run on this host.
                              * Similarly, the values in the loadStop array
                              * specify the stop thresholds for the corresponding
                              * load indices. If any of the load index values of
                              * the host goes beyond its stop threshold, the job
                              * will be suspended. The loadSched and loadStop
                              * arrays are indexed by the following constants:
                              *\n R15S
                              *\n 15-second average CPU run queue length.
                              *\n R1M
                              *\n 1-minute average CPU run queue length.
                              *\n R15M
                              *\n 15-minute average CPU run queue length.
                              *\n UT
                              *\n CPU utilization over the last minute.
                              *\n PG
                              *\n Average memory paging rate, in pages per second.
                              *\n IO
                              *\n Average disk I/O rate, in KB per second.
                              *\n LS
                              *\n Number of current login users.
                              *\n IT
                              *\n Idle time of the host in minutes.
                              *\n TMP
                              *\n The amount of free disk space in the file
                              * system containing /tmp, in MB.
                              *\n SWP
                              *\n The amount of swap space available, in MB.
                              *\n MEM
                              *\n The amount of available user memory on this
                              * host, in MB. */
    char   *windows;         /**< ASCII desp of run windows.One or more time
                              * windows in a week during which batch jobs may
                              * be dispatched to run on this host . The default
                              * is no restriction, or always open (i.e., 24 hours
                              * a day seven days a week). These windows are
                              * similar to the dispatch windows of batch job
                              * queues. See \ref lsb_queueinfo. */
    int    userJobLimit;     /**< The maximum number of job slots any user
                              * is allowed to use on this host. */
    int    maxJobs;          /**< The maximum number of job slots that the
                              * host can process concurrently. */
    int    numJobs;          /**< The number of job slots running or suspended
                              * on the host. */
    int    numRUN;           /**< The number of job slots running on the host. */
    int    numSSUSP;         /**< The number of job slots suspended by the batch
                              * daemon on the host. */
    int    numUSUSP;         /**< The number of job slots suspended by the job
                              * submitter or the LSF system administrator.*/
    int    mig;              /**< The migration threshold in minutes after which
                              * a suspended job will be considered for migration.*/

    int    attr;             /**< The host attributes; the bitwise inclusive
                              * OR of some of \ref host_attributes */
/**
 * \addtogroup host_attributes host_attributes
 * The host attributes
 */
/*@{*/                           
#define H_ATTR_CHKPNTABLE  0x1 /**< This host can checkpoint jobs */
#define H_ATTR_CHKPNT_COPY 0x2 /**< This host provides kernel support for
                                * checkpoint copy.*/ 
#define H_ATTR_VNODE 0x4 /**< This host is a virtual node.*/
#define H_ATTR_DC_REPROVISIONABLE 0x08 /**< This DC host is re-provisionable */
/*@}*/
    float *realLoad;         /**< The effective load of the host. */
    int   numRESERVE;        /**< The number of job slots reserved by LSF for
                              * the PEND jobs. */
    int   chkSig;            /**< If attr has an H_ATTR_CHKPNT_COPY attribute,
       	* chkSig is set to the signal which triggers 
       	* checkpoint and copy operation. Otherwise, 
       	* chkSig is set to the signal which triggers 
       	* checkpoint operation on the host */

    float   cnsmrUsage;    /**< Num of resource used by the consumer */
    float   prvdrUsage;    /**< Num of resource used by the provider */
    float   cnsmrAvail;    /**< Num of resource available for the consumer
                            * to use */
    float   prvdrAvail;    /**< Num of resource available for the provider
                            * to use */
    float   maxAvail;      /**< Num maximum of resource available in total */
    float   maxExitRate;   /**< The job exit rate threshold on the host */ 
    float   numExitRate;   /**< Number of job exit rate on the host */
    char    *hCtrlMsg;     /**< AdminAction - host control message */
    int     numNetwork;    /**< The number of PE networks on this host */
    struct networkInfoEnt   *networkInfoArray;  /**< Detailed PE network information */
    char    *affinityCpulist; /**< The CPU list which can be used in
                                * affinity scheduling */
    lsf_topology_t hostTp; /**< host topology tree info */
};

/**
 * \brief  Host information condition entry.
 */
struct condHostInfoEnt {

    char  *name; 		/**< Host name*/

    int   howManyOk;		/**< How many hosts are in the ok status*/
    int   howManyBusy;		/**< How many hosts are in the busy status*/
    int   howManyClosed;	/**< How many hosts are in the closed status*/
    int   howManyFull;		/**< How many hosts are in the full status*/
    int   howManyUnreach;	/**< How many hosts are in the unreach status*/
    int   howManyUnavail;	/**< How many hosts are in the unavail status*/

    struct hostInfoEnt *hostInfo; /**< The status of each host in the host group*/

};

typedef struct adjustParam {
    char* key;                                /* key name of share adjustment */
    float value;                              /* value of the key */
} adjustParam;

#define FAIR_ADJUST_CPU_TIME_FACTOR              0    /* cpu time factor */
#define FAIR_ADJUST_RUN_TIME_FACTOR              1    /* run time factor */
#define FAIR_ADJUST_RUN_JOB_FACTOR               2    /* run job factor */
#define FAIR_ADJUST_COMMITTED_RUN_TIME_FACTOR    3    /* committed run time factor */
#define FAIR_ADJUST_ENABLE_HIST_RUN_TIME         4    /* enable hist run time */
#define FAIR_ADJUST_HIST_CPU_TIME                5    /* cpu time of finished jobs with decay */
#define FAIR_ADJUST_NEW_USED_CPU_TIME            6    /* cpu time of finished jobs within decay */
#define FAIR_ADJUST_RUN_TIME                     7    /* total time that job spend in RUN state */
#define FAIR_ADJUST_HIST_RUN_TIME                8    /* historical run time of finished jobs */
#define FAIR_ADJUST_COMMITTED_RUN_TIME           9    /* committed run time of started jobs */
#define FAIR_ADJUST_NUM_START_JOBS               10   /* number of job slots used by started jobs */
#define FAIR_ADJUST_NUM_RESERVE_JOBS             11   /* number of reserved slots used by pending jobs */
#define FAIR_ADJUST_MEM_USED                     12   /* total amount of memory used by started jobs */
#define FAIR_ADJUST_MEM_ALLOCATED                13   /* average memory allocated per slot */
#define FAIR_ADJUST_ENABLE_RUN_TIME_DECAY        14   /* enable run time decay */
#define FAIR_ADJUST_DECAYED_RUN_TIME             15   /* decayed run time */
#define FAIR_ADJUST_KVPS_SUM                     16   /* total number of fairshare adjustment key value pairs */

extern char *FairAdjustPairArrayName[FAIR_ADJUST_KVPS_SUM];

typedef struct shareAdjustPair {
#define SHAREACCTTYPEQUEUE      0x01           /* queue share account */
#define SHAREACCTTYPEHP         0x02           /* host partition share account */
#define SHAREACCTTYPESLA        0x04           /* SLA share account */
    int shareAcctType;                         /* type of share account*/
    char* holderName;                          /* name of the share holder that use the share */
    char* providerName;                        /* name of the provider policy name(name of queue, host partition or SLA) */
    int numPair;                               /* number of share adjustment key value pair */
    struct adjustParam* adjustParam;           /* share adjustment key value pair */
} shareAdjustPair;
     
#if !defined(WIN32)
extern float   fairshare_adjustment(const struct shareAdjustPair *);
#else
__declspec(dllexport) float   fairshare_adjustment(const struct shareAdjustPair *);
#endif

/* For lsb_hostpartinfo() call  */
/**
 * \brief   gets user information about host partitions.
 */
struct hostPartUserInfo {
    char     *user;     
         /**< The user name or user group name. 
          * See \ref lsb_userinfo 
          * and \ref lsb_usergrpinfo*/
    int      shares;                     
         /**< The number of shares assigned to the user or user group, as 
          *configured in the file lsb.hosts. (See lsb.hosts.)*/
    float    priority;                   
    	 /**< The priority of the user or user group to use the host
    	  * partition. Bigger values represent higher priorities. Jobs
    	  * belonging to the user or user group with the highest priority
    	  * are considered first for dispatch when resources in the host
    	  * partition are being contended for. In general, a user or user
    	  * group with more shares, fewer numStartJobs and less histCpuTime
    	  * has higher priority.
       	  * 
       	  *The storage for the array of hostPartInfoEnt structures will be
       	  * reused by the next call.*/
    int      numStartJobs;               
         /**< The number of job slots belonging to the user or user group
          * that are running or suspended in the host partition.*/
    float    histCpuTime;                
    	 /**< The normalized CPU time accumulated in the host partition
    	  * during the recent period by finished jobs belonging to the
    	  * user or user group. The period may be configured in the file
    	  * lsb.params (see lsb.params), with a default value of five (5)
          * hours.*/
    int      numReserveJobs;             
       	 /**< The number of job slots that are reserved for the PEND jobs
       	  * belonging to the user or user group in the host partition.*/
    int      runTime;  /**< The time unfinished jobs spend  in RUN state */
    float    shareAdjustment;           
         /**< The fairshare adjustment value from the fairshare plugin 
       	  * (libfairshareadjust*). The adjustment is enabled and weighted
       	  * by setting the value of FAIRSHARE_ADJUSTMENT_FACTOR in lsb.params.*/
};

/* For lsb_hostpartinfo() call  */
/**
 * \brief  gets information entry about host partitions.
 */
struct hostPartInfoEnt {
    char   hostPart[MAX_LSB_NAME_LEN]; /**< The name of the host partition */
    char   *hostList;                 
        /**< A blank-separated list of names of hosts and host groups
         * which are members of the host partition. The name of a host
         * group has a '/' appended. see \ref lsb_hostgrpinfo.*/
    int    numUsers;                   
        /**< The number of users in this host partition. i.e., the number
         * of hostPartUserInfo structures.*/
    struct hostPartUserInfo* users;    
       	/**< An array of hostPartUserInfo structures which hold information
       	 * on users in this host partition.*/
    char     *hostStr; /* save original valid host list */
};

/* Library rappresentation of the share account */

/* flags in shareAcctInfoEnt
 */
#define SHARE_ACCT_GLOBAL   1   /* It indicates the share account is
                                 * participated into a global fairshare
                                 * policy.
                                 */

/**
 * \brief Library rappresentation of the share account
 */
struct shareAcctInfoEnt {
    char *   shareAcctPath;              /**< The user name or user group name.
                                          * (See \ref lsb_userinfo and
                                          * \ref lsb_usergrpinfo.) */
    int      shares;                     /**< The number of shares assigned to 
                                          * the user or user group, as
                                          * configured in the file lsb.queues.
                                          */
    float    priority;                   
             /**< The priority of the user or user group in the fairshare
              * queue. Larger values represent higher priorities. Job belonging
              * to the user or user group with the highest priority are
              * considered first for dispatch in the fairshare queue. In
              * general, a user or user group with more shares, fewer
              * numStartJobs and less histCpuTime has higher
              * priority. */
    int      numStartJobs;               
            /**< The number of job slots (belonging to the user or user group)
             * that are running or suspended in the fairshare queue.  */
    float    histCpuTime;                
            /**< The normalized CPU time accumulated in the fairshare queue
             * by jobs belonging to the user or user group, over the time
             * period configured in the file lsb.params. The default time
             * period is 5 hours. */
    int      numReserveJobs;             
            /**< The number of job slots that are reserved for the PEND jobs
             * belonging to the user or user group in the host partition.*/
    int      runTime;     /**< The time unfinished jobs spend in the RUN state. */
    float    shareAdjustment;            
            /**< The fairshare adjustment value from the fairshare plugin 
             * (libfairshareadjust.SOEXT). The adjustment is enabled and weighted
             *  by setting the value of FAIRSHARE_ADJUSTMENT_FACTOR in
             * lsb.params. */
    int      numForwPendJobs;            /**< number of forwared pending jobs */

    float    remoteLoad;
             /**< Remote share load which indicates how much fairshare
              * resource that is used by the share account in other
              * clusters. It only take effect when SHARE_ACCT_GLOBAL is ON.*/

    int      flags;     /**< other flag bits for this share account */
};

/* boundaries and default value used by mbatchd for the maxJobId */
#define DEF_MAX_JOBID   999999
#define MAX_JOBID_LOW   999999
#define MAX_JOBID_HIGH (INFINIT_INT-1)

#define  DEF_PREEMPTION_WAIT_TIME    300    /* default preemption wait time */
#define  DEF_MAX_ASKED_HOSTS         512    /* default number of hosts specified by -m */

/* For lsb_parameterinfo() call */
/* values for BJOBS_RES_REQ_DISPLAY*/
#define DEF_DISPLAY_RESREQ_BRIEF   BJOBS_DISPLAY_RESREQ_BRIEF
#define BJOBS_DISPLAY_RESREQ_NONE   "none"   /* bjobs display nothing about resreq */
#define BJOBS_DISPLAY_RESREQ_NONE_QUOTE   "\"none\""
#define BJOBS_DISPLAY_RESREQ_BRIEF  "brief"
#define BJOBS_DISPLAY_RESREQ_BRIEF_QUOTE  "\"brief\""
                                              /* default is "brief", bjobs only
                                              * display combined, effective resreq
                                              */
#define BJOBS_DISPLAY_RESREQ_FULL   "full"
#define BJOBS_DISPLAY_RESREQ_FULL_QUOTE   "\"full\""
                                   /*bjobs display all levels resreq, see following:
                                    *job level: resreq specified when submit
                                    *app level: specified in the lsb.applications
                                    *queue level: specified in the lsb.queues
                                    *combined level: merge job/app/queue level
                                    *    resreq together by mbatchd
                                    *effective level: the one scheduler used to
                                    *    dispatched job.
                                    */
#define DEF_DISPLAY_RESREQ_BRIEF_INT   BJOBS_DISPLAY_RESREQ_BRIEF_INT
#define BJOBS_DISPLAY_RESREQ_BRIEF_INT 0
#define BJOBS_DISPLAY_RESREQ_NONE_INT 1
#define BJOBS_DISPLAY_RESREQ_FULL_INT 2
/**
 * \brief The parameterInfo structure contains the following fields:
 */
struct parameterInfo {
    char *defaultQueues;    /**< DEFAULT_QUEUE: A blank_separated list of
                             * queue names for automatic queue selection. */
    char *defaultHostSpec;  /**< DEFAULT_HOST_SPEC: The host name or host
                             * model name used as the system default for
                             * scaling CPULIMIT and RUNLIMIT. */
    int  mbatchdInterval;   /**< MBD_SLEEP_TIME: The interval in seconds at
                             * which the mbatchd dispatches jobs. */
    int  sbatchdInterval;   /**< SBD_SLEEP_TIME: The interval in seconds at
                             * which the sbatchd suspends or resumes jobs. */
    int  jobAcceptInterval; /**< JOB_ACCEPT_INTERVAL: The interval at which 
                             * a host accepts two successive jobs.
                             * (In units of SBD_SLEEP_TIME.)*/
    int  maxDispRetries;    /**< MAX_RETRY: The maximum number of retries
                             * for dispatching a job.*/
    int  maxSbdRetries;     /**< MAX_SBD_FAIL: The maximum number of retries
                             * for reaching an sbatchd. */
    int  preemptPeriod;     /**< PREEM_PERIOD: The interval in seconds for
                             * preempting jobs running on the same host.*/
    int  cleanPeriod;       /**< CLEAN_PERIOD: The interval in seconds during
                             * which finished jobs are kept in core. */
    int  maxNumJobs;        /**< MAX_JOB_NUM: The maximum number of finished
                             * jobs that are logged in the current event file. */
    float historyHours;     /**< HIST_HOURS: The number of hours of resource
                             * consumption history used for fair share scheduling
                             * and scheduling within a host partition.*/
    int  pgSuspendIt;       /**< PG_SUSP_IT: The interval a host must be idle
                             * before resuming a job suspended for excessive
                             * paging.*/
    char *defaultProject;   /**< The default project assigned to jobs. */
    int  retryIntvl; 	    /**< Job submission retry interval*/
    int  nqsQueuesFlags;    /**< For Cray NQS compatiblilty only. Used by LSF
                             * to get the NQS queue information*/
    int  nqsRequestsFlags;  /**< nqsRequestsFlags*/
    int  maxPreExecRetry;   /**< The maximum number of times to attempt the
                             * preexecution command of a job from a remote
                             * cluster ( MultiCluster only)*/
    int  localMaxPreExecRetry; /**< Maximum number of pre-exec retry times for local cluster*/
    int  eventWatchTime;       /**< Event watching Interval in seconds*/
    float runTimeFactor;       /**< Run time weighting factor for fairshare
                                * scheduling*/
    float waitTimeFactor;      /**< Used for calcultion of the fairshare
                                * scheduling formula*/
    float runJobFactor;        /**< Job slots weighting factor for fairshare
                                * scheduling*/
    int  eEventCheckIntvl;     /**< Default check interval*/
    int  rusageUpdateRate;     /**< sbatchd report every sbd_sleep_time*/
    int  rusageUpdatePercent;  /**< sbatchd updates jobs jRusage in mbatchd
                                * if more than 10% changes*/
    int  condCheckTime;        /**< Time period to check for reconfig*/
    int  maxSbdConnections;    /**< The maximum number of connections between
                                * master and slave batch daemons*/
    int  rschedInterval;       /**< The interval for rescheduling jobs*/
    int  maxSchedStay;         /**< Max time mbatchd stays in scheduling routine,
                                * after which take a breather*/
    int  freshPeriod;          /**< During which load remains fresh*/
    int  preemptFor;	       /**< The preemption behavior, GROUP_MAX, GROUP_JLP,
                                * USER_JLP, HOST_JLU,MINI_JOB, LEAST_RUN_TIME*/
    int  adminSuspend;         /**< Flags whether users can resume their jobs
                                * when suspended by the LSF administrator*/
    int  userReservation;      /**< Flags to enable/disable normal user to
                                * create advance reservation*/
    float cpuTimeFactor;       /**< CPU time weighting factor for fairshare
                                * scheduling*/
    int  fyStart;	       /**< The starting month for a fiscal year*/
    int     maxJobArraySize;   /**< The maximum number of jobs in a job array*/
    time_t  exceptReplayPeriod;/**< Replay period for exceptions, in seconds*/
    int jobTerminateInterval;  /**< The interval to terminate a job*/
    int disableUAcctMap;       /**<  User level account mapping for remote
                                * jobs is disabled*/
    int enforceFSProj;	       /**< If set to TRUE, Project name for a job will
                                * be considerred when doing fairshare scheduling,
                                * i.e., as if user has submitted jobs using -G*/
    int enforceProjCheck;      /**< Enforces the check to see if the invoker
                                * of bsub is in the specifed group when the -P
                      		* option is used*/
    int     jobRunTimes;	/**< Run time for a job*/
    int     dbDefaultIntval;	/**< Event table Job default interval*/
    int     dbHjobCountIntval;  /**< Event table Job Host Count*/
    int     dbQjobCountIntval;  /**< Event table Job Queue Count*/
    int     dbUjobCountIntval;  /**< Event table Job User Count*/
    int     dbJobResUsageIntval;/**< Event table Job Resource Interval*/
    int     dbLoadIntval;	/**< Event table Resource Load Interval*/
    int     dbJobInfoIntval;    /**< Event table Job Info*/
    int     jobDepLastSub;      /**< Used with job dependency scheduling*/
    int     maxJobNameDep;      /**< Used with job dependency scheduling, 
                                 * deprecated*/
    char   *dbSelectLoad;       /**< Select resources to be logged */
    int     jobSynJgrp;         /**< Job synchronizes its group status */
    char   *pjobSpoolDir;       /**< The batch jobs' temporary output directory*/

    int     maxUserPriority;     /**< Maximal job priority defined for all users*/
    int     jobPriorityValue;    /**< Job priority is increased by the system
                                  * dynamically based on waiting time*/
    int     jobPriorityTime;     /**< Waiting time to increase Job priority
                                  * by the system dynamically*/    
    int     enableAutoAdjust;    /**< Enable internal statistical adjustment */
    int     autoAdjustAtNumPend; /**< Start to autoadjust when the user has
       	                          *  this number of pending jobs */
    float   autoAdjustAtPercent; /**< If this number of jobs has been visited
       	                          * skip the user */
    int     sharedResourceUpdFactor; /**<  Static shared resource update interval
                                      * for the cluster actor*/
    int     scheRawLoad;     /**< Schedule job based on raw load info */
    char   *jobAttaDir;	     /**<  The batch jobs' external storage for
                              * attached data*/
    int     maxJobMsgNum;    /**< Maximum message number for each job*/
    int     maxJobAttaSize;  /**< Maximum attached data size to be transferred
                              * for each message*/
    int     mbdRefreshTime;  /**< The life time of a child MBD to serve queries
                              * in the MT way*/
    int     updJobRusageInterval; /**< The interval of the execution cluster
                                   * updating the job's resource usage*/
    char    *sysMapAcct;       /**< The account to which all windows workgroup
                                * users are to be mapped*/
    int     preExecDelay;      /**< Dispatch delay internal */
    int     updEventUpdateInterval;    /**< Update duplicate event interval  */
    int     resourceReservePerSlot;    /**< Resources are reserved for parallel
                                        * jobs on a per-slot basis*/
    int	    maxJobId;          /**< Maximum job id --- read from the lsb.params*/
    char    *preemptResourceList; /**< Define a list of preemptable resource 
       	                           * names  */
    int     preemptionWaitTime;   /**< The preemption wait time */
    int     maxAcctArchiveNum;    /**< Maximum number of rollover lsb.acct files
       	                           * kept by mbatchd.
       	                           */
    int     acctArchiveInDays;    /**< mbatchd Archive Interval */
    int     acctArchiveInSize;    /**< mbatchd Archive threshold */  
    float committedRunTimeFactor; /**< Committed run time weighting factor */
     int  enableHistRunTime;      /**< Enable the use of historical run time
                                   * in the calculation of fairshare scheduling
                      		   * priority, Disable the use of historical
                      		   * run time in the calculation of fairshare
                     		   * scheduling priority */
#ifdef PS_SXNQS
    int   nqsUpdateInterval;      /**< NQS resource usage update interval */
#endif
    int  mcbOlmReclaimTimeDelay;  /**< Open lease reclaim time*/
    int  chunkJobDuration;        /**< Enable chunk job dispatch for jobs with
                                   * CPU limit or run limits */
    int  sessionInterval;	  /**< The interval for scheduling jobs by
                                   * scheduler daemon */
    int  publishReasonJobNum;     /**< The number of jobs per user per queue
                                   * whose pending reason is published at the
                   		   * PEND_REASON_UPDATE_INTERVAL interval*/
    int  publishReasonInterval;   /**< The interval for publishing job pending
                                   * reason by scheduler daemon*/
    int  publishReason4AllJobInterval;
       	                          /**< Interval(in seconds) of pending reason 
       	                           * publish for all jobs */
    int  mcUpdPendingReasonInterval; /**< MC pending reason update interval
                                      * (0 means no updates) */
    int  mcUpdPendingReasonPkgSize;  /**< MC pending reason update package
                                      * size (0 means no limit)*/
    int noPreemptRunTime;       /**< No preemption if the run time is greater 
                                 * than the value defined in here 
                                 */
    int noPreemptFinishTime;    /**< No preemption if the finish time is less
                                 * than the value defined in here 
                                 */
    char *  acctArchiveAt;      /**< mbatchd Archive Time*/  
    
    int  absoluteRunLimit;      /**< Absolute run limit for job */
    int  lsbExitRateDuration;   /**< The job exit rate duration*/
    int  lsbTriggerDuration;    /**<  The duration to trigger eadmin */
    int  maxJobinfoQueryPeriod; /**< Maximum time for job information query
                                 * commands (for example,with bjobs) to wait*/
    int jobSubRetryInterval;    /**< Job submission retrial interval for client
                                 */   
    int pendingJobThreshold;    /**< System wide max pending jobs*/
    
    int maxConcurrentJobQuery;  /**< Max number of concurrent job query */
    int minSwitchPeriod;        /**< Min event switch time period */

    int condensePendingReasons; /**< Condense pending reasons enabled */
    int slotBasedParallelSched; /**< Schedule Parallel jobs based on slots
                                 * instead of CPUs */
    int disableUserJobMovement; /**< Disable user job movement operations,
       	                         * like btop/bbot.
       	                         */
    int detectIdleJobAfter;     /**< Detect and report idle jobs
       	                         * only after specified minutes.
       	                         */
    int useSymbolPriority;      /**< Use symbolic when specifing priority
                                 * of symphony jobs
                                 */ 
    int JobPriorityRound;       /**< Priority rounding for symphony jobs */
    char* priorityMapping;      /**< The mapping of the symbolic priority 
                                 * for symphony jobs
                                 */
    int maxInfoDirs;            /**< Maximum number of subdirectories
       	                         * under LSB_SHAREDIR/cluster/logdir/info
       	                         */
    int minMbdRefreshTime;      /**< The minimum period of a child MBD to
                                 * serve queries in the MT way */
    int enableStopAskingLicenses2LS;   /**< Stop asking license to LS not due
                                        * to lack license */
    int expiredTime;              /**< Expire time for finished job which will
                                   * not taken into account when calculating
                                   * queue fairshare priority */
    char* mbdQueryCPUs;           /**< MBD child query processes will only run
                                   * on the following CPUs*/ 
    char *defaultApp;             /**< The default application profile assigned
                                   * to jobs */
    int  enableStream;            /**< Enable or disable data streaming */
    char *streamFile;		  /**< File to which lsbatch data is streamed*/
    int  streamSize;              /**< File size in MB to which lsbatch data
                                   * is streamed*/
    int syncUpHostStatusWithLIM;  /**< Sync up host status with master LIM
                                   * is enabled */
    char   *defaultSLA;           /**< Project schedulign default SLA */
    int    slaTimer;              /**< EGO Enabled SLA scheduling timer period
                                   */
    int    mbdEgoTtl;             /**< EGO Enabled SLA scheduling time to live
                                   */
    int    mbdEgoConnTimeout;     /**< EGO Enabled SLA scheduling connection
                                   * timeout */
    int    mbdEgoReadTimeout;     /**< EGO Enabled SLA scheduling read timeout
                                   */
    int    mbdUseEgoMXJ;          /**< EGO Enabled SLA scheduling use MXJ flag */
    int    mbdEgoReclaimByQueue;  /**< EGO Enabled SLA scheduling reclaim
                                   * by queue */
    int    defaultSLAvelocity;    /**< EGO Enabled SLA scheduling default
                                   * velocity*/
    char  *exitRateTypes;	  /**< Type of host exit rate exception
                                   * handling types: EXIT_RATE_TYPE*/
    float  globalJobExitRate;     /**< Type of host exit rate exception
                                   * handling types: GLOBAL_EXIT_RATE */
    int    enableJobExitRatePerSlot; /**< Type of host exit rate exception
                                      * handling types ENABLE_EXIT_RATE_PER_SLOT*/
    int  enableMetric;            /**< Performance metrics monitor is enabled 
                                   * flag */
    int  schMetricsSample;        /**< Performance metrics monitor sample
                                   * period flag*/
    float maxApsValue;            /**< Used to bound: (1) factors, (2) weights,
                                   * and (3) APS values */
    int  newjobRefresh;           /**< Child mbatchd gets updated information
                                   * about new jobs from the parent mbatchd*/  
    int  preemptJobType;          /**< Job type to preempt, PREEMPT_JOBTYPE_BACKFILL,
                                   * PREEMPT_JOBTYPE_EXCLUSIVE */
    char *defaultJgrp;            /**< The default job group assigned to jobs */
    int    jobRunlimitRatio;      /**< Max ratio between run limit and runtime
                                   * estimation*/
    int    jobIncludePostproc;    /**< Enable the post-execution processing
                                   * of the job to be included as part of
                                   * the job flag*/
    int    jobPostprocTimeout;    /**< Timeout of post-execution processing*/
    int    sschedUpdateSummaryInterval; /**< The interval, in seconds, for
                                         * updating the session scheduler
                                         * status summary*/
    int    sschedUpdateSummaryByTask;  /**< The number of completed tasks for
                                        * updating the session scheduler status
                                        * summary*/
    int    sschedRequeueLimit;     /**< The maximum number of times a task can
                                    * be requeued via requeue exit values*/
    int    sschedRetryLimit;       /**< The maximum number of times a task can
                                    * be retried after a dispatch error */
    int    sschedMaxTasks;         /**< The maximum number of tasks that can be
                                    * submitted in one session*/
    int    sschedMaxRuntime;       /**< The maximum run time of a single task*/
    char * sschedAcctDir;          /**< The output directory for task accounting
                                    * files*/
    int    jgrpAutoDel;            /**< If TRUE enable the job group automatic
                                    * deletion functionality (default is FALSE).*/
    int    maxJobPreempt;          /**< Maximum number of job preempted
                                    * times */
    int    maxJobRequeue;          /**< Maximum number of job re-queue times */
    int    noPreemptRunTimePercent;    /**< No preempt run time percent */
    int    noPreemptFinishTimePercent; /**< No preempt finish time percent */

    int slotReserveQueueLimit;      /**< The reservation request being within
                                     * JL/U. */
    int maxJobPercentagePerSession; /**< Job accept limit percentage. */
    int useSuspSlots;		    /**< The low priority job will use the slots
                                     * freed by preempted jobs. */

    int  maxStreamFileNum;          /**< Maximum number of the backup stream.utc
                                     * files */
    int    privilegedUserForceBkill; /**< If enforced only admin can use
                                      * bkill -r option */
    int mcSchedulingEnhance; /**< It controls the remote queue selection flow. */
    int mcUpdateInterval;    /**< It controls update interval of the counters 
                              * and other original data in MC implementation */
    int intersectCandidateHosts;    /**< Jobs run on only on hosts belonging
                                     * to the intersection of the queue the
                                     * job was submitted to, advance reservation
                                     * hosts, and any hosts specified by
                                     * bsub -m at the time of submission.*/
    int  enforceOneUGLimit;         /**< Enforces the limitations of a single
                                     * specified user group. */
    int    logRuntimeESTExceeded;   /**< Enable or disable logging runtime
                                     * estimation exceeded event */
    char* computeUnitTypes;         /**< Compute unit types.*/
    float fairAdjustFactor;         /**< Fairshare adjustment weighting factor */
    int   simEnableCpuFactor;          /**< abs runtime and cputime for LSF simulator */
    int   extendJobException;       /**< switch for job exception enhancement */
    char *preExecExitValues;        /**< If the pre_exec script of a job in the cluster
                                     * exits with an exit code specified in preExecExitValues,
                                     * the job will be re-dispatched to a different host. */

    int  enableRunTimeDecay;        /**< Enable the decay of run time
                                     * in the calculation of fairshare scheduling
                                     * priority */
    int advResUserLimit;           /* The maximum number of advanced reservations 
                                    * for each user or user group 
                                    */
    int    noPreemptInterval;       /**< Uninterrupted running time (minutes) before job can be preempted. */    
    int    maxTotalTimePreempt;     /**< Maximum accumulated preemption time (minutes). */
    int strictUGCtrl;               /**< enable or disable strict user group control */
    char *defaultUserGroup;         /**< enable or disable job's default user group */
    int enforceUGTree;              /**< enable or disable enforce user group tree */
#if defined(LSF_8_0_MOCK_PARAM)
    int    mockParamA;
    char *mockParamB;
    float mockParamC;
#endif /* LSF_8_0_MOCK_PARAM */
    int preemptDelayTime;/**< The grace period before preemption*/
    int    jobPreprocTimeout;       /**< Timeout of pre-execution processing*/
    char* allowEventType;           /**< Log specified events into stream file */
    int   runtimeLogInterval;       /**< Interval between runtime status log */
    char* groupPendJobsBy;          /**< Group pending jobs by these key fields */
    char* pendingReasonsExclude;    /**< Don't log the defined pending reason */
    char* pendingTimeRanking;       /**< Group pending jobs by pending time */
    int   includeDetailReasons;     /**< Disable to stop log detailed pending reasons */
    char* pendingReasonDir;       /**< Full path to the pending reasons cache file directory */
    int  forceKillRunLimit;      /**< Force kill the job exceeding RUNLIMIT */

    char *forwSlotFactor;         /**< GridBroker slot factor */ 
    int  gbResUpdateInterval;         /**< Resource update interval to GridBroker  */ 
    int  depJobsEval;   /**< Number of jobs threshold of mbatchd to evaluate jobs with dependency */
    float  rmtPendJobFactor;         /**< LSF/XL remote pending factor */
    int    numSchedMatchThreads;     /**< number scheduler matching threads */
    char *    bjobsResReqDisplay;       /**< control how many levels resreq bjobs can display*/
    int jobSwitch2Event;          /**< Enable/Disable "JOB_SWITCH2" event log */
    char * enableDiagnose;	       /**< Enable diagnose class types: query */ 
    char * diagnoseLogdir;     /**< The log directory for query diagnosis */
    char * mcResourceMatchingCriteria; /**< The MC scheduling resource criterion */
    int lsbEgroupUpdateInterval; /** <Interval to dynamically update egroup managed usergroups */
    int isPerJobSortEnableFlg; /** <TURE if SCHED_PER_JOB_SORT=Y/y */
    char * defaultJobCwd;      /**< default job cwd*/
    int    jobCwdTtl;          /**< job cwd TTL*/
    int    ac_def_job_memsize;      /**< Default memory requirement for a VM job (MB) */
    int    ac_job_memsize_round_up_unit; /**< The round-up unit of the memory size for a VM job (MB) */
    int    ac_job_dispatch_retry_num;  /**< The number of times that a Dynamic Cluster job can be retried after a dispatch failure */
    int    ac_jobvm_restore_delay_time;  /**< The job vm restore delay time */
    int    ac_permit_alternative_resreq; /***<  Dynamic Cluster alternative resource requirement (logical OR between hardware-specific and provisionable resreq) is permissive */
    char * defaultJobOutdir;      /**< default job outdir*/
    int    bswitchModifyRusage; /***<  bswitch modify job resource usage */
    int resizableJobs;  /**< Enable or disable the resizable job feature */
    int slotBasedSla;  /**< Enable or disable slots based request for EGO-enabled SLA */
    int releaseMemForSuspJobs; /* Do not reserve memory when a job is suspended */
    int    stripWithMinimumNetwork; /**< strip with minimum network */
    int    maxProtocolInstance;    /**< maximum allowed window instances for pe job */
    int jobDistributeOnHost;         /**< how to distribute tasks for different jobs on same host */
    char * defaultResReqOrder;  /**< batch part default order */
    int    ac_timeout_waiting_sbd_start;  /* Dynamic Cluster timeout waiting for sbatchd to start */
    int maxConcurrentQuery;  /**< Max number of concurrent query */
}; /* struct parameterInfo */
       	      

/* for admin right */
#define ADMIN_USERSHARES 0x01
#define ADMIN_FULL 0x02

/* Bits for preemptFor parameter */
#define GROUP_MAX   0x0001
#define GROUP_JLP   0x0002
#define USER_JLP    0x0004
#define HOST_JLU    0x0008
#define MINI_JOB    0x0010                /* minimum of job */
#define LEAST_RUN_TIME    0x0020          /* least run time */
#define OPTIMAL_MINI_JOB  0x0040          /* optimal mini job */

/* Bits for mcSchedulingEnhance parameter */
#define RESOURCE_ONLY 0x0001 
#define COUNT_PREEMPTABLE 0x0002 
#define HIGH_QUEUE_PRIORITY 0x0004 
#define PREEMPTABLE_QUEUE_PRIORITY 0x0008 
#define PENDING_WHEN_NOSLOTS 0x0010
#define DYN_CLUSTER_WEIGHTING 0x0020

/* For resizableJobs */
#define RESIZABLE      0x01
#define AUTO_RESIZABLE 0x02    /* Not support so far */

/* options for bcaladd, bcalmod, bcaldel */
#define CAL_FORCE   0x0001

/* Bits for preemptJobType parameter, 
 * used to enable backfill and exclusive 
 * preemption */
#define PREEMPT_JOBTYPE_EXCLUSIVE   0x0001
#define PREEMPT_JOBTYPE_BACKFILL    0x0002
#define PREEMPT_JOBTYPE_AFFINITY    0x0004

/* constant values for Dynamic Cluster memory size
 */
#define LSF_AC_MIN_JOB_MEMSIZE  512
#define LSF_AC_DEF_JOB_MEMSIZE  1024
#define LSF_AC_JOB_MEMSIZE_ROUND_UP_UNIT  512 
#define LSF_AC_DEF_DISPATCH_NUM_RETRY 5
#define LSF_DEF_AC_JOBVM_RESTORE_DELAY_TIME  0
#define AC_DEF_TIMEOUT_WAITING_SBD_RESTART 1800 

/* For lsb_calendarinfo() call */
/**
 * \brief  calendar Information Entry.
 */
struct calendarInfoEnt {
    char   *name;		/**< A pointer to the name of the calendar.*/
    char   *desc;		/**< A description string associated with
                                 * the calendar.*/
    char   *calExpr;		/**< Calendar Expression*/
    char   *userName;		/**< User name */
    int    status;	        /**< Calendar status */
    int    options;		/**< For future use */
    int    lastDay; 	      	/**< Last time event of the calendar */
    int    nextDay; 	      	/**< Next time event of the calendar */
    time_t creatTime;		/**< Create Time*/
    time_t lastModifyTime;	/**< Last Modify Time*/
    int    flags;         	/**< Type of calendar, etc. */
};

#define ALL_CALENDARS   0x1
    
#define EVE_HIST        0x1
#define EVENT_ACTIVE           1
#define EVENT_INACTIVE         2
#define EVENT_REJECT	       3

#define EVENT_TYPE_UNKNOWN     0 
#define EVENT_TYPE_LATCHED     1 
#define EVENT_TYPE_PULSEALL    2
#define EVENT_TYPE_PULSE       3
#define EVENT_TYPE_EXCLUSIVE   4

/* define event types */
#define EV_UNDEF		0
#define EV_FILE       1
#define EV_EXCEPT		2
#define EV_USER       3

struct loadInfoEnt {
    char   *hostName;
    int    status;
    float  *load;
};

struct queuePairEnt {
    char *local;
    char *remote;
    int  send;		
    int  status;   
};

struct rmbCluAppEnt {
    char *name;
    char *description;
    int               numkvs;          /**< number of additional parameters within the same version */
    struct keyVal     *kvs;            /**< key value pair for backward
                                       * compatibility within the same version
                                       */
};

/* define 'cluster status' in lease model 
 * for bclusters command 
 */

#define LEASE_CLU_STAT_DISC      1  /* disconnection */
#define LEASE_CLU_STAT_CONN      2  /* policy is exchanged but no lease is signed */
#define LEASE_CLU_STAT_OK        3  /* there are leases signed between two clusters */
#define LEASE_CLU_STAT_NUMBER    3  
/* consumer cluster status in lease model */
struct consumerCluEnt {
    char  *cluName;     /* consumer cluster name */
    int   status;      /* cluster status, Ref- 'cluster status' definitions */
};
/* provider cluster status in lease model */
struct providerCluEnt {
    char  *cluName;     /* provider cluster name */
    int   status;      /* cluster status, Ref- 'cluster status' definitions */
};
/* for remote batch model, its definition is same as  clusterInfoEnt*/
struct rmbCluInfoEnt {
    char  *cluster;
    int   numPairs;
    struct queuePairEnt *queues;
    int   numApps;
    struct rmbCluAppEnt *apps;
};

/* for leasing model */
struct leaseCluInfoEnt {
    int flags;           /* 1, import from all if "allremote" defined in lease queue*/
    int numConsumer;     /* the array size of consumer cluster array */
    struct consumerCluEnt *consumerClus; /* the consumer cluster array */
    int numProvider;     /* the array size of provider cluster array */
    struct providerCluEnt *providerClus; /* the provider cluster array */
};

/* This is the old data structure, we
 * leave it here to keep backward compatibility.
 * It's definition is same as structure rmbCluInfoEnt.
 * It is to transfer cluster status between mbatchd with
 * old(4.x) bclusters command and old API-lsb_clusterinfo()
 */
struct clusterInfoEnt {
    char   *cluster;
    int    numPairs;
    struct queuePairEnt *queues;
    int   numApps;
    struct rmbCluAppEnt *apps;
};
/* the new data structure to transfer cluster status between mbatchd with
 * new(5.0) bclusters command and new API-lsb_clusterinfoEx() 
 */
struct clusterInfoEntEx {
    struct rmbCluInfoEnt *rmbCluInfo;   /* cluster status related to remote batch*/
    struct leaseCluInfoEnt leaseCluInfo;/* cluster status related to resource lease*/
};

struct eventInfoEnt {
    char     *name;          /* name of event */
    int      status;         /* one of ACTIVE or INACTIVE */
    int      type;           /* one of LATCHED, PULSE and EXCLUSIVE */
    int      eType;          /* one of FILE, ALARM, USER  */
    char     *userName;      /* user who created the event */
    char     *attributes;    /* event's attributes sent back from eeventd */
    int      numDependents;  /* number of expression dependent on the event */
    time_t   updateTime;     /* last time when eeventd sent back message */
    LS_LONG_INT lastDisJob;  /* last dispatched job dependent on the event */
    time_t   lastDisTime;    /* the time when the last job was dispatched */
};
#define ALL_EVENTS        0x01

/*
* define options for Dynamic Cluster host
*/
#define AC_HOST_VM       0x1    /*Dynamic Cluster VM host*/
#define AC_HOST_HV       0x2    /*Dynamic Cluster hypervisor host*/
#define AC_HOST_PM       0x4    /*Dynamic Cluster Physical host*/
#define AC_VM_SHOW_ALL   0x8    /*show all VMs even the status is off or unavailable*/
#define AC_VM_GET_BY_ID  0x10   /*get specified VMs by ID*/
#define AC_GET_MACHINE_BY_NAME 0x20 /*get specified machine by name*/

/*
* define options for Dynamic Cluster provision info
*/
#define AC_PROV_QUERY_BY_JOB    0x1    /*Query Dynamic Cluster provision request by Job ID*/
#define AC_PROV_QUERY_BY_ID     0x2    /*Query Dynamic Cluster provision request by provision ID*/

/*
* define options for Dynamic Cluster template info
*/
#define AC_TEMPLATE_QUERY_BY_ID 0x1 /*Query Dynamic Cluster template request by template ID*/
#define AC_TEMPLATE_QUERY_BY_NAME 0x2 /*Query Dynamic Cluster template request by template name*/
#define AC_TEMPLATE_QUERY_BY_RES 0x4 /*Query Dynamic Cluster template request by template resname*/
/**
 * \addtogroup groupinfo_define groupinfo_define
 * define options for \ref lsb_usergrpinfo and \ref lsb_hostgrpinfo calls
 */
/*@{*/
#define USER_GRP          0x1	  /**< User group*/
#define HOST_GRP          0x2	  /**< Host group*/
#define HPART_HGRP        0x4	  /**< Host part group*/
/**
 * \defgroup group_membership_option group_membership_option
 * \ingroup groupinfo_define
 * group membership options
 */
/*@{*/
#define GRP_RECURSIVE     0x8     /**< Expand the group membership recursively.
                                   * That is, if a member of a group is itself
                                   * a group, give the names of its members
                                   * recursively, rather than its name, which is
                                   * the default.*/
#define GRP_ALL           0x10    /**< Get membership of all groups.*/
/*@}*/
#define NQSQ_GRP          0x20	  /**< NQSQ_GRP*/
#define GRP_SHARES        0x40	  /**< Group shares*/
#define DYNAMIC_GRP       0x800	  /**< Dynamic group*/
#define GRP_CU            0x1000  /**< Group cu*/
/*@}*/

/**
 *\brief Structure for representing the shares assigned to a user group.
 */
struct userShares {
    char  *user;     /**< This can be a user or a keyword "default" or others */ 
    int   shares;    /**< The number of shares assigned to the user */
};

/**
 *\brief Structure for representing the admin right in user group.
 */
struct adminRight {
    int value;     /**< the value representing a specific right. */ 
    char * string;    /**< The string of right, such as "usergroup" */
};

   
/**
 * \brief  group information entry. 
 */
struct groupInfoEnt {

    char               *group;        	 /**< Group name */
    char               *memberList;    	 /**< ASCII list of member names */
    char               *adminMemberList; /**< ASCII list of group admin member names */ 
    int                numUserShares;    /**< The number of users with shares */ 
    struct userShares  *userShares;      /**< The user shares rappresentation */
    int                numGfsPolicies; /**< The number of global fairshares
                                                 which this user group participants */
    char               **gfsPolicies;  /**< The array of global fairshares */
    char                *hostStr; /* save original valid host name list */    
    

/**
 * \addtogroup group_define group_define
 *  group define statements
 */
/*@{*/
#define GRP_NO_CONDENSE_OUTPUT 0x01  /**< Group output is in regular
                                      * (uncondensed) format.*/
#define GRP_CONDENSE_OUTPUT    0x02  /**< Group output is in condensed format.*/
#define GRP_HAVE_REG_EXP       0x04  /**< Group have regular expresion*/       	
#define GRP_SERVICE_CLASS      0x08  /**< Group is a service class.*/
#define GRP_IS_CU              0x10  /**< Group is a compute unit.*/
#define GRP_IS_EGROUP          0x20  /**< Group is egroup.*/
#define GRP_IS_HP              0x40  /**< Group is host partition. */
#define GRP_IS_QUEUE           0x80  /**< Group is queue. */
#define GRP_IS_UGRP            0x100 /**< Group is a user group. */
#define GRP_HAS_PARENT         0x200 /**< Group has parent. */
#define GRP_CACHE_UGINFO       0x400 /**< Group is cached in ugroupHT. */
#define GRP_CACHE_UGDATA       0x800 /**< Group is cached in tmpUGrpTab. */
#define GRP_DELETED            0x1000 /** <Group is removed. */
#define GRP_SLA_NAME           0x2000 /** <Group name is same as another sla name. */
#define GRP_DYNAMIC            0x4000 /** <for dynamic hostgroup> */
/*@}*/ 
    int      options;     /**< Options.see \ref group_define*/ 
    char     *pattern;    /**< Host membership pattern */
    char     *neg_pattern;/**< Negation membership pattern */
    int      cu_type;     /**< Compute unit type */
};
    
/**
 * \brief  run job request.
 */
struct runJobRequest {
    LS_LONG_INT jobId;    /**< Jobid of the requested job */
    int     numHosts;     /**< The number of hosts */
    char    **hostname;   /**< Vector of hostnames */
/**
 * \addtogroup runjob_option runjob_option
 * Options used for lsb_runjob:
 */
/*@{*/
  
#define RUNJOB_OPT_NORMAL     0x01     /**< Normal jobs*/
#define RUNJOB_OPT_NOSTOP     0x02     /**< Nostop jobs*/
#define RUNJOB_OPT_PENDONLY   0x04     /**< Pending jobs only, no finished
                                        * jobs */
#define RUNJOB_OPT_FROM_BEGIN 0x08     /**< Check point job only, from beginning */
#define RUNJOB_OPT_FREE       0x10     /**< brun to use free CPUs only */
#define RUNJOB_OPT_IGNORE_RUSAGE  0x20 /**< brun ignoring rusage */
/*@}*/ 
    int     options;      /**< Run job request options, see \ref runjob_option */
    int     *slots;       /**< Vector of number of slots per host */
};

/**
 * \addtogroup external_msg_processing external_msg_processing
 * options for \ref lsb_readjobmsg call
 */
/*@{*/

/**
 * \defgroup external_msg_post external_msg_post
 * options specifying if the message has an attachment to be posted
 */
/*@{*/
#define EXT_MSG_POST   0x01    /**< Post the external job message. There 
                                * is no attached data file. */
#define EXT_ATTA_POST  0x02    /**< Post the external job message
                                * and data file posted to the job.*/
/*@}*/      
                         
#define EXT_MSG_READ   0x04    /**<Read the external job message. There is
                                * no attached data file. */
#define EXT_ATTA_READ  0x08    /**< Read the external job message and data
                                * file posted to the job.If there is no data
                                * file attached, the error message "The
                                * attached data of the message is not available"
                                * is displayed, and the external job message is
                                * displayed.*/
#define EXT_MSG_REPLAY 0x10    /**< Replay the external message*/
#define EXT_MSG_POST_NOEVENT 0x20  /**< Post the external job noevent
                                    * message*/
/*@}*/

#define BRUN_FORWARD_MEGAHOST "*BRUN_FORWARD_MEGAHOST*"


/**
 * \brief structure jobExternalMsgReq contains the information required to
 * define an external message of a job.
 */
struct jobExternalMsgReq {
    int    options;    /**< Specifies if the message has an attachment
                        * to be read.
                        *<lsf/lsbatch.h> defines the following flags
                        * constructed from bits. These flags
                     	* correspond to options.
                     	*\n EXT_MSG_READ
                     	*\n Read the external job message. There is no
                     	* attached data file.
                     	*\n EXT_ATTA_READ
                     	*\n Read the external job message and data file
                     	* posted to the job.
                     	*\n If there is no data file attached, the error
                     	* message "The attached data of the message is not
                        * available" is displayed, and the external job 
                        * message is displayed.*/
    LS_LONG_INT jobId;  /**< The system generated job Id of the job. */
    char   *jobName;  	/**< The name of the job if jobId is undefined (<=0)*/
    int    msgIdx;      /**< The message index. A job can have more than one
                         * message. Use msgIdx in an array
                     	 * to index messages.*/
    char   *desc;       /**< Text description of the msg */
    int    userId;      /**< The userId of the author of the message.*/
    long   dataSize;    /**< The size of the data file. If no data file is
                         * attached, the size is 0. */
    time_t postTime;    /**< The time the author posted the message. */
    char   *userName;   /**< The author of the message. */
};

/**
 * \addtogroup ext_data_status ext_data_status
 */
/*@{*/
#define EXT_DATA_UNKNOWN    0    /**< Transferring the message's data file. */
#define EXT_DATA_NOEXIST    1    /**< The message does not have an attached 
                                  * data file. */
#define EXT_DATA_AVAIL	    2    /**< The message's data file is available.  */
#define EXT_DATA_UNAVAIL    3    /**< The message's data file is corrupt. */
/*@}*/

/**
 * \brief structure jobExternalMsgReply contains the information required to
 * define an external message reply.
 */
struct jobExternalMsgReply {
    LS_LONG_INT jobId;        /**< The system generated job Id of the job
                               * associated with the message. */
    int    msgIdx;            /**< The message index. A job can have more
                               * than one message. Use msgIdx in an array
                     	       * to index messages. */
    char   *desc;             /**< The message you want to read. */
    int    userId;            /**< The user Id of the author of the message. */
    long   dataSize;          /**< The size of the data file attached. If no
                               * data file is attached, the size is 0. */
    time_t postTime;          /**< The time the message was posted. */
    int    dataStatus;        /**< The status of the attached data file. 
                               * The status of the data file can be one of the
                     	       * following:
                     	       *\n EXT_DATA_UNKNOWN
                     	       *\n Transferring the message's data file.
                     	       *\n EXT_DATA_NOEXIST
                     	       *\n The message does not have an attached data file.
                     	       *\n EXT_DATA_AVAIL
                     	       *\n The message's data file is available. 
                     	       *\n EXT_DATA_UNAVAIL
                     	       *\n The message's data file is corrupt.
                     	       */
    char   *userName;         /**< The author of the msg */
};


/* Data structures representing the symphony job status update request.
 */
struct symJobInfo {
    char * partition;       /* the service parititon that SSM works for */    
    int priority;                       /* the priority of the symphony job */
    char * jobFullName;     		/* the full name that indicates the job relationship */    
    char * auxCmdDesc;       /* the auxiliary description to help updating command info */
    char * auxJobDesc;       /* the auxiliary description to help updating job description info */
};

struct symJobStatus {
    char   *desc;       /* text description of the symphony job status */
};

struct symJobProgress {
    char   *desc;       /* text description of the symphony job progress */
};


struct symJobStatusUpdateReq {
    LS_LONG_INT jobId;      /* the job to be update info into MBD */
#define	SYM_JOB_UPDATE_NONE		0x0
#define	SYM_JOB_UPDATE_INFO		0x1
#define SYM_JOB_UPDATE_STATUS		0x2
#define SYM_JOB_UPDATE_PROGRESS		0x4
    int    bitOption;	            /* the option to update the info */
    struct symJobInfo        info;
    int    numOfJobStatus;      
    struct symJobStatus  * status;
    struct symJobProgress  progress;    
};

struct symJobStatusUpdateReqArray {
    int numOfJobReq;
    struct symJobStatusUpdateReq * symJobReqs;
};


/* Data structures representing the symphony job status update reply.
 */

struct symJobUpdateAck {
#define	SYM_UPDATE_ACK_OK	0
#define	SYM_UPDATE_ACK_ERR	1
    int      ackCode;
    char   * desc;       /* text description of job info update acknowledgement */    
};

struct symJobStatusUpdateReply {
    LS_LONG_INT jobId;      		/* the job to be update info into MBD */
#define	SYM_UPDATE_INFO_IDX		0
#define SYM_UPDATE_STATUS_IDX		1
#define SYM_UPDATE_PROGRESS_IDX		2
#define NUM_SYM_UPDATE_ACK		3
    struct symJobUpdateAck    acks[NUM_SYM_UPDATE_ACK];
};

struct symJobStatusUpdateReplyArray {
    int numOfJobReply;
    struct symJobStatusUpdateReply * symJobReplys;
};

/* Data structure representing the job array requeue operation.
 * o jobId is the Lsbatch id of the job array to be requeued
 * o status is the desired requeue status of the job, by default
 *   it is JOB_STAT_PEND, or user specified JOB_STAT_PSUSP
 * o options specifies the status of the array elements that have
 *   to be requeued.
 *
 * The function that operates on the data is lsb_requeuejob()
 */
 
/**
 * \addtogroup requeuejob_options requeuejob_options
 * define statements used by \ref lsb_requeuejob.
 */
/*@{*/
#define REQUEUE_DONE   0x1  /**< Requeues jobs that have finished running.
              	             * Jobs that have exited are not re-run. Equivalent
              	             * to brequeue -d command line option.*/
#define REQUEUE_EXIT   0x2  /**< Requeues jobs that have exited. Finished jobs
              	             * are not re-run. Equivalent to brequeue -e
              	             * command line option.*/
#define REQUEUE_RUN    0x4  /**< Requeues running jobs and puts them in PEND
              	             * state. Equivalent to brequeue -r command line
              	             * option. */
#define REQUEUE_RMT_PEND   0x8  /**< Requeues remote pending jobs in mc submission cluster,
                                    * Equivalent to brequeue -a command line
                                    * option to requeue remote pending job */                            
    
#define REQUEUE_RMT_PEND_RISEPRIORITY    0x10  /**< Requeues remote pending jobs in mc submission cluster,
                                                    * Equivalent to brequeue -p command line
                                                    * option. */

/*@}*/

/**
 * \brief  requeued job
 */
struct jobrequeue {
    LS_LONG_INT      jobId;   /**< Specifies the jobid of a single job or
           		       * an array of jobs.*/
    int              status;  /**< Specifies the lsbatch status of the
		               * requeued job after it has been requeued.
		               * The job status can be JOB_STAT_PEND or
		               * JOB_STATE_PSUSP. The default status is
		               * JOB_STAT_PEND. */
    int              options; /**< Specifies the array elements to be requeued. 
                               * see \ref requeuejob_options
                               */
};

struct requeueEStruct {
    int type;                 /* requeue type: normal, exclude, other, 
                                 prefer_other, etc. */
#define RQE_NORMAL   0        /* requeue type: normal - as in 2.2 */
#define RQE_EXCLUDE  1        /* requeue type: exclude */
#define RQE_END     255       /* indicate the end of the list */
    int  value;               /* requeue exit value */
    int  interval;            /* requeue interval */
};

typedef struct requeue{
    int numReqValues;
    struct requeueEStruct *reqValues;
} requeue_t;

/* The Service Level Agreement in LSF
 */

struct guarPoolInfo {
    char *name;                    /* Guarantee pool names */
    int type;                      /* Guarantee pool type */
    char *rsrcName;                /* Only used for shared resource pools */
    int nGuaranteed;               /* Number of guaranteed resources configured*/
    int nUsed;                     /* Total number of used resources */
    int nGuaranteedUsed;           /* Number of guaranteed resources used */
};

/* This is the library representation of the 
 * service class.
 */
struct serviceClass {
    char               *name;         /* SLA name */
    float              priority;      /* SLA priority */
    int                ngoals;        /* The number of goals */
    struct objective   *goals;        /* The array of goals */
    char               *userGroups;   /* Users allowed to use the SLA */
    char               *description;  /* SLA description */
    char               *controlAction; /* SLA control action */
    float              throughput;     /* Finished jobs per CLEAN_PERIOD */
    int                counters[NUM_JGRP_COUNTERS + 1]; /* Job counters */
    char               *consumer;      /* project scheduling enabled sla */
    struct slaControl  *ctrl;          /* SLA EGO control parameters */
    struct slaControlExt  *ctrlExt;       /* SLA EGO control parameters */
    char               *accessControl; /* Guarantee sla access control*/
    int                autoAttach;     /* Guarantee sla auto attach flag */
    int                nGuarPools;     /* Number of guarantee pools */
    struct guarPoolInfo *guarPools;    /* Guarantee pool information */
};

/* This is the library representation of the 
 * Service Level Objective.
 */

#define GOAL_WINDOW_OPEN    0x1
#define GOAL_WINDOW_CLOSED  0x2
#define GOAL_ONTIME         0x4
#define GOAL_DELAYED        0x8
#define GOAL_DISABLED       0x10

/* Enumerate all the possible performance goals
 * for a service class.
 */
enum objectives {
    GOAL_DEADLINE,
    GOAL_VELOCITY,
    GOAL_THROUGHPUT,
    GOAL_GUARANTEE
};

/* The objective of a goal, also called SLO, is represented
 * by this data structure.
 */
struct objective {
    char   *spec;      /* goal specs from lsb.serviceclasses */
    int    type;       /* goal type */
    int    state;      /* the state of the goal OnTime || Delayed */
    int    goal;       /* the configured value */
    int    actual;     /* the actual value */
    int    optimum;    /* the optimum value */
    int    minimum;    /* the minimum value */
};

/* Control parameters for SLA management of hosts belonging 
 * to the EGO cluster. The control parameters are for each 
 * SLA that gets its hosts from EGO.
 */
struct slaControl {
    char   *sla;             /* sla name */
    char   *consumer;        /* EGO consumer the sla is mapped to */
    int    maxHostIdleTime;  /* timeout for returning hosts to EGO */
    int    recallTimeout;    /* timeout left before EGO forcefully reclaims */
    int    numHostRecalled;  /* number of hosts beign recalled */
    char   *egoResReq;       /* EGO resource requirement */
};

struct slaControlExt {
    int    allocflags;       /* whether exclusive allocation */
    int    tile;             /* tile parameter */
};


/* Application Encapsulation in LSF
 *
 * This is the library representation of the 
 * application.
 */
struct appInfoEnt {
    char    *name;                          /* app name */
    char    *description;                   /* app description */
    int     numJobs;                        /* num of total jobs */
    int     numPEND;                        /* num of pending slots */
    int     numRUN;                         /* num of running slots */
    int     numSSUSP;                       /* num of suspend slots */
    int     numUSUSP;                       /* num of ususp slots */
    int     numRESERVE;                     /* reserved job slots */ 
    int     aAttrib;                        /* app attributes */
    int     chunkJobSize;                   /* number of jobs in one chunk */
    char    *requeueEValues;                /* requeue exit values */
    char    *successEValues;                /* success exit values */
    char    *preCmd;                        /* app pre execution */
    char    *postCmd;                       /* app post execution */
    char    *jobStarter;                    /* Job starter command(s) */
    char    *suspendActCmd;                 /* suspend action command */
    char    *resumeActCmd;                  /* resume action command */
    char    *terminateActCmd;               /* terimate action command */
    int     memLimitType;                   /*memory limit level type */
    int     defLimits[LSF_RLIM_NLIMITS];    /* LSF resource limits (soft)*/
    char    *hostSpec;                      /* host spec from CPULIMIT or 
                                             * RUNLIMIT */
    char    *resReq;                        /* resource requirement string */
    int     maxProcLimit;                   /* maximal processor limit */
    int     defProcLimit;                   /* default processor limit */
    int     minProcLimit;                   /* minimal processor limit */
    int     runTime;                        /* estimated run time */
    int     jobIncludePostProc;  /* include postproc as part of job */
    int     jobPostProcTimeOut;    /* time window for postproc */ 	
    char    *rTaskGoneAction;               /* remote task gone action */
    char    *djobEnvScript;                 /* pathname of pjob env script */
    int     djobRuInterval;                 /* DJOB rusage interval */
    int     djobHbInterval;                 /* DJOB heartbeat interval */
    char    *djobCommfailAction;            /* DJOB communication fail action */
    int     djobDisabled;            /* disable Distributed Application Framework */
    int     djobResizeGracePeriod;   /* grace period (in seconds) before terminating tasks
                                      * when a job shrinks
                  */
    char   *chkpntDir;               /* chkpnt directory */
    char   *chkpntMethod;            /* chlpnt method  */
    int    chkpntPeriod;             /* chkpnt period  */
    int    initChkpntPeriod;         /* initial chkpnt period  */
    int    migThreshold;             /* migration  threshold */
    int    maxJobPreempt;                   /* maximum number of job preempted times */
    int    maxPreExecRetry;                 /* maximum number of pre-exec retry times */
    int    localMaxPreExecRetry;            /* maximum number of pre-exec retry times for local cluster */
    int    maxJobRequeue;                   /* maximum number of job re-queue times */
    int    noPreemptRunTime;                /* no preempt run time */
    int    noPreemptFinishTime;             /* no preempt finish time */
    int    noPreemptRunTimePercent;         /* no preempt run time percent */
    int    noPreemptFinishTimePercent;      /* no preempt finish time percent */
    int    usePam;                                        /* use Linux-PAM */
    int    bindingOption;                   /* processor binding options */
    int    persistHostOrder;            /* persistent same hosts and same order */
    char   *resizeNotifyCmd;		    /* job resize notification cmd */
    int    noPreemptInterval;               /**< Uninterrupted running time (minutes) before job can be preempted. */    
    int    maxTotalTimePreempt;             /**< Maximum accumulated preemption time (minutes). */
    int    nice;                            /**<  nice value */
#if defined(LSF_8_0_MOCK_PARAM)
    int    mockParamA;
    char   *mockParamB;
    float  mockParamC;
#endif /* LSF_8_0_MOCK_PARAM */
    int preemptDelayTime; /**< The grace period before preemption*/
    int    jobPreProcTimeOut;    /**< Timeout of pre-execution processing*/
    char   *jobCwd;              /**< The job CWD directory*/
    int    jobCwdTtl;            /**< The job CWD time-to-live parameter*/
    int    acmachtype;                      /**< Dynamic Cluster machine type */
    char   *actemplates;                    /**< Dynamic Cluster Machine Template Name */
    char   *acVmJobPActionStr;                  /**< Dynamic Cluster Vm Job preemption action */
    char   *networkReq;         /**< POE job network requirement string */
    char   *envVars;                      /**< Customer defined environment variable from lsb.applications ENV_VARS >**/
    char   *dcCheckpointStr;        /**< Dynamic Cluster checkpoint period*/
};

/* application attributes
 */
#define A_ATTRIB_RERUNNABLE      0x01       /* rerunnable application */
#define A_ATTRIB_NONRERUNNABLE   0x02       /* non rerunnable application */
#define A_ATTRIB_DEFAULT         0x04       /* default application */
#define A_ATTRIB_ABS_RUNLIMIT    0x08       /* runtime is absolute */
#define A_ATTRIB_JOBBINDING      0x10       /* process binding application */
#define A_ATTRIB_NONJOBBINDING   0x20       /* process binding application */
#define A_ATTRIB_CHKPNT          0x40       /* checkpointable application */
#define A_ATTRIB_RESIZABLE	 0x80       /* Job can be resizable manually */
#define A_ATTRIB_AUTO_RESIZABLE	 0x100      /* Job can be resized automatically */
#define A_ATTRIB_AC_ANY_TEMPLATE 0x200      /* Dynamic Cluster application */
#define A_ATTRIB_AC_TEMPLATE_NONE 0x400     /* No Dynamic Cluster hosts are used */
#define A_ATTRIB_NONRESIZABLE    0x800      /* Job cannot be resizable */
#define A_ATTRIB_AFF_DJOB_TASK_BIND 0x1000    /* Sync up task info on host*/

/* apply to acmachine type 
 */
#define   AC_MACHINE_TYPE_PM     0x01       /* Dynamic Cluster machine type */
#define   AC_MACHINE_TYPE_VM     0x02       /* Dynamic Cluster machine type */
#define   AC_MACHINE_TYPE_ANY    0x04       /* Dynamic Cluster machine type */

/* apply to acVmJobPreemptionAction
 */
#define   AC_VMJOB_PACTION_SAVEVM  0x01     /* Dynamic Cluster save vm */
#define   AC_VMJOB_PACTION_REQUEUEJOB 0x02  /* Dynamic Cluster requeue job */
#define   AC_VMJOB_PACTION_LIVEMIGRATEVM 0x04 /* Dynamic Cluster livemigrate vm */

/**
 * \Dynamic Cluster host status
 */
/*@{*/
typedef enum {
    AC_HOST_STAT_UNKNOWN = 0,          /* Machine unknown. If it is unknown,
                                     * LSF won't operate on it.
				     */
    AC_HOST_STAT_ON,                   /* Machine is on. WMC must guarantee
                                     * both machine's hostname and IP
				     * address are valid before marking
				     * it is on. Once seen its state is on
				     * in wmc_get_provresult() or
				     * WMC_PROVISION_CALLBACK_FUNC_T,
				     * LSF may immediately call
				     * wmc_getmachinebyid() to get the
				     * hostname and IP.
				     */
    AC_HOST_STAT_OFF,                  /* Machine is off       */
    AC_HOST_STAT_SUSPENDING,           /* Machine is suspending. The machine
                                     * is changing from WMC_MSTAT_ON
				     * to WMC_MSTAT_SUSPENDED.
				     */
    AC_HOST_STAT_SUSPENDED,            /* Machine is suspended */
    AC_HOST_STAT_STARTING,             /* Machine is starting. The machine
                                     * is being powered on, and changing
				     * from WMC_MSTAT_OFF to WMC_MSTAT_ON.
				     */
    AC_HOST_STAT_SHUTTING_DOWN,        /* Powering off. The machine is
                                     * changing from WMC_MSTAT_ON
				     * to WMC_MSTAT_OFF.
				     */
    AC_HOST_STAT_INSTALLING,           /* Provisioning. The machine is being
                                     * created and installed, and changing
				     * to WMC_MSTAT_OFF.
				     */
    AC_HOST_STAT_MIGRATING,            /* Migrating. The machine is being
                                     * migrated, and changing from
				     * WMC_MSTAT_ON to WMC_MSTAT_ON.
				     */
    AC_HOST_STAT_PAUSED,             /* machine is paused */
    AC_HOST_STAT_REBOOTING,          /* machine is being rebooted */
    AC_HOST_STAT_CLONING,            /* machine is being cloned */
    AC_HOST_STAT_UNINSTALLING,       /* machine is being uninstalled */
    AC_HOST_STAT_EXPORTING,          /* machine is being exported */
    AC_HOST_STAT_IMPORTING,          /* machine is being imported */
    AC_HOST_STAT_UNINSTALLED,        /* machine is uninstalled */
    AC_HOST_STAT_CONVERTING_TO_TEMPLATE, /* machine is being converted to a template */
    AC_HOST_STAT_MOVING,             /* machine is being moved */
    AC_HOST_STAT_PROVISIONING,       /* machine is being provisioned (PM) */
    AC_HOST_STAT_MAINTENANCE,         /* Machine is in maintenance. This
                                     * state is only for hypervisors. We may
                                     * or may not need this state in the end
                                     * when we do node re-purposing. */
    AC_HOST_STAT_CHECKPOINTING        

} ac_host_state_t;
/*@}*/

/* machine type, this also applies to resource group
 */
#define    AC_MTYPE_UNKNOWN     0         /* unknown type */
#define    AC_MTYPE_HYPERVISOR  0x01      /* hypervisor */
#define    AC_MTYPE_PM          0x02      /* pm */
#define    AC_MTYPE_VM          0x04      /* vm */
#define    AC_MTEMPLATE_SHARED    0x04  /* shared machine template */
#define    AC_MTEMPLATE_LOCAL     0x08  /* local machine template */

/* if machine is hypervisor and pm, it has both attributes
 */
#define    AC_MTYPE_PM_HYPERVISOR  (AC_MTYPE_HYPERVISOR | AC_MTYPE_PM)

typedef struct acHostInfoEnt {
    char          *machid;          /* unique identifer, VM uuid or
                                     * hypervisor/PM hostname
				     */
    char          *mname;           /* machine name, for vm     */
    int           type;             /* current machine type     */
    int           sup_type;         /* supported machine types */
    ac_host_state_t  state;            /* dc host state   */

    /* ISF/RFI may not be able to return a single unique hostname,
     * e.g. if a machine has multiple hostnames and NICs. So it is WMC's
     * responsibility to use EGO API and conf to resolve a unique official
     * hostname and IP for the machine which can be used in EGO/LSF.
     */
    char          *hname;           /* official hostname for EGO/LSF */

    char          *resgroup;        /* current resource group   */
    int            nResGroup;        /* resource group name number*/
    char **        allResGroups;      /* all the resource groups names*/

    char          *templatename;        /* current template name    */
    long          maxmem;           /* in MB            */
    int           ncpus;            /* number of cpus   */
    long          mem;              /* available memory */
    
    /* type specific attributes */
    /* Hypervisor specific data  */
    char          *hvid;            /* Hypervisor machine id      */
    char          *hypertype;       /* KVM or Redhat xen */

    /* VM specific data          */
    char          *hyperid;         /* hypervisor id, which hypervisor
                                       vm is running on */

    /* physical provision specific data */
    char          *pmid;            /* This is the machine ID used in
                                     * WMC_RGATTRIB_PROVISION resource group
				     * for physical provisioning only.
				     */
    /*to avoid call mbd again to get the extend information*/
    int           njobs;
    LS_LONG_INT   *jobids; /*job ID which running on the current dc host*/
    long           remainTime;/*Time To Live*/
    /* raw info regarding Dynamic Cluster template - 'templatename'
     */
    char          *rfi_tname;     /* ISF template name */
    char          *pps_name;       /* post provisioning script name */
    char          *pps_arguments;       /* post provisioning script argument list */
    int         maxLifeRemain;          /*maximum life time remain*/
    time_t         machineCreTime;       /*machine creation time */
    time_t         lastPowOnTime;        /*machine last power on time*/
    time_t         lastPowOffTime;         /*machine last power off time*/
}acHostInfoEnt_t;


/**
 * template type
 */
typedef enum {
    AC_HYPERVISOR_TYPE_UNKNOWN = 0,
    AC_HYPERVISOR_TYPE_XE4 = 1,
    AC_HYPERVISOR_TYPE_ESX3 = 2,
    AC_HYPERVISOR_TYPE_XER5 = 3,
    AC_HYPERVISOR_TYPE_KVM
} ac_hypertype_t;

struct acTemplateInfoEnt {
    char          *tname;           /* template name */
    int           tattrib;          /* template attributes */
    char          *tid;             /* unique identifer */
    char          *resgroup; /* resource group name */

    int           nHosts;
    char**        hosts;     /* local template host name link */

    int            tscope;        /* template scope*/ 
    ac_hypertype_t hyperType;       /* hypervisor type name if vm */
    char          *rfi_tname;     /* ISF template name */
    char          *pps_name;       /* post provisioning script name */
    char          *pps_arguments;       /* post provisioning script argument list */
    char          *description;   /* template description */
    int           nResourcesProvisionable; /* the number of resources provisionable */
    char          **resourcesProvisionable; /* list of resources provisionable */
};


/* wmc provision request 
 */
enum ac_prov_reqcode {
    AC_PROV_REQ_UNKNOWN = -1,     /* unknown Dynamic Cluster provision action*/
    AC_PROV_REQ_NEWVM = 0,        /* get new vm and power on */
    AC_PROV_REQ_IDLEVM,           /* get an idle vm and return, for
                                      optimization purpose, wmc will
                                      not handle this right away */
    AC_PROV_REQ_POWEROFF,         /* power off machines         */
    AC_PROV_REQ_SWITCHPM,         /* switch pm from one template to
                                      another template */
    AC_PROV_REQ_SAVEVM,            /* suspend a vm   */
    AC_PROV_REQ_RESTOREVM,         /* restore a vm on a host  */
    AC_PROV_REQ_POWEROFF_FORCEFULLY,    /* force power off machines */
    AC_PROV_REQ_POWERON,            /* power on machine, right now only
                                      for PM */
    AC_PROV_REQ_USEPM,              /* use physical machine directly */
    AC_PROV_REQ_INSTALL_VM,         /* install a new VM */
    AC_PROV_REQ_UNINSTALL_VM,       /* uninstall a new VM */
    AC_PROV_REQ_SWITCHHV,           /* switch hypervisor  */
    AC_PROV_REQ_MOVEVM,              /* move a vm */
    AC_PROV_REQ_LIVEMIGRATEVM          /* live migrate a vm */
};

typedef enum {
    AC_PROV_REQ_WAIT = 0,   /* not stareted yet */ 
    AC_PROV_REQ_INPROC,   /* in progress */ 
    AC_PROV_REQ_DONE,         /* Done successfully */
    AC_PROV_REQ_FAIL,          /* Fail in the middle */
    AC_PROV_REQ_CLEAN         /*clean the provision*/ 
} ac_prov_reqstate_t;

/* for query active provisions
 */
struct acActionInfoEnt {
    /* the following IDs are provided as internal use only
     * command may display tree like structure by using these.
     */
    int subreqId; /* sub request id to which the action belongs */
    int stepId;  /* step id to which the action belongs */
    int actionId; /* its action id */
    enum ac_prov_reqcode op; 
    char *target; 
    char *host; /* hypervisor name if any */
    ac_prov_reqstate_t state;
    char *templateName; /* the Dynamic Cluster template related to action*/
    char *targetHostName;     /* provision host name*/
};

struct acProvisionInfoEnt {
    /* brief info 
     */
    char *reqId;
    char *workinfo; /* job ID */
    char *who;
    time_t requested;
    time_t finished; /* later we may allow the provision data stay a little bit longer
                      * in order to support a better view around provisioning
		      */
    ac_prov_reqstate_t state;   /* same reason above 'finished' */
    /* detailed info 
     */
    int nHosts;
    char **hosts; /* list of hosts appear in the final steps.
                        */
    int nActions;
    struct acActionInfoEnt *actions; /* array of actions  */
};

/* processor binding options */
#define   BINDING_OPTION_BALANCE            0x1
#define   BINDING_OPTION_PACK               0x2
#define   BINDING_OPTION_ANY                0x4
#define   BINDING_OPTION_USER               0x8
#define   BINDING_OPTION_USER_CPU_LIST      0x10
#define   BINDING_OPTION_NONE               0x20

/* options for lsb_movejob() call */
#define    TO_TOP            1
#define    TO_BOTTOM         2

/* options for lsb_queuecontrol() call */
#define    QUEUE_OPEN        1   
#define    QUEUE_CLOSED      2
#define    QUEUE_ACTIVATE    3 
#define    QUEUE_INACTIVATE  4   
#define    QUEUE_CLEAN       5   

/**
 * \addtogroup movejob_options movejob_options
 * options for \ref lsb_movejob call  
 */
/*@{*/
#define    TO_TOP            1  /**<  To top */
#define    TO_BOTTOM         2  /**<  To bottom */
/*@}*/

/**
 * \addtogroup queue_ctrl_option queue_ctrl_option
 * options for \ref lsb_queuecontrol call 
 */
/*@{*/
#define    QUEUE_OPEN        1   /**< Open the queue to accept jobs.*/
#define    QUEUE_CLOSED      2   /**< Close the queue so it will not accept
                                  * jobs.*/
#define    QUEUE_ACTIVATE    3 	 /**< Activate the queue to dispatch jobs.*/
#define    QUEUE_INACTIVATE  4   /**< Inactivate the queue so it will not
                                  * dispatch jobs.*/
#define    QUEUE_CLEAN       5   /**< Clean the queue*/
/*@}*/

/**
 *  \brief The structure of queueCtrlReq
 */
struct queueCtrlReq {
    char  *queue;      /**< The name of the queue to be controlled. */
    int    opCode;     /**< Operations to be applied, for example, QUEUE_OPEN.
                        * You can refer to \ref queue_ctrl_option for more options. */
    char  *message;    /**< The message attached by the admin */
};

/* options for lsb_hostcontrol() call */
/**
 * \addtogroup host_ctrl_option host_ctrl_option
 * options operations to be applied 
 */
/*@{*/
#define    HOST_OPEN         1  /**< Opens the host to accept jobs. */
#define    HOST_CLOSE        2  /**< Closes the host so that no jobs can be dispatched to it.*/
#define    HOST_REBOOT       3  /**< Restarts sbatchd on the host. sbatchd will receive
	                         * a request from mbatchd and re-execute. This permits the sbatchd
	                         * binary to be updated. This operation fails if no sbatchd is running
	                         * on the specified host.*/
#define    HOST_SHUTDOWN     4  /**< The sbatchd on the host will exit. */
#define    HOST_CLOSE_REMOTE 5  /**< Used for closing leased host on the submission cluster */
/*@}*/

/**
 * \brief  Host control request.
 */
struct hostCtrlReq {
    char  *host;       /**< The host to be controlled. If host is NULL, the local host is assumed. */
    int    opCode;     /**< Operations to be applied in \ref host_ctrl_option. */
    char  *message;    /**< Message attached by the administrator. */
};

/* options for lsb_hgcontrol() call */
#define    HGHOST_ADD        1
#define    HGHOST_DEL        2

struct hgCtrlReq {
    int opCode;
    char* grpname;
    int numhosts;
    char** hosts;
    char* message;
};

struct hgCtrlReply {
    int numsucc;
    int numfail;
    char** succHosts;
    char** failHosts;
    int* failReasons;
};

/* options for lsb_reconfig() call */
/**
 * \addtogroup mbd_operation mbd_operation
 *  options for \ref lsb_reconfig call
 */
/*@{*/
#define    MBD_RESTART       0   /**< mbatchd restart */
#define    MBD_RECONFIG      1   /**< mbatchd reread configuration files */
#define    MBD_CKCONFIG      2   /**< mbatchd check validity of configuration files */
#define    MBD_PRESTART      3   /**< mbd parallel restart */
/*@}*/
/**
 * \brief  mbatchd control request.
 */

struct mbdCtrlReq {    
    int    opCode;     /**< Operation applied, defined in \ref mbd_operation */
    char   *name;      /**< Not used so far */
    char  *message;    /**< The message attached by the admin */
};

/* opcode for turn on or off the perfmon monitor */
#define	   PERFMON_START	1
#define    PERFMON_STOP     	2
#define    PERFMON_SET_PERIOD	3

#define DEF_PERFMON_PERIOD      60      /* defualt sample period 60 */


struct perfmonMetricsEnt{       
    char* name;               /* metrice name */
    long current;       /* last period counters */
    long max;               /* max of (counter/interval)*sample period for one period */	
    long min;       	/* min of (counter/interval)*sample period for one period */
    long avg;       	/* avg of (total/interval)*sample period for one period */
    char* total;       /* total counters from performance monitor turn on  */
};

/*performance monitor info*/
struct perfmonInfo{
    int num;        	/* number of metrics*/
    struct perfmonMetricsEnt *record;   /* array of metrics counter */
    int period;       	/* sample period */
    time_t start;       /* time when the performance moniter turn on */
    time_t end;                         /* time when the performance moniter turn off */
};

/* options for lsb_reljgrp() call */
#define    JGRP_RELEASE_PARENTONLY          0x01


/**
 * \brief Records of logged events
 */
struct logSwitchLog {
    int lastJobId;   /**< The last jobId so far */
#if defined(LSF_SIMULATOR)
    time_t lastTraceTime; /**< last trace record time */
    int    lastTraceType; /**< last trace record type */
    char   *lastTraceInfo;  /**< last trace record info */
#endif
};

/**
 * \brief Records of job CPU data logged event
 */
struct dataLoggingLog {
    time_t loggingTime;   /**< The time of last job cpu data logging  */
};

/**
 * \brief  new job group log.
 */
struct jgrpNewLog {
    int    userId;          /**< The user ID of the submitter*/
    time_t submitTime;      /**< The job submission time*/
    char   userName[MAX_LSB_NAME_LEN];  /**< The name of the submitter*/
    char   *depCond;        /**< The job dependency condition*/
    char   *timeEvent;      /**< Time event string*/
    char   *groupSpec;      /**< Job group name*/
    char   *destSpec;       /**< New job group name*/
    int    delOptions;      /**< Delete options in options field */
    int    delOptions2;     /**< Extended Delete options in options2 field */
    int    fromPlatform;    /**< Platform type: such as Unix, Windows*/
    char   *sla;            /**< SLA service class name under which the job runs*/
    int  maxJLimit;         /**< Max job group slots limit*/ 
    int    options;         /**< Job group creation method: implicit or explicit*/
};

/**
 * \brief  job group control log.
 */
struct jgrpCtrlLog {
    int    userId;       /**< The user ID of the submitter*/
    char   userName[MAX_LSB_NAME_LEN];	 /**< The name of the submitter*/
    char   *groupSpec;   /**< Job group name*/
    int    options;      /**< Options*/
    int    ctrlOp;       /**< Job control JGRP_RELEASE, JGRP_HOLD, JGRP_DEL*/
};

/**
 * \brief  job group status log.
 */
struct jgrpStatusLog {
    char   *groupSpec;  /**< The full group path name for the job group*/
    int    status;     /**< Job group status*/
    int    oldStatus;    /**< Prior status*/
};

/**
 * \brief jobNewLog logged in lsb.events when a job is submitted.
 */
struct jobNewLog {        
    int    jobId;       		/**< The job ID that the LSF assigned
                                         * to the job */
    int    userId;       		/**< The user ID of the submitter */
    char   userName[MAX_LSB_NAME_LEN];  /**< The name of the submitter	*/
    int    options;       		/**< Job submission options. see
                                         * \ref lsb_submit.*/
    int    options2;       		/**< Job submission options. see
                                         * \ref lsb_submit.*/
    int    numProcessors;       	/**< The number of processors requested
                                         * for execution */
    time_t submitTime;       		/**< The job submission time */
    time_t beginTime;       		/**< The job should be started on or
                                         * after this time */
    time_t termTime;       		/**< If the job has not finished by this
                                         * time, it will be killed */
    int    sigValue;       		/**< The signal value sent to the job
                                         * 10 minutes before its run window
                                         * closes */
    int    chkpntPeriod;       	        /**< The checkpointing period */
    int    restartPid;       		/**< The process ID assigned to the
                                         * job when it was restarted */
    int    rLimits[LSF_RLIM_NLIMITS];   /**< The user's resource limits	*/
    char   hostSpec[MAXHOSTNAMELEN];    /**< The model, host name or host type
                                         * for scaling CPULIMIT and RUNLIMIT */
    float  hostFactor;       		/**< The CPU factor for the above model,
                                         * host name or host type */
    int    umask;       		/**< The file creation mask for this
                                         * job*/
    char   queue[MAX_LSB_NAME_LEN];     /**< The name of the queue to which this
                                         * job was submitted */
    char   *resReq;       		/**< The resource requirements of the
                                         * job*/
    char   fromHost[MAXHOSTNAMELEN];    /**< The submission host name*/
    char   *cwd;       		        /**< The current working directory*/
    char   *chkpntDir;       		/**< The checkpoint directory*/
    char   *inFile;       		/**< The input file name */
    char   *outFile;       		/**< The output file name*/
    char   *errFile;       		/**< The error output file name */
    char   *inFileSpool;                /**< Job spool input file */
    char   *commandSpool;               /**< Job spool command file */
    char   *jobSpoolDir;                /**< Job spool directory */
    char   *subHomeDir;       		/**< The home directory of the submitter
                                         */
    char   *jobFile;       		/**< The job file name */
    int    numAskedHosts;       	/**< The number of hosts considered for
                                         * dispatching this job */
    char   **askedHosts;       	        /**< The array of names of hosts
                                         * considered for dispatching this job
                                         */
    char   *dependCond;       		/**< The job dependency condition */
    char   *timeEvent;       		/**< Time event string */
    char   *jobName;       		/**< The job name */
    char   *command;       		/**< The job command */
    int    nxf;                         /**< The number of files to transfer */
    struct xFile *xf;       		/**< The array of file transfer
                                         * specifications. (The xFile structure
                                         * is defined in <lsf/lsbatch.h>)*/
    char   *preExecCmd;          	/**< The command string to be
                                         * pre_executed */
    char   *mailUser;            	/**< User option mail string */
    char   *projectName;         	/**< The project name for this job, used
                                         * for accounting purposes */
    int    niosPort;                    /**< Port to be used for interactive
                                         * jobs */
    int    maxNumProcessors;       	/**< Maximum number of processors*/
    char   *schedHostType;       	/**< Execution host type*/
    char   *loginShell;          	/**< Login shell specified by user */
    char   *userGroup;                  /**< The user group name for this job */
    char   *exceptList;          	/**< List of alarm conditions for job */
    int    idx;                         /**< Array idx, must be 0 in JOB_NEW */
    int    userPriority;         	/**< User priority */
    char   *rsvId;       		/**< Advance reservation ID */
    char   *jobGroup;         		/**< The job group under which the job
                                         * runs.*/
    char   *extsched;       		/**< External scheduling options */
    int    warningTimePeriod;         	/**< Warning time period in seconds,
                                         * -1 if unspecified */
    char   *warningAction;              /**< Warning action, SIGNAL | CHKPNT |
                                         * command, NULL if unspecified */
    char   *sla;                        /**< The service class under which the
                                         * job runs. */
    int    SLArunLimit;                 /**< The absolute run limit of the job
                                         */
    char   *licenseProject;        	/**< License Project */
    int    options3;       		/**< Extended bitwise inclusive OR of
                                         * options flags. See \ref lsb_submit.*/
    char   *app;                        /**< Application profile under which
                                         * the job runs. */
    char   *postExecCmd;       	        /**< Post-execution commands. */
    int      runtimeEstimation;       	/**< Runtime estimate specified.*/
    char   *requeueEValues;       	/**< Job-level requeue exit values.*/
    int     initChkpntPeriod;           /**< Initial checkpoint period */
    int     migThreshold;               /**< Job migration threshold. */
    char   *notifyCmd;       		/**< Resize notify command */
    char   *jobDescription;             /**< Job description. */
    struct submit_ext *submitExt;       /**< For new options in future */
#if defined(LSF_SIMULATOR)
    int    maxmem;                     /**< maximum memory */
    int    exitstatus;                 /**< exit status */
    int    runtime;                    /**< job run time */
    int    cputime;                    /**< system cpu time */
    int    slots;                      /**< allocated slots */
    float  cpufactor;                  /**< cpu factor */
#endif

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */
    int    options4;                   /**< Job submission options. see
                                         * \ref lsb_submit.*/
    int    numAskedClusters;           /**< The number of clusters considered
                                         * for dispatching this job */
    char   **askedClusters;            /**< List of asked clusters */
    char   *flow_id;                   /** < Internal option, indicate the process
                                         * manager flow id. no usage. */
    char   *subcwd;                    /**< The submission current
                                         *working directory*/
    char   *outdir;                    /**< The output directory*/
    char   *dcTmpls;                   /**< Dynamic Cluster templates */
    char   *dcVmActions;               /**< Dynamic Cluster VM actions */
    struct networkReq  network;   /* PE network requirement, specified by -network. */
};

#if defined(LSF_SIMULATOR)
struct jobArrayElementLog {
    int jobId;
    int idx;
    /* Copy LSF simulator related fields from jobNewLog  */
    int maxmem;
    int exitstatus;
    int runtime;
    int cputime;
    int slots;
    float cpufactor;
};
#endif

/**
 * \brief  job modified log.
 */
struct jobModLog {

    char    *jobIdStr;      /**< JobId or jobName in char*/
    int     options;        /**< Job submission options(See \ref lsb_submit)*/
    int     options2;       /**< Job submission options(See \ref lsb_submit)*/
    int     delOptions;     /**< Delete options in options field */
    int     delOptions2;    /**< Extended delete options in options2 field . */
    int     userId;         /**< The user ID of the submitter*/
    char    *userName;      /**< The name of the submitter*/
    int     submitTime;     /**< The job submission time */
    int     umask;          /**< The file creation mask for this job*/
    int     numProcessors;  /**< The number of processors requested for
                             * execution */
    int     beginTime;      /**< The job should be started on or after this
                             * time */
    int     termTime;       /**< If the job has not finished by this time, 
                             * it will be killed */
    int     sigValue;       /**< The signal value sent to the job 10 minutes
                             * before its run window closes */
    int     restartPid;     /**< The process ID assigned to the job when it
                             * was restarted */

    char    *jobName;       /**< The job name*/
    char    *queue;         /**< The name of the queue to which this job was
                             * submitted*/

    int     numAskedHosts;  /**< The number of hosts considered for dispatching
                             * this job */
    char    **askedHosts;   /**< List of asked hosts */

    char    *resReq;        /**< The resource requirements of the job*/
    int     rLimits[LSF_RLIM_NLIMITS];	/**< User's resource limits (soft) */
    char    *hostSpec;      /**< The model, host name or host type for scaling
                             * CPULIMIT and RUNLIMIT*/

    char    *dependCond;    /**< The job dependency condition */
    char    *timeEvent;     /**< Time event string. */

    char    *subHomeDir;    /**< The home directory of the submitter */
    char    *inFile;        /**< The input file name */
    char    *outFile;       /**< The output file name */
    char    *errFile;       /**< The error output file name */
    char    *command;       /**< Command description - this is really
                             * a job description field */
    char    *inFileSpool;   /**< Job spool input file */
    char    *commandSpool;  /**< Job spool command file */
    int     chkpntPeriod;   /**< The checkpointing period*/
    char    *chkpntDir;     /**< The checkpoint directory*/
    int     nxf;            /**< The number of files to transfer */
    struct  xFile *xf;      /**< The array of file transfer specifications. 
                             * (The xFile structure is defined in
       		             * <lsf/lsbatch.h>) */

    char    *jobFile;       /**< The job file name: If == '\\0', indicate
                             * let mbatchd make up name, otherwise, mbatchd
                             * will use given name.  It is '\\0' if it is a
                             * regular job,non-nil means it is a restart job.
                             */
    char    *fromHost;      /**< The submission host name*/
    char    *cwd;           /**< The current working directory*/

    char    *preExecCmd;          /**< The pre-execution command */
    char    *mailUser;            /**< User option mail string */
    char    *projectName;         /**< Project name for the job; used for
                                   * accounting purposes */

    int     niosPort;             /**< NIOS callback port to be used for
                                   * interactive jobs */
    int     maxNumProcessors;     /**< Maximum number of processors */

    char    *loginShell;          /**< The login shell specified by user */
    char    *schedHostType;       /**< Restart job's submission host type */
    char    *userGroup;           /**< The user group name for this job */
    char    *exceptList;          /**< List of job exception conditions */
    int     userPriority;         /**< User priority */
    char    *rsvId;               /**< Advance reservation ID */
    char    *extsched;            /**< External scheduling options */
    int     warningTimePeriod;    /**< Job warning time period in seconds;
                                   * -1 if unspecified */
    char    *warningAction;       /**< Job warning action: SIGNAL | CHKPNT |
                                   * command; NULL if unspecified */
    char    *jobGroup;            /**< The job group under which the job runs */
    char    *sla;                 /**< SLA service class name under which the
                                   * job runs */
    char    *licenseProject; 	  /**< LSF License Scheduler project name */
    int     options3;       	  /**< Extended bitwise inclusive OR of options
                                   * flags. see \ref lsb_submit.*/
    int     delOptions3;          /**< Extended delete options in options3
                                   * field. */
    char    *app;                 /**< Application profile under which the job
                                   * runs. */
    char    *apsString;           /**< Absolute priority scheduling string set
                                   * by administrators to denote static 
          		  	   * system APS value or ADMIN factor APS value.
          		  	   */
    char   *postExecCmd;	  /**< Post-execution commands.*/
    int      runtimeEstimation;	  /**< Runtime estimate.*/
    char   *requeueEValues;	  /**< Job-level requeue exit values.*/
    int     initChkpntPeriod;     /**< Initial checkpoint period */
    int     migThreshold;         /**< Job migration threshold. */
    char   *notifyCmd;            /**< Resize notify command */
    char   *jobDescription;       /**< Job description. */
    struct submit_ext *submitExt; /**< For new options in future */

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */
    int     options4;       	  /**< Extended bitwise inclusive OR of options
                                   * flags. see \ref lsb_submit.*/
    int     delOptions4;          /**< Extended delete options in options4
                                   * field. */
    int     numAskedClusters;  /**< The number of clusters considered for dispatching
                             * this job */
    char    **askedClusters;   /**< List of asked clusters */

    /* These fields are added for MC remote job modification */
    int rmtJobCtrlStage;           /**< Remote job modification stage */
    int numRmtCtrlResult;             /**< The number of remote job modification session which is
                                  *generated by this modify request. 0 means no remote control
                                  session */
    struct rmtJobCtrlRecord *rmtCtrlResult; /**< remote job modifcation records */
    char   *outdir;       /**< Job output directory. */
    char    *dcTmpls;                   /**< Dynamic Cluster templates */
    char    *dcVmActions;               /**< Dynamic Cluster VM actions */
    struct networkReq  network;   /* PE network requirement, specified by -network. */
};

/* Data structures representing job affinity allocation.
 */

struct affinityPU {
    int             num_pu_path;        /**< number of pu in the path */
    unsigned short  *pu_path;           /**< pu in the path */
};
    
struct affinitySubtaskData {
    PU_t            pu_type;            /**< allocated pu type */
    int             num_pu;             /**< number of pu */
    struct affinityPU   *pu_list;       /**< allocated pu list */
    PU_t            expu_type;          /**< exclusive pu type */
    int             expu_scope;         /**< exclusive pu scope */
};

struct affinityTaskData {
    int        mem_node_id;             /**< only makes sense if memory bind is defined */
    int        num_bind_cpu;            /**< number of binding cpu */
    unsigned short *bind_cpu_list;      /**< cpu list for binding */
    int        num_subtask;             /**< number of sub-tasks */
    struct affinitySubtaskData *sdata;  /**< sub-task data */
};

struct affinityHostData {
    char         *hostname;             /**< hostname in string*/
    void         *hptr;                 /**< host data structure */
    float        mem_per_task;          /**< how much memory is allocated per task */ 
    int          num_task;              /**< number of tasks */
    struct affinityTaskData    *tdata;  /**< all tasks on the host */ 
};

struct affinityResreqData {
    PU_t             cpu_bind_level;    /**< which level of cpu binding */ 
    memBindPolicy_t  mem_bind_policy;   /**< localonly or localprefer */
    int              num_hosts;         /**< number of hosts */
    struct affinityHostData    *hdata;  /**< affinity info on each host */
};

struct affinityJobData {
    int                  num_resreq;    /**< number of resreq  */
    struct affinityResreqData  **rdata; /**< affinity info for each resreq */
};

/**
 * \brief  logged in lsb.events when a job is started.
 */
struct jobStartLog {      
    int    jobId;       	/**< The unique ID for the job*/
    int    jStatus;       	/**< The status of the job (see 
                                 * \ref lsb_readjobinfo )*/
    int    jobPid;       	/**< The job process ID*/
    int    jobPGid;       	/**< The job process group ID*/
    float  hostFactor;       	/**< The CPU factor of the first execution
                                 * host*/
    int    numExHosts;       	/**< The number of processors used for
                                 * execution*/
    char   **execHosts;       	/**< The array of execution host names*/
    char   *queuePreCmd;        /**< Pre-execution command defined in the
                                 * queue*/
    char   *queuePostCmd;       /**< Post-execution command defined in the
                                 * queue*/
    int	   jFlags;       	/**< Job processing flags*/
    char   *userGroup;       	/**< The user group name for this job*/
    int    idx;       		/**< Job array index; must be 0 in JOB_NEW*/
    char   *additionalInfo;     /**< Placement information of LSF jobs*/
    int    duration4PreemptBackfill;/**< How long a backfilled job can run;
                                     * used for preemption backfill jobs*/
    int    jFlags2;       	/**<  Job Flags2*/
    char   *effectiveResReq;    /**< Effective resreq which scheduler used to dispatch job */


    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    int   num_network;            /**< allocated PE network number */
    struct networkAlloc *networkAlloc; /**< PE network allocation. */
    struct affinityJobData  *affinity; /**< affinity allocation information */

    int    nextStatusNo;               /**< next status number for statusUpd in MC/XL */
};

/**
 * \brief logged in lsb.events when a job start request is accepted.
 */
struct jobStartAcceptLog {  
    int    jobId;       	/**< The unique ID for the job*/
    int    jobPid;       	/**< The job process ID*/
    int    jobPGid;       	/**< The job process group ID*/
    int    idx;       		/**< Job array index; must be 0 in JOB_NEW*/

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    int    nextStatusNo;               /**< next status number for statusUpd in MC/XL */
};

/**
 * \brief logged in lsb.events when a job is executed.
 */
struct jobExecuteLog {   /* logged in lsb.events when a job is executed */
    int    jobId;       /**< The unique ID for the job*/
    int    execUid;     /**< User ID under which the job is running*/
    char   *execHome;   /**< Home directory of the user denoted by execUid*/
    char   *execCwd;    /**< Current working directory where job is running*/
    int    jobPGid;     /**< The job process group ID*/
    char   *execUsername; /**< User name under which the job is running*/
    int    jobPid;      /**< The job process ID*/
    int    idx;         /**< Job array index; must be 0 in JOB_NEW*/
    char   *additionalInfo; /**< Placement information of LSF jobs*/
    int    SLAscaledRunLimit; /**< The run limit scaled by the exec host */
    int    position;       /**< The position of the job*/
    char   *execRusage;    /**< The rusage satisfied at job runtime*/
    int    duration4PreemptBackfill; /**< The duration for preemptive backfill
                                      * class in seconds*/

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    char   *outdir;     /**< output directory */

    int    nextStatusNo;               /**< next status number for statusUpd in MC/XL */
};


/**
 * \brief Logged when a job's status is changed.
 *        Use this new data structure to parse job status log from LSF8.0
 */
struct jobStatusLog {     
    int    jobId;       /**< The unique ID for the job*/
    int    jStatus;     /**< The job status (see \ref lsb_readjobinfo )*/
    int    reason;      /**< The reason the job is pending or suspended 
                         * (see \ref lsb_pendreason and \ref lsb_suspreason )*/
    int    subreasons;  /**< The load indices that have overloaded the host
                         * (see \ref lsb_pendreason  and \ref lsb_suspreason )*/
    float  cpuTime;     /**< The CPU time consumed before this event occurred*/
    time_t endTime;     /**< The job completion time*/
    int    ru;          /**< Boolean indicating lsfRusage is logged */
    struct lsfRusage lsfRusage;	
            /**< Resource usage statistics
	     *The lsfRusage structure is defined in <lsf/lsf.h>. Note that
	     * the availability of certain fields depends on the platform
	     * on which the sbatchd runs. The fields that do not make sense
	     * on the platform will be logged as -1.0.*/
    int   jFlags;       /**< Job exit status*/
    int   exitStatus;   /**< Job's exit status */
    int    idx;         /**< Job array index; must be 0 in JOB_NEW*/
    int    exitInfo;    /**< Job termination reason, see <lsf/lsbatch.h>*/
    int   numhRusages;  /**< The number of host based rusage entries, only logged in case of JOB_STAT_DONE/JOB_STAT_EXIT */
    struct hRusage * hostRusage; /**< Host based rusage list, one item per host, only logged in case of JOB_STAT_DONE/JOB_STAT_EXIT */
    int   maxMem;  /**< job maximum memory usage*/
    int   avgMem;  /**< job average memory usage */

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    int   maskedJStatus;               /**< masked jStatus by MASK_INT_JOB_STAT */

    int    nextStatusNo;               /**< next status number for statusUpd in MC/XL */
};


/**
 * \brief logged when a job's status is changed
 */
struct sbdJobStatusLog {     
    int    jobId;      /**< The unique ID for the job*/
    int    jStatus;    /**< The status of the job (see \ref lsb_readjobinfo) */
    int    reasons;    /**< The reason the job is pending or suspended (See
                        * \ref lsb_pendreason and \ref lsb_suspreason)*/
    int    subreasons; /**< The load indices that have overloaded the host
                        * (See \ref lsb_pendreason and \ref lsb_suspreason)*/
    int    actPid;     /**< Action process ID */
    int    actValue;   /**< Action Value SIG_CHKPNT | SIG_CHKPNT_COPY | 
                        * SIG_WARNING*/
    time_t actPeriod;  /**< Action period*/
    int    actFlags;   /**< Action flag*/
    int    actStatus;  /**< Action logging status*/
    int    actReasons; /**< Action Reason SUSP_MBD_LOCK | SUSP_USER_STOP |
                        * SUSP_USER_RESUME | SUSP_SBD_STARTUP*/
    int    actSubReasons;  /**< Sub Reason SUB_REASON_RUNLIMIT |
                            * SUB_REASON_DEADLINE |SUB_REASON_PROCESSLIMIT |
                            * SUB_REASON_MEMLIMIT |SUB_REASON_CPULIMIT*/
    int    idx;        /**< Job array index; must be 0 in JOB_NEW*/
    int    sigValue;   /**< The signal value*/
    int    exitInfo;   /**< The termination reason of a job */
    int   numhRusages;  /**< The number of host based rusage entries, only logged in case of JOB_STAT_DONE/JOB_STAT_EXIT */
    struct hRusage * hostRusage; /**< Host based rusage list, one per host, only logged in case of JOB_STAT_DONE/JOB_STAT_EXIT */
};

/**
 * \brief job status that we could send to MBD
 */
struct sbdUnreportedStatusLog {   
    int    jobId;            /**< The unique ID for the job*/
    int    actPid;           /**< Action process ID*/	
    int    jobPid;           /**< The job process ID*/
    int    jobPGid;          /**< The job process group ID*/
    int    newStatus;        /**< New status of the job*/
    int    reason;           /**< Pending or suspending reason code*/
    int    subreasons;       /**< Pending or suspending subreason code*/
    struct lsfRusage lsfRusage;  /**< Resource usage information for the job 
                                  * (see jobFinishLog)*/
    int    execUid;        /**< User ID under which the job is running*/
    int    exitStatus;     /**< Job exit status*/
    char   *execCwd;       /**< Current working directory where job is running*/
    char   *execHome;      /**< Home directory of the user denoted by execUid*/
    char   *execUsername;  /**< User name under which the job is running*/
    int    msgId;          /**< Message index*/
    struct jRusage runRusage; /**< Job's resource usage*/
    int    sigValue;       /**< Signal value*/
    int    actStatus;      /**< Action logging status*/
    int    seq;            /**< Sequence status of the job*/
    int    idx;            /**< Job array index*/
    int    exitInfo;       /**< The termination reason of a job */
    int    numhRusages;  /**< The number of host based rusage entries, only logged in case of JOB_STAT_DONE/JOB_STAT_EXIT */
    struct hRusage * hostRusage; /**< Host based rusage list, one per host, only logged in case of JOB_STAT_DONE/JOB_STAT_EXIT */
    int   maxMem;         /**< job maximum memory usage */
    int   avgMem;         /**< job average memory usage */
    char   *outdir;       /**< output directory*/
};

/**
 * \brief logged when a job is switched to another queue
 */
struct jobSwitchLog {
    int    userId;        /**< The user ID of the submitter*/
    int    jobId;         /**< The unique ID of the job*/
    char   queue[MAX_LSB_NAME_LEN]; /**< The name of the queue the job has
                                     * been switched to*/
    int    idx;           /**< Job array index; must be 0 in JOB_NEW*/
    char   userName[MAX_LSB_NAME_LEN];	/**< The name of the submitter*/

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    int     rmtJobCtrlStage;            /**< The stage of remote job switch */
    int     numRmtCtrlResult;           /**< The number of remote job switch record */
    struct  rmtJobCtrlRecord *rmtCtrlResult; /**< The record of each remote job switch session */
};

/**
 * \brief logged when a job array is switched to another queue
 */
struct jobSwitchLog2 {
    int userId;         /**< The user ID of the submitter */
    int jobId;          /**< The unique ID of the job array */
    char queue[MAX_LSB_NAME_LEN]; /**< The name of the queue the job has
                                   * been switched to */
    char userName[MAX_LSB_NAME_LEN]; /**< The name of the submitter */
    int indexRangeCnt;     /**< The number of ranges that indicates the elements
                            * which are switched successfully */
    int *indexRangeStart;  /**< Array of range start */
    int *indexRangeEnd;    /**< Array of range end */
    int *indexRangeStep;   /**< Array of range step */

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    int rmtJobCtrlStage;        /**< Stage that indicate which milestone the remote job switch
                                  *process is in. */
    int numRmtCtrlResult;             /**<The number of remote job control sessions generated by this job
                                  *switch request. 0 means no remote control session. */
    struct rmtJobCtrlRecord *rmtCtrlResult; /**< The record of each remote control session */
};

/**
 * \brief logged when a job is moved to another position
 */
struct jobMoveLog {       /* logged when a job is moved to another position */
    int    userId;       /**< The user ID of the submitter*/
    int    jobId;        /**< The unique ID of the job*/
    int    position;     /**< The new position of the job*/
    int    base;         /**< The operation code for the move (see 
                          * \ref lsb_movejob)*/
    int    idx;          /**< Job array index; must be 0 in JOB_NEW*/
    char   userName[MAX_LSB_NAME_LEN];	/**< The name of the submitter*/

    int    rmtJobCtrlStage; /**< The stage of remote job move handling process */
    int    numRmtCtrlResult; /**< The number of records for remote job move handling */
    struct rmtJobCtrlRecord *rmtCtrlResult; /**< Remote job move result */
};

/**
 * \brief  check point log.
 */
struct chkpntLog {
    int jobId;     /**< The unique ID of the job*/
    time_t period; /**< The new checkpointing period*/
    int pid;       /**< The process ID of the checkpointing process (a child sbatchd) */
    int ok;        /**< 0: checkpoint started; 1: checkpoint succeeded*/
    int flags;     /**< One of the following:
              	    * \n LSB_CHKPNT_KILL : Kill process if checkpoint successful
              	    * \n LSB_CHKPNT_FORCE : Force checkpoint even if non-checkpointable conditions exist
              	    * \n LSB_CHKPNT_MIG : Checkpoint for the purpose of migration*/
    int    idx;    /**< Job array index; must be 0 in JOB_NEW*/

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */
};

/**
 * \brief  job requeue log.
 */
struct jobRequeueLog {
    int   jobId;   /**< The unique ID of the job*/
    int   idx;     /**< Job array index; must be 0 in JOB_NEW*/
};

/**
 * \brief  job clean log.
 */
struct jobCleanLog {
    int   jobId;    /**< The unique ID for the job*/
    int   idx;      /**< Job array index; must be 0 in JOB_NEW*/	
};
/**
 * \brief  job exception log.
 */
struct jobExceptionLog {
    int jobId;       	/**< The unique ID for the job*/
    int    exceptMask;  /**< Job exception handling mask*/
    int    actMask;     /**< Action Id (kill | alarm | rerun | setexcept)*/
    time_t timeEvent;   /**< Time event string*/
    int    exceptInfo;  /**< Except Info, pending reason for missched or
                         * cantrun exception, the exit code of the
     			 *job for the abend exception, otherwise 0.*/
    int    idx;       	/**< Job array index; must be 0 in JOB_NEW*/
};

/**
 * \brief  signal action log.
 */
struct sigactLog {
    int      jobId;              /**< The unique ID of the job */
    time_t   period;             /**< Action period */
    int      pid;                /**< Action process ID */
    int      jStatus;            /**< Job status */
    int      reasons;            /**< Pending reasons */
    int      flags;              /**< Action flag */
    char     *signalSymbol;  /**< Signal symbol from the set: DELETEJOB |
                              *  KILL | KILLREQUEUE |REQUEUE_DONE |
                              * REQUEUE_EXIT | REQUEUE_PEND |REQUEUE_PSUSP_ADMIN
                              * | REQUEUE_PSUSP_USER | SIG_CHKPNT |
       	                      *  SIG_CHKPNT_COPY */
    int      actStatus;      /**< Action logging status (ACT_NO | ACT_START 
                              *| ACT_PREEMPT | ACT_DONE |
       	                      *  ACT_FAIL) .Shown in signal_action*/
    int      idx;            /**< Job array index; must be 0 in JOB_NEW*/

    /* these two fields are added for MC remote job modification. */
    int      rmtJobCtrlStage;   /**< Remote job control stage */
    int      numRmtCtrlResult;  /**< The number of remote control session info in this record. */
    struct rmtJobCtrlRecord  *rmtCtrlResult; /**< Remote job control result */
};

/**
 * \brief  migration log.
 */
struct migLog {
    int    jobId;           /**< The job to be migrated*/
    int    numAskedHosts;   /**< The number of candidate hosts for migration*/
    char   **askedHosts;    /**< The array of candidate host names*/
    int    userId;          /**< The user ID of the submitter*/
    int    idx;             /**< Job array index; must be 0 in JOB_NEW*/
    char   userName[MAX_LSB_NAME_LEN];  /**< The user name of the submitter*/

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */
    int    sigFlag;         /**< Signal flags*/
    int    acAction;        /**< Dynamic Cluster provision action*/
};

/**
 * \brief  signal log.
 */
struct signalLog {
    int    userId;          /**< The user ID of the submitter*/
    int    jobId;           /**< The unique ID of the job*/
    char   *signalSymbol;   /**< Signal symbol from the set: DELETEJOB |
                             * KILL | KILLREQUEUE |REQUEUE_DONE | REQUEUE_EXIT |
                             * REQUEUE_PEND |REQUEUE_PSUSP_ADMIN |
                             * REQUEUE_PSUSP_USER | SIG_CHKPNT |
                     	     * SIG_CHKPNT_COPY*/
    int    runCount;              	/**< The number of running times*/
    int    idx;              		/**< Job array index; must be 0 in JOB_NEW*/
    char   userName[MAX_LSB_NAME_LEN];	/**< The name of the submitter*/

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */
};

/**
 * \brief logged when bqc command is invoked.
 */
struct queueCtrlLog {     
    int    opCode;  /**< The queue control operation (see \ref lsb_queuecontrol)*/
    char   queue[MAX_LSB_NAME_LEN];     /**< The name of the queue*/
    int    userId;                      /**< The user ID of the submitter*/
    char   userName[MAX_LSB_NAME_LEN];  /**< The name of the submitter*/
    char   message[MAXLINELEN];         /**< Queue control message*/
};

/*
 * \brief  new debug log.
 */
struct newDebugLog {
    int opCode;     /**< The queue control operation */
    int level;      /**< Debug level*/
    int _logclass;  /**< Class of log*/
    int turnOff;    /**< Log enabled, disabled*/
    char logFileName[MAXLSFNAMELEN]; /**< Name of log file*/
    int userId;      /**< The user ID of the submitter*/
 };

/**
 * \brief log the host control information.
 */
struct hostCtrlLog { 
    int    opCode;                     /**< The host control operation (See 
                                        * \ref lsb_hostcontrol)*/
    char   host[MAXHOSTNAMELEN];       /**< The name of the host*/
    int    userId;                     /**< The user ID of the submitter*/
    char   userName[MAX_LSB_NAME_LEN]; /**< The name of the submitter*/
    char   message[MAXLINELEN];        /**< Host control message*/
};

/**
 * \brief logged when dynamic hosts are added to group.
 */
struct hgCtrlLog {      
    int    opCode;                     /**< The host control operation 
                                        * (see \ref lsb_hostcontrol)*/
    char   host[MAXHOSTNAMELEN];       /**< The name of the host*/
    char   grpname[MAXHOSTNAMELEN];    /**< The name of the host group*/
    int    userId;                     /**< The user ID of the submitter*/
    char   userName[MAX_LSB_NAME_LEN]; /**< The name of the submitter*/
    char   message[MAXLINELEN];        /**< Host group control message*/
};

#define   SIMU_STATUS_READYSCHEDULE     0x01/* simulator is ready to schedule jobs */

/**
 * \brief  mbatchd start log.
 */
struct mbdStartLog {
    char   master[MAXHOSTNAMELEN];    /**< The master host name */
    char   cluster[MAXLSFNAMELEN];    /**< The cluster name */
    int    numHosts;  	              /**< The number of hosts in the cluster*/
    int    numQueues;                 /**< The number of queues in the cluster*/
#if defined(LSF_SIMULATOR)
    int    simDiffTime;
    int    pendJobsThreshold;
    int    simStatus;
#endif
};

struct mbdSimStatusLog {
    int    simStatus;                   /* simulator status */
};

/**
 * \brief  mbatchd die log.
 */
struct mbdDieLog {
    char   master[MAXHOSTNAMELEN];    /**< The master host name */
    int    numRemoveJobs;             /**< The number of finished jobs that have
                                       * been removed from the system and logged
           		               * in the current event file*/
    int    exitCode;                  /**< The exit code from the master batch
                                       * daemon*/
    char   message[MAXLINELEN];       /**< mbatchd administrator control message*/
};

/**
 * \brief logged before mbatchd dies.
 */
struct unfulfillLog {          
    int    jobId;        /**< The job ID.*/
    int    notSwitched;  /**< The mbatchd has switched the job to a new queue
                          * but the sbatchd has not been informed of the switch*/
    int    sig;          /**< This signal was not sent to the job*/
    int    sig1;         /**< The job was not signaled to checkpoint itself*/
    int    sig1Flags;    /**< Checkpoint flags. see the chkpntLog structure below.*/
    time_t chkPeriod;    /**< The new checkpoint period for the job*/
    int    notModified;  /**< Flag for bmod running job's parameters */
    int    idx;          /**< Job array index*/
    int    miscOpts4PendSig;  /**< Option flags for pending job signals*/
};

#define TERM_UNKNOWN           0
#define TERM_PREEMPT           1
#define TERM_WINDOW            2
#define TERM_LOAD              3
#define TERM_OTHER             4
#define TERM_RUNLIMIT          5
#define TERM_DEADLINE          6
#define TERM_PROCESSLIMIT      7
#define TERM_FORCE_OWNER       8
#define TERM_FORCE_ADMIN       9
#define TERM_REQUEUE_OWNER     10
#define TERM_REQUEUE_ADMIN     11
#define TERM_CPULIMIT          12
#define TERM_CHKPNT            13
#define TERM_OWNER             14
#define TERM_ADMIN             15
#define TERM_MEMLIMIT          16
#define TERM_EXTERNAL_SIGNAL   17
#define TERM_RMS               18
#define TERM_ZOMBIE            19
#define TERM_SWAP              20
#define TERM_THREADLIMIT       21
#define TERM_SLURM             22
#define TERM_BUCKET_KILL       23
#define TERM_CTRL_PID          24
#define TERM_CWD_NOTEXIST   25
#define TERM_REMOVE_HUNG_JOB 26

/**
 * \brief logged in lsb.acct when a job finished.
 */
struct jobFinishLog {          
    int    jobId;              		/**< The unique ID for the job*/
    int    userId;              	/**< The user ID of the submitter*/
    char   userName[MAX_LSB_NAME_LEN];	/**< The user name of the submitter*/
    int    options;              	/**< Job submission options (see 
                                         * \ref lsb_submit)*/
    int    numProcessors;               /**< The number of processors requested
                                         * for execution*/
    int    jStatus;              	/**< The status of the job (See
                                         * \ref lsb_readjobinfo)*/
    time_t submitTime;              	/**< Job submission time*/
    time_t beginTime;              	/**< The job started at or after
                                         * this time*/
    time_t termTime;              	/**< If the job was not finished by
                                         * this time, it was killed*/
    time_t startTime;              	/**< Job dispatch time*/
    time_t endTime;              	/**< The time the job finished*/
    char   queue[MAX_LSB_NAME_LEN];     /**< The name of the queue to which
                                         * this job was submitted*/
    char   *resReq;              	/**< Resource requirements*/
    char   fromHost[MAXHOSTNAMELEN];    /**< Submission host name*/
    char   *cwd;              		/**< Current working directory*/
    char   *inFile;              	/**< Input file name*/
    char   *outFile;              	/**< Output file name */
    char   *errFile;              	/**< Error output file name*/
    char   *inFileSpool;             	/**< Job spool input file */
    char   *commandSpool;               /**< Job spool command file */
    char   *jobFile;              	/**< Job file name*/
    int    numAskedHosts;               /**< The number of hosts considered
                                         * for dispatching this job*/
    char   **askedHosts;                /**< The array of names of hosts
                                         * considered for dispatching this job*/
    float  hostFactor;              	/**< The CPU factor of the first
                                         * execution host*/
    int    numExHosts;              	/**< The number of processors used for
                                         * execution*/
    char   **execHosts;              	/**< The array of names of execution hosts*/
    float  cpuTime;              	/**< The total CPU time consumed by the job*/
    char   *jobName;              	/**< Job name*/
    char   *command;              	/**< Job command*/
    struct  lsfRusage lsfRusage;    /**< Resource usage statistics.
	                     	      *The lsfRusage structure is defined in
	                     	      * <lsf/lsf.h>. Note that the availability
	                     	      * of certain fields depends on the platform
	                     	      * on which the sbatchd runs. The fields that
	                     	      * do not make sense on this platform will be
	                     	      * logged as -1.0.*/
    char   *dependCond;                 /**< The job dependency condition*/
    char   *timeEvent;              	/**< Time event string*/
    char   *preExecCmd;                 /**< The pre-execution command*/
    char   *mailUser;	                /**< Name of the user to whom job related
                                         * mail was sent */
    char   *projectName;                /**< The project name, used for
                                         * accounting purposes. */
    int    exitStatus;                  /**< Job's exit status */
    int    maxNumProcessors;       	/**< Maximum number of processors
                                         * specified for the job*/
    char   *loginShell;                 /**< Login shell specified by user */
    int    idx;                         /**< Job array index*/
    int    maxRMem;               	/**< Maximum memory used by job */
    int    maxRSwap;               	/**< Maximum swap used by job */
    char   *rsvId;                 	/**< Advanced reservation ID */
    char   *sla;               		/**< Service class of the job*/
    int    exceptMask;               	/**< Job exception handling mask*/
    char   *additionalInfo;          	/**< Placement information of LSF 
                                         * jobs*/
    int    exitInfo;                    /**< Job termination reason, see
                                         * <lsf/lsbatch.h>*/
    int    warningTimePeriod;       	/**< Job warning time period in seconds;
                                         * -1 if unspecified*/
    char   *warningAction;    	        /**< Warning action, SIGNAL | CHKPNT |
                                         * command, NULL if unspecified */
    char   *chargedSAAP;      	        /**< SAAP charged for job */
    char   *licenseProject;          	/**< LSF License Scheduler project name */  
    char   *app;             		/**< Application profile under which the
                                         * job runs. */
    char   *postExecCmd;                /**< Post-execution commands.*/
    int      runtimeEstimation;       	/**< Runtime estimate specified.*/
    char   *jgroup;                     /**< Job group name */
    int     options2;              	/**< Option2*/
    char   *requeueEValues;        	/**< Job requeue exit values */
    char   *notifyCmd;      		/**< Resize notify command */
    time_t lastResizeTime;    	        /**< Last resize start time */
    char   *jobDescription;             /**< Job description. */
    struct submit_ext *submitExt;       /**< For new options in future */
    int numhRusages;                    /**< The number of host based rusages */
    struct hRusage * hostRusage;        /**< The list of host based rusages */
    int avgMem;                         /**< job average memory usage */
    char   *effectiveResReq;            /**< Effective resreq which scheduler used to dispatch job */

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    time_t forwardTime;              	/**< Job forward time*/
    int    runLimit;                   /**< Wall clock run time limit bsub -W */
    int    options3;                  /**< Extended bitwise inclusive OR of options flags lsb_submit_options3. */
	char  *flow_id;                   	/** < Internal option, indicate the process manager flow id. no usage. */
    int    acJobWaitTime;               /**< Dynamic Cluster job total wait time for bacct summary WAIT*/
    int    totalProvisionTime;          /**< Dynamic Cluster job total provision time for bacct summary*/
    char   *outdir;                    /**< The output directory*/
    int    runTime;                    /**< jData->runTime*/
    char   *subcwd;                    /**< The submission current working directory*/
    int     num_network;            /**< The number of allocated network */
    struct networkAlloc *networkAlloc;  /**< Network allocation info */
    struct affinityJobData *affinity;   /**<  Affinity allocation info */
};

/**
 * \brief logged in lsb.stream when a job finished.
 */
struct jobFinish2Log {
    LS_LONG_INT    jobId;               /**< The unique ID for the job*/
    int    userId;                      /**< The user ID of the submitter*/
    char   userName[MAX_LSB_NAME_LEN];  /**< The user name of the submitter*/
    int    options;                     /**< Job submission options (see
                                         * \ref lsb_submit)*/
    int    numProcessors;               /**< The number of processors requested
                                         * for execution*/
    int    jStatus;                     /**< The status of the job (See
                                         * \ref lsb_readjobinfo)*/
    time_t submitTime;                  /**< Job submission time*/
    time_t termTime;                    /**< If the job was not finished by
                                         * this time, it was killed*/
    time_t startTime;                   /**< Job dispatch time*/
    time_t endTime;                     /**< The time the job finished*/
    char   queue[MAX_LSB_NAME_LEN];     /**< The name of the queue to which
                                         * this job was submitted*/
    char   *resReq;                     /**< Resource requirements*/
    char   fromHost[MAXHOSTNAMELEN];    /**< Submission host name*/
    char   *cwd;                        /**< Current working directory*/
    char   *inFile;                     /**< Input file name*/
    char   *outFile;                    /**< Output file name */
    char   *jobFile;                    /**< Job file name*/
    int    numExHosts;                  /**< The number of processors used for
                                         * execution*/
    char   **execHosts;                 /**< The array of names of execution hosts*/
    int    *slotUsages;                 /**< The slots used on each exHost */
    float  cpuTime;                     /**< The total CPU time consumed by the job*/
    char   *jobName;                    /**< Job name*/
    char   *command;                    /**< Job command*/
    struct  lsfRusage lsfRusage;    /**< Resource usage statistics.
                                      *The lsfRusage structure is defined in
                                      * <lsf/lsf.h>. Note that the availability
                                      * of certain fields depends on the platform
                                      * on which the sbatchd runs. The fields that
                                      * do not make sense on this platform will be
                                      * logged as -1.0.*/
    char   *preExecCmd;                 /**< The pre-execution command*/
    char   *projectName;                /**< The project name, used for
                                         * accounting purposes. */
    int    exitStatus;                  /**< Job's exit status */
    int    maxNumProcessors;            /**< Maximum number of processors
                                         * specified for the job*/
    char   *sla;                        /**< Service class of the job*/
    int    exitInfo;                    /**< Job termination reason, see
                                         * <lsf/lsbatch.h>*/
    char   *chargedSAAP;                /**< SAAP charged for job */
    char   *licenseProject;             /**< LSF License Scheduler project name */
    char   *app;                        /**< Application profile under which the
                                         * job runs. */
    char   *postExecCmd;                /**< Post-execution commands.*/
    char   *jgroup;                     /**< Job group name */
    int numhRusages;                    /**< The number of host based rusages */
    struct hRusage * hostRusage;        /**< The list of host based rusages */
    char*  execRusage;                  /**< resVal->execRusage of the job */
    char   *clusterName;
    char   *userGroup;                  /**< jData->userGroup */
    int    runtime;                     /**< jData->runTime */
    int    maxMem;                      /**< job maximum memory usage*/
    int    avgMem;                      /**< job average memory usage */
    char   *effectiveResReq;            /**< Effective resreq which scheduler used to dispatch job */

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */

    time_t forwardTime;                 /**< Job forward time*/
    int    runLimit;                   /**< Wall clock run time limit bsub -W */
    int    options3;                  /**< Extended bitwise inclusive OR of options flags in lsb_submit_options3. */
    char  *flow_id;                   	/** < Internal option, indicate the process manager flow id. no usage. */
    int    totalProvisionTime;          /**< DC vm Job total provision time*/
    char  *outdir;                     /**< job output directory */
    int    dcJobFlags;                  /**< DC job flags */
    char  *subcwd;                     /**< job submission current working directory */
    int     num_network;            /**< The number of allocated network */
    struct networkAlloc *networkAlloc;  /**< Network allocation info */
    struct affinityJobData *affinity;   /**<  Affinity allocation info */
};

/**
 * \brief  Logged into lsb.streams when job start
 */

struct jobStartLimitLog {
    char*  clusterName;                  /**< The cluster name*/
    LS_LONG_INT  jobId;                  /**< Unique job id */
    int    options;                      /**< Whether SUB_RLIMIT_UNIT_IS_KB  is set */
    int    lsfLimits[LSF_RLIM_NLIMITS];   /**< Job start rlimits */
    int    jobRlimits[LSF_RLIM_NLIMITS];  /**< submission rlimits */
};

/**
 * \brief  Logged into lsb.status periodically
 */

struct jobStatus2Log {
    time_t      expectedLogTime;            /**< Expected log time */
    int         sampleInterval;             /**< Sample interval */
    int         jStatus;                    /**< Job status*/
    LS_LONG_INT jobId;                      /**< Job Id */
    char        userName[MAX_LSB_NAME_LEN]; /**< Running user name */
    time_t      submitTime;                 /**< Job submission time*/
    time_t      startTime;                  /**< Job start time*/
    time_t	endTime;	
    char        queue[MAX_LSB_NAME_LEN];    /**< Queue name for the job */
    char*       resReq;                     /**< resreq string for the job */
    char        *projectName;               /**< project name */
    char*       app;                        /**< application name */
    char*       clusterName;                /**< cluster name */
    char*       userGroup;                 /**< user groups */
    int         numProcessors;              /**< Number of start processors */
    int         numJobs;                    /**< Number of jobs */
    int         runtimeDelta;               /**< Runtime since last snapshot */
    struct lsfRusage    lsfRusage;          /**< Rusage of the job */
    int         numhRusages;                /**< Host based rusage */
    struct      hRusage * hostRusage;       /**< Host based rusage */
    int         numExHosts;                 /**< Number of execution hosts */
    char**      execHosts;                  /**< Execution host names */
    int*        slotUsages;                  /**< Slots used on each exHost */
    int         jobRmtAttr;                 /**< Remote attr of the job */
    char*       jgroup;                     /**< Job group name */
    char*       execRusage;                 /**< rusage part of resreq */
    int         num_processors;             /**< rusage part of resreq */
    int         reason;                     /**< Used to store suspend reasonq */
    int         maxMem;                     /**< job maximum memory usage*/
    int         avgMem;                     /**< job average memory usage */

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */
    int         provtimeDelta;               /**< DC vm job provision time delta*/
    int         dcJobFlags;                   /**< DC job flag */
};

/**
 * \brief  Logged into lsb.pendingreasons periodically
 */

struct jobPendingReasonsLog {	
    char	userName[MAX_LSB_NAME_LEN]; /**< User name, default key */	
    char	queue[MAX_LSB_NAME_LEN]; /**< Queue name, default key */	
    char*	projectName;	 /**< Project name, if defined in key */
    char*	licenseProject;	 /**< License Project name, if defined in key */
    char*	resReq;		/**< ResReq of the job, if defined in key */
    char*	app;		/**< Application name, if defined in key */
    int		num_processors;	/**< Number of asked slots */
    		/* The above could be keys*/
    int		mainReason;	/**< Main pending reason */
    int		subReason;	/**< Subreason to main reason */
    int 	detailedReason;	/**< detail reason */
    char*	detail;	/**< Addition resource info for the detailed reason. */
		
    int		numJobs;	/**< Number of pending jobs */
    int 	pendingTime;	/**< Sum of pending time delta  */
    int 	sumDetailReasonHosts;	/**< Sum of hosts for jobs with this reason */
    int 	averagePendingTime; /**< Average pending time of the jobs   */
    time_t	expectedLogTime;    /**< Expected log time */
    int		sampleInterval;	/**< Time since last log */
    char* 	clusterName;	/**< Name of the cluster */
    char* 	pendingTimeRanking; /**< Ranking of the pending time, as configured */
    char* 	hostType; /**< Host type of the detailed reason */
    char*       combinedResReq; /**< Combined resource requirement of job, if defined in key */
};		

/**
 * \brief  load index log.
 */

struct loadIndexLog {
    int nIdx;     /**< The number of load indices*/
    char **name;  /**< The array of load index names*/
};

/**
 * \brief  calendar log.
 */
struct calendarLog {
    int    options;       /**< Reserved for future use*/
    int    userId;        /**< The user ID of the submitter*/
    char   *name;         /**< The name of the calendar*/
    char   *desc;         /**< Description*/
    char   *calExpr;      /**< Calendar expression*/
};

/**
 * \brief  job forward log.
 */
struct jobForwardLog {
    int    jobId;          /**< The unique ID of the job*/
    char   *cluster;       /**< The cluster name*/
    int    numReserHosts;  /**< Number of Reserved Hosts*/
    char   **reserHosts;   /**< Reserved Host Names*/
    int    idx;            /**< Job array index; must be 0 in JOB_NEW*/
    int    jobRmtAttr;     /**< Remote job attributes from:
	                    * \n JOB_FORWARD Remote batch job on submission side
	                    * \n JOB_LEASE Lease job on submission side
	                    * \n JOB_REMOTE_BATCH Remote batch job on execution side
	                    * \n JOB_REMOTE_LEASE Lease job on execution side
	                    * \n JOB_LEASE_RESYNC Lease job resync during restart
	                    * \n JOB_REMOTE_RERUNNABLE Remote batch job
	                    * rerunnable on execution cluster*/

    char *srcCluster;      /**< source cluster name */
    LS_LONG_INT srcJobId;  /**< source job ID */
    LS_LONG_INT dstJobId;  /**< target job ID */
    char *effectiveResReq; /**< Effective resreq which scheduler used to forward job */
};

/**
 * \brief  job accept log.
 */
struct jobAcceptLog {
    int         jobId;      /**< The unique ID of the job*/
    LS_LONG_INT remoteJid;  /**< The unique ID of the remote job*/
    char        *cluster;   /**< The cluster name*/
    int         idx;        /**< Job array index; must be 0 in JOB_NEW*/
    int  	jobRmtAttr; /**< Remote job attributes from:
	                     * \n JOB_FORWARD Remote batch job on submission side
	                     * \n JOB_LEASE Lease job on submission side
	                     * \n JOB_REMOTE_BATCH Remote batch job on execution side
	                     * \n JOB_REMOTE_LEASE Lease job on execution side
	                     * \n JOB_LEASE_RESYNC Lease job resync during restart
	                     * \n JOB_REMOTE_RERUNNABLE Remote batch job
	                     * rerunnable on execution cluster*/

    char *dstCluster;      /**< target cluster name */
    LS_LONG_INT dstJobId;  /**< target job ID */
};

/**
 * \brief  job accept ack log.
 */
struct jobAcceptAckLog {
    int         jobId;      /**< The unique ID of the job*/
    int         idx;        /**< Job array index; must be 0 in JOB_NEW*/
    int         jobRmtAttr; /**< Remote job attributes from:
                             * \n JOB_FORWARD Remote batch job on submission side
                             * \n JOB_LEASE Lease job on submission side
                             * \n JOB_REMOTE_BATCH Remote batch job on execution side
                             * \n JOB_REMOTE_LEASE Lease job on execution side
                             * \n JOB_LEASE_RESYNC Lease job resync during restart
                             * \n JOB_REMOTE_RERUNNABLE Remote batch job
                             * rerunnable on execution cluster*/
    char *srcCluster;      /**< source cluster name */
    LS_LONG_INT srcJobId;  /**< source job ID */
    char *dstCluster;      /**< target cluster name */
    LS_LONG_INT dstJobId;  /**< target job ID */
};


/**
 * \brief  status Ack log.
 */
struct statusAckLog {
    int jobId;     /**< The unique ID of the job*/
    int statusNum; /**< Line number of Status*/
    int    idx;    /**< Job array index; must be 0 in JOB_NEW*/

    /* the four fields are only used for external programs like RTM */
    char *srcCluster;                  /**< source cluster name */
    LS_LONG_INT srcJobId;              /**< source job ID */
    char *dstCluster;                  /**< target cluster name */
    LS_LONG_INT dstJobId;              /**< target job ID */
}; 

/**
 * \brief  job message log.
 */
struct jobMsgLog {
    int   usrId;  /**< The user ID of the submitter*/
    int   jobId;  /**< The unique ID for the job*/
    int   msgId;  /**< Message index*/
    int   type;   /**< Message type*/
    char  *src;   /**< Message source*/
    char  *dest;  /**< Message destination*/
    char  *msg;   /**< Message*/
    int   idx;    /**< Job array index; must be 0 in JOB_NEW*/
};

/**
 * \brief  job message ack log.
 */
struct jobMsgAckLog {
    int usrId;   /**< The user ID of the submitter*/
    int jobId;   /**< The unique ID for the job*/
    int msgId;   /**< Message index */
    int type;    /**< Message type*/
    char *src;   /**< Message source*/
    char *dest;  /**< Message destination*/
    char *msg;   /**< Message*/
    int    idx;  /**< Job array index; must be 0 in JOB_NEW*/
};

/**
 * \brief  job occupy request log. jobOccupyReqLog is for future use.
 */
struct jobOccupyReqLog {
    int userId;            /**< The user ID of the submitter*/
    int jobId;             /**< The unique ID for the job*/
    int numOccupyRequests; /**< Number of Jobs Slots desired*/
    void *occupyReqList;   /**< List of slots occupied*/
    int    idx;            /**< Job array index; must be 0 in JOB_NEW*/
    char userName[MAX_LSB_NAME_LEN]; /**< The name of the submitter*/
};

/**
 * \brief  job vacate log.jobVacatedLog is for future use.
 */
struct jobVacatedLog {
    int userId;    /**< The user ID of the submitter*/
    int jobId;     /**< The unique ID for the job*/
    int    idx;    /**< Job array index; must be 0 in JOB_NEW*/
    char userName[MAX_LSB_NAME_LEN]; /**< The name of the submitter*/
};

/**
 * \brief  job force request log.
 */
struct jobForceRequestLog {
    int     userId;        /**< The user ID of the submitter*/
    int     numExecHosts;  /**< >1 for local/lease jobs; 0 for remote batch
                            * model */
    char**  execHosts;     /**< The array of execution host names*/
    int     jobId;         /**< The unique ID for the job*/
    int     idx;           /**< Job array index; must be 0 in JOB_NEW*/
    int     options;       /**< Job run options (RUNJOB_OPT_NOSTOP |
                            * JFLAG_URGENT_NOSTOP |JFLAG_URGENT)*/
    char    userName[MAX_LSB_NAME_LEN];	/**< The name of the submitter*/
    char   *queue;         /**< The name of the queue to which this job was
                            * submitted */

    int     rmtJobCtrlStage;    /**< Remote job control stage */
    int     numRmtCtrlResult; /**< the number of remote control results */
    struct rmtJobCtrlRecord *rmtCtrlResult; /**< remote control result */
};

/**
 * \brief  job chunck log.
 */
struct jobChunkLog {
    long         membSize;   /**< Size of array membJobId */
    LS_LONG_INT *membJobId;  /**< Job ids of jobs in the chunk */
    long         numExHosts; /**< The number of processors used for execution */
    char       **execHosts;  /**< The array of names of execution hosts */
    int          nextStatusNo;     /**< next status number for statusUpd in MC/XL */
};



/* Used in jobExternalMsgLog->options */

/*
 * When this option is not JEML_OPT_LOCAL_SUBMIT 
 * and is not JEML_OPT_REMOTE_SUBMIT, this option 
 * must be JEML_OPT_DEFAULT.
 * Such as:
 *     1. when the external message is submit to a local job, the option 
 *        will be JEML_OPT_DEFAULT.
 *     2. when the external message is submit to a forwarded job via client,
          and the remote cluster version is < lsf9.1, this option will be
          JEML_OPT_DEFAULT.
       3. When the external message is submist to a forwared job via 
          mc communication and the opcode is MCB_JOB_EXT_MSG, this option 
          will be JEML_OPT_DEFAULT.
 */
#define JEML_OPT_DEFAULT   0 

/* JEML_OPT_REMOTEJOB_LOCALPOST will be set when this external message is post 
 * by local client and this job should be a forwared job and the remote cluster
 * version No. should >= lsf9.1.
 */
#define JEML_OPT_REMOTEJOB_LOCALPOST   1

/* JEML_OPT_REMOTEJOB_REMOTEPOST will be set when this external message is transfered
 * by remote cluster and use the opcode is MCB_JOB_STATUS.
 */
#define JEML_OPT_REMOTEJOB_REMOTEPOST   2



/**
 * \brief  job external message log.
 */
struct jobExternalMsgLog {
    int      jobId;          /**< The unique ID for the job*/
    int      idx;            /**< Job array index; must be 0 in JOB_NEW*/
    int      msgIdx;         /**< The message index*/
    char     *desc;          /**< Message description*/
    int      userId;         /**< The user ID of the submitter*/
    long     dataSize;       /**< Size of the message*/
    time_t   postTime;       /**< The time the author posted the message.*/
    int	     dataStatus;     /**< The status of the message*/
    char     *fileName;      /**< Name of attached data file. If no file
                              * is attached, use NULL.*/
    char     userName[MAX_LSB_NAME_LEN];  /**< The author of the message*/
    int      options;       /**< The options of jobExternalMsgLog 
                              *  The setted LSB means this external message is 
                              *  from local client.
                              */
    int    nextStatusNo;               /**< next status number for statusUpd in MC/XL */
};

/**
 * \brief  reservation request.
 */
struct rsvRes {
    char     *resName;  /**< Name of the resource (currently: host) */
    int      count;     /**< Reserved counter (currently: cpu number)*/
    int      usedAmt;   /**< Used of the reserved counter (not used)*/
};

/**
 * \brief for advanced reservation.
 */
struct rsvFinishLog {               
    time_t        rsvReqTime;  /**< Time when the reservation is required */
    int           options;     /**< Same as the options field in the struct
                                * addRsvRequest(lsbatch.h) */
    int           uid;         /**< The user who creat the reservation */
    char          *rsvId;      /**< Reservation ID */
    char          *name;       /**< Client of the reservation */    
    int           numReses;    /**< Number of resources reserved */
    struct rsvRes *alloc;      /**< Allocation vector */
    char          *timeWindow; /**< Time window within which
		                * the reservation is active \n
       	         		* Two forms:
       	         		* time_t1-time_t2 or
       	         		* [day1]:hour1:0-[day2]:hour2:0 
       	                        */
    time_t        duration;    /**< Duration in seconds.
                                * duration = to - from :
                                * when the reservation expired
                                */
    char          *creator;    /**< Creator of the reservation */
};

/**
 * \brief  CPU Profile Log
 */
struct cpuProfileLog {
    char servicePartition[MAX_LSB_NAME_LEN];    /**< Queue name*/
    int     slotsRequired;     /**< The number of CPU required */
    int     slotsAllocated;    /**< The number of CPU actually allocated */
    int     slotsBorrowed;     /**< The number of CPU borrowed */ 
    int     slotsLent;         /**< The number of CPU lent */
    /** note:  the number of CPU reserved = slotsAllocated - slotsBorrowed + slotsLent */
};

/**
 * \brief  job resize start notify log.
 */
struct jobResizeNotifyStartLog { 
    int    jobId;              /**<  JobId */
    int    idx;                /**<  Index */
    int	   notifyId;           /**<  Notify Id */
    int    numResizeHosts;     /**<  Number of resized hosts. */
    char   **resizeHosts;      /**<  Resize Hosts */
    int    flags;               /**<  Flags */
    int    nextStatusNo;        /**< next status number for statusUpd in MC/XL */
 };

/**
 * \brief  job resize accept notify log.
 */
struct jobResizeNotifyAcceptLog { 
    int    jobId;                  /**<  JobId */
    int    idx;			   /**<  Index */
    int    notifyId;               /**<  Notify Id*/
    int    resizeNotifyCmdPid;     /**<  Resize Notify command pid*/
    int    resizeNotifyCmdPGid;    /**<  Resize Notify command pgid*/
    int    status;                 /**<  Status*/
    int    nextStatusNo;           /**< next status number for statusUpd in MC/XL */
};

/**
 * \brief  job resize done notify log.
 */
struct jobResizeNotifyDoneLog { 
    int    jobId;                 /**<  JobId*/
    int    idx;                   /**<  Index*/
    int    notifyId;              /**<  Notify Id*/
    int    status;                /**<  Status*/
    int    nextStatusNo;          /**< next status number for statusUpd in MC/XL */
};

/**
 * \brief  job resize release log.
 */
struct jobResizeReleaseLog { 
    int    jobId;                 /**<  JobId */
    int    idx;                   /**<  Index */
    int    reqId;                 /**<  Request Id */
    int    options;               /**<  Options */
    int	   userId;                /**<  User Id */
    char   *userName;             /**<  User Name */
    char   *resizeNotifyCmd;      /**<  Resize Notify command */
    int    numResizeHosts;        /**<  Number of resized hosts */
    char   **resizeHosts;         /**<  Resized hosts */
    struct affinityJobData *affinity;    /**<  Affinity allocation info */
    int    nextStatusNo;          /**< next status number for statusUpd in MC/XL */
};

/**
 * \brief  job resize cancel log.
 */
struct jobResizeCancelLog { 
    int    jobId;                 /**<  JobId*/
    int    idx;                   /**<  Index*/
    int    userId;                /**<  User Id*/
    char   *userName;             /**<  User name*/
    int    nextStatusNo;          /**< next status number for statusUpd in MC/XL */
};

/**
 * \brief log the running rusage of a job in the lsb.stream file
 */
struct jobRunRusageLog {
    int              jobid;   /**< The unique ID of the job*/
    int              idx;     /**< Job array index; must be 0 in JOB_NEW*/
    struct jRusage   jrusage; /**< jrusage*/
};

/**
 * \brief  SLA event log.
 */
struct slaLog {
    char   *name;      /**< Service class name */
    char   *consumer;  /**< Consumer name associated with the service class */
    int    goaltype;   /**< Objectives */
    int    state;      /**< The service class state (ontime, delayed) */
    int    optimum;    /**< Optimum number of job slots (or concurrently
                        * running jobs) needed for the
                        *  service class to meet its service-level goals */
    int    counters[NUM_JGRP_COUNTERS];  /**< Job counters for the service
                                          * class */
};

/**
 * \brief  a wrap of structure perfmonLog for performance metrics project
 */
struct perfmonLogInfo {
    int samplePeriod;  /**<  Sample period*/
    int * metrics;     /**<  Metrics */
    time_t startTime;  /**<  Start time*/
    time_t logTime;    /**<  Log time*/
};

/**
 * \brief performance metrics log in lsb.stream
 */
struct perfmonLog {
    int    samplePeriod;       /**< Sample rate*/
    int	   totalQueries;       /**< Number of Queries*/
    int	   jobQuries;          /**< Number of Job Query*/
    int    queueQuries;        /**< Number of Queue Query*/
    int    hostQuries;         /**< Number of Host Query*/
    int    submissionRequest;  /**< Number of Submission Requests*/
    int	   jobSubmitted;       /**< Number of Jobs Submitted*/
    int    dispatchedjobs;     /**< Number of Dispatched Jobs*/
    int    jobcompleted;       /**< Number of Job Completed*/
    int    jobMCSend;          /**< Number of MultiCluster Jobs Sent*/
    int    jobMCReceive;       /**< Number of MultiCluster Jobs Received*/
    time_t startTime;          /**< Start Time*/ 
	LS_LONG_INT mbdFreeHandle;     /**<Free MBD file descriptors*/
	int    mbdUsedHandle;     /**<Used MBD file descriptors*/
	int    scheduleInterval;  /**<Scheduling interval*/
	int    hostRequirements;    /**<Matching host criteria, unique host selection*/
	int    jobBuckets;          /**<Job buckets*/
};

/**
 *  \brief task finish log.Task accounting record in ssched.acct
 */
struct taskFinishLog {
    struct jobFinishLog jobFinishLog;	/**<  Job finish event*/
    int    taskId;          /**< Task ID*/
    int    taskIdx;         /**< Task index*/
    char   *taskName;       /**< Name of task*/
    int    taskOptions;     /**< Bit mask of task options:
	                     * \n TASK_IN_FILE (0x01)-specify input file
	                     * \n TASK_OUT_FILE (0x02)-specify output file
	                     * \n TASK_ERR_FILE (0x04)-specify error file
	                     * \n TASK_PRE_EXEC (0x08)-specify pre-exec command
	                     * \n TASK_POST_EXEC (0x10)-specify post-exec command
	                     * \n TASK_NAME (0x20)-specify task name*/
    int    taskExitReason;  /**< Task Exit Reason
	                     * \n TASK_EXIT_NORMAL = 0- normal exit
	                     * \n TASK_EXIT_INIT = 1-generic task initialization
	                     * failure
	                     * \n TASK_EXIT_PATH = 2-failed to initialize path
	                     * \n TASK_EXIT_NO_FILE = 3-failed to create task file
	                     * \n TASK_EXIT_PRE_EXEC = 4- task pre-exec failed
	                     * \n TASK_EXIT_NO_PROCESS = 5-fork failed
	                     * \n TASK_EXIT_XDR = 6-xdr communication error
	                     * \n TASK_EXIT_NOMEM = 7- no memory
	                     * \n TASK_EXIT_SYS = 8-system call failed
	                     * \n TASK_EXIT_TSCHILD_EXEC = 9-failed to run sschild
	                     * \n TASK_EXIT_RUNLIMIT = 10-task reaches run limit
	                     * \n TASK_EXIT_IO = 11-I/O failure
	                     * \n TASK_EXIT_RSRC_LIMIT = 12-set task resource
	                     * limit failed*/
};

/**
 * \brief End of stream event. The stream is moved to lsb.stream.0 and 
 * a new lsb.stream is opened. Readers of lsb.stream when encounter 
 * the event EVENT_END_OF_STREAM should close and reopen the 
 * lsb.stream file.
 */
struct eventEOSLog {
    int   eos;              	/**< Event end of stream*/
};

/** 
 * \brief job resize event: indicating a realized job allocation change
 */
struct jobResizeLog {
    int    jobId;                /**<  JobId */
    int	   idx;                  /**<  Index */
    time_t startTime;            /**<  Start time */
    int    userId;                /**<  User Id*/
    char   userName[MAX_LSB_NAME_LEN]; /**<  User name */
    int	   resizeType;           /**< 0 grow, 1 shrink */
    time_t lastResizeStartTime;  /**< The start time of last allocation */
    time_t lastResizeFinishTime; /**< The finish time of last allocation */
    int    numExecHosts;         /**< Allocation before the resize */
    char   **execHosts;          /**<  Execute hosts */
    int	   numResizeHosts;       /**< The delta of the allocation change */
    char   **resizeHosts;        /**<  Resize hosts */
    struct affinityJobData *affinity;   /**<  Affinity allocation info */
};

/**
 * \brief  job provisioning start log.
 */
#define AC_JFLAG_MIGRATING            0x080  /* job is being migrated */

struct jobProvisionStartLog { 
    int    jobId;              /**<  JobId */
    int    idx;                /**<  Index */
    char  *provision_reqid; /**< provision request ID */
    int    provisionFlags;  /**< Bit mask of job provisioning flags:
	                     * \n AC_JFLAG_PREDISPATCH (0x01)-indicates that the execution hosts are not ready yet
	                     * \n AC_JFLAG_VMJOB (0x02)-indicates that the job requires virtual machines
	                     * \n AC_JFLAG_SAVING_MACHINES (0x04)-indicates that virtual machines are to be saved
	                     * \n AC_JFLAG_MACHINE_SAVED (0x08)-indicates that virtual machines are saved
	                     * \n AC_JFLAG_RESTORING_MACHINES (0x10)-indicates that virtual machines are to be restored  */
    int    hasJobStartLog; /**< indicates if the job start record is attached */
    struct jobStartLog jobStartLog; /**< job start log */
    char   *liveMigrateHost;       /**< live migrate host */
 };

/**
 * \addtogroup provision_end_event_state
 */
/*@{*/
#define    AC_PROVISION_END_STATE_DONE   1   /**< provision done successfully */
#define    AC_PROVISION_END_STATE_FAILD  2   /**< provision failed */
/*@}*/

/**
 * \brief  job provisioning end log.
 */
struct jobProvisionEndLog { 
    int    jobId;              /**<  JobId */
    int    idx;                /**<  Index */
    char  *provision_reqid; /**< provision request ID */
    int    provisionFlags;  /**< Bit mask of job provisioning flags:
	                     * \n AC_JFLAG_PREDISPATCH (0x01)-indicates that the execution hosts are not ready yet
	                     * \n AC_JFLAG_VMJOB (0x02)-indicates that the job requires virtual machines
	                     * \n AC_JFLAG_SAVING_MACHINES (0x04)-indicates that virtual machines are to be saved
	                     * \n AC_JFLAG_MACHINE_SAVED (0x08)-indicates that virtual machines are saved
	                     * \n AC_JFLAG_RESTORING_MACHINES (0x10)-indicates that virtual machines are to be restored  */
    int    state;  /**< Provision end state, , defined in \ref  provision_end_event_state */
    int  numExecHosts; /**< The number of hosts allocated by provisioning (machineIDs) */
    char **execHosts; /**< Hosts allocated by provisioning (machine IDs) */
    int  numHosts; /**< The number of hosts allocated by provisioning (host names) */
    char **hosts; /**< Hosts allocated by provisioning (host names) */
    char   *liveMigrateHost;       /**< live migrate target host */ 
};

typedef struct {
    memBindPolicy_t  nodePolicy;
    int              nodeId;   
    int              numPU;
    int              numSubTask;
    int              numCPUs;   
    unsigned short * cpuArray;
} TaskAffSpec_T;

typedef struct {
    char          * hostName;
    int             numTasks;
    TaskAffSpec_T * pTaskAffSpec;
    int             numkvs;            
    struct keyVal * kvs;          
} FanoutHostSpec_T;

/**
 * \brief  job fan-out information log.
 */
typedef struct jobFanoutInfoLog {
    struct jobFanoutInfoLog * forw, * back;  
    LS_LONG_INT        jobId;           /**< JobId */
    time_t             dispTimeStamp;   /**< Job dispatch time in seconds */
    FanoutHostSpec_T * pFanoutHostSpec; /**< fan-out information on the host */
    int                flag;            /**< internal use flag */
    int                nextTaskAffIdx;  /**< internal use index */
    int                setupChildPid;   /**< pid of fan-out child sbatchd */
} FanoutInfoListEntry_T;

/**
 * \brief  Log event types.
 */
union  eventLog {
    struct jobNewLog jobNewLog;       		/**<  Job new event*/
    struct jobStartLog jobStartLog;       	/**<  Job start event */
    struct jobStatusLog jobStatusLog;       	/**<  Job status event*/
    struct sbdJobStatusLog sbdJobStatusLog;     /**<  sbatchd job status event*/
    struct jobSwitchLog jobSwitchLog;       	/**<  Job switch event*/
    struct jobSwitchLog2 jobSwitchLog2;       	/**<  Job array switch event*/
    struct jobMoveLog jobMoveLog;       	/**<  Job move event*/
    struct queueCtrlLog queueCtrlLog;       	/**<  Queue control event*/
    struct newDebugLog  newDebugLog;       	/* New debug event*/
    struct hostCtrlLog hostCtrlLog;       	/**<  Host control event*/
    struct mbdStartLog mbdStartLog;       	/**<  mbatchd start event*/
    struct mbdDieLog mbdDieLog;       		/**<  mbatchd die event*/
    struct unfulfillLog unfulfillLog;       	/**<  Unfulfill event*/
    struct jobFinishLog jobFinishLog;       	/**<  Job finish event*/
    struct loadIndexLog loadIndexLog;       	/**<  Load index event*/
    struct migLog migLog;          /**<  Migration initiated event*/
    struct calendarLog calendarLog;       	/**<  Calendar event*/
    struct jobForwardLog jobForwardLog;       	/**<  Job forward event*/
    struct jobAcceptLog jobAcceptLog;       	/**<  Job accept event*/
    struct jobAcceptAckLog jobAcceptAckLog;     /**<  Job accept ack event*/
    struct statusAckLog statusAckLog;       	/**<  Job accepted from another 
                                                 * cluster event*/
    struct signalLog signalLog;                 /**<  Job signal event*/
    struct jobExecuteLog jobExecuteLog;       	/**<  Job execution event*/
    struct jobMsgLog jobMsgLog;       		/**<  Job message event*/
    struct jobMsgAckLog jobMsgAckLog;       	/**<  Job message ackknowledge event*/
    struct jobRequeueLog jobRequeueLog;         /**<  Job requeue event*/
    struct chkpntLog chkpntLog;                 /**<  Checkpoint event */
    struct sigactLog sigactLog;                 /**<  Signal with action event*/
    struct jobOccupyReqLog jobOccupyReqLog;	/**<  Job occupy request
                                                 * event*/
    struct jobVacatedLog jobVacatedLog;       	/**<  Job vacate event*/
    struct jobStartAcceptLog jobStartAcceptLog; /**<  Job start accept
                                                 * event*/  
    struct jobCleanLog jobCleanLog;       	/**<  Job clean event*/
    struct jobExceptionLog jobExceptionLog;     /**<  Job exception event*/
    struct jgrpNewLog jgrpNewLog;       	/**<  Job group new event*/
    struct jgrpCtrlLog jgrpCtrlLog;       	/**<  Job group Ctrl event*/
    struct jobForceRequestLog jobForceRequestLog;/**<  Job Force Request
                                                  * event*/
    struct logSwitchLog logSwitchLog;       	/**<  Event switch event*/
    struct jobModLog jobModLog;       		/**<  Job modify event*/
    struct jgrpStatusLog jgrpStatusLog;       	/**<  Job group stratus
                                                 * event*/
    struct jobAttrSetLog jobAttrSetLog;       	/**<  Job attribute
                                                 * setting event*/
    struct jobExternalMsgLog jobExternalMsgLog;	/**<  Job external message
                                                 * event*/
    struct jobChunkLog jobChunkLog;       	/**<  Job chunk event*/
    struct sbdUnreportedStatusLog sbdUnreportedStatusLog; /**< sbatchd 
                                                           * unreported status
                                                           * event*/
    struct rsvFinishLog rsvFinishLog;       	/**<  Reservation finish
                                                 * event*/
    struct hgCtrlLog hgCtrlLog;                 /**<  Host group control Log*/
    struct cpuProfileLog cpuProfileLog;       	/**<  cpu profile event*/
    struct dataLoggingLog dataLoggingLog;       /**<  Data logging event*/
    struct jobRunRusageLog   jobRunRusageLog; 	/**<  Job run rusage event*/
    struct eventEOSLog       eventEOSLog;       /**<  Event EOS event*/
    struct slaLog            slaLog;            /**<  SLA event*/
    struct perfmonLog	     perfmonLog;       	/**<  Performance event*/
    struct taskFinishLog     taskFinishLog;     /**<  Task finish event*/
    struct jobResizeNotifyStartLog    jobResizeNotifyStartLog;	/**<  Job
                                                                 * resize notify
                                                                 * start event*/
    struct jobResizeNotifyAcceptLog   jobResizeNotifyAcceptLog;	/**<  Job
                                                                 * resize notify
                                                                 * accept event*/
    struct jobResizeNotifyDoneLog jobResizeNotifyDoneLog;/**<  Job resize
                                                          * notify done event*/
    struct jobResizeReleaseLog jobResizeReleaseLog;	/**<  Job resize
                                                         * release event*/
    struct jobResizeCancelLog jobResizeCancelLog;	/**<  Job resize
                                                         * cancel event*/
    struct jobResizeLog  jobResizeLog;       	/**<  Job resize
	                                                 * event*/
#if defined(LSF_SIMULATOR)
    struct jobArrayElementLog jobArrayElementLog;  /**< Job array element event */
    struct mbdSimStatusLog   mbdSimStatusLog;      /**< LSF simulator status event */
#endif
    struct jobFinish2Log jobFinish2Log;       	/**<  JOB_FINISH2 stream event*/
    struct jobStartLimitLog jobStartLimitLog;   /**<  JOB_STATRTLIMIT stream event*/
    struct jobStatus2Log jobStatus2Log;       	/**<  JOB_STATUS2 status event*/
    struct jobPendingReasonsLog jobPendingReasonsLog; /**< JOB_PENDINGREASONS event*/
    struct jobProvisionStartLog jobProvisionStartLog; /**< Job provisioning start event */
    struct jobProvisionEndLog jobProvisionEndLog; /** < Job provisioning end event */
    struct jobFanoutInfoLog jobFanoutInfoLog; /**< Job fan-out Information event */
};


/**
 * \brief  event records.
 */
struct eventRec {
    char   version[MAX_VERSION_LEN];    /**< The mbatchd version number */
    int    type;       	                /**< Event type in \ref event_types*/
    time_t eventTime;       	        /**< The time the event occurred */
    union  eventLog eventLog;           /**< The information for this type of
                                         * event, contained in a structure 
                                         * corresponding to type */
};

struct eventLogFile {
    char eventDir[MAXFILENAMELEN];  /* event file directory */
    time_t beginTime, endTime;      /* start and end event time */
};

struct eventLogHandle {
    FILE *fp;                           /* open event file pointer */
    char openEventFile[MAXFILENAMELEN]; /* current open events file name */
    int curOpenFile;                    /* current open event file number */
    int lastOpenFile;                   /* last open event file number, 0
       		   means lsb.events */
};


#define LSF_JOBIDINDEX_FILENAME  "lsb.events.index"
#define LSF_JOBIDINDEX_FILETAG   "#LSF_JOBID_INDEX_FILE"

/* structures used to handle jobId index file */
struct jobIdIndexS {
    char fileName[MAXFILENAMELEN]; /* the index file name */
    FILE *fp;                   /* open index file pointer */
    float version;              /* version number for future use */
    int totalRows;		/* total number of rows(files) indices */
    time_t lastUpdate;          /* last update time */
    int curRow;                 /* current rows */
       	/* the event file currently handled is */
       	/* (totalRows - curRow + 1)         */
    time_t timeStamp;           /* time stamp of current row */
    LS_LONG_INT minJobId;       /* min jobId in that row */
    LS_LONG_INT maxJobId;       /* max jobId in that row */
    int totalJobIds;            /* total number of jobIds */
    int *jobIds;                /* jobId array of current row */
};

/* structures used to hold one element of sorted int list */
struct sortIntList {
    int value;
    struct sortIntList *forw;      /* points to next element */
    struct sortIntList *back;      /* points to prior element */
};

struct nqsStatusReq {
#ifdef PS_SXNQS
    /* It is expected that general NQS integration will not work
     * with jobArray after 4.0. However, we may revisit this problem
     * in the future for mainline.
  */
    LS_LONG_INT jobId;
#else
    int jobId;
#endif
    int opCode;
    int reportCode;
    char *nqsQueue;
    int  fromUid;
    char *fromUserName;
    char *fromHostName;
    int    idx;
};

struct nqsStatusReply {
    char *orgHost;
    char *orgUser;
    time_t startTime;
    char *jobName;
    char *nqsQueue;
    char *lsbManager;
    int options;
    char *outFile;
    char *errFile;
};

/*
 * SBD uses the following data structure to communicate with 
 * the resource manager.
 *
 */
#define LSB_MAX_SD_LENGTH 128
struct lsbMsgHdr {
    int                usrId;
    LS_LONG_INT        jobId;
    int                msgId;
    int                type;
    char               *src;
    char               *dest;
};

struct lsbMsg {
    struct lsbMsgHdr * header;
    char *             msg;
};

struct lsbJobRusage {
    struct lsbMsgHdr *     header;
    struct jobRusagePack * rusage;
};

/* data structures related to API_CONF */

#define CONF_NO_CHECK    	0x00
#define CONF_CHECK      	0x01
#define CONF_EXPAND     	0X02
#define CONF_RETURN_HOSTSPEC    0X04
#define CONF_NO_EXPAND    	0X08
#define CONF_HAS_CU       	0X10
#define CONF_ERROR_HANDLE   	0X20  /*Live Reconfig Flag, used in fairshare parsing error handling*/
#define CONF_SYNTAX_CHECK   	0X40  
#define CONF_IGNORE_OTHERS    0X80
#define CONF_REJECT_NULLSHARE   0X100 /* Live Reconfig Flag, reject add a null fairshare assignment  */
#define CONF_NO_LOG   0X200 
#define CONF_AT_BACK   0X400  /* Live Reconfig Flag, remove others@ and default@ in queue fairshare  */
#define CONF_RMSHARE 0X800  /* Live Reconfig Flag,   remove user shares*/
#define CONF_DUPLICATE_SECTION 0X1000
#define CONF_IGNORE_CHECK 0X2000
#define CONF_XL_CHECK 0X4000 /* check with XL mode */

struct paramConf {
    struct parameterInfo *param;
};

struct userConf {
    int         numUgroups;
    struct groupInfoEnt *ugroups;
    int         numUsers;
    struct userInfoEnt *users;
    int         numUserEquivalent;
    struct userEquivalentInfoEnt *userEquivalent;
    int         numUserMapping;
    struct userMappingInfoEnt *userMapping;
};

struct hostConf {
    int       numHosts;
    struct hostInfoEnt *hosts;
    int       numHparts;
    struct hostPartInfoEnt *hparts;
    int       numHgroups;
    struct groupInfoEnt *hgroups;
};

/**
 *  \brief  lsb shared resource Instance. 
 */
typedef struct lsbSharedResourceInstance {
    char *totalValue;            /**< Value used by mbatchd */
    char *rsvValue;            	 /**< Reserved value */
    int  nHosts;               	 /**< Number of Hosts associated with the resource. */
    char **hostList;             /**< Hosts list*/
} LSB_SHARED_RESOURCE_INST_T;

/**
 *  \brief lsb shared resource information. 
 */
typedef struct lsbSharedResourceInfo {
    char *resourceName;                      /**< Resource name*/
    int  nInstances;                         /**< Number of instances*/
    LSB_SHARED_RESOURCE_INST_T  *instances;  /**< List of instances*/
} LSB_SHARED_RESOURCE_INFO_T;

struct queueConf {
    int       numQueues;
    struct queueInfoEnt *queues;
};

/**
 *  \brief  frame element information. 
 */
struct frameElementInfo {
    int  jobindex;   /**<  The job index in the frame job array.*/
    int  jobState;   /**<  The job status. */
    int  start;      /**<  The start frame of this frame job. */
    int  end;        /**<  The end frame of this frame job. */
    int  step;       /**<  The step of this frame job.*/
    int  chunk;      /**<  The chunk size of this frame job.*/
};

/**
 *  \brief  frame job Infomation.
 */
struct frameJobInfo {                 
    LS_LONG_INT  jobGid;               /**< The job ID that the LSF system
                                        * assigned to the frame job array. */
    int  maxJob;                       /**< The max job number in one frame
                                        * job array. */
    char userName[MAX_LSB_NAME_LEN];   /**< The user submitted the frame job
                                        * array.  */
    char jobName[MAXLINELEN];          /**< Full job name */
    struct frameElementInfo *frameElementPtr; 
                     /**< The full job name of the frame job array. 
                      * frameElementPtr The pointer to frame ob array table. */
};

#ifdef PS_SXNQS
struct nqsRusageReq {
    LS_LONG_INT jobId;
    int mem;
    float cpuTime;
};

struct nqsRusageReply {
    int status;
};
#endif

/* end of data structures related to API_CONF */

/*
 * Structure used for the Advance Reservation API
 *
 * MBD allows the LSF administration to make advance reservation on 
 * behalf of a user, group or or for system maintenance purposes. 
 * Clients can add a reservation, remove a reservation and show 
 * reservation statuses.  The following data structures are used to
 * encapsulate these requests
 *
 *    addRsvRequest: to add a reservation
 *    rmRsvRequest:  to remove a reservation
 *    rsvInfoEnt:    to display reservation information
 * 
 */
 
typedef struct _rsvEventInfo_prePost_t {
    int shift;
} RsvEventInfo_prePost_t;

#define RSV_EXECEVENTTYPE_PRE   1
#define RSV_EXECEVENTTYPE_POST  2

#define RSV_EXECEVENTNAME_PRE   "pre"
#define RSV_EXECEVENTNAME_POST  "post"

 /**
  * \brief  reservation excution event
  */
typedef struct _rsvExecEvent_t {
    int     type;           /**< Event type */
    int     infoAttached;   /**< Boolean: is there additional info? */
    void    *info;          /**< Info pertaining to event, such as offset */    
} rsvExecEvent_t;
 
 /**
  * \brief  reservation excution command
  */
typedef struct _rsvExecCmd_t {
    char            *path;       /**< Full path to the command name */
    int             numEvents;   /**< Size of events array */
    rsvExecEvent_t  *events;     /**< Array of events that trigger -exec
                                  * command */
} rsvExecCmd_t;

/**
 * \addtogroup reservation_option reservation_option
 *   definitions of reservation options.
 */
/*@{*/
#define RSV_OPTION_USER              0x0001   /**<  User */
#define RSV_OPTION_GROUP             0x0002   /**<  Group*/
#define RSV_OPTION_SYSTEM            0x0004   /**<  System */
#define RSV_OPTION_RECUR             0x0008   /**<  Recur */
#define RSV_OPTION_RESREQ            0x0010   /**<  Resource requirement */
#define RSV_OPTION_HOST              0x0020   /**<  Host */
#define RSV_OPTION_OPEN              0x0040   /**<  Open */
#define RSV_OPTION_DELETE            0x0080   /**<  Delete */
#define RSV_OPTION_CLOSED            0x0100   /**<  Close */
#define RSV_OPTION_EXEC              0x0200   /**<  Execute */
#define RSV_OPTION_RMEXEC            0x0400   /**<  Remote execute */
#define RSV_OPTION_NEXTINSTANCE      0x0800   /**<  Next instance */
#define RSV_OPTION_DISABLE           0x1000   /**<  Disable */
#define RSV_OPTION_ADDHOST           0x2000   /**<  Add host */
#define RSV_OPTION_RMHOST            0x4000   /**<  Remote host */
#define RSV_OPTION_DESCRIPTION       0x8000   /**<  Description */
#define RSV_OPTION_TWMOD             0x10000  /**<  Timewindow mode */
#define RSV_OPTION_SWITCHOPENCLOSE   0x20000  /**<  Switch open/close */
#define RSV_OPTION_USERMOD           0x40000  /**<  User mode */
#define RSV_OPTION_RSVNAME           0x80000  /**<  Reservation name */
#define RSV_OPTION_EXPIRED           0x100000 /**<  Expired */
/*@}*/

/**
 * \brief add reservation request.
 */
struct addRsvRequest {
   int     options;	       /**<Reservation options \ref reservation_option */
    char    *name;             /**< User or group for which the reservation is made */
    struct {
	int     minNumProcs;   /**< Minimum number of processors the required
	                        * to run the job. See the -g option of brsvadd. */
	int     maxNumProcs;   /**< Maximum number of processors the required
	                        * to run the job. */
    } procRange;               /**< Range of number of processors */
    int     numAskedHosts;     /**< The number of invoker specified hosts for
                                * the reservation. If numAskedHosts is 0,
       	                        * all qualified hosts will be considered. */
    char    **askedHosts;      /**< The array of names of invoker specified
                                * hosts hosts for the reservation. The number
       	                        * of hosts is given by numAskedHosts. See
       	                        * the -m option of brsvadd. */
    char    *resReq;           /**< The resource requirements of the reservation.
                                * See the -R option of brsvadd. */
    char    *timeWindow;       /**< Active time window for a recurring reservation.
                                * See the -t option of brsvadd. */
    rsvExecCmd_t  *execCmd;    /**< Info for the -exec option. */
    char   *desc;              /**< Description for the reservation to be created.
                                * The description must be provided as a
		       	        * double quoted text string. The maximum length 
		       	        * is 512 chars. 
		       	        * Equivalent to the value of brsvadd -d.*/
    char    *rsvName;          /**< User-defined advance reservation name unique
                                * in an LSF cluster. The name is a string of
                                * letters, numeric chars, underscores, and
                                * dashes beginning with a letter. The maximum
                                * length of the name is 39 chars. Equivalent
                                * to the value of brsvadd -N. */
};

/**
 * \brief  remove reservation request.
 */
struct rmRsvRequest {
    char    *rsvId;    /**< Reservation ID of the reservation that you wish
                        * to remove. */
};

/**
 * \brief  modifiy reservation request
 */ 
struct modRsvRequest {
   char    *rsvId;         /**< Reservation ID of the reservation that you 
                            * wish to modify. */   
   struct   addRsvRequest   fieldsFromAddReq;  
                           /**< LSF user name for the reservation. 
                            * See the -g option of brsvadd. .*/
   char     *disabledDuration;     /**< Disabled time duration */
};

/**
 * \brief  host reservation infromation entry.
 */
struct hostRsvInfoEnt {
    char    *host;           /**< Host name.*/
    int     numCPUs;         /**< Number of CPUs reserved on the host.*/
    int     numSlots;        /**< Number of job slots reserved on the host.*/
    int     numRsvProcs;     /**< Number of processors reserved on the host.*/ 
    int     numusedRsvProcs; /**< Count for used + suspended from reserved
                              * slots*/ 
    int     numUsedProcs;    /**< Number of processors in use on the host.*/
};

/**
 * \brief  reservation information entry.
 */
struct rsvInfoEnt {
    int     options;      /**< Reservation options, see \ref reservation_option */
    char    *rsvId;       /**< Reservation ID returned from mbatchd. If the
                           * reservation fails, this is NULL. The memory for
                           * rsvid is allocated by the caller.*/
    char    *name;        /**<  LSF user group name for the reservation.
                           * See the -g option of brsvadd. */
    int     numRsvHosts;  /**<  Number of hosts reserved */
    struct hostRsvInfoEnt   *rsvHosts;   /**<  Info about the reserved hosts */
    char                    *timeWindow; /**< Active time window for a recurring
                                          * reservation. See the -t option of
                                          *  brsvadd.*/
    int                     numRsvJobs;  /**< Number of jobs running in the
                                          * reservation.*/
    LS_LONG_INT             *jobIds;     /**< Job IDs of jobs running in the
                                          * reservation.*/
    int                     *jobStatus;  /**< Status of jobs running in the
                                          * reservation.*/
    char                    *desc;       /**<  Description for the reservation
                                          * to be created. The description must
                                          * be provided as a double quoted text
                                          * string. The maximum length is 512
                                          * chars. Equivalent to the
		                     	  *value of brsvadd -d. */
    char              **disabledDurations; /**<  Null-terminated list of
                                            * disabled durations */
    int                     state;         /**<  The current state of the
                                            * reservation - active or inactive. */
    char                    *nextInstance; /**<  The time of the next instance
                                            * of a recurring reservation. */
    char   *creator;                       /**<  Creator of the reservation. */
};

/* backfill window related data structures and functions */
struct slotInfoRequest {
 /* options mask */
#define SLOT_OPTION_RESREQ      0X001   /* Option -R */

    int     options;               
    char    *resReq;               /* Resource requirement string */
};


struct GlobalPolicyInfoRequest {
    char *name;
    char *message;
};

/*copy from struct SRInfo*/
struct SRInfoEnt {
    int       numReserved; /*number of reserved slots*/
    time_t  predictedStartTime;/* job's predicted start time */
};

struct hostSRInfoEnt {
    char *host;
    int hStatus;
    int userJobLimit;
    int maxJobs;
    int numJobs;
    int numRUN;
    int numSSUSP;
    int numUSUSP;
    int numRESERVE;
    int numSR;
    struct SRInfoEnt *SRInfo;
    int numRELEASE;
};

typedef struct rsvInfoEnt ARInfoEnt;

struct slotInfoReply {
    time_t masterTime; /* to store the time of Master host */
    int numHosts;
    struct hostSRInfoEnt *hostInfo;
    int numAR;    
    ARInfoEnt *ARInfo;
};


/* the general limit related data structures and functions */


#define LSB_RSRC_LIMIT_TYPE_SLOTS         	0
#define LSB_RSRC_LIMIT_TYPE_SLOT_PERPSR    	1
#define LSB_RSRC_LIMIT_TYPE_MEM           	2
#define LSB_RSRC_LIMIT_TYPE_MEM_PERCENT  	3
#define LSB_RSRC_LIMIT_TYPE_SWP           	4
#define LSB_RSRC_LIMIT_TYPE_SWP_PERCENT   	5
#define LSB_RSRC_LIMIT_TYPE_TMP           	6
#define LSB_RSRC_LIMIT_TYPE_TMP_PERCENT   	7
#define LSB_RSRC_LIMIT_TYPE_JOBS           	8
#define LSB_RSRC_LIMIT_TYPE_EXT_RSRC      	9   /* all external resources */
#define LSB_RSRC_LIMIT_TYPE_FWD_SLOTS          10

/**
 * \addtogroup _consumertype _consumertype
 *  consumer types
 */
/*@{*/
typedef enum _consumertype {
    LIMIT_QUEUES        = 1,   /**< Queues */
    LIMIT_PER_QUEUE     = 2,   /**< Per-queue */
    LIMIT_USERS         = 3,   /**< Users */
    LIMIT_PER_USER      = 4,   /**< Per-users*/
    LIMIT_HOSTS         = 5,   /**< Hosts */
    LIMIT_PER_HOST      = 6,   /**< Per-host */
    LIMIT_PROJECTS      = 7,   /**< Projects*/
    LIMIT_PER_PROJECT   = 8,    /**< Per-project*/
    LIMIT_LIC_PROJECTS      = 9,   /**< License Projects*/
    LIMIT_PER_LIC_PROJECT   = 10,   /**< Per-License project*/
    LIMIT_CLUSTERS      = 11, /**< Clusters*/
    LIMIT_PER_CLUSTER   = 12 /**< Per-Cluster*/
} consumerType;                /**< Type definitions */
 /*@}*/

/**
 * \brief  limit consumer 
 */
typedef struct _limitConsumer {
    consumerType   type;   /**< Consumer type ( _consumertype ): 
                            * -  Queues per-queue
                            * -  Users and per-user
                            * -  Hosts and per-host
                            * -  Projects and per-project
                            * -  License Projects and per-License Project
                            */
    char           *name;  /**< Consumer name */
} limitConsumer;

/**
 * \brief  limit resource.
 */
typedef struct _limitResource {
    char	*name;      /**< Resource name*/
    int		type;       /**< Resource type*/
    float	val;        /**< Resource val*/
} limitResource;

/**
 * \brief   limit information request
 */
typedef struct _limitInfoReq {
    char            *name;       /**< Limit policy name given by the user. */
    int             consumerC;   /**< Number of consumers*/
    limitConsumer   *consumerV;  /**< Consumer name, queue/host/user/project */
}limitInfoReq;

/** 
 * \brief  limit item.
 */
typedef struct _limitItem {    
    int             consumerC;   /**< Number of consumers*/
    limitConsumer   *consumerV;  /**< Consumers, such as queue, host, user or project */
    int             resourceC;   /**< Number of resources*/  
    limitResource   *resourceV;  /**< Resources list*/
} limitItem;

/**
 * \brief  limit information entry .
 */
typedef struct _limitInfoEnt {
    char        *name;      /**< Limit policy name given by the user */
    limitItem   confInfo;   /**< Limit configuration */
    int         usageC;     /**< Size of limit dynamic usage info array */
    limitItem   *usageInfo; /**< Limit dynamic usage info array */

} limitInfoEnt;

/* action code for threshold based on type/model, is used for
 * predefinedThresholdTypeModel().
 */

#define ADD_THRESHOLD   1
#define GET_THRESHOLD   2
#define DEL_THRESHOLD   3
#define UPD_THRESHOLD   4
#define CHK_THRESHOLD   5

/* Structure to hold thresholds defined based on host's type/model */

struct thresholdEntry{
    
    char                 *attr;          /* Name of type or model */
    struct hostInfoEnt   *hostEntryPtr;  /* Pointer to hostInfo */
};    
/**
 * \page lsb_limitInfo lsb_limitInfo
 * \brief gets resource allocation limit configuration and dynamic usage 
 * information.
 *
 * Displays current usage of resource allocation limits configured in Limit
 * sections in lsb.resources:
 * \li    Configured limit policy name
 * \li    Users
 * \li    Queues
 * \li    Hosts
 * \li    Project names
 *
 * <b>\#include <lsf/lsbatch.h>
 * 
 * int lsb_limitInfo( limitInfoReq *req,  limitInfoEnt **limitItemRef, 
 *                int *size, struct lsInfo* lsInfo)</b>
 *  
 * @param *req input, the user request for limit information
 * @param **limitItemRef output, the limit information array
 * @param *size output, the size of the limit information array
 * @param *lsInfo Please refer to the \ref lsInfo structure. 
 *
 * <b>Data Structures:</b>
 * \par
 * _limitInfoReq
 * \n _limitConsumer
 * \n _limitInfoEnt
 * \n _limitItem
 * \n _limitResource
 *
 * <b>Define Statements:</b>
 * \par
 * \ref _consumertype
 *
 * @return int:socket number
 * \n Function was successful.
 * @return int:-1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * blimits
 *
 * \b Files
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.queues \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.users \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.hosts \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.resources
 *
 * @see \ref lsb_freeLimitInfoEnt 
 */
extern int lsb_limitInfo( limitInfoReq *req,  limitInfoEnt **limitItemRef, int *size, struct lsInfo* lsInfo);
/**
 * \page lsb_freeLimitInfoEnt lsb_freeLimitInfoEnt
 * \brief Frees the memory allocated by \ref lsb_limitInfo.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * void lsb_freeLimitInfoEnt(limitInfoEnt * ent, int size)</b>
 *
 * @param *ent input, the array of limit information
 * @param size input, the size of the limit information array
 *
 * <b>Data Structures:</b>
 * \par
 * _limitInfoEnt
 * \n _limitItem
 * \n _limitConsumer
 * \n _limitResource
 *
 * <b>Define Statements:</b>
 * \par
 * \ref _consumertype
 *
 * @return void
 * \n There's no return value.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * blimits
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.queues \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.users \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.hosts \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.resources
 *
 * @see \ref lsb_limitInfo 
 */

extern void lsb_freeLimitInfoEnt( limitInfoEnt* item, int size);

/**
 * \addtogroup resizablejob_related resizablejob_related
 * Resizable job related definitions.
 */
/*@{*/
#define LSB_RESIZE_REL_NONE         0x0   /**< Means release no slots*/
#define LSB_RESIZE_REL_ALL          0x01  /**< Means release all slots-In
                                           * this case, nHosts, hosts and slots 
                            		   * indicate the slots that are not
                            		   * released*/
#define LSB_RESIZE_REL_CANCEL       0x02  /**< Means cancel any pending resize
                                           * request*/
#define LSB_RESIZE_REL_NO_NOTIFY    0x04  /**< Means execute no resize
                                           * notification command*/
 /*@}*/

/**
 * \brief  job resize release.
 */
struct job_resize_release {
    LS_LONG_INT jobId;       /**< LSF job ID*/
    int         options;     /**< Options is constructed from the bitwise
                              * inclusive OR of zero or more of the flags, as
                              * defined in \ref resizablejob_related .*/
    int         nHosts;      /**< Number of hosts in the hosts list, if no
                              * hosts are to be specified this should be zero*/
    char        **hosts;     /**< Specified hosts list, nHosts number of elements*/
    int         *slots;      /**< Slots list, each element specifies the number of
                              * slots per corresponding host (0 implies all),
                              * nHosts number of elements*/
    char        *notifyCmd;  /**< Name and location of notification command*/
};

struct job_resize_request {
    LS_LONG_INT jobId;
    int         options;
    int         nHosts;             /* array size */
    char        **hosts;            /* array of hosts */
    char        *notifyCmd;         /* notifocation command */
};

/*
 * End of resizable job related definitions
 */

/* Job Dependency Display */


/* Job Dependency Display */
/* for options */
/**
 * \addtogroup query_depend query_depend
 * Job Dependency Display for options
 */
/*@{*/
#define QUERY_DEPEND_RECURSIVELY       0x1  /**<  Recursively */
#define QUERY_DEPEND_DETAIL            0x2  /**<  Detail */
#define QUERY_DEPEND_UNSATISFIED       0x4  /**<  Unsatisfied */
#define QUERY_DEPEND_CHILD             0x8  /**<  Child */
/*@}*/
/**
 * \brief  job dependent request.
 */
struct jobDepRequest{
   LS_LONG_INT     jobId;   /**< Job ID of the queried job or job array.*/
   int             options; /**< You can set the following bits into this field:
		            *\n QUERY_DEPEND_RECURSIVELY
		            *\n Query the dependency information recursively.
		            *\n QUERY_DEPEND_DETAIL
		            *\n Query the detailed dependency information.
		            *\n QUERY_DEPEND_UNSATISFIED
		            *\n Query the jobs that cause this job pend.
		            *\n QUERY_DEPEND_CHILD
		            *\n Query child jobs.*/
   int             level;  /**< The level when you set QUERY_DEPEND_RECURSIVELY.*/
};


/**
 * \brief  queried jobs.
 */
struct queriedJobs {
    LS_LONG_INT         jobId;           /**< Job ID of the queried job or
                                          * job array. */
    char*               dependcondition; /**< The whole dependency condition
                                          * of the job. */
    int                 satisfied;       /**< Whether the condition is
                                          * satisfied. */
};

/* for hasDependency */
/**
 * \addtogroup job_has_depend job_has_depend
 * options for hasDependency 
 */
/*@{*/
#define  JOB_HAS_DEPENDENCY  0x1            /**<  Job has dependency*/
#define  JOB_HAS_INDIVIDUAL_CONDITION  0x2  /**<  Job has individual 
                                             * condition. */
/*@}*/
/**
 * \brief  dependency jobs.
 */
struct dependJobs{
    LS_LONG_INT    jobId;          /**< Job ID. By default, it is the parent
                                    * job of the queried job. Modify to child
                                    * job by setting QUERY_DEPEND_CHILD in
                                    * options of JobDepRequest. */
    char           *jobname;       /**< The job name associated with the job
                                    * ID. */
    int            level;          /**< The number of degrees of separation
                                    * from the original job.*/     
    int            jobstatus;      /**< Job status of the job. */
    int            hasDependency;  /**< Whether the job ID has a dependency or
                                    * not. When you set QUERY_DEPEND_RECURSIVELY
                                    * in options of JobDepRequest, 0 indicates
                                    * job ID does not have a dependency. Otherwise,
                                    * one or more of the following bits displays:
                                    *-  JOB_HAS_DEPENDENCY: Job has a dependency.
                                    *-  JOB_HAS_INDIVIDUAL_CONDITION: Job has
                                    * an individual dependency
                                    * condition when it is an element of job
                                    * array. */
    char           *condition;     /**< When you set "QUERY_DEPEND_DETAIL"
                                    * into options, it is dependency
                                    * condition of jobId. It is "" when you
                                    * do not set "QUERY_DEPEND_DETAIL". */
    int            satisfied;      /**< Whether the condition is satisfied. */
    LS_LONG_INT    depJobId;       /**< Job ID. By default, it is the child
                                    * job. Modify to parent job by setting
                                    * QUERY_DEPEND_CHILD in options of
                                    * JobDepRequest*/
};

/**
 * \brief  job dependent information.
 */

struct jobDependInfo {
    int                 options;        
               /**< You can set the following bits into this field:
       		*\n QUERY_DEPEND_RECURSIVELY
       		*\n Query the dependency information recursively.
       		*\n QUERY_DEPEND_DETAIL
       		*\n Query the detailed dependency information.
       		*\n QUERY_DEPEND_UNSATISFIED
       		*\n Query the jobs that cause this job pend.
       		*\n QUERY_DEPEND_CHILD
       		*\n Query child jobs.*/
    int                 numQueriedJobs; 
               /**< The number of jobs you queried. By default, the value
                * is 1. However, when you set QUERY_DEPEND_DETAIL in the
                * options and you query a job array where some elements
                * have a dependency condition that has changed, the value
                * is the number of the changed element + 1.*/
    struct queriedJobs  *queriedJobs;   /**< The jobs you queried.*/
    int                 level;          /**< The number of levels returned.*/
    int                 numJobs;        /**< The number of jobs returned.*/
    struct dependJobs   *depJobs;       /**< The returned dependency jobs.*/
};

extern int lsb_GridBroker_limitInfo( limitInfoReq *req, char **resourceNames, limitInfoEnt ***entRef, 
                                    int **size, struct lsInfo **lsInfo, 
                                    int* numCluster, char ***clusterNames, int *reqClusterIndex, 
                                    int bConfigInfo);
extern void freeHgrpInfo4GridBroker();

/*
 * Functional prototypes of the Advance Reservation API
 */


/* Macros */
#define  IS_PEND(s)  (((s) & JOB_STAT_PEND) || ((s) & JOB_STAT_PSUSP))

/* Do not test JOB_STAT_UNKWN in IS_START() */
#define  IS_START(s)  (((s) & JOB_STAT_RUN) || ((s) & JOB_STAT_SSUSP) \
		       || ((s) & JOB_STAT_USUSP))

#define  IS_FINISH(s) (((s) & JOB_STAT_DONE) || ((s) & JOB_STAT_EXIT))

#define  IS_SUSP(s) (((s) & JOB_STAT_PSUSP) || ((s) & JOB_STAT_SSUSP) \
                             ||  ((s) & JOB_STAT_USUSP))

/* Macro for checking post job process. (IO_SPOOL) */
#define  IS_POST_DONE(s) ( ( (s) & JOB_STAT_PDONE) == JOB_STAT_PDONE )
#define  IS_POST_ERR(s) ( ( (s) & JOB_STAT_PERR) == JOB_STAT_PERR )
#define  IS_POST_FINISH(s) ( IS_POST_DONE(s) || IS_POST_ERR(s) ) 
#define  IS_WAIT(s) ((s) & JOB_STAT_WAIT )

#define  FACTORS_CHECK(queue, Qparam, Gparam) ((((queue)->qAttrib & Q_ATTRIB_FAIRSHARE) \
		                                  && ((Qparam) > 0)) || \
		                                  (((queue)->qAttrib & Q_ATTRIB_FAIRSHARE) \
		                                  && ((Qparam) < 0) \
		                                  && ((Gparam) == TRUE)) || \
		                                  (!((queue)->qAttrib & Q_ATTRIB_FAIRSHARE) \
	                                          && ((Gparam) == TRUE)))


/*On windows ,for dll library ,need to use _declspec(dllexport) to export
*a symbol .but if do so ,static library will can not work .so we are going
*to change lsberrno to a function.
*/

#define  lsberrno (*lsb_errno())
		

/*
 * Version of the mbatchd that was last contacted.
 * -1 indicates the mbatchd has not been contacted.
 */
extern int lsb_mbd_version;

/*
 * The data definition for host name list operations
 */
#define PRINT_SHORT_NAMELIST  0x01
#define PRINT_LONG_NAMELIST   0x02
#define PRINT_MCPU_HOSTS      0x04

typedef struct nameList {
     int    listSize;  /* number of names */
     char **names;     /* a group of names */
     int   *counter;   /* the ocurrent of corresponding name */
} NAMELIST;

extern NAMELIST * lsb_parseShortStr(char *, int);
extern NAMELIST * lsb_parseLongStr(char *);
extern char * lsb_printNameList(NAMELIST *, int );
extern NAMELIST * lsb_compressStrList(char **, int );
extern NAMELIST * lsb_compressStrList2(char **, int ); /* for internal use only */
extern char * lsb_splitName(char *, unsigned int *);
extern int * lsb_errno(void);



/*
 * lsblib routines 
 */
#if defined(__STDC__) || defined(__cplusplus) || defined(WIN32)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* external routines related to API_CONF */

extern struct paramConf *lsb_readparam P_((struct lsConf *));
extern struct userConf * lsb_readuser  P_((struct lsConf *, int,
       		  struct clusterConf *));
extern struct userConf * lsb_readuser_ex P_((struct lsConf *, int,
       		     struct clusterConf *,
       		     struct sharedConf *));
extern struct hostConf *lsb_readhost P_((struct lsConf *, struct lsInfo *, int,
       		 struct clusterConf *));
extern struct queueConf *lsb_readqueue P_((struct lsConf *, struct lsInfo *, 
       		   int, struct sharedConf *, 
       		   struct clusterConf *));
extern void updateClusterConf(struct clusterConf *);

/* end of external routines related to API_CONF */
/**
 * \page lsb_hostpartinfo lsb_hostpartinfo
 * Returns informaton about host partitions.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct hostPartInfoEnt *lsb_hostpartinfo (char **hostParts,
 * int *numHostParts)</b>
 
 * @param **hostParts An array of host partition names.
 * @param *numHostHosts The number of host partition names.
 * To get information on all host partitions, set hostParts to NULL; *numHostParts
 * will be the actual number of host partitions when this call returns.
 * 
 * <b>Data Structures:</b>
 * \par
 * hostPartInfoEnt
 * \n hostPartUserInfo
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct hostPartInfoEnt *:hostPartInfoEnt 
 * \n On success, returns an array of hostPartInfoEnt structures which hold
 * information on the host partitions, and sets *numHostParts to the number of
 * hostPartInfoEnt structures.
 * @return NULL 
 * \n Function failed.
 *
 * <b>Errors:</b>
 * \par
 * If the function fails, lsberrno is set to indicate the error. If lsberrno is
 * LSBE_BAD_HPART, (*hostParts)[*numHostParts] is not a host partition known
 * to the LSF system. Otherwise, if *numHostParts is less than its original value,
 * *numHostParts is the actual number of host partitions found.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.hosts
 *
 * @see \ref lsb_usergrpinfo 
 * @see \ref lsb_hostgrpinfo 
 */
extern struct hostPartInfoEnt *lsb_hostpartinfo P_((char **, int *)); 
/**
 * \page lsb_init lsb_init
 * \brief Initializes the LSF batch library (LSBLIB), and gets the
 * configuration environment.
 *
 * You must use \ref lsb_init before any other LSBLIB library routine in your
 * application.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_init(char *appname)</b>
 *
 * @param  *appName The name of your application.
 * If appName holds the name of your application, a logfile with the same
 * name as
 * your application receives LSBLIB transaction information.
 * If appName is NULL, the logfile $LSF_LOGDIR/bcmd receives LSBLIB
 * transaction information.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * <b>Errors:</b>
 * \par 
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see none
 */
extern int lsb_init P_((char *appName));
extern int sch_lsb_init(void);
/**
 * \page lsb_openjobinfo lsb_openjobinfo
 * \brief Returns the number of jobs in the master batch daemon.
 *
 * \ref lsb_openjobinfo accesses information about pending, running and
 * suspended jobs in the master batch daemon. Use \ref lsb_openjobinfo to
 * create a connection to the master batch daemon. Next, use \ref lsb_readjobinfo
 * to read job records.Close the connection using \ref lsb_closejobinfo.
 *
 * \ref lsb_openjobinfo opens a connection with mbatchd and returns the total
 * number of records in the connection on success.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_openjobinfo(LS_LONG_INT jobId, char *jobName,
 *                     char *userName, char *queueName, char *hostName,
 *                      int options)</b>
 *
 * @param  jobId Passes information about jobs with the given job ID. 
 * If jobId is 0, \ref lsb_openjobinfo looks to another parameter to return
 * information about jobs.If a member of a job array is to be passed, use
 * the array form jobID[ i ] where jobID is the job array name, and i is
 * the index value.
 * @param  *jobName Passes information about jobs with the given job name.
 * If jobName is NULL, \ref lsb_openjobinfo looks to another parameter to return
 * information about jobs.
 * @param  *userName Passes information about jobs submitted by the named user
 * or user group, or by all users if user is all. If user is NULL, 
 * \ref lsb_openjobinfo assumes the user is invoking this call.
 * @param  *queueName Passes information about jobs belonging to the named 
 * queue. If queue is NULL,jobs in all the queues of the batch system are counted.
 * @param  *hostName Passes information about jobs on the named host, host
 * group or cluster name. If host is NULL, jobs on all hosts of the batch
 * system will be considered.
 * @param  options <lsf/lsbatch.h> defines the flags shown in
 * \ref defs_lsb_openjobinfo constructed from bits. Use the bitwise OR to set more
 * than one flag.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * \ref defs_lsb_openjobinfo_a
 * \n \ref defs_lsb_openjobinfo             
 * 
 * @return int:value \n
 * The total number of records in the connection.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 *                  bjobs
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see   \ref               lsb_openjobinfo_a  
 * @see   \ref               lsb_openjobinfo_a_ext 
 * @see   \ref               lsb_openjobinfo_req  
 * @see   \ref               lsb_closejobinfo  
 * @see   \ref               lsb_readjobinfo  
 * @see   \ref               lsb_readframejob  
 */
extern int lsb_openjobinfo P_((LS_LONG_INT, char *, char *, char *, char *, 
              int));
/**
 * \page lsb_openjobinfo_a lsb_openjobinfo_a
 * \brief Provides the name and number of jobs and hosts in the master batch
 * daemon.
 *
 * \ref lsb_openjobinfo_a provides more information on pending, running and
 * suspended jobs than \ref lsb_openjobinfo. Use \ref lsb_openjobinfo_a to create a
 * connection to the master batch daemon. Next, use \ref lsb_readjobinfo to read
 * job records. Close the connection using \ref lsb_closejobinfo.
 *
 * \ref lsb_openjobinfo_a passes information about jobs based on the value of
 * jobId,jobName, userName, queueName, or hostName. Only one parameter can be
 * chosen. The other parameters must be NULL or 0.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct jobInfoHead *lsb_openjobinfo_a(LS_LONG_INT jobId,
 *                                      char *jobName,
 *                                      char *userName,
 *                                      char *queueName,
 *                                      char *hostName,
 *                                        int options)</b>
 *
 *
 * @param jobId Passes information about jobs with the given job ID. If jobId
 * is 0, \ref lsb_openjobinfo looks to another parameter to return information
 * about jobs.
 * If information about a member of a job array is to be passed, use the array
 * form jobID[ i ] where jobID is the job array name, and i is the index value.
 * @param *jobName Passes information about jobs with the given job name. If
 * jobName is NULL, \ref lsb_openjobinfo looks to another parameter to return
 * information about jobs.
 * @param *userName Passes information about jobs submitted by the named user
 * or user group, or by all users if userName is all. If userName is NULL,
 * \ref lsb_openjobinfo_a assumes the user is invoking this call.
 * @param *queueName Passes information about jobs belonging to the named queue.
 * If queueName is NULL, jobs in all queues of the batch system will be
 * considered.
 * @param *hostName Passes information about jobs on the named host, host group
 * or cluster name. If hostName is NULL, jobs on all hosts of the batch system
 * will be considered.
 * @param options <lsf/lsbatch.h> defines the flags shown in def_lsb_openjobinfo_a
 * constructed from bits. Use the bitwise OR to set more than one flag.
 *
 * <b>Data Structures:</b>
 * \par
 * jobInfoHead
 *
 * <b>Define Statements:</b>
 * \par
 * \ref defs_lsb_openjobinfo_a
 * 
 * @return struct jobInfoHead * \n
 * On success, returns an array of data type struct jobInfoHeadExt *, which
 * represents the name and number of jobs and hosts in the master batch daemon
 * with the host group information.
 * @return NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * bjobs
 * 
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_openjobinfo  
 * @see \ref lsb_closejobinfo  
 * @see \ref lsb_readjobinfo  
 * @see \ref lsb_readframejob  
 *
 */
extern struct jobInfoHead *lsb_openjobinfo_a P_((LS_LONG_INT, char *,char *,
               char *, char *, int));
/**
 * \page lsb_openjobinfo_a_ext lsb_openjobinfo_a_ext
 * \brief  Returns the name and number of jobs and hosts in the master batch
 * daemon with additional host group information.
 *
 * \ref lsb_openjobinfo_a_ext is run from \ref lsb_openjobinfo_a using the same
 * parameters and provides the same information as \ref lsb_openjobinfo_a, but with
 * additional host group information.
 *
 * \ref lsb_openjobinfo_a_ext passes information about jobs based on the value of
 * jobId, jobName, userName, queueName, or hostName. Only one parameter can be
 * chosen. The other parameters must be NULL or 0.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct jobInfoHeadExt *
 * lsb_openjobinfo_a_ext (LS_LONG_INT jobId, char *jobName,
 *                                         char *userName, char *queueName,
 *                                         char *hostName, int options)</b>
 * 
 * @param jobId Passes information about jobs with the given job ID. If jobId
 * is 0, \ref lsb_openjobinfo_a_ext looks to another parameter to return information
 * about jobs. If information about a member of a job array is to be passed, use
 * the array form jobID[ i ] where jobID is the job array name, and i is the
 * index value.
 * @param *jobName Passes information about jobs with the given job name. If
 * jobName is NULL, \ref lsb_openjobinfo_a_ext looks to another parameter to return
 * information about jobs.
 * @param *userName Passes information about jobs submitted by the named user
 * or user group, or by all users if userName is all. If userName is NULL,
 * \ref lsb_openjobinfo_a_ext assumes the user is invoking this call.
 * @param *queueName Passes information about jobs belonging to the named queue.
 * If queueName is NULL, jobs in all queues of the batch system will be considered.
 * @param *hostName Passes information about jobs on the named host, host group
 * or cluster name. If hostName is NULL, jobs on all hosts of the batch system
 * will be considered.
 * @param options <lsf/lsbatch.h> defines the flags shown in
 * def_lsb_openjobinfo_a constructed from bits. Use the bitwise OR to set more
 * than one flag.
 *
 * <b>Data Structures:</b>
 * \par
 * jobInfoHeadExt
 *
 * <b>Define Statements:</b>
 * \par
 * \ref defs_lsb_openjobinfo_a
 * 
 * @return struct jobInfoHeadExt * \n
 * On success, returns an array of data type struct jobInfoHeadExt *, which
 * represents the name and number of jobs and hosts in the master batch daemon
 * with the host group information.
 * @return NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * bjobs
 * 
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_openjobinfo  
 * @see \ref lsb_closejobinfo  
 * @see \ref lsb_readjobinfo  
 * @see \ref lsb_readframejob  
 */
extern struct jobInfoHeadExt *lsb_openjobinfo_a_ext P_((LS_LONG_INT, char *,
              	char *, char *, char *,
              	int)); 
/**
 * \page lsb_openjobinfo_req lsb_openjobinfo_req
 * \brief  Extensible API. 
 *
 * Instead of submitting individual requests this API defines
 * all job info requests as objects, and can easily be enhanced to include 
 * additinal requests.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct jobInfoHeadExt *lsb_openjobinfo_req (struct jobInfoReq *req)</b>
 *
 * @param  *req  job information request.
 *
 * <b>Data Structures:</b>
 * \par
 * jobInfoReq
 * \n \ref jobInfoHeadExt
 *
 * <b>Define Statements:</b>
 * \par
 * \ref defs_lsb_openjobinfo_a
 * \n \ref defs_lsb_openjobinfo             
 * 
 * @return struct jobInfoHeadExt *:A pointer to job Information head extent.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see   \ref               lsb_openjobinfo_a  
 * @see   \ref               lsb_openjobinfo_a_ext  
 * @see   \ref               lsb_closejobinfo  
 * @see   \ref               lsb_readjobinfo  
 * @see   \ref               lsb_readframejob  
 */
extern struct jobInfoHeadExt *lsb_openjobinfo_req P_((struct jobInfoReq *)); 

extern int lsb_queryjobinfo P_((int, long *,char *));
extern int lsb_queryjobinfo_2 P_((struct jobInfoQuery *,char *));

extern struct jobInfoHeadExt *lsb_queryjobinfo_ext P_((int, long *,char *));
extern struct jobInfoHeadExt *lsb_queryjobinfo_ext_2 P_((struct jobInfoQuery *,char *));

extern struct jobInfoEnt *lsb_fetchjobinfo P_((int *, int, long *, char * ));
extern struct jobInfoEnt *lsb_fetchjobinfo_ext P_((int *, int, long *, char *, struct jobInfoHeadExt *));

#if defined(COMPAT32) && defined(INT32JOBID)
extern struct jobInfoEnt32 *lsb_readjobinfo P_((int *));
extern struct jobInfoEnt *lsb_readjobinfo40 P_((int *));
extern void submit40to32 P_((struct submit32 *, struct submit *));
extern void jobInfo40to32 P_((struct jobInfoEnt32 *, struct jobInfoEnt *));
extern LS_LONG_INT lsb_submit40 P_((struct submit*, struct submitReply*));
extern LS_LONG_INT lsb_submit P_((struct submit32*, struct submitReply*));
extern void submit32to40 P_((struct submit*, struct submit32*));
#else
/**
 * \page lsb_readjobinfo lsb_readjobinfo
 * \brief Returns the next job information record in mbatchd.
 *
 * \ref lsb_readjobinfo reads the number of records defined by the more parameter.
 * The more parameter receives its value from either \ref lsb_openjobinfo or
 * \ref lsb_openjobinfo_a. Each time \ref lsb_readjobinfo is called, it returns one
 * record from mbatchd. Use \ref lsb_readjobinfo in a loop and use more to
 * determine how many times to repeat the loop to retrieve job information records.
 *
 * <b>\#include <lsf/lsbatch.h>
 * \n \#include <time.h>
 * \n \#include <lsf/lsf.h>
 *
 * struct jobInfoEnt *lsb_readjobinfo(int *more)</b>
 * @param *more Number of job records in the master batch daemon.
 *
 * <b>Data Structures:</b>
 * \par
 * jobInfoEnt
 * \n jobExternalMsgReply 
 * \n jRusage
 * \n pidInfo
 * \n reserveItem 
 * \n submit
 *
 * <b>Define Statements:</b>
 * \par
 * \ref job_states
 * \n \ref jobgroup_counterIndex
 * \n \ref group_nodetypes
 *
 * @return struct jobInfoEnt *:A pointer to job information entry.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If there are no more records, then lsberrno is set to LSBE_EOF.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.queues
 *
 * @see \ref lsb_openjobinfo 
 * @see \ref lsb_openjobinfo_a 
 * @see \ref lsb_closejobinfo 
 * @see \ref lsb_hostinfo 
 * @see \ref lsb_pendreason 
 * @see \ref lsb_queueinfo 
 * @see \ref lsb_suspreason 
 */
extern struct jobInfoEnt *lsb_readjobinfo P_((int *));
/**
 * \page  lsb_submit lsb_submit
 * Submits or restarts a job in the batch system.
 *
 * \ref lsb_submit submits or restarts a job in the batch system according to the
 * jobSubReq specification.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * LS_LONG_INT lsb_submit (struct submit *jobSubReq,
 *                         struct submitReply *jobSubReply)</b>
 *
 * @param *jobSubReq
 * Describes the requirements for job submission to the batch system.
 * A job that does not meet these requirements is not submitted to the
 * batch system and an error is returned.
 * @param *jobSubReply
 * Describes the results of the job submission to the batch system.
 *
 * <b>Data Structures:</b>
 * \par
 * submit
 * \n submitReply
 *
 * <b>Define Statements:</b>
 * \par
 * \ref lsb_submit_options
 * \n \ref lsb_submit_options2
 * \n \ref lsb_submit_options3
 *
 * @return LS_LONG_INT:job ID \n
 * The function was successful, and sets the queue field of jobSubReply to
 * the name of the queue that the job was submitted to.
 * If the environment variable BSUB_CHK_RESREQ is set, \ref lsb_submit
 * returns a jobid less than zero (0).
 * @return LS_LONG_INT:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 * If the environment variable BSUB_CHK_RESREQ is set, the value of lsberrno is
 * either LSBE_RESREQ_OK or LSBE_RESREQ_ERR, depending on the result of
 * resource requirement string checking. The badJobName field in the submitReply
 * structure contains the detailed error message.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bsub
 * \n brestart
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_modify 
 * @see \ref ls_info  
 * @see \ref lsb_queueinfo  
 */
extern LS_LONG_INT lsb_submit P_((struct submit  *, struct submitReply *));
/**
 * \page  lsb_submitPack lsb_submitPack
 * Submits multiple jobs via pack method in the batch system
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_submitPack (struct packSubmit *packSub,
 *                         struct packSubmitReply *packSubmitReply,
 *                         int *acceptedNum, int *rejectedNum)</b>
 *
 * @param *packSub
 * Describes the requirements for job submissions to the batch system.
 * Those jobs that do not meet these requirements are not submitted to the
 * batch system. The caller needs to allocate the memory and release after usage.
 *
 * @param *packSubmitReply
 * Describes the results of the job submissions in one pack to the batch system.
 *
 * @param *acceptedNum
 * Describes the amount of accepted job requests in one pack.
 * Or if environment variable BSUB_CHK_RESREQ is set,
 * it is the amount of job requests containing valid resreq.
 *
 * @param *rejectedNum
 * Describes the amount of rejected job requests in one pack.
 * Or if environment variable BSUB_CHK_RESREQ is set,
 * it is the amount of job requests containning invalid resreq.
 * 
 * <b>Data Structures:</b>
 * \par
 * packSubmit
 * \n packSubmitReply
 *
 * @return int:1 \n
 * All job requests are accepted by MBD and jobs are created.
 *
 *@return int:-1 \n
 * There are invalid job requests or errors occur.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bsub
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 */
extern int lsb_submitPack P_((struct packSubmit *, struct packSubmitReply *, int *, int *));
#endif
/**
 * \page lsb_readjobinfo_cond lsb_readjobinfo_cond
 * \brief Returns the next job information record for condensed host groups
 * in mbatchd.
 *
 * \ref lsb_readjobinfo_cond reads the number of records defined by the more
 * parameter. The more parameter receives its value from either \ref lsb_openjobinfo
 * or \ref lsb_openjobinfo_a. Each time \ref lsb_readjobinfo_cond is called, it
 * returns one record from mbatchd. Use \ref lsb_readjobinfo_cond in a loop and use
 * more to determine how many times to repeat the loop to retrieve job information
 * records.
 *
 * \ref lsb_readjobinfo_cond differs from \ref lsb_readjobinfo in that if jInfoHExt
 * is not NULL, \ref lsb_readjobinfo_cond substitutes hostGroup (if it is a condensed
 * host group) for job execution hosts.
 *
 * <b>\#include <lsf/lsbatch.h>
 * \n \#include <time.h>
 * \n \#include <lsf/lsf.h>
 *
 * struct jobInfoEnt *lsb_readjobinfo_cond(int *more,
 *               	struct jobInfoHeadExt *jInfoHExt);</b>
 *
 * @param *more Number of job records in the master batch daemon.
 * @param *jInfoHExt Job information header info for the condensed host group.
 *
 * <b>Data Structures:</b>
 * \par
 * jobInfoEnt
 * \n jobExternalMsgReply 
 * \n jRusage
 * \n pidInfo
 * \n reserveItem 
 * \n submit
 *
 * <b>Define Statements:</b>
 * \par
 * \ref external_msg_processing
 * \n \ref group_nodetypes
 *
 * @return struct jobInfoEnt *:A pointer to job information entry.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If there are no more records, then lsberrno is set to LSBE_EOF.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.queues
 *
 * @see \ref lsb_openjobinfo 
 * @see \ref lsb_openjobinfo_a  
 * @see \ref lsb_closejobinfo  
 * @see \ref lsb_hostinfo  
 * @see \ref lsb_pendreason  
 * @see \ref lsb_queueinfo  
 * @see \ref lsb_readjobinfo  
 * @see \ref lsb_suspreason  
 */    
extern struct jobInfoEnt *lsb_readjobinfo_cond P_((int *more, 
       		   struct jobInfoHeadExt *jInfoHExt));
/**
 * \page lsb_readframejob lsb_readframejob
 * \brief Returns all frame jobs information which matchs the specified
 * parameters and fills related information into the frame job information table.
 *
 * \ref lsb_readframejob gets all frame jobs information that matches the specified
 * parameters and fills related information into the frame job information table.
 * \ref lsb_readframejob is a wrapper of \ref lsb_openjobinfo, \ref lsb_readjobinfo, and
 * \ref lsb_closejobinfo. Memory allocated in frameJobInfoTbl will be freed by
 * user.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_readframejob(LS_LONG_INT jobId, char *frameName,
 *       char *user, char *queue, char *host, int options,
 *       struct frameJobInfo **frameJobInfoTbl)</b>
 *
 * @param jobId Get information about the frame jobs with the given job ID.
 * If jobID is 0, get information about frame jobs which satisfy the other
 * specifications. If a job in a job array is to be modified, use the array
 * form jobID[i] where jobID is the job array name, and i is the index value.
 * @param *frameName Get information about frame jobs with the given frame name.
 * @param *user Get information about frame jobs submitted by the named user
 * or user group, or by all users if user is all. If user is NULL, the user
 * invoking this routine is assumed.
 * @param *queue Get information about frame jobs belonging to the named queue.
 * If queue is NULL,jobs in all queues of the batch system will be considered.
 * @param *host Get information about frame jobs on the named host, host
 * group or cluster name.If host is NULL, jobs on all hosts of the batch
 * system will be considered.
 * @param options <lsf/lsbatch.h> defines the following flags \ref defs_lsb_openjobinfo_a 
 * constructed from bits. Use the bitwise OR to set more than one flag.
 * @param **frameJobInfoTbl The result of all frame jobs information.
 *
 * <b>Data Structures:</b>
 * \par
 * \n frameJobInfo
 * \n frameElementInfo
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:Length of frame job information table. 
 * \n Function was successful.
 * @return int:-1 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_openjobinfo 
 * @see \ref lsb_readjobinfo 
 * @see \ref lsb_closejobinfo 
 */

extern int lsb_readframejob P_((LS_LONG_INT, char *, char *, char *, char *, int,  struct frameJobInfo **));
/**
 * \page lsb_closejobinfo lsb_closejobinfo
 * \brief Closes job information connection with the master batch daemon.
 *
 * Use \ref lsb_closejobinfo to close the connection to the master batch daemon
 * after opening a job information connection with \ref lsb_openjobinfo and reading
 * job records with \ref lsb_readjobinfo.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * void lsb_closejobinfo(void)</b>
 *
 * @param void \n
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return void
 * \n There's no returns value.
 *
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_openjobinfo 
 * @see \ref lsb_openjobinfo_a 
 * @see \ref lsb_readjobinfo 
 */

extern void lsb_closejobinfo P_((void));
/**
 * \page  lsb_hostcontrol lsb_hostcontrol
 * Opens or closes a host, or restarts or shuts down its slave batch daemon.
 *
 * \ref lsb_hostcontrol opens or closes a host, or restarts or shuts down its
 * slave batch daemon. Any program using this API must be setuid to root if
 * LSF_AUTH is not defined in the lsf.conf file.
 *
 * To restart the master batch daemon, mbatchd, in order to use updated
 * batch LSF configuration files, use \ref lsb_reconfig.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_hostcontrol (struct hostCtrlReq *req)</b>
 *
 * @param *req The host control request.
 *
 * <b>Data Structures:</b>
 * \par
 * hostCtrlReq
 *
 * <b>Define Statements:</b>
 * \par
 * \ref host_ctrl_option
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * Function failed.
 *
 * <b>Errors:</b>
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *    
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_reconfig 
 */
extern int  lsb_hostcontrol P_((struct hostCtrlReq *));
extern int  lsb_hghostcontrol P_((struct hgCtrlReq *, struct hgCtrlReply* reply));
/**
 * \page lsb_queueinfo lsb_queueinfo
 * \brief Returns information about batch queues.
 *
 * \ref lsb_queueinfo gets information about batch queues. See lsb.queues for more
 * information about queue parameters.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 *struct queueInfoEnt *lsb_queueinfo(char **queues,
 *                     int *numQueues, char *hosts, char *users,
 *                    int options)</b>
 *
 * @param **queues An array of names of queues of interest.
 * @param *numQueues The number of queue names. To get information on all queues,
 * set *numQueues to 0; *numQueues will be updated to the actual number of
 * queues when this call returns.If *numQueues is 1 and queues is NULL,
 * information on the system default queue is returned.
 * @param *hosts The host or cluster names. If hosts is not NULL, then only
 * the queues that are enabled for the hosts are of interest.
 * @param *user The name of user. If user is not NULL, then only the queues
 * that are enabled for the user are of interest.
 * @param options Reserved for future use; supply 0.
 *
 * <b>Data Structures:</b>
 * \par
 * queueInfoEnt
 * \n shareAcctInfoEnt
 * \n apsFactorInfo 
 * \n apsFactorMap 
 * \n apsLongNameMap 
 *
 * <b>Define Statements:</b>
 * \par
 * \ref queue_status
 * \n \ref queue_attribute
 *
 * @return struct queueInfoEnt *:\n An array of queueInfoEnt structures which stores the queue information and
 * sets numQueues to the size of the array.
 * \n Function was successful.
 * @return NULL 
 * \n Function Failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 * If lsberrno is LSBE_BAD_QUEUE, (*queues)[*numQueues] is not a queue known
 * to the LSF system. Otherwise, if *numQueues is less than its original value,
 * *numQueues is the actual number of queues found.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bqueues
 * 
 * \b Files:
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.queues
 * 
 * @see \ref lsb_hostinfo 
 * @see \ref lsb_userinfo 
 * @see \ref lsb_usergrpinfo 
 */
extern struct queueInfoEnt *lsb_queueinfo P_((char **queues, int *numQueues, char *host, char *userName, int options));
/**
 * \page lsb_reconfig lsb_reconfig
 * \brief Dynamically reconfigures an LSF batch system.
 *
 * \ref lsb_reconfig dynamically reconfigures an LSF batch system to pick up new
 * configuration parameters and changes to the job queue setup since system
 * startup or the last reconfiguration (see lsb.queues).
 *
 * To restart a slave batch daemon, use \ref lsb_hostcontrol. This call is
 * successfully invoked only by root or by the LSF administrator.
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not
 * defined in the lsf.conf file.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_reconfig (struct mbdCtrlReq *req)</b>
 *
 * @param *req mbatchd control request.
 *
 * <b>Data Structures:</b>
 * \par
 * mbdCtrlReq
 *
 * <b>Define Statements:</b>
 * \par
 * \ref mbd_operation
 *
 * @return int:0 \n
 * Function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * badmin reconfig
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_openjobinfo 
 */
extern int  lsb_reconfig P_((struct mbdCtrlReq *));
 /**
 * \page lsb_signaljob lsb_signaljob
 * \brief Sends a signal to a job.
 *
 * Use \ref lsb_signaljob when migrating a job from one host to another. Use
 * \ref lsb_signaljob to stop or kill a job on a host before using \ref lsb_mig to
 * migrate the job. Next, use \ref lsb_signaljob to continue the stopped job at
 * the specified host.
 * 
 * Generally, use \ref lsb_signaljob to apply any UNIX signal to a job or process.
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined
 * in the lsf.conf file.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * int lsb_signaljob (LS_LONG_INT jobId, int sigValue)</b>
 *
 * @param jobId The job to be signaled. If a job in a job array is to be
 * signaled, use the array form jobID[ i ] where jobID is the job array name,
 * and i is the index value.
 * @param sigValue SIGSTOP, SIGCONT, SIGKILL or some other UNIX signal.
 *
 * <b>Data Structures:</b>
 *\par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bkill \n
 * bstop \n
 * bresume  
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_chkpntjob  
 * @see \ref lsb_forcekilljob  
 * @see \ref lsb_mig  
 */

extern int  lsb_signaljob P_((LS_LONG_INT, int));

/**
 * \page lsb_killbulkjobs lsb_killbulkjobs
 * \brief Kills bulk jobs as soon as possible.
 *
 * Use \ref lsb_killbulkjobs to kill bulk jobs on a local host immediately, or
 * to kill other jobs as soon as possible. If mbatchd rejects the request, it
 * issues NULL as the reservation ID.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_killbulkjobs(struct signalBulkJobs *s)</b>
 *
 * @param *s The signal to a group of jobs.
 *
 * <b>Data Structures:</b>
 * \par
 * signalBulkJobs
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The bulk jobs were successfully killed.
 * @return int:-1 \n
 * The bulk jobs were not killed.
 * 
 * \b Error:
 * \par
 * On failure, lsberrno is set to indicate the error.
 * 
 * <b>Equivalent line command:</b>
 * \par
 * bkill -b
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see none
 */

extern int  lsb_killbulkjobs P_((struct signalBulkJobs *));
extern int  lsb_msgjob P_((LS_LONG_INT, char *));
/**
 * \page lsb_chkpntjob lsb_chkpntjob
 * \brief Checkpoints a job.
 *
 * Checkpoints a job.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_chkpntjob(LS_LONG_INT jobId, time_t period, int options)</b>
 *
 * @param jobId The job to be checkpointed.
 * @param period The checkpoint period in seconds. The value 0 
 * disables periodic checkpointing.
 * @param options The bitwise inclusive OR of some of the following:
 * \n LSB_CHKPNT_KILL
 * Checkpoint and kill the job as an atomic action.
 * \n LSB_CHKPNT_FORCE
 * Checkpoint the job even if non-checkpointable conditions exist.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * \ref chkpnt_job_option
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \note Any program using this API must be setuid to root if LSF_AUTH
 * is not defined in the lsf.conf file.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bchkpnt
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see none
 */
extern int  lsb_chkpntjob P_((LS_LONG_INT, time_t, int));
/**
 * \page lsb_deletejob lsb_deletejob
 * \brief Kills a job in a queue
 *
 * Use \ref lsb_deletejob to send a signal to kill a running, user-suspended, 
 * or system-suspended job. The job can be requeued or deleted from the batch
 * system.If the job is requeued, it retains its submit time but it is dispatched
 * according to its requeue time. When the job is requeued, it is assigned the
 * PEND status and re-run.If the job is deleted from the batch system, it is
 * no longer available to be requeued.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_deletejob (LS_LONG_INT jobId, int times, int options)</b>
 *
 * @param jobId The job to be killed. If an element of a job array is to be
 * killed, use the array form jobID[i] where jobID is the job array name, 
 * and i is the index value.
 * @param times Original job submit time.
 * @param options If the preprocessor macro LSB_KILL_REQUEUE in lsbatch.h is
 * compared with options and found true, then requeue the job using the same job ID.
 * If the preprocessor macro LSB_KILL_REQUEUE in lsbatch.h is compared with
 * options and found false, then the job is deleted from the batch system.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * \ref kill_requeue
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \note Any program using this API must be setuid to root if LSF_AUTH is not defined in the
 *	\n lsf.conf file.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bkill
 * \n brequeue -J
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_signaljob 
 * @see \ref lsb_chkpntjob 
 */
extern int  lsb_deletejob P_((LS_LONG_INT, int, int));
/**
 * \page lsb_forcekilljob lsb_forcekilljob
 * \brief This function is used to send special force kill signal.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_forcekilljob(LS_LONG_INT jobId)</b>
 *
 * @param jobId which job is to be killed.
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
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_signaljob
 */
extern int  lsb_forcekilljob P_((LS_LONG_INT));
/**
 * \page lsb_submitframe lsb_submitframe
 * \brief Submits a frame job to the batch system.
 *
 * \ref lsb_submitframe submits a frame job to the batch system according to the
 * jobSubReq specification and frameExp.
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined
 * in the lsf.conf file.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 *int lsb_submitframe (struct submit *jobSubReq, char *frameExp,
 *       		struct submitReply *jobSubReply)</b>
 *
 * @param *jobSubReq Describes the requirements for job submission to the
 * batch system. A job that does not meet these requirements is not submitted
 * to the batch system and an error is returned. \n
 * See \ref lsb_submit for descriptions of the submit structure fields.
 * @param *frameExp The syntax of frameExp is: \n
 * <b>frame_name[indexlist]</b> \n
 * frame_name is any name consisting of alphanumerics, periods, forward slashes,
 * dashes or underscores. indexlist is a list of one or more frame indexes, 
 * separated by commas. These indexes can each be either a single integer or
 * a range, specified in the following format: \n
 * <b>start-end[xstep[:chunk]]</b> \n
 * start, end, step, and chunk are integers, but chunk must be positive.
 * If step and
 * chunk are ommitted, the default value is 1.\n
 * An example of a valid expression for frameExp is:\n
 * <b>Frame_job_1[5,10-15,20-30x2:3]</b>
 * @param **jobSubReply Describes the results of the job submission to the
 * batch system. \n
 * See \ref lsb_submit for descriptions of the submitReply structure
 * fields.
 *
 * <b>Data Structures:</b>
 * \par
 * submit
 * \n submitReply
 *
 * <b>Define Statements:</b>
 * \par
 * \ref lsb_submit_options
 * \n \ref lsb_submit_options2
 * \n \ref lsb_submit_options3
 * \n \ref lsb_submit_options4
 *
 * @return int:jobId 
 * \n The function was successful, and sets the queue field of jobSubReply to the name
 * of the queue that the job was submitted to.
 * @return int:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error and jobSubReply gives
 * additional information about the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 *none
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see none
 */
extern int  lsb_submitframe P_((struct submit *, char *, struct submitReply *));
/**
 * \page lsb_requeuejob lsb_requeuejob
 * \brief Requeues job arrays, jobs in job arrays, and individual jobs.
 *
 * Use \ref lsb_requeuejob to requeue job arrays, jobs in job arrays, and individual
 * jobs that are running, pending, done, or exited. In a job array, you can
 * requeue all the jobs or requeue individual jobs of the array.
 *
 * \ref lsb_requeuejob requeues jobs as if the jobs were in an array. A job not in an
 * array is considered to be a job array composed of one job.
 *
 * Jobs in a job array can be requeued independently of each other regardless of
 * any job's status (running, pending, exited, done). A requeued job is requeued
 * to the same queue it was originally submitted from or switched to. The job
 * submission time does not change so a requeued job is placed at the top of the
 * queue. Use \ref lsb_movejob to place a job at the bottom or any other position
 * in a queue.
 *
 * If a clean period is reached before \ref lsb_requeuejob is called, the cleaned
 * jobs cannot be requeued. Set the variable CLEAN_PERIOD in your lsb.params file
 * to determine the amount of time that job records are kept in MBD core memory
 * after jobs have finished or terminated.
 *
 * To requeue a job assign values to the data members of the jobrequeue data
 * structure, process command line options in case the user has specified a
 * different job, and call \ref lsb_requeuejob to requeue the job array.
 *
 * Assign values to the jobID, status, and options data members of the jobrequeue
 * data structure. Assign the job identification number to jobID. Assign
 * JOB_STAT_PEND or JOB_STAT_PSUSP to status. Assign REQUEUE_DONE, REQUEUE_EXIT,
 * and or REQUEUE_RUN to requeue running jobs.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_requeuejob(struct jobrequeue * reqPtr)</b>
 *
 * @param *reqPtr This structure contains the information required to requeue a job.
 *
 * <b>Data Structures:</b>
 * \par
 * jobrequeue
 *
 * <b>Define Statements:</b>
 * \par
 * \ref requeuejob_options
 *
 * @return int:0 \n
 * The function is successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * brequeue -d
 * \n brequeue -e
 * \n brequeue -a
 * \n brequeue -r
 * \n brequeue -H
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 * \n $LSB_SHAREDIR
 *
 * @see \ref lsb_movejob 
 * @see \ref lsb_pendreason 
 */
extern int  lsb_requeuejob P_((struct jobrequeue *));
/**
 * \page lsb_sysmsg lsb_sysmsg
 * \brief Returns a pointer to static data.
 *
 * \ref lsb_sysmsg returns a pointer to static data which stores the batch error 
 * message corresponding to lsberrno. The global variable lsberrno maintained 
 * by LSBLIB holds the error number from the most recent LSBLIB call that caused
 * an error. If lsberrno == LSBE_SYS_CALL, then the system error message defined
 * by errno is also returned. If lsberrno == LSBE_LSLIB, then the error message
 * returned by \ref ls_sysmsg is returned.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * char *lsb_sysmsg (void)</b>
 *
 * @param void \n
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return char *:Return LSBATCH error message defined by lsberrno.
 * \n If lsberrno == LSBE_SYS_CALL, then the system error message defined by
 * errno is also returned.
 * \n If lsberrno == LSBE_LSLIB, then the error message returned by
 * \ref ls_sysmsg is returned.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
  * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref ls_perror 
 * @see \ref ls_sysmsg 
 */
extern char *lsb_sysmsg P_((void));
/**
 * \page lsb_perror lsb_perror
 * \brief Prints a batch LSF error message on stderr.
 *
 * \ref lsb_perror prints a batch LSF error message on stderr. The usrMsg is
 * printed out first, followed by a ":" and the batch error message corresponding
 * to lsberrno.
 * 
 * \ref lsb_perror - Print LSBATCH error message on stderr. In addition
 * to the error message defined by lsberrno, user supplied message usrMsg1
 * is printed out first and a ':' is added to separate * usrMsg1 and LSBATCH
 * error message.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * void lsb_perror (char *usrMsg)</b>
 *
 * @param *usrMsg A user supplied error message.
 *
 * <b>Data Structures:</b>
 * \par
 * none  
 *
 * <b>Define Statements:</b>
 * \par 
 * none
 *
 * @return void \n
 * Prints out the user supplied error message.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * none
 *
 * @see none
 */
extern void lsb_perror P_((char *));
extern void lsb_errorByCmd P_((char *, char *, int));

#if 0 /* LSF_PARALLEL #1351 */
extern char *lsb_sperror P_((void *, char *));
#endif
extern char *lsb_sperror P_((char *));
/**
* \page lsb_peekjob lsb_peekjob
* \brief Returns the base name of the file related to the job ID
*  
* \ref lsb_peekjob retrieves the name of a job file.
* 
* Only the submitter can peek at job output.
* 
* The storage for the file name will be reused by the next call.
* 
* Any program using this API must be setuid to root if LSF_AUTH 
* is not defined in the lsf.conf file.  
* The caller is responsible for freeing the memory allocated for the
* job output file
*
*<b>\#include <lsf/lsbatch.h>
*
* char * lsb_peekjob (LS_LONG_INT jobId)</b>
*
* @param jobId The job ID that the LSF system assigned to the job. If a job
* in a job array is to be returned, use the array form jobID[i] where jobID
* is the job array name, and i is the index value.
*
* <b>Data Structures:</b>
* \par
* none
*
* <b>Define Statements:</b>
* \par
* none
* 
* @return char *:Name of job Output File.
* \n Function was successful.
* @return NULL
* \n Function failed.
*
* \b Errors:
* \par
* If the function fails, lsberrno is set to indicate the error.
*
* <b>Equivalent line command:</b>
* \par
* bpeek 
*
* \b Files:
* \par
* ${LSF_ENVDIR:-/etc}/lsf.conf
*
* @see none
*/
extern char *lsb_peekjob P_((LS_LONG_INT));
/** 
 * \page lsb_mig lsb_mig
 * \brief Migrates a job from one host to another.
 *
 * \ref lsb_mig migrates a job from one host to another. Any program using
 * this API must be setuid to root if LSF_AUTH is not defined
 * in the lsf.conf file.
 *
 * <b>\#include <lsf/lsbatch.h>
 * 
 * int lsb_mig(struct submig *mig, int *badHostIdx)</b>
 *
 * @param *mig The job to be migrated.
 * @param *badHostIdx If the call fails, (**askedHosts)[*badHostIdx] is not a
 * host known to the LSF system.
 * 
 * <b>Data Structures:</b>
 * \par
 * submig
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error and badHostIdx indicates
 * which askedHost is not acceptable.
 *
 * <b>Equivalent line command:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_submit  
 */
extern int lsb_mig P_((struct submig *, int *badHostIdx));    

extern struct clusterInfoEnt *lsb_clusterinfo P_((int *, char **, int));
extern struct clusterInfoEntEx *lsb_clusterinfoEx P_((int *, char **, int)
);
/**
 * \page lsb_hostinfo lsb_hostinfo
 * Returns information about job server hosts.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * struct hostInfoEnt *lsb_hostinfo(char **hosts, int *numHosts)</b>
 *
 * @param **hosts
 * An array of host or cluster names.
 * @param *numHosts
 * The number of host names.
 * To get information on all hosts, set *numHosts to 0; *numHosts will be set to the
 * actual number of hostInfoEnt structures when this call returns.
 * If *numHosts is 1 and hosts is NULL, information on the local host is returned.
 *
 * <b>Data Structures:</b>
 *\par
 * hostInfoEnt
 *
 * <b>Define Statements:</b>
 * \par
 * \ref host_status
 * \n \ref host_load_BusyReason
 * \n \ref host_attributes
 *
 * @return struct hostInfoEnt *:\n An array of hostInfoEnt structures which hold the host
 * information and sets *numHosts to the number of hostInfoEnt structures.
 * \n Function was successful.
 * @return struct hostInfoEnt *:NULL
 * \n Function failed.
 *
 * <b>Errors:</b>
 * \par
 * If the function fails, lsberrno is set to indicate the error. If lsberrno is
 * LSBE_BAD_HOST, (*hosts)[*numHosts] is not a host known to the batch system.
 * Otherwise, if *numHosts is less than its original value, *numHosts is the actual
 * number of hosts found.
 * 
 * <b>Equivalent line commands:</b>
 * \par
 * bhosts
 * 
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.hosts
 *
 * @see \ref lsb_hostinfo_ex 
 * @see \ref ls_info 
 * @see \ref ls_loadofhosts 
 * @see \ref lsb_queueinfo 
 * @see \ref lsb_userinfo  
 */
extern struct hostInfoEnt *lsb_hostinfo P_(( char **, int *));
/**
 * \page lsb_hostinfo_ex lsb_hostinfo_ex
 * Returns informaton about job server hosts that satisfy specified resource
 * requirements. \ref lsb_hostinfo_ex returns information about job server hosts
 * that satisfy the specified resource requirements.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * struct hostInfoEnt *lsb_hostinfo_ex(char **hosts,
 * int *numHosts, char *resReq, int options)</b>
 
 * @param **hosts An array of host or cluster names.
 * @param *numHosts The number of host names.
 * To get information on all hosts, set *numHosts to 0; *numHosts will be set
 * to the actual number of hostInfoEnt structures when this call returns.
 * If *numHosts is 1 and hosts is NULL, information on the local host is returned.
 * @param *resReq Resource requirements.
 * If this option is specified, then only host information for those hosts
 * that satisfy the resource requirements is returned. Returned hosts are
 * sorted according to the load on the resource() given in resReq, or by
 * default according to CPU and paging load.
 * @param options Options is reserved for the future use.
 *
 * <b>Data Structures:</b>
 *\par
 * hostInfoEnt
 *
 * <b>Define Statements:</b>
 * \par
 * \ref host_status 
 * \n \ref host_load_BusyReason
 * \n \ref host_attributes
 *
 * @return struct hostInfoEnt *:\n An array of hostInfoEnt structures which hold the host
 * information and sets *numHosts to the number of hostInfoEnt structures.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 *
 * <b>Errors:</b>
 * \par
 * If the function fails, lsberrno is set to indicate the error. If lsberrno is
 * LSBE_BAD_HOST, (*hosts)[*numHosts] is not a host known to the batch system.
 * Otherwise, if *numHosts is less than its original value, *numHosts is the actual
 * number of hosts found.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.hosts
 *
 * @see \ref ls_info  
 * @see \ref ls_loadofhosts 
 * @see \ref lsb_hostinfo 
 * @see \ref lsb_queueinfo 
 * @see \ref lsb_userinfo 
 */

extern struct hostInfoEnt *lsb_hostinfo_ex P_(( char **, int *, char *, int));
/**
 * \page lsb_hostinfo_cond lsb_hostinfo_cond
 * Returns condensed information about job server hosts.
 *
 * \ref lsb_hostinfo_cond returns condensed information about job server hosts.
 * While \ref lsb_hostinfo returns specific information about individual hosts,
 * \ref lsb_hostinfo_cond returns the number of jobs in each state within the 
 * entire host group. The condHostInfoEnt structure contains counters that
 * indicate how many hosts are in the ok, busy, closed, full, unreach, and 
 * unavail states and an array of hostInfoEnt structures that indicate the 
 * status of each host in the host
 * group.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 *struct condHostInfoEnt * lsb_hostinfo_cond
 *(char **hosts, int *numHosts,
 *char *resReq, int options)</b>
 *
 * @param **hosts An array of host names belonging to the host group.
 * @param *numHosts The number of host names in the host group.
 * To get information on all hosts in the host group, set *numHosts to 0;
 * *numHosts will be set to the actual number of hostInfoEnt structures in
 * the host group when this call returns.
 * @param *resReq Any resource requirements called with the function.
 * @param options Any options called with the function.
 *
 * <b>Data Structures</b>
 * \par
 * condHostInfoEnt
 * \n hostInfoEnt
 * 
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct condHostInfoEnt *:
 * \n The condHostInfoEnt structure contains condensed information about the status
 * of job server hosts in the host group. If there is no condensed host group
 * matching the specified host, *name is set to NULL and *hostInfo contains 
 * specific host information to be displayed instead of the condensed host 
 * group information.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 * 
 * <b Errors:</b>
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_hostinfo 
 */
extern struct condHostInfoEnt *lsb_hostinfo_cond P_(( char **, int *, char *, 
                    int));
/**
 * \page lsb_movejob lsb_movejob
 * \brief Changes the position of a pending job in a queue.
 *
 * Use \ref lsb_movejob to move a pending job to a new position that you specify 
 * in a queue. Position the job in a queue by first specifying the job ID.
 * Next, count, beginning at 1, from either the top or the bottom of the queue,
 * to the position you want to place the job.
 *
 * To position a job at the top of a queue, choose the top of a queue parameter
 * and a postion of 1.To position a job at the bottom of a queue, choose the
 * bottom of the queue parameter and a position of 1.
 *
 * By default, LSF dispatches
 * jobs in a queue in order of their arrival (such as first-come-first-served),
 * subject to the availability of suitable server hosts.
 *
 * <b>\#include <lsf/lsbatch.h>
 * 
 * int lsb_movejob (LS_LONG_INT jobId, int *position, int opCode)</b>
 * 
 * @param jobId The job ID that the LSF system assigns to the job. If a job
 * in a job array is to be moved, use the array form jobID[ i ] where jobID is
 * the job array name, and i is the index value.
 * @param *position The new position of the job in a queue. position must be
 * a value of 1 or more.
 * @param opCode The top or bottom position of a queue.
 *  \n \b TO_TOP
 *  \n The top position of a queue.
 *  \n \b TO_BOTTOM
 *  \n The bottom position of a queue.
 *  \n If an opCode is not specified for the top or bottom position, the
 *  function fails.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * \ref movejob_options
 *
 * @return int:0 \n
 * The function is successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 *                   btop
 *                 \n bbot
 *                \n bjobs -q
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_pendreason  
 */

extern int sortHostByCluster P_((const void *, const void *));

extern int lsb_movejob P_((LS_LONG_INT jobId, int *, int));
/**
 * \page lsb_switchjob lsb_switchjob
 * \brief Switches an unfinished job to another queue.
 *
 * \ref lsb_switchjob switches an unfinished job to another queue. Effectively,
 * the job is removed from its current queue and re-queued in the new queue.
 *
 * The switch operation can be performed only when the job is acceptable to
 * the new queue. If the switch operation is unsuccessful, the job will stay
 * where it is.A user can only switch his/her own unfinished jobs, but root
 * and the LSF administrator can switch any unfinished job.
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined
 * in the lsf.conf file.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_switchjob (LS_LONG_INT jobId, char *queue)</b>
 *
 * @param jobId The job to be switched. If an element of a job array is to
 * be switched, use the array form jobID[i] where jobID is the job array name,
 * and i is the index value.
 * @param *queue The new queue for the job.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The function was successful.
 * @return  int:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bswitch
 *
 * <b>Files:</b>
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see none
 */
extern int lsb_switchjob P_((LS_LONG_INT jobId, char *queue));
/**
 * \page lsb_queuecontrol lsb_queuecontrol
 * \brief Changes the status of a queue.
 *
 * \ref lsb_queuecontrol changes the status of a queue.
 *
 * Any program using this API must be setuid to root if LSF_AUTH is not defined
 * in the lsf.conf file.
 *
 * If a queue is inactivated by its dispatch window (see lsb.queues), then it
 * cannot be re-activated by this call.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_queuecontrol (struct queueCtrlReq *req)</b>
 *
 * @param *req queue control request.
 *
 * <b>Data Structures:</b>
 * \par
 * queueCtrlReq
 *
 * <b>Define Statements:</b>
 * \par
 * \ref queue_ctrl_option
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_queueinfo 
 */
extern int lsb_queuecontrol P_((struct queueCtrlReq *));
/**
 * \page lsb_userinfo lsb_userinfo
 * \brief Returns the maximum number of job slots that a user can use
 * simultaneously on any host and in the whole local LSF cluster.
 *
 * \ref lsb_userinfo gets the maximum number of job slots that a user can use
 * simultaneously on any host and in the whole local LSF cluster, as well as
 * the current number of job slots used by running and suspended jobs or
 * reserved for pending jobs. The maximum numbers of job slots are defined
 * in the LSF configuration file lsb.users (see lsb.users). The reserved
 * user name default, defined in the lsb.users configuration file, matches
 * users not listed in the lsb.users file who have no jobs started in the 
 * system.
 *
 * The returned array will be overwritten by the next call.
 * 
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct userInfoEnt *lsb_userinfo(char **users, int *numUsers)</b>
 *
 * @param **users An array of user names.
 * @param *numUsers The number of user names.
 * To get information about all users, set *numUsers = 0; *numUsers will
 * be updated to the actual number of users when this call returns. To get
 * information on the invoker, set users = NULL, *numUsers = 1.
 *
 * <b>Data Structures:</b>
 * \par
 * userInfoEnt
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct userInfoEnt *: A static pointer to a struct userInfoEnt.
 * \n The function was successful, and *numUsers is set to the number of userInfoEnt
 * structures returned.
 * @return NULL \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error. If lsberrno is
 * LSBE_BAD_USER, (*users)[*numUsers] is not a user known to the LSF system.
 * Otherwise, if *numUsers is less than its original value, *numUsers is the actual
 * number of users found.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * busers
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.users
 *
 * @see \ref lsb_hostinfo  
 * @see \ref lsb_queueinfo  
 */
extern struct userInfoEnt *lsb_userinfo P_(( char **, int *));
 /**
 * \page lsb_hostgrpinfo lsb_hostgrpinfo
 * Returns LSF host group membership.
 *
 * \ref lsb_hostgrpinfo gets LSF host group membership.
 * 
 * LSF host group is defined in the configuration file lsb.hosts.
 *
 * The storage for the array of groupInfoEnt structures will be reused by
 * the next call.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct groupInfoEnt *lsb_hostgrpinfo (char **groups,int *numGroups,
 *         int options)</b>
 *
 * @param **groups An array of group names.
 * @param *numGroups The number of group names. *numGroups will be updated
 * to the actual number of groups when this call returns.
 * @param options The bitwise inclusive OR of some of the following flags:
 * \n GRP_RECURSIVE
 * \n Expand the group membership recursively. That is, if a member of a
 * group is itself a group, give the names of its members recursively, rather
 * than its name, which is the default.
 * \n GRP_ALL
 * \n Get membership of all groups. 
 *
 * <b>Data Structures:</b>
 * \par
 * groupInfoEnt
 * \n userShares
 * 
 * <b>Define Statements:</b>
 * \par
 * \ref group_membership_option
 * \n \ref group_define
 *
 * @return struct groupInfoEnt *: \n
 * On success, returns an array of groupInfoEnt structures which hold the group
 * name and the list of names of its members. If a member of a group is itself 
 * a group(i.e., a subgroup), then a '/' is appended to the name to indicate
 * this. *numGroups is the number of groupInfoEnt structures returned.
 * @return NULL \n
 * Function failed.
 *
 * <b>Errors:</b>
 * \par
 * On failure, returns NULL and sets lsberrno to indicate the error. If there
 * are invalid groups specified, the function returns the groups up to the
 * invalid ones and then sets lsberrno to LSBE_BAD_GROUP, which means that
 * the specified (*groups)[*numGroups] is not a group known to the LSF system.
 * If the first group specified is invalid, the function returns NULL.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *   
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.hosts \n
 * $LSB_CONFDIR/cluster_name/configdir/lsb.users
 *
 * @see \ref lsb_usergrpinfo 
 */
extern struct groupInfoEnt *lsb_hostgrpinfo P_((char**, int *, int));
/**
 * \page lsb_usergrpinfo lsb_usergrpinfo
 * \brief Returns LSF user group membership.
 *
 * \ref lsb_usergrpinfo gets LSF user group membership.
 * LSF user group is defined in the configuration file lsb.users.
 * The storage for the array of groupInfoEnt structures will be reused by
 * the next call.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct groupInfoEnt *lsb_usergrpinfo (char **groups,
 *       		int *numGroups, int options)</b>
 *
 * @param **groups An array of group names.
 * @param *numGroups The number of group names. *numGroups will be updated
 * to the actual number of groups when this call returns.
 * @param options The bitwise inclusive OR of some of flags in \ref group_membership_option 
 *
 * <b>Data Structures:</b>
 * \par
 * groupInfoEnt
 *
 * <b>Define Statements:</b>
 * \par
 * \ref group_membership_option
 * \n \ref group_define
 *
 * @return struct groupInfoEnt *: \n
 * On success, returns an array of groupInfoEnt structures which hold the group
 * name and the list of names of its members. If a member of a group is itself
 * a group(i.e., a subgroup), then a '/' is appended to the name to indicate
 * this. *numGroups is the number of groupInfoEnt structures returned.
 * @return NULL \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * On failure, returns NULL and sets lsberrno to indicate the error. If there
 * are invalid groups specified, the function returns the groups up to the 
 * invalid ones. It then set lsberrno to LSBE_BAD_GROUP, that is the specified
 * (*groups)[*numGroups] is not a group known to the LSF system. If the first
 * group is invalid, the function returns NULL.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.hosts
 * \n $LSB_CONFDIR/cluster_name/configdir/lsb.users
 *
 * @see \ref lsb_hostgrpinfo 
 */
extern struct groupInfoEnt *lsb_usergrpinfo P_((char **, int *, int));
/**
 * \page lsb_parameterinfo lsb_parameterinfo
 * \brief Returns information about the LSF cluster.
 *
 * \ref lsb_parameterinfo gets information about the LSF cluster.
 *
 * The static storage for the parameterInfo structure is reused on the next call.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * struct parameterInfo *lsb_parameterinfo(char **names,
 *                                        int *numUsers, int options)</b>
 *
 * @param **names Reserved but not used; supply NULL.
 * @param *numUsers Reserved but not used; supply NULL.
 * @param  options Reserved but not used; supply 0.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref parameterInfo
 *
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return struct parameterInfo *:A pointer to a parameterInfo structure.\n
 * The function was a successful.
 * @return NULL \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * none
 *
 * \b Files:
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see none
 */             
extern struct parameterInfo *lsb_parameterinfo P_((char **, int *, int));
 /**
 * \page lsb_modify lsb_modify
 * \brief  Modifies a submitted job's parameters.
 *
 * lsb_modify() allows for the modification of a submitted job's parameters.
 *
 * <b>\#include <lsf/lsbatch.h>
 * 
 * LS_LONG_INT lsb_modify (struct submit *jobsubReq, 
 *                         struct submitReply *jobSubReply,
 *                         LS_LONG_INT jobId)</b>
 *
 * @param *jobSubReq Describes the requirements for job modification to the
 * batch system. A job that does not meet these requirements is not submitted
 * to the batch system and an error is returned.
 * @param *jobSubReply Describes the results of the job modification to the
 * batch system.
 * @param jobId The job to be modified. If an element of a job array is to
 * be modified, use the array form jobID[i] where jobID is the job array name,
 * and i is the index value.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref submit         
 * \n \ref submitReply 
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return LS_LONG_INT:jobID \n
 * The function was successful, and sets the queue field of jobSubReply to
 * the name of the queue that the job was submitted to.
 * @return LS_LONG_INT:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command :</b>
 * \par
 * bmod
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_submit  
 * @see \ref ls_info  
 * @see \ref ls_rtask 
 * @see \ref lsb_queueinfo  
 */
extern LS_LONG_INT lsb_modify P_((struct submit *, struct submitReply *, LS_LONG_INT));
extern float * getCpuFactor P_((char *, int));
/**
 * \page lsb_suspreason lsb_suspreason
 * \brief Explains why a job was suspended.
 *
 * Using the SBD, \ref lsb_suspreason explains why system-suspended and
 * user-suspended jobs were suspended.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * char *lsb_suspreason(int reasons, int subreasons,
 *     struct loadIndexLog *ld)</b>
 *
 * @param reasons Reasons a job suspends.
 * @param subreasons If reasons is SUSP_LOAD_REASON, subreasons indicates
 * the load indices that are out of bounds. The integer values for the load
 * indices are found in lsf.h.If reasons is SUSP_RES_LIMIT, subreasons 
 * indicates the job's requirements for resource reservation are not satisfied.
 * The integer values for the job's requirements for resource reservation are
 * found in lsbatch.h.
 * \n Subreasons a job suspends if reasons is SUSP_LOAD_REASON:
 * - \b	R15S
 * \n 15 second CPU run queue length
 * - \b	R1M
 * \n 1 minute CPU run queue length
 * - \b	R15M
 * \n 15 minute CPU run queue length
 * - \b	UT
 * \n 1 minute CPU utilization
 * - \b	PG
 * \n Paging rate
 * - \b	IO
 * \n Disk IO rate
 * - \b LS
 *  \n Number of log in sessions
 * - \b IT
 *       \n Idle time
 * - \b TMP
 *       \n Available temporary space
 * - \b SWP
 *       \n Available swap space
 * - \b MEM
 *       \n Available memory
 * - \b USR1
 * \n USR1 is used to describe unavailable or out of bounds user defined load
 * information of an external dynamic load indice on execution hosts.
 * - \b USR2
 * \n USR2 is used to describe unavailable or out of bounds user defined load
 *  information of an external dynamic load indice on execution hosts.
 *
 * @param *ld When reasons is SUSP_LOAD_REASON, ld is used to determine the
 * name of any external load indices. ld uses the most recent load index log
 * in the lsb.events file.
 *
 * <b>Data Structures:</b>
 * \par
 * loadIndexLog
 *
 * <b>Define Statements:</b>
 * \par
 * \ref suspending_reasons \n
 * \ref suspending_subreasons
 *
 * @return char *:Returns the suspending reason string.
 * \n Function was successful.
 * @return NULL \n
 * The function failed. The reason code is bad.
 *
 * \b Errors:
 * \par
 * No error handling
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bjobs -s
 *
 * <b>Environment Variable:</b>
 * \par
 * LSB_SUSP_REASONS
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.queues \n
 * $LSB_SHAREDIR/cluster_name/logdir/lsb.events 
 *
 * @see \ref lsb_pendreason 
 */
extern char *lsb_suspreason P_((int, int, struct loadIndexLog *));
/**
 * \page lsb_pendreason  lsb_pendreason
 * \brief Explains why a job is pending.
 *
 * Use \ref lsb_pendreason to determine why a job is pending. Each pending reason is
 * associated with one or more hosts.
 *            
 * <b>\#include <lsf/lsbatch.h>
 *
 * char *lsb_pendreason (int numReasons, int *rsTb,
 *                      struct jobInfoHead *jInfoH,
 *                      struct loadIndexLog *ld, int clusterId)</b>
 *
 * @param numReasons The number of reasons in the rsTb reason table.
 * @param *rsTb The reason table. Each entry in the table contains one of \ref pending_reasons
 * @param *jInfoH jInfoH contains job information.
 * @param *ld From \ref lsb_suspreason, when reasons is SUSP_LOAD_REASON, ld is used to
 * determine the name of any external load indices. ld uses the most recent load
 * index log in the lsb.events file.
 * @param clusterId MultiCluster cluster ID. If clusterId is greater than or 
 * equal to 0, the job is a pending remote job, and \ref lsb_pendreason checks for
 * host_name\@cluster_name. If host name is needed, it should be found in 
 * jInfoH->remoteHosts. If the remote host name is not available, the constant
 * string remoteHost is used.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref jobInfoHead
 * \n \ref loadIndexLog   
 *
 * <b>Define Statements:</b>
 * \par 
 * \ref pending_reasons 
 * \n \ref suspending_reasons
 * \n \ref suspending_subreasons
 *
 * @return char *:reasons \n
 * The function is successful. It returns a reason why the job is pending.
 * @return NULL \n
 * The function fails. The reason code is bad.
 *
 * \b Errors:
 * \par
 * If no PEND reason is found, the function fails and lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * bjobs -p
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see \ref lsb_geteventrec  
 */
extern char *lsb_pendreason P_((int, int *, struct jobInfoHead *,   
                            struct loadIndexLog *, int));
/**
 * \page lsb_calendarinfo lsb_calendarinfo
 * \brief Gets information about calendars defined in the batch system.
 *
 * \ref lsb_calendarinfo gets information about calendars defined in the batch system.
 * 
 * On success, this routine returns a pointer to an array of calendarInfoEnt
 * structures which stores the information about the returned calendars and
 * numCalendars gives number of calendars returned. On failure NULL is returned
 * and lsberrno is set to indicate the error.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct calendarInfoEnt *lsb_calendarinfo(char **calendars,
 * int *numCalendars, char *user)</b>
 *
 * @param **calendars calendars is a pointer to an array of calendar names.
 * @param *numCalendars *numCalendars gives the number of calendar names. If 
 * *numCalendars is 0, then information about all calendars is returned. 
 * By default, only the invokers calendars are considered.
 * @param *user Setting the user parameter will cause the given users calendars
 * to be considered.Use the reserved user name all to get calendars of all users.
 *
 * <b>Data Structures:</b>
 * \par
 * calendarInfoEnt
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct calendarInfoEnt *:An array of the calendars and their info.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_calendarop 
 */
extern struct calendarInfoEnt * lsb_calendarinfo P_((char **, int *, char *));
extern int lsb_calExprOccs P_((char *, int, int, char *, int **));
/**
 * \page lsb_calendarop lsb_calendarop
 * \brief Adds, modifies or deletes a calendar.
 *
 * \ref lsb_calendarop is used to add, modify or delete a calendar. The oper 
 * parameter is one of CALADD, CALMOD, or CALDEL. When the operation CALADD
 * is specified, the first element of the names array is used as the name of
 * the calendar to add. The desc and calExpr parameters should point to the
 * description string and the time expression list, respectively. See bcaladd()
 * for a description of time expressions. 
 *
 * CALMOD permits the modification of the
 * description or time expression list associated with an existing calendar. The
 * first name in the names array indicates the calendar to be modified. The desc
 * and calExpr parameters can be set to the updated value or to NULL to 
 * indicate that the existing value should be maintained.
 *
 * If the operation is 
 * CALDEL then the names parameter points to an array of calendar names to be
 * deleted. numNames gives the number of names in the array. options is
 * reserved for the future use.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * int *lsb_calendarop(int oper, int numNames, char **names, char
 * *desc, char *calExpr, int options, char **badStr)</b>
 *
 * @param oper One of CALADD, CALMOD, or CALDEL. Depending on which one is
 * chosen, adds, modifies, or deletes a calendar.Defined in \ref calendar_command.
 * @param numNames The number of names in the array.
 * @param **names Depending on oper, it defines the name of the calendar is going
 * to be added, modified or deleted.
 * @param *desc The calendar's description list.
 * @param *calExpr A calendar expression.
 * @param options Currently unused.
 * @param **badStr Return from mbatchd indicating bad name or event time of calendar.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * \ref calendar_command
 *
 * @return int: 0 
 * \n Function was successful.
 * @return int:-1
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error. If error
 * is related to bad calendar name or time expression, the routine returns
 * the name or expression in badStr.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 *
 * @see \ref lsb_calendarinfo 
 */
extern int lsb_calendarop P_((int, int, char **, char *, char *, int, char **));
/**
 * \page lsb_puteventrec lsb_puteventrec
 * \brief Puts information of an eventRec structure pointed to by logPtr 
 * into a log file.
 *
 * \ref lsb_puteventrec puts information of an eventRec structure pointed to by
 * logPtr into a log file. log_fp is a pointer pointing to the log file name
 * that could be either event a log file or job log file.
 *
 * See \ref lsb_geteventrec for detailed information about parameters.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 *int lsb_puteventrec(FILE *log_fp, struct eventRec *logPtr)</b>
 *
 * @param *logPtr The eventRec structure pointed to by logPtr into a log file.
 * @param *log_fp A pointer pointing to the log file name that could be either
 * event a log file or job log file.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 * \n eventLog
 * \n xFile
 * \n jobAttrSetLog
 * \n logSwitchLog
 * \n dataLoggingLog
 * \n jgrpNewLog
 * \n jgrpCtrlLog
 * \n jgrpStatusLog
 * \n jobNewLog
 * \n jobModLog
 * \n jobStartLog
 * \n jobStartAcceptLog
 * \n jobExecuteLog
 * \n jobStatusLog
 * \n sbdJobStatusLog
 * \n sbdUnreportedStatusLog
 * \n jobSwitchLog
 * \n jobMoveLog
 * \n chkpntLog
 * \n jobRequeueLog
 * \n jobCleanLog
 * \n jobExceptionLog
 * \n sigactLog
 * \n migLog
 * \n signalLog
 * \n queueCtrlLog
 * \n hostCtrlLog
 * \n hgCtrlLog
 * \n mbdStartLog
 * \n mbdDieLog
 * \n unfulfillLog
 * \n jobFinishLog
 * \n loadIndexLog
 * \n calendarLog
 * \n jobForwardLog
 * \n jobAcceptLog
 * \n statusAckLog
 * \n jobMsgLog
 * \n jobMsgAckLog
 * \n jobOccupyReqLog
 * \n jobVacatedLog
 * \n jobForceRequestLog
 * \n jobChunkLog
 * \n jobExternalMsgLog
 * \n rsvRes
 * \n rsvFinishLog
 * \n cpuProfileLog
 * \n jobRunRusageLog
 * \n slaLog
 * \n perfmonLogInfo
 * \n perfmonLog
 * \n taskFinishLog
 * \n eventEOSLog
 * \n jobResizeNotifyStartLog
 * \n jobResizeNotifyAcceptLog
 * \n jobResizeNotifyDoneLog
 * \n jobResizeReleaseLog
 * \n jobResizeCancelLog
 * \n jobResizeLog
 * \n jRusage
 * \n jobProvisionStartLog
 * \n jobProvisionEndLog
 *
 * <b>Define Statements:</b>
 * \par
 * \ref event_types
 * \n \ref defs_lsb_XF_OP
 * \n \ref jobgroup_controltypes
 * \n \ref signal_action
 *
 * @return int:0 \n
 * The function was successful, and the information of eventRec structure has been
 * put into log file pointed by log_fp
 * @return int:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_SHAREDIR/cluster_name/logdir/lsb.events
 *
 * @see \ref lsb_geteventrec 
 */
extern int lsb_puteventrec P_((FILE *, struct eventRec *));
extern int lsb_puteventrecRaw P_((FILE *, struct eventRec *, char *));
/**
 * \page lsb_geteventrec lsb_geteventrec
 * \brief Get an event record from a log file
 *
 * \ref lsb_geteventrec returns an eventRec from a log file.
 * 
 * The storage for the eventRec structure returned by \ref lsb_geteventrec will be
 * reused by the next call.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct eventRec *lsb_geteventrec(FILE * log_fp,int * lineNum)</b>
 *
 * @param *log_fp Either an event log file or a job log file.
 * @param *lineNum The number of the event record. 
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 * \n eventLog
 * \n xFile
 * \n jobAttrSetLog
 * \n logSwitchLog
 * \n dataLoggingLog
 * \n jgrpNewLog
 * \n jgrpCtrlLog
 * \n jgrpStatusLog
 * \n jobNewLog
 * \n jobModLog
 * \n jobStartLog
 * \n jobStartAcceptLog
 * \n jobExecuteLog
 * \n jobStatusLog
 * \n sbdJobStatusLog
 * \n sbdUnreportedStatusLog
 * \n jobSwitchLog
 * \n jobMoveLog
 * \n chkpntLog
 * \n jobRequeueLog
 * \n jobCleanLog
 * \n jobExceptionLog
 * \n sigactLog
 * \n migLog
 * \n signalLog
 * \n queueCtrlLog
 * \n hostCtrlLog
 * \n hgCtrlLog
 * \n mbdStartLog
 * \n mbdDieLog
 * \n unfulfillLog
 * \n jobFinishLog
 * \n loadIndexLog
 * \n calendarLog
 * \n jobForwardLog
 * \n jobAcceptLog
 * \n statusAckLog
 * \n jobMsgLog
 * \n jobMsgAckLog
 * \n jobOccupyReqLog
 * \n jobVacatedLog
 * \n jobForceRequestLog
 * \n jobChunkLog
 * \n jobExternalMsgLog
 * \n rsvRes
 * \n rsvFinishLog
 * \n cpuProfileLog
 * \n jobRunRusageLog
 * \n slaLog
 * \n perfmonLogInfo
 * \n perfmonLog
 * \n taskFinishLog
 * \n eventEOSLog
 * \n jobResizeNotifyStartLog
 * \n jobResizeNotifyAcceptLog
 * \n jobResizeNotifyDoneLog
 * \n jobResizeReleaseLog
 * \n jobResizeCancelLog
 * \n jobResizeLog
 * \n jRusage
 * \n jobProvisionStartLog
 * \n jobProvisionEndLog
 *
 * <b>Define Statements:</b>
 * \par
 * \ref event_types
 * \n \ref defs_lsb_XF_OP
 * \n \ref jobgroup_controltypes
 * \n \ref signal_action
 *
 * @return struct eventRec *:\n A pointer to an eventRec which contains information 
 * on a job event and updates lineNum to point to the next line of the log file.
 * \n Function was successful.
 * @return NULL \n
 * Function failed.If there are no more records, returns NULL and sets
 * lsberrno to LSBE_EOF.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_SHAREDIR/cluster_name/logdir/lsb.acct
 * \n $LSB_SHAREDIR/cluster_name/logdir/lsb.events
 * \n $LSB_SHAREDIR/cluster_name/logdir/lsb.rsv.ids
 * \n $LSB_SHAREDIR/cluster_name/logdir/lsb.rsv.state
 *
 * @see \ref lsb_hostcontrol  
 * @see \ref lsb_movejob 
 * @see \ref lsb_pendreason  
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_queuecontrol 
 * @see \ref lsb_readjobinfo 
 * @see \ref lsb_submit 
 * @see \ref lsb_suspreason 
 */
extern struct eventRec *lsb_geteventrec P_((FILE *, int *));
extern struct  eventRec *lsb_geteventrec_decrypt(FILE *, 
               int *);

extern struct eventRec *lsb_geteventrecord(FILE *, int *);
extern struct eventRec *lsb_geteventrecordEx(FILE *, int *, char **);
extern struct eventRec *lsb_getnewjob_from_string(char *);

extern struct eventInfoEnt *lsb_eventinfo P_((char **, int *, char *));
/**
 * \page lsb_sharedresourceinfo lsb_sharedresourceinfo
 * \brief Returns the requested shared resource information in dynamic values.
 *
 * \ref lsb_sharedresourceinfo returns the requested shared resource information in
 * dynamic values. The result of this call is a chained data structure as 
 * defined in <lsf/lsbatch.h>, which contains requested information.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * LSB_SHARED_RESOURCE_INFO_T *lsb_sharedresourceinfo(
 *                     char **resources,
 *                     int *numResources,
 *                     char *hostName, int options)</b>
 *
 * @param **resources resources is an NULL terminated string array storing
 * requesting resource names.Setting resources to point to NULL returns all
 * shared resources.
 * @param *numResources numResources is an input/output parameter. On input
 * it indicates how many resources are requested. Value 0 means requesting 
 * all shared resources. On return it contains qualified resource number.
 * @param *hostName hostName is a string containing a host name. Only shared resource
 * available on the specified host will be returned. If hostName is a NULL,
 * shared resource available on all hosts will be returned.
 * @param options options is reserved for future use. Currently, it should be set to 0.
 *
 * <b>Data Structures:</b>
 * \par
 * lsbSharedResourceInfo
 * \n lsbSharedResourceInstance
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct lsbSharedResourceInfo *:\n A pointer to an LSB_SHARED_RESOURCE_INFO_T 
 * structure, which contains complete shared resource information.
 * \n Function was successful.
 * @return NULL \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
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
 * @see \ref ls_sharedresourceinfo 
 */
extern struct lsbSharedResourceInfo *lsb_sharedresourceinfo P_((char **, int *, char *, int));
/**
 * \page lsb_geteventrecbyline lsb_geteventrecbyline
 * Parse an event line and put the result in an event record structure.
 * The \ref lsb_geteventrecbyline function parses an event line and puts the result
 * in an event record structure.
 *
 * If the line to be parsed is a comment line, \ref lsb_geteventrecbyline sets errno to
 * bad event format and logs an error.
 *
 * To prevent the event record structure from being corrupted on subsequent calls 
 * to the function, use memory copy to allocate memory and store the event record structure.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_geteventrecbyline(char *line, struct eventRec *logRec)</b>
 *
 * @param *line
 * Buffer containing a line of event text string 
 * @param *logRec
 * Pointer to an eventRec structure
 *
 * <b>Data Structures:</b>
 * \par
  * eventRec
 * \n eventLog
 * \n xFile
 * \n jobAttrSetLog
 * \n logSwitchLog
 * \n dataLoggingLog
 * \n jgrpNewLog
 * \n jgrpCtrlLog
 * \n jgrpStatusLog
 * \n jobNewLog
 * \n jobModLog
 * \n jobStartLog
 * \n jobStartAcceptLog
 * \n jobExecuteLog
 * \n jobStatusLog
 * \n sbdJobStatusLog
 * \n sbdUnreportedStatusLog
 * \n jobSwitchLog
 * \n jobMoveLog
 * \n chkpntLog
 * \n jobRequeueLog
 * \n jobCleanLog
 * \n jobExceptionLog
 * \n sigactLog
 * \n migLog
 * \n signalLog
 * \n queueCtrlLog
 * \n hostCtrlLog
 * \n hgCtrlLog
 * \n mbdStartLog
 * \n mbdDieLog
 * \n unfulfillLog
 * \n jobFinishLog
 * \n loadIndexLog
 * \n calendarLog
 * \n jobForwardLog
 * \n jobAcceptLog
 * \n statusAckLog
 * \n jobMsgLog
 * \n jobMsgAckLog
 * \n jobOccupyReqLog
 * \n jobVacatedLog
 * \n jobForceRequestLog
 * \n jobChunkLog
 * \n jobExternalMsgLog
 * \n rsvRes
 * \n rsvFinishLog
 * \n cpuProfileLog
 * \n jobRunRusageLog
 * \n slaLog
 * \n perfmonLogInfo
 * \n perfmonLog
 * \n taskFinishLog
 * \n eventEOSLog
 * \n jobResizeNotifyStartLog
 * \n jobResizeNotifyAcceptLog
 * \n jobResizeNotifyDoneLog
 * \n jobResizeReleaseLog
 * \n jobResizeCancelLog
 * \n jobResizeLog
 * \n jRusage
 * \n jobFinish2Log
 * \n jobStartLimitLog
 * \n jobProvisionStartLog
 * \n jobProvisionEndLog
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * <b>Pre-Conditions:</b>
 * \par
 * The event record structure must have been initialized outside the
 * \ref lsb_geteventrecbyline function.
 *
 * @return int: 0
 * \n Function was successful.
 * @return int:-1
 * \n Function failed and lserrno was set.
 *
 * \b Errors:
 * \par
 * none
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see none
 */

extern int lsb_geteventrecbyline(char *, struct eventRec *);
/* Retain lsb_connect for now */
#define lsb_connect(a) lsb_rcvconnect()
extern int lsb_rcvconnect P_((void));
extern int lsb_sndmsg P_((struct lsbMsgHdr *, char *, int));
extern int lsb_rcvmsg P_((struct lsbMsgHdr *, char **, int));
/**
 * \page  lsb_runjob lsb_runjob
 * Starts a batch job immediately on a set of specified host().
 * \ref lsb_runjob starts a batch job immediately on a set of specified host(). 
 * The job must have been submitted and is in PEND or FINISHED status. Only
 * the LSF administrator or the owner of the job can start the job. If the
 * options is set to RUNJOB_OPT_NOSTOP, then the job will not be suspended by
 * the queue's RUNWINDOW,loadStop and STOP_COND and the hosts' RUNWINDOW and
 * loadStop conditions. By default, these conditions apply to the job as do
 * to other normal jobs.
 * 
 * Any program using this API must be setuid to root
 * if LSF_AUTH is not defined in the lsf.conf file.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_runjob(struct runJobRequest* runJobRequest)</b>
 *
 * @param runJobRequest 
 * The job-starting request.
 *
 * <b>Data Structures:</b>
 * \par
 * runJobRequest
 *
 * <b>Define Statements:</b>
 * \par
 * \ref runjob_option
 *
 * @return int:0 \n
 * Function was successful.
 * @return int:-1 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 *\par
 * brun
 *
 * \b Files:
 * \par
 * ${LSF_ENVDIR:-/etc}/lsf.conf
 *
 * @see none
 */
extern int lsb_runjob P_((struct runJobRequest*));

/* API for job group */
extern int lsb_addjgrp P_((struct jgrpAdd *, struct jgrpReply **));
extern int lsb_modjgrp P_((struct jgrpMod *, struct jgrpReply **));
extern int lsb_holdjgrp P_((char *, int, struct jgrpReply **));
extern int lsb_reljgrp P_((char *, int, struct jgrpReply **));
extern int lsb_deljgrp P_((char *, int, struct jgrpReply **));
extern int lsb_deljgrp_ext P_((struct jgrpCtrl *, struct jgrpReply **));
extern struct jgrp *lsb_listjgrp P_((int *));
extern struct serviceClass    *lsb_serviceClassInfo(int *);

/* API for Application Encapsulation */
extern struct appInfoEnt *lsb_appInfo P_((int *));
extern void   lsb_freeAppInfoEnts P_((int, struct appInfoEnt *));

/* routine to merge job Id and index into a string */
char * lsb_jobidindex2str(LS_LONG_INT jobId, int index);

/* routine to convert the job id to string */
extern char *lsb_jobid2str P_((LS_LONG_INT));
extern char *lsb_jobid2str_r P_((LS_LONG_INT, char[]));
extern char *lsb_jobidinstr P_((LS_LONG_INT));
extern char *lsb_jobidinstr_r P_((LS_LONG_INT, char[]));
/* routine to compose and decompose 64bit jobId */
extern void jobId32To64 P_((LS_LONG_INT*, int, int));
extern void jobId64To32 P_((LS_LONG_INT, int*, int*));
/* API for job attribute operations */
extern int lsb_setjobattr(int, struct jobAttrInfoEnt *);

/* API for remote task execution */
extern LS_LONG_INT lsb_rexecv(int, char **, char **, int *, int);
extern int lsb_catch(const char *, int (*)(void *));
extern void lsb_throw(const char *, void *);

/* API for job external message */

/** 
 * \page lsb_postjobmsg lsb_postjobmsg
 * \brief Sends messages and data posted to a job.
 * 
 * Use \ref lsb_postjobmsg to post a message and data to a job, open a TCP
 * connection, and transfer attached message and data from the mbatchd. Use
 * \ref lsb_readjobmsg to display messages and copy data files posted by
 * \ref lsb_postjobmsg.
 *
 * While you can post multiple messages and attached data files to a job,
 * you must call \ref lsb_postjobmsg for each message and attached data file
 * you want to post. By default, \ref lsb_postjobmsg posts a message to position
 * 0 of the message index (msgId) (see PARAMETERS) of the specified job. 
 * To post additional messages to a job, call \ref lsb_postjobmsg and increment
 * the message index.
 * 
 * \ref lsb_readjobmsg reads posted job messages by their
 * position in the message index.
 *
 * If a data file is attached to a message and the flag EXT_ATTA_POST is set,
 * use the JOB_ATTA_DIR parameter in lsb.params(5) to specify the directory
 * where attachment data fies are saved. The directory must have at least 1MB
 * of free space.The mbatchd checks for available space in the job attachment
 * directory before transferring the file.
 *
 * Use the MAX_JOB_ATTA_SIZE parameter in lsb.params(5) to set a maximum size
 * for job message attachments.
 *
 * Users can only send messages and data from their own jobs. Root and LSF
 * administrators can also send messages of jobs submtted by other users, but
 * they cannot attach data files to jobs owned by other users.
 *
 * You can post messages and data to a job until it is cleaned from the system. 
 * You cannot send messages and data to finished or exited jobs.
 *
 * <b>\#include <lsf/lsbatch.h> \n
 *    \#include <time.h>
 *
 * int lsb_postjobmsg(struct jobExternalMsgReq *jobExternalMsg,
 *                   char *filename)</b>
 *
 * @param *jobExternalMsg This structure contains the information required to 
 * define an external message of a job.
 * @param *filename Name of attached data file. If no file is attached, use NULL.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref jobExternalMsgReq
 *
 * <b>Define Statements:</b>
 * \par 
 * \ref external_msg_post
 *
 * @return int:value \n
 * The successful function returns a socket number.
 * @return int:0 \n
 * The EXT_ATTA_POST bit of options is not set or there is no attached data.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * bpost
 *
 * \b Files:
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 * \n $JOB_ATTA_DIR
 * \n $LSB_SHAREDIR/info/
 *
 * @see \ref lsb_readjobmsg 
 *
 */
extern int lsb_postjobmsg P_((struct jobExternalMsgReq *, char *));
/**
 * \page lsb_readjobmsg lsb_readjobmsg
 * \brief Reads messages and data posted to a job.
 *
 * Use \ref lsb_readjobmsg to open a TCP connection, receive attached messages and
 * data from the mbatchd, and display the messages posted by \ref lsb_postjobmsg.
 * 
 * By default, \ref lsb_readjobmsg displays the message "no description" or the
 * message at index position 0 of the specified job. To read other messages, 
 * choose another index position. The index is populated by \ref lsb_postjobmsg.
 *
 * If a data file is attached to a message and the flag EXT_ATTA_READ is set,
 * \ref lsb_readjobmsg gets the message and copies its data file to the default 
 * directory JOB_ATTA_DIR, overwriting the specified file if it already exists.
 * If there is no file attached, the system reports an error.
 *
 * Users can only read messages and data from their own jobs. Root and LSF
 * administrators can also read messages of jobs submtted by other users, 
 * but they cannot read data files attached to jobs owned by other users.
 * 
 * You can read messages and data from a job until it is cleaned from the
 * system. You cannot read messages and data from done or exited jobs.
 *
 * <b>\#include <lsf/lsbatch.h> \n
 * \#include <time.h> \n
 * int lsb_readjobmsg(struct jobExternalMsgReq *jobExternalMsg,
 *       	struct jobExternalMsgReply *jobExternalMsgReply)</b>
 *
 * @param *jobExternalMsg the information required to define an external
 * message of a job.
 * @param *jobExternalMsgReply the information required to define an
 * external message reply.
 *
 * <b>Data Structures:</b>
 * \par
 * jobExternalMsgReq
 * \n jobExternalMsgReply
 *
 * <b>Define Statements:</b>
 * \par
 * \ref external_msg_processing
 * \n \ref ext_data_status
 *
 * @return int:value \n
 * The successful function returns a socket number.
 * @return int:0 \n
 * The EXT_ATTA_READ bit of options is not set or there is no
 * attached data.
 * @return int:-1 \n 
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lserrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bread
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 * \n $JOB_ATTA_DIR
 * \n $LSB_SHAREDIR/info/
 *
 * @see \ref lsb_postjobmsg 
 */
extern int lsb_readjobmsg P_((struct jobExternalMsgReq *, struct jobExternalMsgReply *));

/* API for symphony job information update in bulk mode */
extern int lsb_bulkJobInfoUpdate P_((struct symJobStatusUpdateReqArray *, struct symJobStatusUpdateReplyArray *));

/* API for advance reservation */
/**
 * \page lsb_addreservation lsb_addreservation
 * \brief Makes an advance reservation.
 *
 * Use \ref lsb_addreservation to send a reservation request to mbatchd. If
 * mbatchd grants the reservation, it issues the reservation ID. If mbatchd
 * rejects the request, it issues NULL as the reservation ID.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_addreservation (struct addRsvRequest *request, char *rsvId)</b>
 *
 * @param *request The reservation request
 * @param *rsvId Reservation ID returned from mbatchd. If the reservation 
 * fails, this is NULL. The
 * memory for rsvid is allocated by the caller.
 *
 * <b>Data Structures:</b>
 * \par
 * addRsvRequest
 * \n _rsvExecCmd_t
 * \n _rsvExecEvent_t
 *
 * <b>Define Statements:</b>
 * \par
 * \ref reservation_option
 *
 * @return int:0 \n
 * The reservation was successful.
 * @return int:-1 \n
 * The reservation failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * brsvadd
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_removereservation 
 * @see \ref lsb_modreservation  
 * @see \ref lsb_reservationinfo  
 */
extern int lsb_addreservation P_((struct addRsvRequest *, char *));
/**
 * \page lsb_removereservation lsb_removereservation
 * \brief Removes a reservation.
 *
 * Use \ref lsb_removereservation to remove a reservation. mbatchd removes the
 * reservation with the specified reservation ID.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * int lsb_removereservation(char *rsvId)</b>
 *
 * @param *rsvId Reservation ID of the reservation that you wish to remove.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The reservation was removed successfully.
 * @return int:-1 \n
 * The reservation removal failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * brsvdel
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_addreservation 
 * @see \ref lsb_modreservation 
 * @see \ref lsb_reservationinfo 
 */
extern int lsb_removereservation P_((char *));
/**
 * \page lsb_reservationinfo lsb_reservationinfo
 * \brief Retrieve reservation information to display active advance reservations.
 *
 * Use \ref lsb_reservationinfo to retrieve reservation information from mbatchd.
 * This function allocates memory that the caller should free. 
 *
 * If the \ref lsb_reservationinfo function succeeds, it returns the reservation
 * records pertaining to a particular reservation ID (rsvId) as an array of
 * rsvInfoEnt structs.
 *
 * If rsvId is NULL, all reservation information will be returned. If a
 * particular rsvId  is specified:
 * \li If found, the reservation record pertaining to a particular rsvId is
 * returned
 * \li If not found, the number of reservation records is set to zero and
 * the lsberrno  is set appropiately
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct rsvInfoEnt *lsb_reservationinfo(char *rsvId, int *numEnts,
 *       		int options)</b>
 *
 *
 * @param *rsvId Reservation ID of the requested reservation.
 * @param *numEnts Number of reservation entries that mbatchd returns.
 * @param options The parameter options is currently ignored.
 *
 * <b>Data Structures:</b>
 * \par
 * rsvInfoEnt
 * \n hostRsvInfoEnt
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct rsvInfoEnt *:A pointer to reservation infomation entry.
 * \n Function was successful.
 * @return NULL 
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * brsvs
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_addreservation 
 * @see \ref lsb_modreservation 
 * @see \ref lsb_removereservation 
 */

extern struct rsvInfoEnt * lsb_reservationinfo P_((char *, int *, int));
extern int lsb_freeRsvExecCmd(rsvExecCmd_t **);
extern rsvExecCmd_t *lsb_dupRsvExecCmd(rsvExecCmd_t *);
extern int lsb_parseRsvExecOption(const char *, rsvExecCmd_t **);
/**
 * \page lsb_modreservation lsb_modreservation
 * \brief Modifies an advance reservation.
 *
 * Use \ref lsb_modreservation to modify an advance reservation. mbatchd receives
 * the modification request and modifies the reservation with the specified
 * reservation ID.
 *
 * <b>\#include <lsf/lsbatch.h>
 * 
 * int lsb_modreservation(struct modRsvRequest *request)</b>
 *
 * @param *request modify reservation request.
 *
 * <b>Data Structures:</b>
 * \par
 * modRsvRequest 
 * \n addRsvRequest
 * \n _rsvExecCmd_t
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The reservation was modified successfully.
 * @return int:-1 \n
 * The reservation modification failed.
 * 
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * brsvmod
 *
 * \b Files:
 * \par
 * none
 *
 * @see \ref lsb_addreservation  
 * @see \ref lsb_removereservation  
 * @see \ref lsb_reservationinfo  
 */

extern int lsb_modreservation(struct modRsvRequest *);

/** global fairshare
  **/
extern int lsb_globalpolicy(struct GlobalPolicyInfoRequest *);
		   
/* routines for sorted integer list */
struct sortIntList * initSortIntList(int);
int insertSortIntList(struct sortIntList *, int);
struct sortIntList * getNextSortIntList(struct sortIntList *, struct sortIntList *, int *);
void freeSortIntList(struct sortIntList *);
int getMinSortIntList(struct sortIntList *, int *);
int getMaxSortIntList(struct sortIntList *, int *);
int getTotalSortIntList(struct sortIntList *);

int updateJobIdIndexFile (char *, char *, int);

/* Structures and routine for obtaining subset of info about jobs 
 * This is being used by Maui integration.
 */
struct jobExtschInfoReq {
    int    qCnt;
    char **queues;
};

struct jobExtschInfo {
    LS_LONG_INT     jobId;
    int             status;
    time_t          jRusageUpdateTime;
    struct jRusage  runRusage;
};

struct jobExtschInfoReply {
    int                    jobCnt;
    struct jobExtschInfo **jobs;
};

extern int getjobinfo4queues(struct jobExtschInfoReq *,
            struct jobExtschInfoReply *);
extern void free_jobExtschInfoReply(struct jobExtschInfoReply *);
extern void free_jobExtschInfoReq(struct jobExtschInfoReq *);

/* For RFC 725 */
extern char *longer_strcpy(char *dest, const char *src);

/* Structures and API for job diagnostics.  These are applicable only if
 * CONDENSE_PENDING_REASONS is enabled in lsb.params.
 */
struct diagnoseJobReq {
    int          jobCnt;
    LS_LONG_INT  *jobId;
};

#define DIAGNOSIS_STOP  0x1
struct diagnoseQueryReq {
    char *logFileName; 	/* log file */
    int duration;   	/* Duration for the diagnosis feature to be ON */
    int options;    	/* '-o' option will set this feature OFF */
};	     

extern int lsb_diagnosejob(struct diagnoseJobReq *);

extern int lsb_diagnoseQuery(struct diagnoseQueryReq *);

#define  SIM_STATUS_RUN      0x01
#define  SIM_STATUS_SUSPEND  0x02

/* simulator status reply
 */
struct simStatusReply {
    int      simStatus;
    time_t   curTime;
    time_t   startTime;
};

extern struct simStatusReply *lsb_simstatus(void);
extern void free_simStatusReply(struct simStatusReply *);

/* batch command options flag for lease */
#define LSB_HOST_OPTION_EXPORT 0x1
/* bhosts -x option */
#define LSB_HOST_OPTION_EXCEPT 0x2
/* retrieve hosts that belong to batch partition */
#define LSB_HOST_OPTION_BATCH  0x4

#define LSB_HOST_OPTION_CONDENSED 0x08    /* Display condensed host output */

#define LSB_HOST_OPTION_JOBVM_IGN   0x10  /* ignore job vm when querying lsfhost*/
#define LSB_HOST_OPTION_ACHOST_IGN  0x20  /* ignore achost when querying lsfhost*/
#define LSB_HOST_OPTION_AFFINITY    0x40  /* bhosts -aff option */

/* error codes, structures and routines for syntax check of RMS external scheduler options */
#define	RMS_NON_RMS_OPTIONS_ERR  (-1)     /*  non-rms option shown up in RMS[] */
#define RMS_NODE_PTILE_ERR       (-2)     /*  nodes and ptile co-exist */
#define RMS_RAIL_RAILMASK_ERR    (-3)     /*  rails and railmask co-exist */
#define RMS_NODES_OUT_BOUND_ERR  (-4)     /*  nodes is out of range 1..LSB_RMS_MAXNUMNODES */
#define RMS_PTILE_OUT_BOUND_ERR  (-5)     /*  ptile is out of range 1..LSB_RMS_MAXPTILE */
#define RMS_RAIL_OUT_BOUND_ERR   (-6)     /*  rails is out of range 1..LSB_RMS_MAXNUMRAILS */
#define RMS_RAILMASK_OUT_BOUND_ERR  (-7)    /*  railmask syntax error */
#define RMS_NODES_SYNTAX_ERR     (-8)     /*  nodes syntax error */
#define RMS_PTILE_SYNTAX_ERR     (-9)     /*  ptile syntax error */
#define RMS_RAIL_SYNTAX_ERR      (-10)     /*  rails syntax error */
#define RMS_RAILMASK_SYNTAX_ERR  (-11)    /*  railmask syntax error */
#define RMS_BASE_SYNTAX_ERR  (-12)    /*  base syntax error */
#define RMS_BASE_TOO_LONG  (-13)    /*  base string too long*/
#define RMS_TOO_MANY_ALLOCTYPE_ERR  (-14) /*  >=1 allocation types are specified */
#define RMS_NO_LSF_EXTSCHED_Y_ERR  (-15) /*  =1 allocation types are specified */
#define RMS_READ_ENV_ERR          (-20)   /*  error reading env from lsf.conf inside syntax check */
#define RMS_MEM_ALLOC_ERR        (-21)    /*  memory allocation problems inside syntax check function */
#define RMS_BRACKETS_MISMATCH_ERR (-22)   /*  [] mis-matched in RMS[] */

typedef enum {
    RMS_ALLOC_TYPE_UNKNOWN = 0,
    RMS_ALLOC_TYPE_SLOAD,
    RMS_ALLOC_TYPE_SNODE,
    RMS_ALLOC_TYPE_MCONT
} rmsAllocType_t;

typedef enum {
    RMS_TOPOLOGY_UNKNOWN = 0,
    RMS_TOPOLOGY_PTILE,
    RMS_TOPOLOGY_NODES
} rmsTopology_t;

typedef enum {
    RMS_FLAGS_UNKNOWN = 0,
    RMS_FLAGS_RAILS,
    RMS_FLAGS_RAILMASK
} rmsFlags_t;

typedef struct rmsextschedoption {
    rmsAllocType_t alloc_type;
    rmsTopology_t  topology;
    int topology_value;
    int set_base;
    char base[MAXHOSTNAMELEN];
    rmsFlags_t flags;
    int flags_value;
} rmsExtschedOption_t;

extern int parseRmsOptions(char *, 
          rmsExtschedOption_t *, 
          struct config_param *);
/* Stream interface.
 * By default the stream lsb.stream is located in a subdirectory 
 * stream of the cluster working directory i.e.:
 * work/<clustername>/logdir/stream and the size of 
 * lsb.stream is 1024MB
 */
#define MBD_DEF_STREAM_SIZE (1024 * 1024 * 1024)
#define DEF_MAX_STREAM_FILE_NUMBER  10     /* default maximum number of backup stream.utc file */

/**
 * \brief  Stream interface.
 */
struct lsbStream {
    char   *streamFile;     /**< Pointer to full path to the stream file */
    int    maxStreamSize;   /**< Max size of the stream file */
    int    maxStreamFileNum;  /**< Max number of backup stream files */
    int    trace;           /**< Set to 1 to enable trace of the stream */
    int    (*trs)(const char *); /**< Pointer to a function that the library
                                  * invokes, passing a trace buffer. */
    char   *statusFile;           /**< Pointer to full path to the status file */
    char   *pendingreasonsFile;   /**< Pointer to full path to pendingreason file */
};


#if !defined(WIN32)
/**
 * \page lsb_openstream  lsb_openstream
 * \brief Open and create an lsb_stream file.
 *   
 * \ref lsb_openstream opens the streamFile .
 * 
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_openstream(const struct lsbStream *params)</b>
 * 
 * @param *params Parameters.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref lsbStream
 *
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return  int:0 \n
 * The function was successful.
 * @return int:-1 or NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestream 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_writestream 
 * @see \ref lsb_readstream 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_readstreamlineMT 
 * @see \ref lsb_freelogrec 
 *
 */
extern int   lsb_openstream(const struct lsbStream *);
/**
 * \page lsb_closestream lsb_closestream
 * \brief Close an lsb_stream file.
 *
 * \ref lsb_closestream closes the streamFile.
 * 
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_closestream(const char *config)</b>
 *
 * @param * config Pointer to the handle of the stream file.
 *
 * <b>Data Structures:</b>
 *\par
 *none
 *
 * <b>Define Statements:</b>
 * \par
 *none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_openstream 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_writestream 
 * @see \ref lsb_readstream 
 * @see \ref lsb_streamversion 
 */
extern int   lsb_closestream(const char *);
/**
 * \page lsb_streamversion lsb_streamversion
 * \brief Version of the current event type supported by mbatchd.
 *
 * \ref lsb_streamversion returns the event version number of mbatchd, which is the
 * version of the events to be written to the stream file. This API function
 * is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * char * lsb_streamversion(void)</b>
 *
 * @param void \n
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return char *:Pointer to a string of the current event version in mbatchd,
 * also known as THIS_VERSION.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestream 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstream 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_writestream 
 * @see \ref lsb_readstream 
 */
extern char  *lsb_streamversion(void);
 /**
 * \page lsb_writestream lsb_writestream
 * \brief Writes a current version eventRec structure into the lsb_stream file.
 *
 * \ref lsb_writestream writes an eventrRec to the open streamFile.
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_writestream(struct eventRec *logPtr)</b>
 *
 * @param *logPtr Pointer to the eventRec structure.
 * \n see \ref lsb_geteventrec for details on the eventRec structure.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 * \n eventLog
 * \n xFile
 * \n jobAttrSetLog
 * \n logSwitchLog
 * \n dataLoggingLog
 * \n jgrpNewLog
 * \n jgrpCtrlLog
 * \n jgrpStatusLog
 * \n jobNewLog
 * \n jobModLog
 * \n jobStartLog
 * \n jobStartAcceptLog
 * \n jobExecuteLog
 * \n jobStatusLog
 * \n sbdJobStatusLog
 * \n sbdUnreportedStatusLog
 * \n jobSwitchLog
 * \n jobMoveLog
 * \n chkpntLog
 * \n jobRequeueLog
 * \n jobCleanLog
 * \n jobExceptionLog
 * \n sigactLog
 * \n migLog
 * \n signalLog
 * \n queueCtrlLog
 * \n hostCtrlLog
 * \n hgCtrlLog
 * \n mbdStartLog
 * \n mbdDieLog
 * \n unfulfillLog
 * \n jobFinishLog
 * \n loadIndexLog
 * \n calendarLog
 * \n jobForwardLog
 * \n jobAcceptLog
 * \n statusAckLog
 * \n jobMsgLog
 * \n jobMsgAckLog
 * \n jobOccupyReqLog
 * \n jobVacatedLog
 * \n jobForceRequestLog
 * \n jobChunkLog
 * \n jobExternalMsgLog
 * \n rsvRes
 * \n rsvFinishLog
 * \n cpuProfileLog
 * \n jobRunRusageLog
 * \n slaLog
 * \n perfmonLogInfo
 * \n perfmonLog
 * \n taskFinishLog
 * \n eventEOSLog
 * \n jobResizeNotifyStartLog
 * \n jobResizeNotifyAcceptLog
 * \n jobResizeNotifyDoneLog
 * \n jobResizeReleaseLog
 * \n jobResizeCancelLog
 * \n jobResizeLog
 * \n jRusage
 * \n jobProvisionStartLog
 * \n jobProvisionEndLog
 *
 * <b>Define Statements:</b>
 * \par
 * \ref event_types
 * \n \ref defs_lsb_XF_OP
 * \n \ref jobgroup_controltypes
 * \n \ref signal_action
 *
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestream 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstream 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_readstream 
 */
extern int   lsb_writestream(struct eventRec *);
/**
 * \page lsb_readstream lsb_readstream
 * \brief Reads a current version eventRec structure from the lsb_stream file.
 *
 * \ref lsb_readstream reads an eventrRec from the open streamFile.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct eventRec lsb_readstream(int *nline)</b>
 *
 * @param *nline Line number in the stream file to be read.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestream 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstream 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_writestream 
 */
extern struct eventRec   *lsb_readstream(int *);
/**
 * \page lsb_readstreamline lsb_readstreamline
 * \brief Reads a current version eventRec structure from the lsb_stream file.
 *
 * \ref lsb_readstreamline reads an eventrRec from the open streamFile
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct eventRec *lsb_readstreamline(const char *line)</b>
 *
 * @param * line Line number in the stream file to be read.
 * See \ref lsb_puteventrec and \ref lsb_geteventrec for details on the eventRec structure.
 * Additionally, there are three additional event types supported.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct eventRec *: A pointer to eventRec \n
 * The function was successful.
 * @return NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestream 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstream 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstream 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_writestream 
 * @see \ref lsb_readstreamlineMT 
 * @see \ref lsb_freelogrec 
 */
extern struct eventRec   *lsb_readstreamline(const char *);
/**
 * \page lsb_readstreamlineMT lsb_readstreamlineMT
 * \brief Reads a current version eventRec structure from the lsb_stream file.
 *
 * \ref lsb_readstreamlineMT reads an eventrRec from the open streamFile. 
 * The caller is responsible for freeing the memory allocated for the 
 * eventRec, using lsb_freelogrec.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct eventRec *lsb_readstreamline(const char *line)</b>
 *
 * @param * line Line number in the stream file to be read.
 * See \ref lsb_puteventrec and \ref lsb_geteventrec for details on the eventRec structure.
 * Additionally, there are three additional event types supported.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct eventRec *: A pointer to eventRec \n
 * The function was successful.
 * @return NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestream 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstream 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstream 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_writestream 
 * @see \ref lsb_freelogrec 
 */
extern struct eventRec   *lsb_readstreamlineMT(const char *);
/**
 * \page lsb_freelogrec lsb_freelogrec
 * \brief Frees up the memory allocated for the pass in eventRec object.
 *
 * \ref lsb_freelogrec frees memory allocated for an eventRec object.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * void lsb_freelogrec(struct eventRec* logrec)</b>
 *
 * @param * The eventRec to be freed.
 * See \ref lsb_puteventrec and \ref lsb_geteventrec for details on the eventRec structure.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return void
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestream 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstream 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstream 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_writestream 
 * @see \ref lsb_readstreamlineMT 
 */
extern void lsb_freelogrec(struct eventRec*);
#else
__declspec(dllexport) int   lsb_openstream(const struct lsbStream *);
__declspec(dllexport) int   lsb_closestream(const char *);
__declspec(dllexport) char  *lsb_streamversion(void);
__declspec(dllexport) int   lsb_writestream(struct eventRec *);
__declspec(dllexport) struct eventRec  *lsb_readstream(int *);

__declspec(dllexport) struct eventRec  *lsb_readstreamline(const char *);
__declspec(dllexport) struct eventRec  *lsb_readstreamlineMT(const char *, struct eventRec*);
__declspec(dllexport) void  lsb_freelogrec(struct eventRec*);
#endif

#if !defined(WIN32)
/**
 * \page lsb_openstatus  lsb_openstatus
 * \brief Open and create an lsb.status file.
 *   
 * \ref lsb_openstatus opens the statusFile .
 * 
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_openstatus(const struct lsbStream *params)</b>
 * 
 * @param *params Parameters.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref lsbStream
 *
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return  int:0 \n
 * The function was successful.
 * @return int:-1 or NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestatus 
 * @see \ref lsb_readstatusline 
 * @see \ref lsb_writestatus 
 * @see \ref lsb_readstatus 
 *
 */
extern int   lsb_openstatus(const struct lsbStream *);
/**
 * \page lsb_closestatus lsb_closestatus
 * \brief Close an lsb_status file.
 *
 * \ref lsb_closestatus closes the statusFile.
 * 
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_closestatus(const char *config)</b>
 *
 * @param * config Pointer to the handle of the status file.
 *
 * <b>Data Structures:</b>
 *\par
 *none
 *
 * <b>Define Statements:</b>
 * \par
 *none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_openstatus 
 * @see \ref lsb_readstatusline 
 * @see \ref lsb_writestatus 
 * @see \ref lsb_readstatus 
 */
extern int   lsb_closestatus(const char *);
 /**
 * \page lsb_writestatus lsb_writestatus
 * \brief Writes a current version eventRec structure into the lsb_status file.
 *
 * \ref lsb_writestatus writes an eventrRec to the open statusFile.
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_writestatus(struct eventRec *logPtr)</b>
 *
 * @param *logPtr Pointer to the eventRec structure.
 * \n see \ref lsb_geteventrec for details on the eventRec structure.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 * \n eventLog
 * \n xFile
 * \n jobAttrSetLog
 * \n logSwitchLog
 * \n dataLoggingLog
 * \n jgrpNewLog
 * \n jgrpCtrlLog
 * \n jgrpStatusLog
 * \n jobNewLog
 * \n jobModLog
 * \n jobStartLog
 * \n jobStartAcceptLog
 * \n jobExecuteLog
 * \n jobStatusLog
 * \n sbdJobStatusLog
 * \n sbdUnreportedStatusLog
 * \n jobSwitchLog
 * \n jobMoveLog
 * \n chkpntLog
 * \n jobRequeueLog
 * \n jobCleanLog
 * \n jobExceptionLog
 * \n sigactLog
 * \n migLog
 * \n signalLog
 * \n queueCtrlLog
 * \n hostCtrlLog
 * \n hgCtrlLog
 * \n mbdStartLog
 * \n mbdDieLog
 * \n unfulfillLog
 * \n jobFinishLog
 * \n loadIndexLog
 * \n calendarLog
 * \n jobForwardLog
 * \n jobAcceptLog
 * \n statusAckLog
 * \n jobMsgLog
 * \n jobMsgAckLog
 * \n jobOccupyReqLog
 * \n jobVacatedLog
 * \n jobForceRequestLog
 * \n jobChunkLog
 * \n jobExternalMsgLog
 * \n rsvRes
 * \n rsvFinishLog
 * \n cpuProfileLog
 * \n jobRunRusageLog
 * \n slaLog
 * \n perfmonLogInfo
 * \n perfmonLog
 * \n taskFinishLog
 * \n eventEOSLog
 * \n jobResizeNotifyStartLog
 * \n jobResizeNotifyAcceptLog
 * \n jobResizeNotifyDoneLog
 * \n jobResizeReleaseLog
 * \n jobResizeCancelLog
 * \n jobResizeLog
 * \n jRusage
 *
 * <b>Define Statements:</b>
 * \par
 * \ref event_types
 * \n \ref defs_lsb_XF_OP
 * \n \ref jobgroup_controltypes
 * \n \ref signal_action
 *
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestatus 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstatus 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstatusline 
 * @see \ref lsb_readstatus 
 */
extern int   lsb_writestatus(struct eventRec *);
/**
 * \page lsb_readstatus lsb_readstatus
 * \brief Reads a current version eventRec structure from the lsb_status file.
 *
 * \ref lsb_readstatus reads an eventrRec from the open statusFile.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct eventRec lsb_readstatus(int *nline)</b>
 *
 * @param *nline Line number in the status file to be read.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestatus 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstatus 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstatusline 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_writestatus 
 */
extern struct eventRec   *lsb_readstatus(int *);
/**
 * \page lsb_readstatusline lsb_readstatusline
 * \brief Reads a current version eventRec structure from the lsb_status file.
 *
 * \ref lsb_readstatusline reads an eventrRec from the open statusFile
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct eventRec *lsb_readstatusline(const char *line)</b>
 *
 * @param * line Line number in the status file to be read.
 * See \ref lsb_puteventrec and \ref lsb_geteventrec for details on the eventRec structure.
 * Additionally, there are three additional event types supported.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return struct eventRec *: A pointer to eventRec \n
 * The function was successful.
 * @return NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closestatus 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openstatus 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstatus 
 * @see \ref lsb_statusversion 
 * @see \ref lsb_writestatus 
 */
extern struct eventRec   *lsb_readstatusline(const char *);
#else
__declspec(dllexport) int   lsb_openstatus(const struct lsbStream *);
__declspec(dllexport) int   lsb_closestatus(const char *);
__declspec(dllexport) int   lsb_writestatus(struct eventRec *);
__declspec(dllexport) struct eventRec  *lsb_readstatus(int *);

__declspec(dllexport) struct eventRec  *lsb_readstatusline(const char *);
#endif

#if !defined(WIN32)
/**
 * \page lsb_openpendingreasons  lsb_openpendingreasons
 * \brief Open and create an lsb.pendingreasons file.
 *   
 * \ref lsb_openpendingreasons opens the pendingreasons .
 * 
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_openpendingreasons(const struct lsbStream *params)</b>
 * 
 * @param *params Parameters.
 *
 * <b>Data Structures:</b>
 * \par
 * \ref lsbStream
 *
 * <b>Define Statements:</b>
 * \par
 * none
 * 
 * @return  int:0 \n
 * The function was successful.
 * @return int:-1 or NULL \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closependingreasons 
 * @see \ref lsb_readpendingreasons 
 * @see \ref lsb_writependingreasons 
 * @see \ref lsb_readpendingreasons 
 *
 */
extern int   lsb_openpendingreasons(const struct lsbStream *);
/**
 * \page lsb_closependingreasons lsb_closependingreasons
 * \brief Close an lsb_pendingreasons file.
 *
 * \ref lsb_closependingreasons closes the pendingreasonsFile.
 * 
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_closependingreasons(const char *config)</b>
 *
 * @param * config Pointer to the handle of the pendingreasons file.
 *
 * <b>Data Structures:</b>
 *\par
 *none
 *
 * <b>Define Statements:</b>
 * \par
 *none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_openpendingreasons 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_writependingreasons 
 * @see \ref lsb_readpendingreasons 
 */
extern int   lsb_closependingreasons(const char *);
 /**
 * \page lsb_writependingreasons lsb_writependingreasons
 * \brief Writes a current version eventRec structure into the lsb_pendingreasons file.
 *
 * \ref lsb_writependingreasons writes an eventrRec to the open pendingreasonsFile.
 * This API function is inside liblsbstream.so.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_writependingreasons(struct eventRec *logPtr)</b>
 *
 * @param *logPtr Pointer to the eventRec structure.
 * \n see \ref lsb_geteventrec for details on the eventRec structure.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 * \n eventLog
 * \n eventEOSLog
 * \n jobPendingReasonsLog
 *
 * <b>Define Statements:</b>
 * \par
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closependingreasons 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openpendingreasons 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_readpendingreasons 
 */
extern int   lsb_writependingreasons(struct eventRec *);
/**
 * \page lsb_readpendingreasons lsb_readpendingreasons
 * \brief Reads a current version eventRec structure from the lsb_pendingreasons file.
 *
 * \ref lsb_readpendingreasons reads an eventrRec from the open pendingreasonsFile.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct eventRec lsb_readpendingreasons(int *nline)</b>
 *
 * @param *nline Line number in the pendingreasons file to be read.
 *
 * <b>Data Structures:</b>
 * \par
 * eventRec
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * The function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * On failure, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.params
 *
 * @see \ref lsb_closependingreasons 
 * @see \ref lsb_geteventrec 
 * @see \ref lsb_openpendingreasons 
 * @see \ref lsb_puteventrec 
 * @see \ref lsb_readstreamline 
 * @see \ref lsb_streamversion 
 * @see \ref lsb_writependingreasons 
 */
extern struct eventRec   *lsb_readpendingreasons(int *);
#else
__declspec(dllexport) int   lsb_openpendingreasons(const struct lsbStream *);
__declspec(dllexport) int   lsb_closependingreasons(const char *);
__declspec(dllexport) int   lsb_writependingreasons(struct eventRec *);
__declspec(dllexport) struct eventRec  *lsb_readpendingreasons(int *);

#endif

#define NUM_EXITRATE_TYPES        4

/* options for exit rate type */

#define JOB_EXIT 0x01   /* all exited jobs */
#define JOB_INIT 0x02   /* jobs failed to start due to initialization problem
                         * on execution host
                      */
#define HPC_INIT 0x04   /* jobs failed to start due to HPC specific initialization
                         * problem on execution host
                      */
#define JOB_EXIT_NONLSF 0x08  /* jobs exited not related to reasons set by LSF */

/**
 * \brief  APS factor information
 */
struct apsFactorInfo {
    char *name;      /**<  Name */
    float weight;    /**<  Weight */
    float limit;     /**<  Limit */
    int gracePeriod; /**<  Grace period */
};

/* options for job group delete */
#define JGRP_DEL_USER_GROUPS     0x01 /* delete the specified user's all empty
                                       * job groups
                                    */
#define JGRP_DEL_CHILD_GROUPS 0x02 /* delete one job group's all empty children
                                       * groups including itself
                                    */
#define JGRP_DEL_ALL             0x04 /* delete all empty job groups */

/**------------------------------------------------------------------------
 * lsb_getallocFromHhostfile
 *
 * Read the specified hostfile and return the host list. If path is NULL
 * then read the hostfile specified by LSB_DJOB_HOSTFILE. The hostfile
 * is assumed to be in simple format of one host per line. A host
 * can be repeated.
 *
 * This function will allocate the memory for hostlist.
 * It is the responsibility of the caller to free the lists when no longer 
 * needed. On success hostlist will be a list of strings.
 * Before freeing hostlist the individual 
 * elements must be freed.
 *
 * Parameters:
 *    hostlist  [OUT]
 *    path      [IN]    path to hostfile, if NULL check LSB_DJOB_HOSTFILE
 *
 * Return Value:
 *   >0    success, length of hostlist not including the null last element
 *   -1    failure, lsberrno is set
 *-------------------------------------------------------------------------
 */
extern int lsb_getallocFromHostfile P_((char ***, char *));


/*
 * for status info reply
 */
/*PKVPs key*/
#define LAST_BOOT_TIME "Last boot time"
#define LAST_RECONFIG_TIME "Last reconfig time"
#define NUM_HOSTS "Total number of hosts"
#define NUM_OK_HOSTS "Number of OK hosts"
#define NUM_CLOSED_HOSTS "Number of closed hosts"
#define NUM_UNREACH_HOSTS "Number of unreach hosts"
#define NUM_UNAVAIL_HOSTS "Number of unavail hosts"
#define NUM_JOBS "Total number of jobs"
#define NUM_RUN_JOBS "Total number of run jobs"
#define NUM_PEND_JOBS "Total number of pend jobs"
#define NUM_FINISHED_JOBS "Total number of finish jobs"
#define NUM_SUSP_JOBS "Total number of suspend jobs"
#define NUM_USERS "Total number of users"
#define NUM_USER_GROUPS "Total number of user groups"
#define NUM_ACTIVE_USERS "Total number of active users"
#define CURRENT_MBD_PID "Current MBD pid"
#define STR_CURRENT_CLIENTS "Total number of clients"
#define STR_PEAK_CLIENTS "Peak number of clients"
#define STR_CURRENT_SERVERS "Total number of servers"
#define STR_PEAK_SERVERS "Peak number of servers"
#define STR_CURRENT_CPUS    "Total number of CPUs"
#define STR_PEAK_CPUS    "Peak number of CPUs"
#define STR_CURRENT_CORES   "Total number of cores"
#define STR_PEAK_CORES   "Peak number of cores"
#define STR_CURRENT_SLOTS   "Total number of slots"
#define STR_PEAK_SLOTS   "Peak number of slots"



/*
 * for status info reply
 */
/*PKVPs key*/
#define LAST_BOOT_TIME "Last boot time"
#define LAST_RECONFIG_TIME "Last reconfig time"
#define NUM_HOSTS "Total number of hosts"
#define NUM_OK_HOSTS "Number of OK hosts"
#define NUM_CLOSED_HOSTS "Number of closed hosts"
#define NUM_UNREACH_HOSTS "Number of unreach hosts"
#define NUM_UNAVAIL_HOSTS "Number of unavail hosts"
#define NUM_JOBS "Total number of jobs"
#define NUM_RUN_JOBS "Total number of run jobs"
#define NUM_PEND_JOBS "Total number of pend jobs"
#define NUM_FINISHED_JOBS "Total number of finish jobs"
#define NUM_SUSP_JOBS "Total number of suspend jobs"
#define NUM_USERS "Total number of users"
#define NUM_USER_GROUPS "Total number of user groups"
#define NUM_ACTIVE_USERS "Total number of active users"
#define CURRENT_MBD_PID "Current MBD pid"
#define STR_CURRENT_CLIENTS "Total number of clients"
#define STR_PEAK_CLIENTS "Peak number of clients"
#define STR_CURRENT_SERVERS "Total number of servers"
#define STR_PEAK_SERVERS "Peak number of servers"
#define STR_CURRENT_CPUS    "Total number of CPUs"
#define STR_PEAK_CPUS    "Peak number of CPUs"
#define STR_CURRENT_CORES   "Total number of cores"
#define STR_PEAK_CORES   "Peak number of cores"
#define STR_CURRENT_SLOTS   "Total number of slots"
#define STR_PEAK_SLOTS   "Peak number of slots"
#define STR_CURRENT_DC_SERVERS "Total number of DC servers"
#define STR_PEAK_DC_SERVERS "Peak number of DC servers"
#define STR_CURRENT_DC_CORES   "Total number of DC cores"
#define STR_PEAK_DC_CORES   "Peak number of DC cores"
#define STR_DC_JOB_CONTAINERS "DC VM Job Containers"
#define STR_PEAK_DC_JOB_CONTAINERS "Peak DC VM Job Containers"

struct statusInfo {
    time_t lastBootTime;
    time_t lastReconfigTime;

    int numHosts;
    int numOKHosts;
    int numClosedHosts;
    int numUnreachHosts;
    int numUnavailHosts;

    int numJobs;
    int numRunJobs;
    int numSuspendJobs;
    int numPendJobs;
    int numFinishedJobs;

    int numUsers;
    int numUserGroups;
    int numActiveUsers;

    int pid;
    time_t newStartupTime;
    int newpid;

    int numClients;
    int maxNumClients;
    int numServers;
    int maxNumServers;

    int numCPUs;
    int maxNumCPUs;
    int numCores;
    int maxNumCores;
    int numSlots;
    int maxNumSlots;

    /* Dynamic Cluster */
    int numDCServers;
    int maxNumDCServers;
    int numDCCores;
    int maxNumDCCores;
    int numDCJobContainers;
    int maxNumDCJobContainers;
};

struct queryJobIds {
    int numMapped;
    LS_LONG_INT *mappedJobIds;
};

#define MAX_NUM_JOB_IDS  100

/* API function to get status info */
extern int lsb_getmbdstatus(struct statusInfo *statusInfoReply);


#if defined(SR1_PJOBS)

/**
 * \addtogroup defs_lsb_launch defs_lsb_launch
 * lsb_launch() Valid options are:
 */

/*@{*/
#define LSF_DJOB_DISABLE_STDIN  0x01  /**< Disable standard input and
                                       * redirect input from the special 
       		                       * device /dev/null. This is equivalent
       		                       * to blaunch -n.*/
#define LSF_DJOB_REPLACE_ENV    0x02  /**< Replace existing enviornment
                                       * variable values with envp.*/
#define LSF_DJOB_NOWAIT         0x04  /**< Non-blocking mode; the parallel
                                       * job does not wait once all tasks start. 
       		                       * This forces \ref lsb_launch not to wait
       		                       * for its tasks to finish.*/
#define LSF_DJOB_STDERR_WITH_HOSTNAME 0x08  /**< Display standard error
                                             * messages with a corresponding
                                             * host name where the message was
                                             * generated.Cannot be specified
                                             * with LSF_DJOB_NOWAIT.*/
#define LSF_DJOB_STDOUT_WITH_HOSTNAME 0x10 /**< Display standard output messages
                                            * with a corresponding host name 
       		                            * where the message was generated.
       		                            * Cannot be specified with
       		                            * LSF_DJOB_NOWAIT.*/
#define LSF_DJOB_USE_LOGIN_SHELL      0x20  /**< Use user's login shell to 
                                             * launch tasks */
#define LSF_DJOB_USE_BOURNE_SHELL     0x40  /**< Use /bin/sh to launch tasks */
#define LSF_DJOB_STDERR               0x80  /**< Separate stderr from stdout */
#define LSF_DJOB_POE                  0x100  /**< POE job */
/*@}*/

/*
 *-------------------------------------------------------------------------
 * lsb_launch (where, argv, options, envp)
 *
 * DESCRIPTION:
 *
 *   The specified command (i.e., argv) will be launched on the remote
 *   nodes in parallel
 *
 * ARGUMENTS:
 *   where [IN]:
 *       A NULL terminated list of hosts.
 *       If this parameter is NULL then the environment variable
 *       LSB_MCPU_HOSTS will be used.
 *       A task will be launched for each slot.
 *   options [IN]:
 *       options value obtained by ORing
 *   Envp [IN]:
 *       A Null terminated list of environment variables (in 'variable=value'
 *       format).
 *       The environment to set for each task.
 *       If this parameter is NULL then the same environment used to start
 *       the first task will be used.
 *       If non-NULL, it is appended to the environment used for the
 *       first task.
 *       If LSF_DJOB_REPLACE_ENV is specified, Envp entries will overwrite
 *       existing values except those LSF needs.
 *
 * RETURN:
 *   < 0 on failure
 *   > 0 upon success (i.e., number of tasks issued)
 *
 */
/**
 * \page lsb_launch lsb_launch
 * \brief  Launch commands on remote hosts in parallel.
 *
 * \ref lsb_launch is a synchronous API call to allow source level integration with
 * vendor MPI implementations. This API will launch the specified command (argv)
 * on the remote nodes in parallel.
 * \n LSF must be installed before integrating your MPI implementation with
 * \ref lsb_launch. The \ref lsb_launch API requires the full set of liblsf.so,
 * libbat.so (or liblsf.a, libbat.a).
 *
 * <b>\#include <lsf/lsbatch.h>
 * 
 * int lsb_launch (char** where, char** argv, int userOptions, char** envp)</b>
 *
 * @param **where [IN] A NULL-terminated list of hosts. A task will be launched
 * for each slot.If this parameter is NULL then the environment variable
 * LSB_MCPU_HOSTS will be used.
 * @param **argv [IN] The command to be executed
 * @param userOptions [IN] Options to modify the behavior of \ref lsb_launch
 * Multiple option values can be specified. For example option values can be
 * separated by OR (|):
 * \n \ref lsb_launch (where, argv, LSF_DJOB_REPLACE_ENV | LSF_DJOB_DISABLE_STDIN, envp);
 * @param **envp [IN] A NULL-terminated list of environment variables specifying
 * the environment to set for each task.If envp is NULL, \ref lsb_launch uses the
 * same environment used to start the first task on the first execution host.
 * If non-NULL, envp values are appended to the environment used for the first
 * task.If the LSF_DJOB_REPLACE_ENV option is specified, envp entries will
 * overwrite all existing environment values except those needed by LSF.
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * \ref defs_lsb_launch
 *
 * @return > 0 \n
 * Function was successful (the number of tasks launched).
 * @return < 0 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * blaunch
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see none
 */
extern int lsb_launch P_((char**, char**, int, char**));
extern int lsb_launch_poe(char** , char** , int , int *);

/*
 *-------------------------------------------------------------------------
 * lsb_getalloc
 *
 * This function will allocate the memory for hostlist.
 *
 * It is the responsibility of the caller to free the lists when no longer
 * needed. On success hostlist will be a list of strings.
 * Before freeing hostlist the individual
 * elements must be freed.
 *
 * Parameters:
 *    hostlist     [OUT]     null terminated list of hosts
 *
 * Returns:
 *   >0    success, length of hostlist not including the null last element
 *   -1    failure, lsberrno is set
 *
 *-------------------------------------------------------------------------
 */
/**
 * \page lsb_getalloc lsb_getalloc
 * \brief Allocates memory for a host list to be used for launching parallel
 * tasks through blaunch and the \ref lsb_launch API.
 *
 * It is the responsibility of the caller to free the host list when it is
 * no longer needed.On success, the host list will be a list of strings.
 * Before freeing host list, the individual elements must be freed.
 *
 * An application using the \ref lsb_getalloc API is assumed to be part of an
 * LSF job, and that LSB_MCPU_HOSTS is set in the environment.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * int lsb_getalloc(char ***hostlist)</b>
 *
 * @param ***hostlist [OUT] A null-terminated list of host names
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return > 0 \n
 * Function was successful. Returns length of hostlist, not including the null
 * last element.
 * @return < 0 \n
 * Function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see none
 */
extern int lsb_getalloc P_((char ***));

#endif /* SR1_PJOBS */

/*
 * for status info reply
  */
  /*PKVPs key*/
  #define LAST_BOOT_TIME "Last boot time"
  #define LAST_RECONFIG_TIME "Last reconfig time"
  #define NUM_HOSTS "Total number of hosts"
  #define NUM_UNLICENSED_HOSTS "Number of Unlicensed hosts"
  #define NUM_OK_HOSTS "Number of OK hosts"
  #define NUM_CLOSED_HOSTS "Number of closed hosts"
  #define NUM_UNREACH_HOSTS "Number of unreach hosts"
  #define NUM_UNAVAIL_HOSTS "Number of unavail hosts"
  #define NUM_JOBS "Total number of jobs"
  #define NUM_RUN_JOBS "Total number of run jobs"
  #define NUM_PEND_JOBS "Total number of pend jobs"
  #define NUM_FINISHED_JOBS "Total number of finish jobs"
  #define NUM_SUSP_JOBS "Total number of suspend jobs"
  #define NUM_USERS "Total number of users"
  #define NUM_USER_GROUPS "Total number of user groups"
  #define NUM_ACTIVE_USERS "Total number of active users"
  #define CURRENT_MBD_PID "Current MBD pid"
  #define NEW_STARTUP_TIME "New MBD startup time"
  #define NEW_MBD_PID "New MBD pid"
  #define NUM_DC_JOB_CONTAINERS "Number of DC VM Job Containers"


/**
 * \page lsb_resize_cancel lsb_resize_cancel
 * \brief Cancels a pending job resize allocation request.
 *
 * Use \ref lsb_resize_cancel to cancel a pending allocation request for a
 * resizable job. A running job can only have one pending request at any 
 * particular time. If one request is still pending, additional requests
 * are rejected with a proper error code.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_resize_cancel(LS_LONG_INT jobId);</b>
 *
 * @param jobId LSF job ID
 *
 * <b>Data Structures:</b>
 * \par
 * none
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * On success, returns zero.
 * @return int:-1 \n
 * On failure, returns -1.
 *
 * \b Errors:
 * \par
 * lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bresize cancel job_ID
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_resize_release 
*/

extern int lsb_resize_cancel P_((LS_LONG_INT));
/**
 * \page lsb_resize_release lsb_resize_release
 * \brief Releases part of the allocation of a running resizable job.
 *
 * Use \ref lsb_resize_release to release part of a running job allocation.
 * A running job can only have one pending request at any particular time. 
 * If one request is still pending, additional requests are rejected with
 * a proper error code.
 *
 * If a notification command is defined through job submission, application
 * profile,or the \ref lsb_resize_release API, the notification command is invoked
 * on the first execution host of the job allocation once allocation resize
 * requests have been satisfied.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_resize_release(struct job_resize_release *req);</b>
 *
 * @param *req job resize release request. 
 *
 * <b>Data Structures:</b>
 * \par
 * job_resize_release
 *
 * <b>Define Statements:</b>
 * \par
 * \ref resizablejob_related
 *
 * @return int:0 \n
 * On success, returns zero.
 * @return int:-1 \n
 * On failure, returns -1.
 *
 * \b Errors:
 * \par
 * lsberrno is set to indicate the error.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * release [-c] [-rnc resize_notification_cmd | -rncn] released_host_specification job_ID
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see \ref lsb_resize_cancel 
 */
extern int lsb_resize_release P_((struct job_resize_release *));
extern int lsb_resize_request P_((struct job_resize_request *));
/**
 * \page  lsb_getjobdepinfo lsb_getjobdepinfo
 * Returns the job dependency information.
 *
 * \ref lsb_getjobdepinfo returns information about jobs (including job arrays) when
 * a job has one or more dependencies on it.
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * struct jobDependInfo *
 * lsb_getjobdepinfo(struct jobDepRequest *jobdepReq)</b>
 *
 * @param *jobdepReq Job dependent Request.
 * 
 * <b>Data Structures:</b>
 * \par
 *  dependJobs
 * \n queriedJobs
 * \n jobDependInfo
 * \n jobDepRequest
 *
 * <b>Define Statements:</b>
 * \par
 * \ref job_has_depend
 * \n \ref query_depend
 *
 * @return struct jobDependInfo *:A pointer to job dependency information.
 * \n Function was successful.
 * @return NULL
 * \n Function failed.
 *
 * \b Errors:
 * \par
 * none
 *
 * <b>Equivalent line commands:</b>
 * \par
 * none
 *
 * <b>Files:</b>
 * \par
 * none
 *
 * @see none
 */
extern struct jobDependInfo * lsb_getjobdepinfo P_((struct jobDepRequest *));

/**
 * \brief guaranteed resource pool information request
 */
typedef struct _guaranteedResourcePoolInfoReq {
    int      poolsC;         /**< Number of guaranteed resource pools requested, 0 implies all*/
    char     **poolNames;    /**< Guaranteed resource pools being requested*/
    char     *queueName;     /**< Queue name to be verified */
} guaranteedResourcePoolInfoReq;


/**
 * \brief guaranteed resource pool consumer information
 */
typedef struct _guarConsumer {
    char           *name;      /**< Consumer name, a guarantee SLA*/
    int            share;      /**< Share distribution*/

#define GUAR_CONSUMER_SHARE_TYPE_ABSOLUTE      1
#define GUAR_CONSUMER_SHARE_TYPE_PERCENT       2
    int            shareType;  /**< Share type, absolute or percent*/
    int            deserved;   /**< Number of guaranteed resources*/
    int            guarUsed;   /**< Number of guaranteed resources used*/
    int            totalUsed;  /**< Total number of resources used*/
} guarConsumer;


/**
 * \brief guaranteed resource pool host information
 */
typedef struct _guarHost {
    char *name;

#define GUARHOST_FLAGS_UNAVAIL    0x01
    int flags;
    int total;
    int avail;
    int consumerUse;
    int otherUse;
} guarHost;

typedef struct _preempt_info_t {
    int nQueues;     
    char **queues;  /* preemptive queues */
    int *amounts;   /* amount avail by preemption */
} PREEMPT_INFO_T;


/**
 * \brief guaranteed resource pool information
 */
typedef struct _guaranteedResourcePoolEnt {
    char           *name;             /**< Guaranteed resource pool name*/
    char           *description;      /**< Guaranteed resource pool description*/

#define GUAR_RESOURCE_POOL_TYPE_SLOTS          1
#define GUAR_RESOURCE_POOL_TYPE_HOSTS          2
#define GUAR_RESOURCE_POOL_TYPE_RESOURCE       3
#define GUAR_RESOURCE_POOL_TYPE_PACKAGE        4
    int            type;              /**< Resource type; hosts or slots*/
    char           *rsrcName;         /**< Name of the shared resource */
    int            slotsPerHost;      /**< Slots per host is obsolete after LSF 9.1 but remains for compatibility.*/

#define GUAR_RESOURCE_POOL_STATUS_OK            0x0 /* GRP_STATUS_OK */
#define GUAR_RESOURCE_POOL_STATUS_OVERCOMMIT    0x1 /* GRP_STATUS_OVERCOMMIT */
#define GUAR_RESOURCE_POOL_STATUS_UNKNOWN       0x2 /* GRP_STATUS_UNKNOWN */
#define GUAR_RESOURCE_POOL_STATUS_LOANSUSP      0x4 /* GRP_STATUS_LOANSUSP */
    int            status;            /**< Status of pool*/
    int            total;             /**< Total resources in pool*/
    int            free;              /**< Free resources in pool*/
    int            guar;              /**< Total guaranteed resources in pool*/
    int            unused;           /**< Unused guaranteed resources in pool*/
    int            consumerC;         /**< Number of consumers*/
    guarConsumer   *consumerV;        /**< Data about the consumers*/
    char           *configuredHosts;  /**< Configured hosts string*/
    char           *resSelect;        /**< Resource select string*/
    int            currentHostsC;     /**< Number of current hosts*/
    char           **currentHosts;    /**< List of current hosts*/

#define GUAR_RESOURCE_POOL_POLICIES_NONE               (0)
#define GUAR_RESOURCE_POOL_POLICIES_LOAN_DELAY      (0x01)
#define GUAR_RESOURCE_POOL_POLICIES_LOAN_RESTRICT   (0x02)
#define GUAR_RESOURCE_POOL_POLICIES_RETAIN_PERCENT  (0x04)
    int            policies;          /**< Policies, bit field*/
    int            loanDelay;         /**< Loan delay (minutes, 0 == any)*/
    int            queuesC;           /**< Number of queues*/
    char           **queues;          /**< List of queues*/
    int            nHostUse;          /**< Number of elements in hostUse array */
    guarHost       *hostUse;          /**< Host use info, for current hosts */
    PREEMPT_INFO_T preemptInfo;       /**< Amounts available to each preemptive queue */
    int            retain;            /**< value of RETAIN in LOAN_POLICIES */ 
} guaranteedResourcePoolEnt;

/**
 * \brief guaranteed resource pool information reply
 */
typedef struct _guaranteedResourcePoolInfoReply {
    int                        numEnt; /**< Number of guaranteed resource pools*/
    guaranteedResourcePoolEnt  *entries; /**< Array of resource pools*/
} guaranteedResourcePoolInfoReply;

/**
 * \page lsb_guaranteedResourcePoolInfo lsb_guaranteedResourcePoolInfo
 * \brief Returns information about guaranteed resource pools.
 * \ref lsb_guaranteedResourcePoolInfo gets information about guaranteed resource pools. See lsb.resources for more information about parameters.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 *int lsb_guaranteedResourcePoolInfo(guaranteedResourcePoolInfoReq *request,
 *                                   guaranteedResourcePoolEnt **entries,
 *                                   int *nEntries)</b>
 *
 * @param *request Used to describe resource pools of interest.
 * @param **entries Used to pass back information about request guaranteed resource pools.
 * @param *nEntries The number of entries returned via entries.
 *
 * <b>Data Structures:</b>
 * \par
 * guaranteedResourcePoolInfoReq
 * guaranteedResourcePoolEnt
 *
 * @return int:0 \n
 * The function succeeded.
 * @return int:-1 \n
 * The function failed.
 *
 * \b Errors:
 * \par
 * If the function fails, lsberrno is set to indicate the error.
 *
 * <b>Equivalent line command:</b>
 * \par
 * bresoures
 *
 * <b>Files:</b>
 * \par
 * ${LSB_CONFDIR/cluster_name/configdir/lsb.resources
 */
extern int lsb_guaranteedResourcePoolInfo P_((guaranteedResourcePoolInfoReq *,
					      guaranteedResourcePoolEnt **,
					      int *));

/**
 * \page lsb_freeGuaranteedResourcePoolEntArray lsb_freeGuaranteedResourcePoolEntArray
 * \brief Used to free the memory used by an array of type guaranteedResourcePoolEnt.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * void lsb_freeGuaranteedResourcePoolEntArray(guaranteedResourcePoolEnt *ent,
 *                                             int nEnt)</b>
 *
 * <b>Data Structures:</b>
 * \par
 * guaranteedResourcePoolEnt
 *
 * @return void \n
 * This function has no return value.
 *
 * \b Errors:
 * \par
 * None.
 *
 * <b>Equivalent line command:</b>
 * \par
 * None.
 *
 * <b>Files:</b>
 * \par
 * None.
 */
extern void 
lsb_freeGuaranteedResourcePoolEntArray P_((guaranteedResourcePoolEnt *, int));

/**
 * \page  lsb_liveconfig lsb_liveconfig
 * Updates LSF configuration according to the parameter specified.
 *
 * \ref lsb_liveconfig dynamically changes LSF configuration without daemons restarting, 
 * and without impacting regular LSF operations. One LSF component (object) with one 
 * operation (action) is updated at a time. The update persists in configuration files that 
 * are the merge between original configuration files and live reconfig changes.
 *
 * <b>\#include <lsf/lsbatch.h>
 *       
 * int
 * lsb_liveconfig(LiveConfReq   *liveReq, char **reply)</b>
 *
 * @param *liveReq The request which specifies the configuration update information.
 * @param **reply The error messages from mbatchd.
 * 
 * <b>Data Structures:</b>
 * \par
 *  liveConfReq
 *
 * <b>Define Statements:</b>
 * \par
 * \ref live_config_objects
 * \n \ref live_config_actions
 *
 * @return int: 0
 * \n Function was successful. 
 * \n lsberrno is LSBE_NO_ERROR.
 * @return int: n, (0<n<255)
 * \n Function failed. 
 * \n lsberrno is LSBE_LIVECONF_MBD_RETERR. The request was partially accepted, 
 *     where n is the number of failed parts and a detailed error message in reply gives 
 *     failure reasons. 
 * @return int: -1
 * \n Function failed. 
 * \n lsberrno is LSBE_LIVECONF_MBD_REJECT. The request was entirely rejected. 
 *	 A detailed error message in reply gives failure reasons.
 *
 * \b Errors:
 * \par
 *   If the request fails, lsberrno is set to indicate the error and
 *   reply gives the detailed error messages.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bconf
 *
 * <b>Files:</b>
 * \par
 * lsf.conf
 *
 * @see none
 */
extern int lsb_liveconfig P_((LiveConfReq *, char **));








extern struct jobQueryCounters* lsb_queryjobcounters P_((struct queryInfo *));

/*
 * Global fairshare SYNC_MODE
 */
#define SYNC_MODE_PARTIAL   0
#define SYNC_MODE_ALL       1

/**
 * \brief Configuration of one global fairshare policy.
 */
struct globalFairshareConfEnt
{
    char *policyName;       /**< The name of the fairshare policy */
    int numParticipants;    /**< The number of participants */
    char **participants;    /**< Participants array */
    int syncInterval;       /**< Sync up interval */
    int syncMode;           /**< Sync up mode */
    char *description;      /**< Description of this fairshare policy */
};

/* FIXME: The comments is not right */
/**
 * \page lsb_globalFairshareConf lsb_globalFairshareConf
 * \brief Returns configuration information about global fairshare
 *
 * \ref See lsb.globalpolicies for more information about global fairshare
 * parameters.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * struct globalFairshareConfEnt *
 * lsb_globalFairshareConf(char **policies, int *numPolicies);
 *
 * @param policies [IN] An array of names of global fairshare policies.
 * @param numPolicies [OUT] Global fairshare configuration array
 *
 * @return int:0 \n
 * Function was successful.
 * @return int:-1 \n
 * The function failed.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bgpinfo conf
 *
 * \b Files:
 * \par
 * $LSB_CONFDIR/cluster_name/configdir/lsb.globalpolicies
 */
extern struct globalFairshareConfEnt *
lsb_globalFairshareConf P_((char **policyNames, int *numPolicyNames));

/**
 * \addtogroup Global policy connection status
 * \brief   Each entry in the table contains one global policy connection status
 */
/*@{*/
#define PARTICIPANT_STAT_OK         0
#define PARTICIPANT_STAT_DISC       1
#define PARTICIPANT_STAT_REJECT     2
/*@}*/

/**
 * \addtogroup Global policy status request flag
 * \brief   Each entry in the table contains one global policy status request flag
 */
/*@{*/
#define GPOLICY_STATUS_REQ_GPOLICYD    0x0001
#define GPOLICY_STATUS_REQ_FAIRSHARE   0x0002
/*@}*/

/*
 * \brief Request for getting global policy status
 */
struct globalPolicyStatusReq
{
    int statusReqFlag;              /**< Status request flag */
    int numGlobalFairshareNames;    /**< Number of policy names requested, 0 implies all */
    char **globalFairshareNames;    /**< The names of policies being requested. */
};

/**
 * \brief Information of a global fairshare participant.
 */
struct globalFairshareParticipantEnt
{
    char *name;         /**< Name of the participant */
    int connStatus;     /**< Connection status of this participant */
};

/**
 * \brief Status of global fairshare
 */
struct globalFairshareStatusEnt
{
    char *policyName;       /**< The name of this global fairshare */
    int numParticipants;    /**< The number of participants */
    struct globalFairshareParticipantEnt *participants;
};

/**
 * \brief Status of all global policies
 */
struct globalPolicyStatusInfo
{
    char *gpolicydCluster;  /**< cluster name where gpolicyd exist */
    char *gpolicydHost;     /**< host name where gpolicyd exist */

    /*
     * Global fairshare status
     */

    int numFairshareEnts;      /**< The number of global fairshare */

    /** Global fairshare status array */
    struct globalFairshareStatusEnt *fairshareEnts;
};

extern struct globalPolicyStatusInfo *
lsb_globalPolicyStatus(struct globalPolicyStatusReq *statusReq,
                       int *indexOfBadName);

/* share load for dynamic changes */
struct shareLoad {
    int runTime;
    int numStartJobs;
    int numReserveJobs;
    float histCpuTime;
    float newUsedCpuTime;
    float histRunTime;
    float decayedRunTime;
    float committedRunTime;
    float shareAdjustment;
};

/**
 * \brief Load information for a gloal fairshare node.
 */
struct participantShareLoad
{
    char *participant;      /**< If it is NULL, it is a sum load from
                              * all participants
                              */
    struct shareLoad load;
};

/**
 * \brief Global fairshare account informaion
 */
struct globalShareAcctEnt
{
    char *path;                             /**< Shared account path */
    struct shareLoad sumLoad;
    int numParticipantLoads;              /**< Number of load in participants */
    struct participantShareLoad *participantLoads;  /**< Participants load array */
};

/**
 * \brief All load informaion of a global fairshare policy.
 */
struct globalFairshareLoadEnt
{
    char *policyName;       /**< Policy name */
    int numShareAcctEnts;      /**< Number of global share accounts */
    struct globalShareAcctEnt *shareAcctEnts;   /**< Global share account array */
};

/**
 * \page lsb_globalFairshareLoadInfo lsb_globalFairshareLoadInfo
 * \brief Returns load information about global faireshare.
 *
 *<b>\#include <lsf/lsbatch.h>
 *
 * int
 * lsb_globalFairshareLoad(struct globalFairshareLoadReq *loadReq,
 *                         int *numGlobalFairshare;
 *                         struct globalFairshareLoadInfo *loads);
 *
 * @param loadReq [IN] Load request.
 * @param numGlobalFairshare [OUT] The number of global fairshare.
 * @param loads [IN] Load array of global fairshares.
 *
 * @return 0 on success, -1 on failure.
 *
 * <b>Equivalent line commands:</b>
 * \par
 * bgpinfo fsload
 *
 */
extern struct globalFairshareLoadEnt *
lsb_globalFairshareLoad P_((char **policies,
                            int *numPolicies,
                            char *clusterName));

/* options for lsb_gpdControl() call */
/**
 * \addtogroup gpd_control_flag gpd_control_flag
 *  options for \ref lsb_gpdShutdown call
 */
/*@{*/
#define    GPD_CTRL_RESTART       0   /**< gpolicyd restart */

extern int lsb_gpdCtrl P_((int ctrlCode, char *msg, int options));


#undef P_

#if defined(__cplusplus)
}
#endif

#endif

/** 
 * \page lsb_jsdl2submit lsb_jsdl2submit
 * \brief  Accepts a JSDL job submission file as input and converts the file
 *  for use with LSF.
 *
 * \ref lsb_jsdl2submit converts parameters specified in the JSDL file and merges
 * them with the other command line and job script options. The merged submit
 * request is then sent to mbatchd for processing.
 * 
 * Code must link to LSF_LIBDIR/libbat.jsdl.lib
 *
 * <b>\#include <lsf/lsbatch.h>
 *
 * int lsb_jsdl2submit(struct submit* req, char *template);</b>
 *
 * @param *req Reads the specified JSDL options and maps them to the
 * submitReq structure. Code must specify either jsdl or jsdl_strict.
 * @param *template The default template, which contains all of the bsub
 * submission options.
 *
 * <b>Data Structures:</b>
 * \par
 * submit
 *
 * <b>Define Statements:</b>
 * \par
 * none
 *
 * @return int:0 \n
 * Function completed successfully.
 * @return int:-1 \n
 * Function failed.
 *
 * <b>Errors:</b>
 * \par
 * On failure, sets lsberrno to indicate the error.
 * 
 * <b>Equivalent line command:</b>
 * \par
 *  bsub with options
 *
 * <b>Files:</b>
 * \par
 * $LSF_LIBDIR/jsdl.xsd
 * \n $LSF_LIBDIR/jsdl-posix.xsd
 * \n $LSF_LIBDIR/jsdl-lsf.xsd
 *
 * @see \ref lsb_submit 
 * @see \ref lsb_modify 
 */

/**
 * \page lsblib lsblib
 * \brief Application Programming Interface (API) library functions for batch jobs
 *
 * LSBLIB functions allow application programs to get information about the hosts, 
 * queues, users, jobs and configuration of the batch system. Application programs 
 * can also submit jobs and control hosts, queues and jobs. Finally, application 
 * programs can read batch log files and write batch error messages.
 *
 * \note 
 * \par 
 * All LSBLIB APIs require that the batch header file <lsf/lsbatch.h> be included. 
 * \par 
 * Many LSBLIB APIs return a pointer to an array or structure. These data structures 
 * are in static storage or on the heap. The next time the API is called, the storage 
 * is overwritten or freed. 
 * \par 
 * Any program using LSBLIB APIs that change the state of the batch system (that 
 * is, except for APIs that just get information about the system) must be setuid 
 * to root if LSF_AUTH is not defined in the lsf.conf file. 
 * \par 
 * On systems which have both System V and BSD programming interfaces, LSBLIB 
 * typically requires the BSD programming interface. On System V-based versions of 
 * UNIX, for example SGI IRIX, it is normally necessary to link applications using 
 * LSBLIB with the BSD compatibility library. 
 * \par 
 * On AFS systems, the following needs to be added to the end of your linkage 
 * specifications when linking with LSBLIB (assuming your AFS library path is 
 * /usr/afsws): 
 * \par 
 * For HP-UX and Solaris, 
 * \par 
 * -lc -L/usr/afsws/lib -L/usr/afsws/lib/afs -lsys -lrx -llwp /usr/afsws/lib/afs/util.a 
 * \par 
 * For other platforms, 
 * \par 
 * -lc -L/usr/afsws/lib -L/usr/afsws/lib/afs -lsys -lrx -llwp 
 * 
 * \b Files:
 * \par 
 * ${LSF_ENVDIR:-/etc}/lsf.conf 
 * \n $LSF_CONFDIR/lsf.shared 
 * \n $LSF_CONFDIR/lsf.cluster.cluster_name 
 * \n $LSF_CONFDIR/lsf.task 
 * \n $LSF_CONFDIR/lsf.task.cluster_name 
 * \n $LSB_CONFDIR/cluster_name/configdir/lsb.hosts 
 * \n $$LSB_CONFDIR/cluster_name/configdir/lsb.params 
 * \n $LSB_CONFDIR/cluster_name/configdir/lsb.queues 
 * \n $LSB_CONFDIR/cluster_name/configdir/lsb.users 
 *
 * @see lsblibapis
 */


/*TODO: add comments*/
extern struct acHostInfoEnt * lsb_ac_vminfo (char **, int *, int );
extern struct acHostInfoEnt * lsb_ac_hostinfo (char **, int *, int );
extern struct acProvisionInfoEnt * lsb_ac_provisioninfo(char **, int *, int, int *, char *** );
extern struct acTemplateInfoEnt * lsb_ac_templateinfo (char **, int *, int );
extern struct acParameterDataReply *lsb_ac_parameters();

