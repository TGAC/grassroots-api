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
 * @brief None of these functions should be called directly, they will be set up by
 * the generated system and all code should use the platform-agnostic AllocMemory(),
 * AllocMemoryArray() and FreeMemory() instead.
 */
#ifndef AMIGA_PLATFORM_H
#define AMIGA_PLATFORM_H

#include <exec/types.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Initialise the memory allocator for Amiga OS4.
 */
void InitAmigaAllocator (void);


/**
 * Free any resources used by the memory allocator for AmigaOS 4.x.
 */
void ExitAmigaAllocator (void);


/**
 * Allocate a block of memory of the given size on AmigaOS 4.x.
 *
 * @param size The size of the required memory block in bytes.
 * @return The uninitialised memory block or <code>NULL</code> upon error.
 */
void *AllocateAmigaMemory (uint32 size);


/**
 * Allocate an array of memory blocks on AmigaOS 4.x.
 *
 * @param num_elements The number of memory blocks to allocate.
 * @param size The size of each of the required memory block in bytes.
 * @return The memory blocks which have all been initialised to 0 or <code>NULL</code> upon error.
 */
void *AllocateAmigaZeroedArray (uint32 num_elements, uint32 size);

/**
 * Reallocate a  memory block on AmigaOS 4.x.
 *
 * The values will be copied from the old block to
 * the new one if required whilst respecting buffer limits.
 *
 * @param old_p The memory block to reallocate.
 * @param new_size The size of the new memory block.
 * @param old_size The size of the old memory block.
 * @return The reallocated memory block or <code>NULL</code> upon error.
 */
void *ReallocateAmigaMemory (void *old_p, uint32 new_size, uint32 old_size);

/**
 * Free some memory created by AllocAmigaMemory() or AllocAmigaMemoryArray()
 *
 * @param mem_p The memory block to free.
 */
void FreeAmigaMemory (void *mem_p);

#ifdef __cplusplus
}
#endif

#endif		/*#ifndef AMIGA_PLATFORM_H */

