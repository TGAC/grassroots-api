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

/**
 * @file
 * @brief
 */
/**@file memory_allocations.h
*/

#ifndef MEMORY_ALLOCATIONS_H
#define MEMORY_ALLOCATIONS_H

#include "typedefs.h"

/**
 * An enum specifying the particular status of a piece of dynamically allocated memory for
 * a particular object. Its use is to allow many objects to have a pointer to some memory
 * but only one takes care of deallocating it.
 */
typedef enum MEM_FLAG
{
	/** The memory has already been freed so no action need be taken. */
	MF_ALREADY_FREED,

	/** Make a separate copy of the object pointed at by the source pointer.  */
	MF_DEEP_COPY,

	/** Use the pointer value. We will call free on this used value. Nothing else may free this pointer. */
	MF_SHALLOW_COPY,

	/** Use the pointer value. We will not call free on this used value, another module must free the memory. */
	MF_SHADOW_USE
} MEM_FLAG;


#ifdef __cplusplus
	extern "C" {
#endif


#ifdef AMIGA
	#include "amiga_platform.h"
	#define InitAllocator()	InitAmigaAllocator()
	#define ExitAllocator()	ExitAmigaAllocator()
	#define AllocMemory(x)	AllocateAmigaMemory(x)
	#define AllocMemoryArray(x,y)	AllocateAmigaZeroedArray(x,y)
	#define ReallocMemory(x,y,z)	ReallocateAmigaMemory(x,y,z)
	#define FreeMemory(x)	FreeAmigaMemory(x)
	#define IsAllocatorThreadSafe() IsAmigaAllocatorThreadSafe()
#else

	#ifdef WIN32
	//	#define USE_NEDMALLOC_ALLOCATOR
	#elif defined UNIX

	#endif

	#ifdef USE_TLSF_ALLOCATOR
	#elif defined USE_NEDMALLOC_ALLOCATOR
		#include "nedmalloc_allocator.h"
		#define InitAllocator()	InitNedmallocAllocator()
		#define ExitAllocator()	ExitNedmallocAllocator()
		#define AllocMemory(x)	AllocateNedmallocMemory(x)
		#define AllocMemoryArray(x,y)	AllocateNedmallocZeroedArray(x,y)
		#define ReallocMemory(x,y,z)	ReallocateNedmallocMemory(x,y,z)
		#define FreeMemory(x)	FreeNedmallocMemory(x)
		#define IsAllocatorThreadSafe() IsNedmallocAllocatorThreadSafe()
	#else
		#if defined _DEBUG && defined _MSC_VER
				#define _CRTDBG_MAP_ALLOC
				#include <stdlib.h>
				#include <crtdbg.h>
		#else
			#include <stdlib.h>
		#endif

		/** Initialise the platform memory allocator */
		#define InitAllocator()	do {} while (0);

		/** Close the platform memory allocator */
		#define ExitAllocator()	do {} while (0);

		/**
		 * Allocate the memory for, x, the given number of bytes.
		 */
		#define AllocMemory(x)	malloc(x)


		/**
		 * Allocate an array of memory where s is the given number
		 * of entries in the array with each entry being y bytes in size.
		 * If successful, all entries will be set to zero.
		 */
		#define AllocMemoryArray(x,y)	calloc(x,y)

		/**
		 * Reallocate the memory for the given number of bytes.
		 */
		#define ReallocMemory(x,y,z)	realloc(x,y)

		/** Free the memory pointed to by x */
		#define FreeMemory(x)	free(x)

		/** Is the current memory allocator suitable for use in a threaded environment> */
		#define IsAllocatorThreadSafe() (1)
	#endif

#endif



/**
 * Allocate some memory that can shared between different processes.
 *
 * @param id_s An identifier that will be used to access the shared memory segment
 * if it is successfully allocated.
 * @param size The size of the requested segment in bytes.
 * @param flags The permissions for the owner, group and world permissions in
 * standard unix format e.g. 0644 for owner to have read and write permissions whilst
 * everyone else having read permissions.
 * @return The segment identifier for the shared memory segment or -1 upon error.
 */
int AllocateSharedMemory (const char *id_s, size_t size, int flags);


/**
 * Free the shared memory segment for a given id.
 *
 * @param id The segment identifier for the shared memory segment to free.
 * @return <code>true</code> if the memory segment was freed successfully,
 * <code>false</code> otherwise.
 */
bool FreeSharedMemory (int id);


/**
 * Open a shared memory segment.
 *
 * @param id The segment identifier for the shared memory segment to open.
 * @param flags This can be SHM_RDONLY for read-only memory.
 * @return The pointer to the shared memory to be used or <code>NULL</code> upon error.
 */
void *OpenSharedMemory (int id, int flags);


/**
 * Close a shared memory segment.
 *
 * @param value_p The shared memory segment.
 * @return <code>true</code> if the memory segment was closed successfully,
 * <code>false</code> otherwise.
 */
bool CloseSharedMemory (void *value_p);


#ifdef __cplusplus
}
#endif

#endif	/* #ifndef MEMORY_ALLOCATIONS_H */
