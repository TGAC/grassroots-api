#include <stdio.h>

#include <proto/dos.h>
#include <proto/elf.h>
#include <proto/exec.h>

#include "platform.h"
#include "amiga_filesystem.h"

static BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version);
static void CloseLib (struct Library *library_p, struct Interface *interface_p);

/*
struct Library *DOSBase = NULL;
struct DOSIFace *IDOS = NULL;
*/

struct Library *ElfBase;
struct ElfIFace *IElf;

BOOLEAN InitPlatform ()
{
	if (OpenLib (&DOSBase, "dos.library", 52L, (struct Interface **) &IDOS, "main", 1))
		{
			if (OpenLib (&ElfBase, "elf.library", 52L, (struct Interface **) &IElf, "main", 1))
				{
					return TRUE;
				}

			CloseLib (DOSBase, (struct Interface *) IDOS);
		}

	return FALSE;
}


void ExitPlatform ()
{
	/* if we have changed the current directory, we must restore the original */
	RestoreOriginalDirectory ();

	CloseLib (ElfBase, (struct Interface *) IElf);
	CloseLib (DOSBase, (struct Interface *) IDOS);
}


static BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version)
{
	if ((*library_pp = IExec->OpenLibrary (lib_name_s, lib_version)) != NULL)
		{
			if ((*interface_pp = IExec->GetInterface (*library_pp, interface_name_s, interface_version, NULL)) != NULL)
				{
					return TRUE;
				}
			else
				{
					printf ("failed to open interface \"%s\" version %lu from \"%s\"\n", interface_name_s, interface_version, lib_name_s);
				}
			IExec->CloseLibrary (*library_pp);
		}
	else
		{
			printf ("failed to open library \"%s\" version %lu\n", lib_name_s, lib_version);
		}

	return FALSE;
}


static void CloseLib (struct Library *library_p, struct Interface *interface_p)
{
	if (interface_p)
		{
			IExec->DropInterface (interface_p);
		}

	if (library_p)
		{
			IExec->CloseLibrary (library_p);
		}
}



void InitAmigaAllocator (void)
{
}

void ExitAmigaAllocator (void)
{
}

void *AllocateAmigaMemory (uint32 size)
{
	return IExec->AllocVecTags (size, TAG_DONE);
}


void *AllocateAmigaZeroedArray (uint32 num_elements, uint32 size)
{
	return IExec->AllocVecTags (num_elements * size, AVT_ClearWithValue, 0, TAG_DONE);
}


void *ReallocateAmigaMemory (void *old_p, long new_size, long old_size)
{
	void *new_p = IExec->AllocVecTags (new_size, TAG_DONE);

	if (new_p)
		{
			if (old_p)
				{
					IExec->CopyMem (old_p, new_p, old_size);
					IExec->FreeVec (old_p);
				}
		}

	return new_p;
}


void FreeAmigaMemory (void *mem_p)
{
	IExec->FreeVec (mem_p);
}


BOOLEAN IsAmigaAllocatorThreadSafe (void)
{
	return TRUE;
}
