/* $id: lssched.h,v 1.6 2004/05/27 17:01:11 qwang exp $
 ****************************************************************************
 *
 * Load Sharing Facility
 *
 * Header file for external scheduler plugins
 *
 ****************************************************************************
 */
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#if defined(WIN32)
#include <string.h>
#else
#include <strings.h>
#endif
#include "lsf.h"

#ifndef      FALSE
#define      FALSE      0
#endif
#ifndef      TRUE
#define      TRUE       1
#endif

#if !defined(WIN32)
extern int sched_version(void *);
extern int sched_init(void *);
extern int sched_pre_proc(void *);
extern int sched_match_limit(void *);
extern int sched_order_alloc(void *);
extern int sched_post_proc(void *);
extern int sched_finalize(void *);
#else
__declspec(dllexport) int sched_version(void *);
__declspec(dllexport) int sched_init(void *);
__declspec(dllexport) int sched_pre_proc(void *);
__declspec(dllexport) int sched_match_limit(void *);
__declspec(dllexport) int sched_order_alloc(void *);
__declspec(dllexport) int sched_post_proc(void *);
__declspec(dllexport) int sched_finalize(void *);
#endif

/*
 * Data structure for how slots are distributed over a set of hosts for a job.
 *
 * For a parallel job, this structure gives information about how many slots
 * have been given to the job on each candidate host, and how many slots are
 * available to be given to the job on the host.
 *
 * SEE ALSO
 *
 * lsb_cand_getavailslot
 * candHost
 */
struct hostSlot {
    struct candHost * candHost;	/* information for which candidate host */
    char *name;			/* name of candidate host */
    int  nslotAvail;		/* available slots on the host */
    int  nslotAlloc;		/* allocated slots on the host */
    int  ncpus;			/* number of cpu on the host */
};

/*
 * Data structure representing a candidate host for a job.
 *
 * SEE ALSO
 *
 * candHostGroup
 * hostSlot
 * lsb_reason_set
 * lsb_cand_getavailslot
 */
struct candHost {
    int  hostIdx;		/* index to the internal host table */
    void *hostPtr;		/* pointer to internal host data    */
};

/*
 * Data structure representing a group of candidate hosts for a job.
 * 
 * SEE ALSO
 *
 * candHost
 * lsb_cand_getnextgroup
 * lsb_cand_removehost
 * lsb_alloc_gethostslot
 */
struct candHostGroup {
    struct candHostGroup *forw; /* forward pointer to other cand groups for
				 * the job */
    struct candHostGroup *back; /* backward pointer to other cand groups for
				 * the job */
    int    maxOfMembers;	/* total number of hosts */
    int    numOfMembers;	/* current number of candidate hosts for
				 * the group */
    struct candHost *candHost;	/* array of candidate host info */
};

/* 
 * Iterate to the next group of candidate hosts for a job. 
 * 
 * PARAMS
 * 
 * candGroupList - current pointer to group of candidate hosts.
 *
 * SEE ALSO
 * 
 * candHostGroup
 */
extern struct candHostGroup *lsb_cand_getnextgroup(void *candGroupList);

/* These names are the basic ones used in LSF to represent the load
 * and the static resources of a host. These names are always present.
 * These names appear in the resName member of the resources data
 * structure. All other resources defined in the cluster, via lsf.shared
 * or via the license scheduler, appear in the data structure as well,
 * but since they have custom definition they cannot appear in this symbol
 * table. The names in the table have the _ postfix to not conflict with
 * other definitions in the scheduler.
 */

#define cpuf_     "cpuf"
#define ncpus_    "ncpus" 
#define ndisks_   "ndisks" 
#define maxmem_   "maxmem"
#define maxswp_   "maxswp"
#define maxtmp_   "maxtmp"
#define r15s_     "r15s" 
#define r1m_      "r1m" 
#define r15m_     "r15m" 
#define ut_       "ut" 
#define pg_       "pg" 
#define io_       "io" 
#define ls_       "ls" 
#define it_       "it" 
#define tmp_      "tmp" 
#define swp_      "swp" 
#define mem_      "mem" 
#define slot_     "slots" 

/* This data structure wraps the resources available on each host
 */
struct hostResources {
    char               *hostName;
    char               *cluster;
    int                nres;
    struct resources   *res;
};

/* This is the type of the resource as configured in LSF base
 */
enum restype {
    NONE,
    STATIC_RESOURCE,
    DYNAMIC_RESOURCE,
    SHARED_RESOURCE
};

/* This is the C type of the resource value, bool (basically int), 
 * 0 terminated char * or float.
 */
enum ctype {
    BOOL_VAL,
    FLOAT_VAL,
    STRING_VAL
};

/* This is the representation of each individual resource.
 * A resource is basically a pair (name, value) representing a
 * named quantity.
 */
struct resources {
    char        *resName;
    int         resType;
    int         cType;
    union {
	char     *sval;
	int      bval;
	float    fval;
    } val;
};

/* This API takes in input a void pointer to a scheduler internal
 * representation of a host and returns the resources on the host.
 * The host is for example the void * pointer of the candHost data
 * structure.
 * The API returns a pointer to a statically allocated memory
 * which is freed at the next fuction invocation. The caller must
 * copy the data if they have to be kept across function invocations.
 */
extern struct hostResources *lsb_host_resources(void *);

/* A little helper routine returning the string from the enum restype.
 */
extern char *getResType(const struct resources *);

/* Data structure representing a job inside the plugin
 */
struct jobInfo {
    int    jobId;
    int    jobIndex;
    char   *cluster;
    char   *user;
    char   *queue;
    char   *resKey;
    int    status;          /* job states from lsbatch.h */
    int    pendReason;
    int    submitTime;
    char   *jobGroup;
    char   *serviceClass;
    char   *project;
    char   *enforcedGroup;
    char   *schedHostType;
    char   *schedHostModel;
    int    userPriority;   
    int    incPriority;    
    int    cpuLimit;       
    int    runLimit;       
};

/* The scheduler uses different data structures for the job
 * in different phases of the scheduling.
 * The API lsb_get_jobinfo() should be use in ALLOC or NOTIFY
 * phases, the API has to be told in which phase is being invoked
 * in order to derefrence the opaque job address correctly.
 */
enum phase {
    PHASE_ALLOC,
    PHASE_NOTIFY
};
/* This API returns the above data structure with job information.
 * The API returns a pointer to a statically allocated memory
 * which is freed at the next fuction invocation. The caller must
 * copy the data if they have to be kept across function invocations.
 */
extern struct jobInfo *lsb_get_jobinfo(void *job,
				       int phase);

/* 
 * Provide the reason why a host is not considered usable for a job.
 *
 * PARAMS
 * 
 * reasonPtr - (IN/OUT) the pointer to internal object that collects
 * 	       pending reasons for the job.
 * candHost - (IN) host that cannot be used by job
 * reasonId - (IN) the ID of the reason why host cannot be used.
 *
 * PRECONDITIONS
 *
 * reasonId must be either a pre-defined reason number, as given by
 * lsb_pendreason, or a customized reason number between 20001 and 25000.
 * LSF reserves pending reason numbers from 1 - 20000.  Please refer
 * lsb_pendreason and lsbatch.h for reason ID definitions.
 *
 * RETURNS
 *
 * 0 if successfully set reason, <0 if failed.
 *
 * SEE ALSO
 *
 * candHost
 * lsb_pendreason
 */
extern int lsb_reason_set(void *reasonPtr, 
			  struct candHost *candHost, 
			  int reasonId);

/* 
 * Remove a candidate host from the group.
 *
 * PARAMS
 * 
 * group - (IN/OUT) group of hosts to be modified
 * index - (IN) index of the candidate host in the group to be removed.
 *
 * PRECONDITIONS
 *
 * Remaining hosts in the group are shifted up to the current index.
 * Thus, do not increment the current index if iterating through the 
 * candidate hosts in this group, 
 *
 * SEE ALSO
 *
 * candHostGroup
 * candHost
 * lsb_reason_set
 */
extern void lsb_cand_removehost(struct candHostGroup *group, int index);

/* 
 * Get number of available slots on a candidate host. 
 * This function can also be used to get candidate host name, which is 
 * retruned in hostSlot.
 *
 * PARAMS
 * 
 * candHost - (IN) host to get slot availability from.
 * 
 * RETURNS
 *
 * Available slots on host.
 *
 * SEE ALSO
 * 
 * hostSlot
 * candHost
 */
extern struct hostSlot *lsb_cand_getavailslot(struct candHost *candHost);


/* 
 * Signature for functions that handle newly submitted resource requirement.
 *
 * Use these functions for newly submitted jobs to 
 * - get the user-specified resource requirement messages
 * - attach handler-specific data to peer jobs.
 *
 * PARAMS
 * 
 * resreq - (IN) Resource requirement object for newly submitted job.
 *          This is an internal object representing resource requirements for
 *          peer jobs.  
 *
 * RETURNS
 *
 * 0 if successfully processed newly submitted resource requirements.
 * <0 if failed to process.
 *
 * SEE ALSO
 * 
 * RsrcReqHandlerType
 * RsrcReqHandler_FreeFn
 * lsb_resreq_registerhandler
 * lsb_resreq_getextresreq
 */
typedef int (* RsrcReqHandler_NewFn) (void *resreq);

/* 
 * Signature for functions that handles freeing of handler-specific
 * data when no more peer jobs refer to a resource requirement.
 *
 * PARAMS
 * 
 * handlerData-(IN/OUT) Handler-specific data that was created and attached to 
 *	    a resource requirement in above New function. Scheduler 
 *	    framework invokes this callback when the data is no longer
 *          usable.
 *
 * SEE ALSO
 * 
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * lsb_resreq_registerhandler
 */
typedef void (* RsrcReqHandler_FreeFn)(void *handlerData);

/* 
 * Signature for functions that check candidate hosts against resource
 * requirements for peer jobs.
 *
 * PARAMS
 * 
 *  handlerData - (IN) Handler-specific data attached to resource requirements
 *  	          for peer jobs.
 *  candGroupList - (IN/OUT) list of candidate host groups.
 *  reasonPtr - (IN/OUT) pointer to internal object that collects pending
 *  	        reasons.
 *
 * SEE ALSO
 * 
 * lsb_cand_getnextgroup
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * lsb_resreq_registerhandler
 */
typedef int (* RsrcReqHandler_MatchFn)(void *handlerData,
				       void *candGroupList,
				       void *reasonPtr);

/* 
 * Signature for functions that sort candidate hosts by preferences for peer
 * jobs.
 *
 * These "sort" functions are called after matching is finished.
 *
 * PARAMS
 * 
 *  handlerData - (IN) Handler-specific data attached to resource requirements
 *  	          for peer jobs.
 *  candGroupList - (IN/OUT) list of candidate host groups.
 *  reasonPtr - (IN/OUT) pointer to internal object that collects
 *              pending reasons.
 *
 * SEE ALSO
 * 
 * lsb_cand_getnextgroup
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * lsb_resreq_registerhandler
 */
typedef int (*RsrcReqHandler_SortFn)(void *handlerData,
				     void *candGroupList,
				     void *reasonPtr);

/* These define are imported from the framework header files.
 */
#define NOTIFY_IN_SCHED_SESSION     0x001   /* inside scheduling session
					     */
#define NOTIFY_OUT_SCHED_SESSION    0x002   /* outside scheduling session
					     */
#define NOTIFY_ALLOC                0x004   /* allocation notification 
					     */
#define NOTIFY_DEALLOC              0x008   /* deallocation notification 
					     */
#define NOTIFY_STARTUP              0x010   /* job allocation at startup 
					     */

typedef int (*RsrcReqHandler_CheckAllocFn)(
        void *info,
        void *job, 
        void *alloc,
        void *allocLimitList);

typedef int (*RsrcReqHandler_NotifyAllocFn)(void *info,
					    void *job, 
					    void *alloc,
					    void *allocLimitList,
					    int  flag);
/* 
 * Data structure containing handler-specific functions for resource
 * requirements.
 *
 * SEE ALSO
 *
 * lsb_resreq_registerhandler
 * RsrcReqHandler_NewFn
 * RsrcReqHandler_FreeFn
 * RsrcReqHandler_MatchFn
 * RsrcReqHandler_SortFn
 * RsrcReqHandler_NotifyAllocFn
 *
 */
typedef struct _RsrcReqHandlerType {

    /* Handler-specific function to handle newly submitted
     * jobs' resource requirement. 
     */
    RsrcReqHandler_NewFn         newFn;

    /* Handler-specific function to free handler-specific
     * data when no more peer jobs refer to a resource
     * requirement. 
     */
    RsrcReqHandler_FreeFn        freeFn;

    /* Handler-specific function to check candidate hosts
     * against peer jobs' resource requirements. 
     */
    RsrcReqHandler_MatchFn       matchFn;

    /* Handler-specific function to sort candidate hosts by
     * peer jobs' preferences. 
     */
    RsrcReqHandler_SortFn        sortFn;

    /* This function is invoked when a job dispatch decision 
     * was made and plugins may want to update their own 
     * internals.
     */
    RsrcReqHandler_NotifyAllocFn   notifyAllocFn;

    /* This function is invoked when a job dispatch decision 
     * was made and plugins may want to check 
     */
    RsrcReqHandler_CheckAllocFn   checkAllocFn;

} RsrcReqHandlerType;

/* 
 * Register a new handler for job resource requirements.
 *
 * Registered handler functions will be called during scheduling 
 * to handle peer jobs' resource requirements.
 *
 * PARAMS
 * 
 * handlerId (IN) identifier for the handler
 * handler (IN) pointer to the handler.  Memory must be dynamically allocated.
 *         Memory must not be freed.
 * 
 * PRECONDITIONS
 *
 * The handlerId must be a number between 100 and 199.  Each handler must have
 * a different identifier.
 * 
 * Fields of RsrcReqHandlerType that are not used must be set to NULL.
 *
 * SEE ALSO
 *
 * RsrcReqHandlerType
 * RsrcReqHandler_NewFn
 * RsrcReqHandler_FreeFn
 * RsrcReqHandler_MatchFn
 * RsrcReqHandler_SortFn
 * RsRcReqHandler_NotifyAlloc
 *
 */
extern int lsb_resreq_registerhandler(int handlerId, 
				      RsrcReqHandlerType *handler);

/* 
 * Attach handler-specific data to a set of peer jobs.
 *
 * Jobs are classified as peer jobs when they have the same handler-specific
 * data attached (as indicated by a unique string representation).  Peer jobs
 * are scheduled together.
 *
 * PARAMS
 *
 * resreq - (IN) Internal object representing resource requirements of
 *          peer jobs.
 * handlerId - (IN) identifier for the handler.
 * key - (IN) the unique string representing the handler-specific data.
 * handlerData - (IN) handler-specific data to be attached to the peer jobs.
 *
 * PRECONDITIONS
 *
 * 1. The handlerId must be an ID previously registered with
 *    lsb_resreq_registerhandler.
 *
 * 2. Peer jobs are scheduled together, so handlerData attached with the same
 *    key must GUARANTEE to produce the same list of candidate hosts for all
 *    peer jobs.
 *
 * 3. The key will be copied.
 *
 * 4. The handlerData should not be freed until framework calls the freeFn of
 *    the registered handler type.
 *
 * SEE ALSO
 *
 * lsb_resreq_registerhandler
 * RsrcReqHandler_NewFn
 * RsrcReqHandler_FreeFn
 */
extern void lsb_resreq_setobject(void *resreq, 
				 int handlerId, 
				 char *key, 
				 void *handlerData);

/* 
 * Get the handler-specific data attached to the peer jobs.
 *
 * PARAMS
 *
 * job - (IN) Internal job object.  Get handler-specific data attached
 *       to this job.
 * handlerId - (IN) identifier for handler.
 *
 * RETURNS
 *
 * Pointer to the handler-specific data attached to the peer jobs.
 *
 * SEE ALSO
 *
 * lsb_resreq_registerhandler
 *
 * NOTE    This api is not supported in current version(beta 1).
 */
extern void *lsb_job_getrsrcreqobject(void *jobPtr, int handlerId);



/* 
 * Signature for functions that adjusts allocation decisions for jobs. 
 *
 * Use these functions to adjust allocations made by internal scheduling
 * functions.
 *
 * Note that these functions will only be called for jobs that have 
 * some potentially usable candidate hosts.  These functions may not
 * be called for certain jobs if 
 * - all candidate hosts have been previously determined to not satify 
 *   resource requirements of the peer jobs.
 * - the decision, SCH_MOD_DECISION_PENDPEERJOBS, has been given for
 *   a peer job to this job.
 *
 * PARAMS
 *
 * job - (IN) Internal job object. Make allocation decision for this job.
 * groupList - (IN/OUT) List of candidate host groups usable for the job.
 * reasonPtr - (IN/OUT) Internal object to collect pending reasons for the
 *             job.
 * alloc - (IN/OUT) Internal object representing resources allocated to the
 * 	   job.
 *
 * RETURNS
 *
 * Decision for the job:
 * - SCH_MOD_DECISION_NONE: The function does not care to alter the decision
 *   for this job.
 * - SCH_MOD_DECISION_DISPATCH: Decide to dispatch the job with the given
 *   allocation in alloc.
 * - SCH_MOD_DECISION_RESERVE: Decide to reserve resources for the job with
 *   the given reservation in alloc.
 * - SCH_MOD_DECISION_PENDPEERJOBS: Decide to keep this job pending, along
 *   with all its peer jobs.
 * - SCH_MOD_DECISION_PENDJOB: Decide to keep this job only pending. Peer
 *   jobs will be scheduled separately.
 *
 * SEE ALSO
 *
 * lsb_alloc_registerallocator
 * lsb_cand_getnextgroup
 * candHostGroup
 * lsb_alloc_modify
 * lsb_alloc_gethostslot
 * lsb_job_getaskedslot
 * lsb_job_getextresreq
 * lsb_job_getrsrcreqobject
 * lsb_resreq_setobject
 */
typedef int (*AllocatorFn)(void *job,
			   void *groupList,
			   void *reasonPtr,
			   void **alloc);

#define SCH_MOD_DECISION_NONE            0
#define SCH_MOD_DECISION_DISPATCH        1
#define SCH_MOD_DECISION_RESERVE         2
#define SCH_MOD_DECISION_PENDPEERJOBS    3
#define SCH_MOD_DECISION_PENDJOB         4

/* 
 * Register a functions that adjusts allocation decisions for jobs. 
 *
 * These functions will be called after internal scheduling policies
 * have decided allocations for jobs.
 *
 * PARAMS
 *
 * allocator - (IN) Pointer to the function that adjusts alocations.
 *
 * RETURN
 *
 * 0 if succesfully registered. <0 if failed to register the function.
 *
 * SEE ALSO
 *
 * AllocatorFn
 */
extern int lsb_alloc_registerallocator(AllocatorFn  allocator);

/* 
 * Get user-specified resource requirement message for the peer jobs.
 *
 * PARAMS
 *
 * resreq - (IN) Internal object representing resource requirements for peer
 *          jobs.
 * msgCnt - (OUT) Number of user-specified messages submitted with the
 *          peer jobs.
 * RETURNS
 *
 * Array of user-specified messages submitted with the peer jobs.
 * If there are no such messages, returns NULL.
 *
 * SEE ALSO
 *
 * lsb_job_getextresreq
 */
extern char **lsb_resreq_getextresreq(void *resreq, int *msgCnt);

/*
 * Get queue level MANDATORY_EXTSCHED and DEFAULT_EXTSCHED for job
 *
 * PARAMS
 *
 * resreq - (IN) Internal object representing resource requirements for peer
 *          jobs.
 * mand_extsched - (OUT) return the setting of MANDATORY_EXSCHED of job 
 *          queue, mand_extsched should not be NULL. It will points the same
 *          string as resreq->qInfo->queueConf->mandExtSched.
 * default_extsched - (OUT) return the setting of DEFAULT_EXSCHED of job
 *          queue, default_extsched should not be NULL. It will points the 
 *          same string as resreq->qInfo->queueConf->defExtSched.
 * RETURNS
 *
 * If no error occurs, return 0, otherwise return -1,
 * If MANDATORY_EXTSCHED has been set in job's queue, *mand_extsched will
 *    points to the same string of resreq->qInfo->queueConf->mandExtSched, 
 *    otherwise it will be
 *    NULL.
 * If DEFAULT_EXTSCHED has been set in job's queue, *default_extsched will
 *    points to the same string of resreq->qInfo->queueConf->defExtSched, 
 *    otherwise it will be NULL.
 * 
 *
 * SEE ALSO
 *
 * lsb_job_getextresreq
 */
extern int lsb_resreq_getqueextsched(void * resreq, char **mand_extsched,
                                   char  **default_extsched);
/* 
 * Get the user-specified resource requirements messages for the job.
 * 
 * Same as lsb_resreq_getextresreq, but uses pointer to the job,
 * rather than the resource requirements of the peer jobs.
 *
 * PARAMS
 *
 * job - (IN) Internal job object.
 * msgCnt - (OUT) Number of user-specified messages submitted with the
 *          peer jobs.
 *
 * RETURNS
 *
 * Array of user-specified messages submitted with the peer jobs.
 * If there are no such messages, returns NULL.
 *
 * SEE ALSO
 *
 * lsb_resreq_getextresreq
 */
extern char **lsb_job_getextresreq(void *jobPtr, int *msgCnt);

/* 
 * Get the number of slots requested by a job.
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object.
 *
 * RETURNS
 *
 * Returns (max) number of slots requested by job.
 */
extern int lsb_job_getaskedslot(void *jobPtr);

/* 
 * Get the distribution of slots on various candidate hosts in job's
 * allocation.
 *
 * PARAMS
 *
 * alloc - (IN) Internal allocation object. Allocated slots for the job.
 * candGroupList - (IN) list of candidate host groups for the job.
 * hostCnt - (OUT) number of hostSlot information returned.
 * group - (OUT) which candidate host group is allocated.
 *
 * RETURNS
 *
 * Array of hostSlot data structures, giving the number of slots given
 * to the job on each host, and slots available on the host.
 */
extern struct hostSlot* lsb_alloc_gethostslot(void *alloc, 
					      void *candGroupList, 
				              int *hostCnt,
					      struct candHostGroup **group);
/* 
 * Return the type of allocation given to the job (reservation or dispatch).
 *
 * PARAMS
 *
 * alloc - (IN) Internal allocation object. Allocated slots for the job.
 *
 * RETURNS
 *
 * SCH_MOD_DECISION_NONE: alloc is NULL. No decision has been made.
 * SCH_MOD_DECISION_DISPATCH: alloc is a dispatch decision.
 * SCH_MOD_DECISION_RESERVE: alloc is a reservation decision.
 */
extern int lsb_alloc_type(void *alloc);

/* 
 * Adjust the number of slots allocated to a job on a candidate host,
 * if possible.
 *
 * Reduce or increaese the number of slots allocated to a job on a host.  If
 * increasing, this function will check that other scheduling policies are not
 * violated by the new allocation.
 *
 * PARAMS
 *
 * alloc - (IN/OUT) Internal allocation object. 
 * host - (IN) The host to be modified. nslotAlloc field in hostSlot indicates
 *             how many slots need to be allocated on the host.
 * reason - (IN) Internal reason object.
 * group - (IN) which candidate host group is used in the allocation. 
 *
 * PRECONDITIONS
 *
 * The nslotAlloc field of host parameter must be greater than 1 (that is,
 * hosts cannot be removed from an allocation).
 *
 * RETURNS
 *
 * TRUE if allocation was successfuly adjusted.  FALSE if adjustment
 * failed (due to conflict with other scheduling policies).
 *
 * SEE ALSO
 *
 * lsb_alloc_gethostslot
 * AllocatorFn
 * lsb_alloc_registerallocator
 */
extern int lsb_alloc_modify(void *jobptr, void **alloc, 
			    struct hostSlot *host,
			    void *reason,
			    struct candHostGroup *group);

/*******************************************
 ** The following functions are used      **
 ** for the external FCFS plugin module   **
 *******************************************
 */
/* 
 * Signature for the function to decide job dispatch order.
 *
 * Use this funtion to decide which job in the ready job list passed in
 * from the queue should go first
 *
 * PARAMS
 * 
 * queueName - (IN) the name of the queue to which the job list belongs
 * jobList   - (IN) the ready job list belonging to the queue
 *
 * RETURNS
 *
 * Pointer to the job which should go first. If no decision can be made
 * then a NULL will be returned. 
 *
 * SEE ALSO
 * 
 * lsb_order_registerFCFS4AllQueues 
 */
typedef void * (*JobOrderFn4Que) (char *queueName, void *jobList);

/* 
 * Register a function that decides job dispatch order for all queues.
 *
 * PARAMS
 *
 * fcfsOrderFn - (IN) Pointer to the function that decides job dispatch order.
 *
 * RETURN
 *
 * 0 if succesfully registered. <0 if failed to register the function.
 *
 * SEE ALSO
 *
 * JobOrderFn4Que
 */
extern int    lsb_order_registerOrderFn4AllQueues(JobOrderFn4Que  orderFn);

/* 
 * Check if the jobList is empty.
 *
 * PARAMS
 *
 * jobList - (IN) Ready to dispatch job list.
 *
 * RETURNS
 *     TRUE  - if no job in the list
 *     FALSE - there are jobs in the job list
 */
extern int lsb_order_isJobListEmpty(void * jobList); 

/* 
 * Get the first job in the job list.
 *
 * PARAMS
 *
 * jobList - (IN) The passed in job list.
 *
 * RETURNS
 *     Return the first job in jobList if jobList is not empty,
 *     otherwise return NULL. 
 */
extern void * lsb_order_getFirstJobOfList(void * jobList);

/* 
 * Get the next job in the job list.
 *
 * PARAMS
 *
 * currJob - (IN) Pointer to the current job.
 * jobList - (IN) The passed in job list.
 *
 * RETURNS
 *     Return the next job in jobList if jobList is not empty,
 *     otherwise return NULL. 
 */
extern void * lsb_order_getNextJobOfList(void *currJob,  
                                         void *jobList);

/* 
 * Get the previous job in the job list.
 *
 * PARAMS
 *
 * currJob - (IN) Pointer to the current job.
 * jobList - (IN) The passed in job list.
 *
 * RETURNS
 *     Return the previous job in jobList if jobList is not empty,
 *     otherwise return NULL. 
 */ 
extern void * lsb_order_getPreJobOfList(void *currJob,  
                                        void *jobList);

/* 
 * Get the number of jobs in the job list.
 *
 * PARAMS
 *
 * jobList - (IN) The passed in job list.
 *
 * RETURNS
 *     The number of jobs in the job list.
 */ 
extern int lsb_order_getJobNumOfList(void *jobList);

/* 
 * Get the priority of a job, which could be defined by bsub -sp. 
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object.
 *
 * RETURNS
 *
 * Returns priority of job.
 */
extern int    lsb_order_getJobPriority(void* job);

/* 
 * Get the sequential number of a job. 
 *
 * Sequential number is used by LSF internally to decide a job's FCFS 
 * priority in a queue. Beside a job's submission time, btop/bbot and
 * other relevant features; e.g, job requeue, are also considered when 
 * determining a job's sequential number.
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object.
 *
 * RETURNS
 *
 * Returns the sequential number of a job.
 */
extern unsigned int lsb_order_getJobSeqNum(void* job);

/* 
 * Get the submission time of a job. 
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object.
 *
 * RETURNS
 *
 * Returns the submission time of job.
 */
time_t lsb_order_getJobSubmitTime(void* job);

/* 
 * Get the user name of a job. 
 *
 * PARAMS
 *
 * jobPtr - (IN) Internal job object.
 *
 * RETURNS
 *
 * Returns the user name of job.
 */
extern char * lsb_order_getJobUser(void *job);
