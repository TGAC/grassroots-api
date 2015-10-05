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
/**@file memory_allocations.h
*/

#ifndef MEMORY_ALLOCATIONS_H
#define MEMORY_ALLOCATIONS_H

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

	#define InitAllocator()	do {} while (0);
	#define ExitAllocator()	do {} while (0);
	#define AllocMemory(x)	malloc(x)
	#define AllocMemoryArray(x,y)	calloc(x,y)
	#define ReallocMemory(x,y,z)	realloc(x,y)
	#define FreeMemory(x)	free(x)
	#define IsAllocatorThreadSafe() (1)
#endif

#endif

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
}
#endif

#endif	/* #ifndef MEMORY_ALLOCATIONS_H */
