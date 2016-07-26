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

/*
 * unix_shared_memory.c
 *
 *  Created on: 25 Jan 2016
 *      Author: billy
 */

#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>

#include "memory_allocations.h"
#include "streams.h"


int AllocateSharedMemory (const char *id_s, size_t size, int flags)
{
	int shm_id = -1;
	key_t key = ftok (id_s, 'A');

	if (key != (key_t) -1)
		{
			shm_id = shmget (key, size, flags | IPC_CREAT);

			if (shm_id == -1)
				{
					const char *error_s = NULL;

					/* The error strings are taken from http://linux.die.net/man/2/shmget */
					switch (errno)
						{
							case EACCES:
								error_s = "The user does not have permission to access the shared memory segment, and does not have the CAP_IPC_OWNER capability.";
								break;

							case EEXIST:
								error_s = "IPC_CREAT | IPC_EXCL was specified and the segment exists.";
								break;

							case EINVAL:
								error_s = "A new segment was to be created and size < SHMMIN or size > SHMMAX, or no new segment was to be created, a segment with given key existed, but size is greater than the size of that segment.";
								break;

							case ENFILE:
								error_s = "The system limit on the total number of open files has been reached.";
								break;

							case ENOENT:
								error_s = "No segment exists for the given key, and IPC_CREAT was not specified.";
								break;

							case ENOMEM:
								error_s = "No memory could be allocated for segment overhead.";
								break;

							case ENOSPC:
								error_s = "All possible shared memory IDs have been taken (SHMMNI), or allocating a segment of the requested size would cause the system to exceed the system-wide limit on shared memory (SHMALL).";
								break;

							case EPERM:
								error_s = "The SHM_HUGETLB flag was specified, but the caller was not privileged (did not have the CAP_IPC_LOCK capability).";
								break;

							default:
								error_s = "Unknown error occurred";
								break;
						}		/* switch (errno) */

					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, error_s);
				}		/* if (shm_id == -1) */

		}		/* if (key != (key_t) -1) */
	else
		{
			/* details taken from http://pubs.opengroup.org/onlinepubs/009695399/functions/ftok.html */

			const char *error_s = NULL;

			/* The error strings are taken from http://linux.die.net/man/2/shmget */
			switch (errno)
				{
					case EACCES:
						error_s = "Search permission is denied for a component of the path prefix.";
						break;

					case ELOOP:
						error_s = "A loop exists in symbolic links encountered during resolution of the path argument.";
						break;

					case ENAMETOOLONG:
						error_s = "The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}.";
						break;

					case ENOENT:
						error_s = "A component of path does not name an existing file or path is an empty string.";
						break;

					case ENOTDIR:
						error_s = "A component of the path prefix is not a directory.";
						break;

					case ENOSPC:
						error_s = "All possible shared memory IDs have been taken (SHMMNI), or allocating a segment of the requested size would cause the system to exceed the system-wide limit on shared memory (SHMALL).";
						break;

					case EPERM:
						error_s = "The SHM_HUGETLB flag was specified, but the caller was not privileged (did not have the CAP_IPC_LOCK capability).";
						break;

					default:
						error_s = "Unknown error occurred";
						break;
				}		/* switch (errno) */

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, error_s);
		}

	return shm_id;
}


bool FreeSharedMemory (int id)
{
	int res = shmctl (id, IPC_RMID, NULL);

	if (res != -1)
		{
			return true;
		}
	else
		{
			const char *error_s = NULL;

			/* The error strings are taken from http://linux.die.net/man/2/shmget */
			switch (errno)
				{
					case EACCES:
						error_s = "IPC_STAT or SHM_STAT is requested and shm_perm.mode does not allow read access for shmid, and the calling process does not have the CAP_IPC_OWNER capability.";
						break;

					case EFAULT:
						error_s = "The argument cmd has value IPC_SET or IPC_STAT but the address pointed to by buf isn't accessible.";
						break;

					case EINVAL:
						error_s = "shmid is not a valid identifier, or cmd is not a valid command. Or: for a SHM_STAT operation, the index value specified in shmid referred to an array slot that is currently unused.";
						break;

					case EIDRM:
						error_s = "shmid points to a removed identifier.";
						break;

					case ENOENT:
						error_s = "No segment exists for the given key, and IPC_CREAT was not specified.";
						break;

					case ENOMEM:
						error_s = "SHM_LOCK was specified and the size of the to-be-locked segment would mean that the total bytes in locked shared memory segments would exceed the limit for the real user ID of the calling process. This limit is defined by the RLIMIT_MEMLOCK soft resource limit (see setrlimit(2)).";
						break;

					case EOVERFLOW:
						error_s = "IPC_STAT is attempted, and the GID or UID value is too large to be stored in the structure pointed to by buf.";
						break;

					default:
						error_s = "Unknown error occurred";
						break;
				}

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, error_s);
			return false;
		}		/* if (res == -1) */
}


void *OpenSharedMemory (int id, int flags)
{
	void *mem_p = shmat (id, NULL, flags);

	if (mem_p != (char *) -1)
		{
			return mem_p;
		}
	else
		{
			const char *error_s = NULL;

			/* The error strings are taken from http://linux.die.net/man/2/shmat */
			switch (errno)
				{
					case EACCES:
						error_s = "The calling process does not have the required permissions for the requested attach type, and does not have the CAP_IPC_OWNER capability.";
						break;

					case EINVAL:
						error_s = "Invalid id value, unaligned (i.e., not page-aligned and SHM_RND was not specified) or invalid shmaddr value, or can't attach segment at shmaddr, or SHM_REMAP was specified and shmaddr was NULL.";
						break;

					case EIDRM:
						error_s = "id points to a removed identifier.";
						break;

					case ENOMEM:
						error_s = "Could not allocate memory for the descriptor or for the page tables.";
						break;

					default:
						error_s = "Unknown error occurred";
						break;
				}

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, error_s);

			return NULL;
		}
}


bool CloseSharedMemory (void *value_p)
{
	int res = shmdt (value_p);

	if (res != -1)
		{
			return true;
		}
	else
		{
			const char *error_s = NULL;

			/* The error strings are taken from http://linux.die.net/man/2/shmat */
			switch (errno)
				{
					case EINVAL:
						error_s = "There is no shared memory segment attached at shmaddr; or, shmaddr is not aligned on a page boundary.";
						break;

					default:
						error_s = "Unknown error occurred";
						break;
				}

			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, error_s);

			return false;
		}
}

