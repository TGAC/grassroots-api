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
#ifndef AMIGA_PLATFORM_H
#define AMIGA_PLATFORM_H

#include <exec/types.h>

#ifdef __cplusplus
	extern "C" {
#endif

void InitAmigaAllocator (void);

void ExitAmigaAllocator (void);

void *AllocateAmigaMemory (uint32 size);

void *AllocateAmigaZeroedArray (uint32 num_elements, uint32 size);

void *ReallocateAmigaMemory (void *old_p, uint32 new_size, uint32 old_size);

void FreeAmigaMemory (void *mem_p);

#ifdef __cplusplus
}
#endif

#endif		/*#ifndef AMIGA_PLATFORM_H */

