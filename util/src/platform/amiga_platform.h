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

