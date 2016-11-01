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


bool InitPlatform ()
{
	if (OpenLib (&DOSBase, "dos.library", 52L, (struct Interface **) &IDOS, "main", 1))
		{
			if (OpenLib (&ElfBase, "elf.library", 52L, (struct Interface **) &IElf, "main", 1))
				{
					return true;
				}

			CloseLib (DOSBase, (struct Interface *) IDOS);
		}

	return false;
}


void ExitPlatform ()
{
	/* if we have changed the current directory, we must restore the original */
	RestoreOriginalDirectory ();

	CloseLib (ElfBase, (struct Interface *) IElf);
	CloseLib (DOSBase, (struct Interface *) IDOS);
}


uint32 Snooze (uint32 milliseconds)
{
	uint32 error;
	uint32 seconds = milliseconds * 0.0010;
	uint32 microseconds = (milliseconds - (seconds * 1000)) * 1000;

	// create message port
	struct MsgPort *TimerMP = IExec->AllocSysObjectTags (ASOT_PORT, TAG_END);
	if (TimerMP != NULL)
		{
			struct TimeRequest *TimerIO;
			struct Message *TimerMSG;

			// allocate IORequest struct for TimeRequest
			TimerIO = IExec->AllocSysObjectTags (ASOT_IOREQUEST, ASOIOR_Size,
																					 sizeof(struct TimeRequest),
																					 ASOIOR_ReplyPort, TimerMP, TAG_END);

			// test if we got our IORequest
			if (TimerIO != NULL)
				{
					// Open the timer.device
					if (!(error = IExec->OpenDevice (TIMERNAME, UNIT_VBLANK,
																					 (struct IORequest *) TimerIO, 0L)))
						{
							// timer.dev opened - Initialize other IORequest fields
							TimerIO->Request.io_Command = TR_ADDREQUEST;
							TimerIO->Time.Seconds = seconds;
							TimerIO->Time.Microseconds = microseconds;

							// Send IORequest
							IExec->SendIO ((struct IORequest *) TimerIO);

							// There might be other processing done here

							// Now go to sleep with WaitPort() waiting for the request to return
							IDOS->Printf ("Now to wait for the rest of %ld secs...\n",
														seconds);
							IExec->WaitPort (TimerMP);

							// Get the reply message
							TimerMSG = IExec->GetMsg (TimerMP);

							if (TimerMSG == (struct Message *) TimerIO)
								IDOS->Printf (
										"Request returned.  Closing device and quitting...\n");

							// close timer.device
							IExec->CloseDevice ((struct IORequest *) TimerIO);
						}
					else
						IDOS->Printf ("\nError: Could not OpenDevice.\n");

					// dispose of IORequest
					IExec->FreeSysObject (ASOT_IOREQUEST, TimerIO);
				}
			else
				{
					IDOS->Printf ("Error: could not allocate IORequest.\n");
				}

			// Close and dispose of message port.
			IExec->FreeSysObject (ASOT_PORT, TimerMP);
		}
	else
		{
			IDOS->Printf ("\nError: Could not create message port.\n");
		}

	return 0;
}


static bool OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version)
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

	return false;
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


bool IsAmigaAllocatorThreadSafe (void)
{
	return true;
}


