#ifndef CONNECT___NCBI_LBSM_IPC__H
#define CONNECT___NCBI_LBSM_IPC__H

/* $Id: ncbi_lbsm_ipc.h 424440 2014-01-15 22:48:03Z lavr $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Author:  Anton Lavrentiev, Denis Vakatov
 *
 * File Description:
 *    Implementation of the LBSM client-server data exchange API
 *    with the use of SYSV IPC (shared memory and semaphores)
 *
 */

#include "ncbi_config.h"
#include <connect/ncbi_heapmgr.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef HAVE_SEMUN
/* This sequence of defines causes 'union semun' be undefined on IRIX */
#  ifdef _XOPEN_SOURCE
#    define _XOPEN_SOURCE_SAVE _XOPEN_SOURCE
#    undef  _XOPEN_SOURCE
#  endif
#  define _XOPEN_SOURCE 1
#endif
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#ifndef HAVE_SEMUN
#  undef _XOPEN_SOURCE
#  ifdef _XOPEN_SOURCE_SAVE
#    define _XOPEN_SOURCE _XOPEN_SOURCE_SAVE
#  endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if 0/*defined(_DEBUG) && !defined(NDEBUG)*/
#  define LBSM_DEBUG 1
#endif

/* Keys to access the LBSM shared memory and semaphores.
 * Presently we have 2 copies of the shared memory table (heap) of services,
 * to reduce the waiting time on a table being updated by the daemon.
 * Both tables and daemon instance itself are guarded by a single sem array.
 */
#define LBSM_SHMEM_KEY_1 0x1315549
#define LBSM_SHMEM_KEY_2 0x12CC3BC
#define LBSM_MUTEX_KEY   0x130DFB2

#define LBSM_SEM_PROT                                               \
    ( S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH )

#define LBSM_SHM_PROT                                               \
    ( S_IRUSR | S_IWUSR | S_IRGRP           | S_IROTH           )

#if !defined(HAVE_SEMUN)  &&  !defined(__FreeBSD__)  &&  !defined(__MWERKS__) \
    &&  (!defined(__GNU_LIBRARY__)  ||  defined(_SEM_SEMUN_UNDEFINED))
union semun {
    int              val;
    struct semid_ds* buf;
    unsigned short*  array;
};
#endif


/* Check (and lock, if "check_n_lock" is TRUE) an instance of LBSMD.
 * Return value:
 * -1 means the mutex could not be acquired;
 *  0 means the mutex was vacant, and the operation went through;
 *  1 means the lock operation failed (mutex was already locked, or whatever).
 * In cases of non-zero return code, "errno" must be analyzed to
 * figure out the problem (if any).  Locking is reserved for the sole
 * use by the daemon, and is undone automatically upon program termination.
 *
 * This must be the first call prior to any shared resources use, as it
 * sets up an internal semaphore ID for all locking/unlocking code.
 */
int LBSM_LBSMD(int/*bool*/ check_n_lock);


/* Remove LBSMD-specific internal semaphore ID (i.e. undo LBSM_LBSMD()).
 * This call is mostly for use by daemon (called with argument > 0),
 * but if called with its argument zero then could also be used by
 * clients to obtain PID of the daemon running (still, LBSM_LBSMD must be
 * called priorly).  Return value (if differs from 0) is most likely the PID
 * of running LBSM daemon (actually, the PID of LBSM shared memory creator).
 * As a side effect in client, the call detaches all LBSM shared memory
 * segments (if any have been previously attached by service mapping API).
 * The call uses CORE_LOCK when detaching from LBSM shmem and semaphores.
 */
pid_t LBSM_UnLBSMD(int/*bool*/ undaemon);


/* Return an attached LBSMD shared memory in a heap or NULL on error.
 * Successfully attached shared memory is kept read-locked and must be
 * released by a call to LBSM_Shmem_Detach().
 * @note The call can modify internal static variables,
 *       and thus must be protected in an MT environment.
 * @note Returned heap relies on virtual memory mapping that may
 *       change by LBSM_Shmem_Attach() if called in an MT app. concurrently.
 * The argument "fallback" selects secondary shmem copy if passed non-zero.
 */
HEAP LBSM_Shmem_Attach(int/*bool*/ fallback);


/* Detach heap (previously resulted from LBSM_Shmem_Attach()) and
 * release the shared memory read lock (kept in LBSM semaphore).
 * @note "heap" may not be NULL.
 */
void LBSM_Shmem_Detach(HEAP heap);


/* Initialize shared memory based LBSM heap to work with.
 * Warn if the shared memory segment exists already and is to be re-created.
 * Designed for use solely by the LBSM daemon.
 * Returned HEAP is NOT actually located in shared memory, so after any
 * change it yet has to be copied in there (committed) by LBSM_Shmem_Update().
 */
HEAP LBSM_Shmem_Create(void);


/* Destroy the shared memory LBSM heap (created by LBSM_Shmem_Create())
 * and its shmem-based copy.
 */
void LBSM_Shmem_Destroy(HEAP heap);


/* Synchronize the shared memory heap (used by clients) with local 'heap'.
 * This call is for use in the LBSM daemon only.  Does proper write-locking
 * using LBSM semaphores (not MT_LOCKs!).  Some clients may have been sent a
 * signal, if hanging on the semaphores for too long (when "wait" is false).
 * Return bitmask of updated segments, zero on error (none updated).
 */
unsigned int LBSM_Shmem_Update(HEAP heap, int/*bool*/ wait);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CONNECT___NCBI_LBSM_IPC__H */
