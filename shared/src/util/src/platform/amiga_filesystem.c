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

#include <stdarg.h>
#include <string.h>


#include <proto/dos.h>
#include <proto/exec.h>

#include <dos/dos.h>

#include "amiga_debugging.h"
#include "amiga_filesystem.h"

#include "filesystem.h"
#include "linked_list.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "string_linked_list.h"


static STRPTR CompilePattern (CONST_STRPTR pattern_s);


static char *s_original_working_directory_s = NULL;
static BPTR s_original_lock_p = ZERO;

const char GetFileSeparatorChar (void)
{
	return '/';
}

const char *GetCurrentDirectoryString (void)
{
	return "";
}

const char *GetParentDirectory (void)
{
	return "/";
}

const char *GetPluginPattern (void)
{
	return "lib#?.so";
}


BOOLEAN IsPathValid (const char * const path_s)
{
	BOOLEAN result = FALSE;
	BPTR lock_p = IDOS->Lock (path_s, SHARED_LOCK);

	if (lock_p != ZERO)
		{
			result = TRUE;
			IDOS->UnLock (lock_p);
		}

	return result;
}


BOOLEAN IsPathAbsolute (const char * const path_s)
{
	BOOLEAN absolute_path_flag = FALSE;

	if (path_s)
		{
			absolute_path_flag = (strchr (path_s, ':') != NULL);
		}

	return absolute_path_flag;
}


/**** @TODO ****/
LinkedList *GetMatchingFiles (const char * const pattern)
{
	LinkedList *names_p = NULL;
	STRPTR compiled_pattern_s = CompilePattern (pattern);

	if (compiled_pattern_s)
		{
			struct AnchorPath *ap_p = IDOS->AllocDosObjectTags (DOS_ANCHORPATH,
				ADO_Strlen, 2048,
				ADO_Flags, 0,
				ADO_Mask, SIGBREAKF_CTRL_C,
				TAG_DONE);

			if (ap_p)
				{
					names_p = AllocateLinkedList (FreeStringListNode);

					if (names_p)
						{
							int32 error;

							for (error = IDOS->MatchFirst (compiled_pattern_s, ap_p); error == 0; error = IDOS->MatchNext (ap_p))
								{
									StringListNode *node_p = AllocateStringListNode (ap_p -> ap_Info.fib_FileName, MF_DEEP_COPY);

									if (node_p)
										{
											LinkedListAddTail (names_p, (ListNode * const) node_p);
										}
									else
										{
											error = 1;
										}
								}

							if (error != 0)
								{
									FreeLinkedList (names_p);
									names_p = NULL;
								}
						}

					IDOS->FreeDosObject (DOS_ANCHORPATH, ap_p);
				}

			FreeMemory (compiled_pattern_s);
		}

	return names_p;
}



BOOLEAN EnsureDirectoryExists (const char * const path_s)
{
	size_t l = strlen (path_s);
	char *buffer_s = (char *) AllocMemory (l + 1);
	BOOL success_flag = FALSE;

	if (buffer_s)
		{
			IExec->CopyMem (path_s, buffer_s, l);
			* (buffer_s + l) = '\0';

			/* remove any trailing slashes */
			-- l;
			while (* (buffer_s + l) == '/')
				{
					* (buffer_s + l) = '\0';
					-- l;
				}

			if (*buffer_s != '\0')
				{
					if (* (buffer_s + l) == ':')
						{
							struct ExamineData *ex_data_p = IDOS->ExamineObjectTags (EX_StringNameInput, (uint32) buffer_s, TAG_DONE);

							if (ex_data_p)
								{
									if (((ex_data_p -> Protection) & EXDF_WRITE) == 0)
										{
											DB (KPRINTF ("%s %ld - \"%s\" is writeable\n", __FILE__, __LINE__, buffer_s));
											success_flag = TRUE;
										}
									else
										{
											DB (KPRINTF ("%s %ld - \"%s\" is not writeable\n", __FILE__, __LINE__, buffer_s));
										}

									if (((ex_data_p -> Protection) & EXDF_READ) == 0)
										{
											DB (KPRINTF ("%s %ld - \"%s\" is readable\n", __FILE__, __LINE__, buffer_s));
										}
									else
										{
											DB (KPRINTF ("%s %ld - \"%s\" is not readable\n", __FILE__, __LINE__, buffer_s));
										}

									IDOS->FreeDosObject (DOS_EXAMINEdouble64, ex_data_p);
								}
							else
								{
									DB (KPRINTF ("%s %ld - failed to examine \"%s\"\n", __FILE__, __LINE__, buffer_s));
								}
						}
					else
						{
							BPTR lock_p = ZERO;

							DB (KPRINTF ("%s %ld - creating \"%s\"\n", __FILE__, __LINE__, buffer_s));
							lock_p = IDOS->CreateDirTree (buffer_s);

							if (lock_p)
								{
									DB (KPRINTF ("%s %ld - created \"%s\"\n", __FILE__, __LINE__, buffer_s));
									IDOS->UnLock (lock_p);
									success_flag = TRUE;
								}
							else
								{
									DB (KPRINTF ("%s %ld - failed to create \"%s\" error code is %ld\n", __FILE__, __LINE__, buffer_s, IDOS->IoErr ()));
								}
						}
				}

			FreeMemory (buffer_s);
		}   /* if (buffer_s) */

	return success_flag;
}



BOOLEAN CopyToNewFile (const char * const src_filename_s, const char * const dest_filename_s, void (*callback_fn) ())
{
	BPTR src_handle_p = IDOS->Open (src_filename_s, MODE_OLDFILE);
	BOOLEAN success_flag = FALSE;

	if (src_handle_p != ZERO)
		{
			BPTR dest_handle_p = IDOS->Open (dest_filename_s, MODE_READWRITE);

			if (dest_handle_p != ZERO)
				{
					uint32 num_blocks = 32;
					uint32 block_size = 256;
					char buffer [8192];   /* 32 * 256 = num_blocks * block_size */
					BOOLEAN loop_flag = TRUE;

					success_flag = TRUE;
					IDOS->SetIoErr (0);
					memset (buffer, 0, num_blocks * block_size);

					while (loop_flag && success_flag)
						{
							uint32 src_count = IDOS->FRead (src_handle_p, buffer, block_size, num_blocks);

							if (src_count > 0)
								{
									uint32 dest_count = IDOS->FWrite (dest_handle_p, buffer, block_size, src_count);

									if (src_count != dest_count)
										{
											success_flag = FALSE;
										}
									else
										{
											if (src_count != num_blocks)
												{
													success_flag = (IDOS->IoErr () == 0);

													if (success_flag)
														{
															/* do we have any left over data? */
															char *start_p = buffer + (src_count * block_size);
															char *end_p = start_p + block_size;
															char *marker_p = start_p;
															uint32 i = 0;

															DB (KPRINTF ("%s %ld - c [-2]=\'%c\' %x\n", __FILE__, __LINE__, * (marker_p - 2), * (marker_p - 2)));
															DB (KPRINTF ("%s %ld - c [-1]=\'%c\' %x\n", __FILE__, __LINE__, * (marker_p - 1), * (marker_p - 1)));

															while ((*marker_p != '\0') && (marker_p != end_p))
																{
																	DB (KPRINTF ("%s %ld - c [%lu]=\'%c\' %x\n", __FILE__, __LINE__, i, *marker_p, *marker_p));
																	++ marker_p;
																	++ i;
																}

															DB (KPRINTF ("%s %ld - leftover %lu bytes\n", __FILE__, __LINE__, i));

															if (i > 0)
																{
																	if (IDOS->FWrite (dest_handle_p, start_p, i, 1) != 1)
																		{
																			success_flag = FALSE;
																		}
																}

														}

													loop_flag = FALSE;
												}
											else
												{
													memset (buffer, 0, num_blocks * block_size);
												}
										}
								}
							else
								{
									loop_flag = FALSE;
								}

						}   /* while (success_flag) */

					IDOS->Close (dest_handle_p);

				}   /* if (out_f) */

			IDOS->Close (src_handle_p);

		}   /* if (in_f) */

	return success_flag;

}


BOOLEAN SetCurrentWorkingDirectory (const char * const path_s)
{
	BOOLEAN result = FALSE;

/*
		You should be very, very carefull to preserve the current directory
		your program was started with and to restore it before it exits.
		Do not return with a different 'current directory' set, and do
		not return with a different lock referring to the same directory
		as the one you were started with. Always exit with exactly the
		same lock on the current directory your program was started with.
*/
	BPTR lock_p = IDOS->Lock (path_s, SHARED_LOCK);

	if (lock_p != ZERO)
		{
			if (s_original_lock_p == ZERO)
				{
					s_original_lock_p = IDOS->CurrentDir (lock_p);
				}
			else
				{
					BPTR old_lock_p = IDOS->CurrentDir (lock_p);

					IDOS->UnLock (old_lock_p);
				}

			result = TRUE;
		}

	return result;
}


void RestoreOriginalDirectory (void)
{
	if (s_original_lock_p != ZERO)
		{
			BPTR lock_p = IDOS->CurrentDir (s_original_lock_p);
			IDOS->UnLock (lock_p);
		}
}


char *GetCurrentWorkingDirectory (void)
{
	#define BUF_LEN (2048)
	char *dir_s = NULL;
	char buffer [BUF_LEN];
	BPTR lock_p = ZERO;

	DB (KPRINTF ("%s %ld - About to get current dir lock\n", __FILE__, __LINE__));
	lock_p = IDOS->GetCurrentDir ();

	DB (KPRINTF ("%s %ld - current dir lock is  %lu\n", __FILE__, __LINE__, (uint32) BADDR (lock_p)));

	if (lock_p != ZERO)
		{
			if (IDOS->NameFromLock (lock_p, buffer, BUF_LEN - 1) != DOSFALSE)
				{
					DB (KPRINTF ("%s %ld - got name from current dir lock: \"%s\"\n", __FILE__, __LINE__, buffer));
					dir_s = CopyToNewString (buffer, 0, FALSE);
				}

			//IDOS->UnLock (lock_p);
		}

	return dir_s;
}


BOOLEAN IsDirectory (const char * const path_s)
{
	BOOLEAN result = FALSE;
	BPTR lock_p = IDOS->Lock (path_s, SHARED_LOCK);

	if (lock_p)
		{
			struct FileInfoBlock *fib_p = (struct FileInfoBlock *) IDOS->AllocDosObject (DOS_FIB, TAG_DONE);

			if (fib_p)
				{
					result = FIB_IS_DRAWER (fib_p);

					IDOS->FreeDosObject (DOS_FIB, fib_p);
				}

			IDOS->UnLock (lock_p);
		}

	return result;
}


static STRPTR CompilePattern (CONST_STRPTR pattern_s)
{
	int32 pattern_length = (int32) strlen (pattern_s);
	pattern_length = (pattern_length * 2) + 2;

	STRPTR buffer_s = (STRPTR) AllocMemory (pattern_length);

	if (buffer_s)
		{
			int32 res = IDOS->ParsePattern (pattern_s, buffer_s, pattern_length);

			if (res >= 0)
				{
					return buffer_s;
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to compile \"%s\"\n", pattern_s);
				}

			FreeMemory (buffer_s);
		}

	return NULL;
}

