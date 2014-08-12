
#include <stdarg.h>
#include <string.h>

#include <windows.h>
#include <Dbghelp.h>

#include "filesystem.h"
#include "linked_list.h"
#include "memory_allocations.h"
#include "string_utils.h"
#include "string_linked_list.h"



char GetFileSeparatorChar (void)
{
	return '\\';
}

const char *GetCurrentDirectoryString (void)
{
	return ".\\";
}

const char *GetParentDirectory (void)
{
	return "..\\";
}

const char *GetPluginPattern (void)
{
	return "*.dll";
}


BOOLEAN IsPathValid (const char * const path)
{
	DWORD attrs = GetFileAttributes (path);
	return ((attrs | FILE_ATTRIBUTE_DIRECTORY) != 0);
}


BOOLEAN IsPathAbsolute (const char * const path_s)
{
	BOOLEAN absolute_path_flag = FALSE;

	if (path_s)
		{
			const size_t len = strlen (path_s);

			if (len >= 2)
				{
					if (strncmp ("\\\\", path_s, 2) == 0)
						{
							absolute_path_flag = TRUE;
						}
					else if ((len >= 3) && (isalpha (*path_s)) && (strncmp (":\\", path_s + 1, 2) == 0))
						{
							absolute_path_flag = TRUE;
						}
				}
		}

	return absolute_path_flag;
}


LinkedList *GetMatchingFiles (const char * const pattern)
{
	LinkedList *list_p = AllocateLinkedList (FreeStringListNode);

	if (list_p)
		{
			WIN32_FIND_DATA file_data;
			char filename [MAX_PATH + 1];
			HANDLE handle;
			size_t len = strlen (pattern);

			if (len <= MAX_PATH)
				{
					strcpy (filename, pattern);

					if ((handle = FindFirstFile (filename, &file_data)) != INVALID_HANDLE_VALUE)
						{
							uint32 value = (file_data.dwFileAttributes) & FILE_ATTRIBUTE_DIRECTORY;

							if (value != FILE_ATTRIBUTE_DIRECTORY)
								{
									StringListNode *node_p = AllocateStringListNode (file_data.cFileName, MF_DEEP_COPY);
									if (node_p)
										{
											LinkedListAddTail (list_p, (ListNode *) node_p);
										}

									while (FindNextFile (handle, &file_data))
										{
											value = (file_data.dwFileAttributes) & FILE_ATTRIBUTE_DIRECTORY;
											if (value != FILE_ATTRIBUTE_DIRECTORY)
												{
													node_p = AllocateStringListNode (file_data.cFileName, MF_DEEP_COPY);
													if (node_p)
														{
															LinkedListAddTail (list_p, (ListNode *) node_p);
														}

												}
										}
								}

							if (! (FindClose (handle)))
								{
									// couldn't close search handle
								}
						}
				}

			if (list_p -> ll_size > 0)
				{
					return list_p;
				}
			else
				{
					FreeLinkedList (list_p);
				}
		}

	return NULL;
}


BOOLEAN EnsureDirectoryExists (const char * const path_s)
{
	return MakeSureDirectoryPathExists (path_s);
}


BOOLEAN CopyToNewFile (const char * const src_filename, const char * const dest_filename, void (*callback_fn) ())
{
	LPPROGRESS_ROUTINE prog_fn = (LPPROGRESS_ROUTINE) callback_fn;

	if (callback_fn)
		{
			return (CopyFileEx (src_filename, dest_filename, prog_fn, NULL, 0, 0) != 0);
		}
	else
		{
			return (CopyFile (src_filename, dest_filename, TRUE) != 0);
		}
}


BOOLEAN SetCurrentWorkingDirectory (const char * const path)
{
	return (SetCurrentDirectory (path) != 0);
}


char *GetCurrentWorkingDirectory (void)
{
  TCHAR buffer [MAX_PATH];
  DWORD path_length = GetCurrentDirectory (MAX_PATH, buffer);

  if (path_length == 0)
	  {
			/* Failed */
		  return NULL;
		}
	else if (path_length > MAX_PATH)
	  {
		  /* Buffer too small */
			return NULL;
		}
	else
		{
			return CopyToNewString (buffer, 0, FALSE);
		}
}


BOOLEAN IsDirectory (const char * const path)
{
	DWORD dw = GetFileAttributes (path);

	return (dw & FILE_ATTRIBUTE_DIRECTORY);
}



/*

STRING *WindowsFileSystem :: makeLibName (const STRING &str)
{
	return new STRING (str + ".dll");
}

STRING *WindowsFileSystem :: makeFilename (const STRING &path, const STRING &filename)
{
	const uint32 path_length = path.length ();
	const uint32 filename_length = filename.length ();
	const STRING &parent_dir = getParentDirectory ();
	const uint32 parent_length = parent_dir.length ();

	uint32 parent_count = 0;
	uint32 i = 0;
	STRING sub;

	// does filename use ..\ ?
	while ((filename.compare (i, parent_length, parent_dir)) == 0)
		{
			i += parent_length;
			++ parent_count;
		}


	if (parent_count)
		{
			uint32 j = path_length - 1; // ignore last char as if sep is irrelevant
			uint32 sep_count = 0;
			char sep = getSeparator ();

			for ( ; j > 0; -- j)
				{
					if (path [j] == sep)
						{
							++ sep_count;

							if (sep_count == parent_count)
								{
									break;
								}
						}
				}

			if (sep_count == parent_count)
				{
					STRING *s_p = new STRING (path.substr (0, j + 1));

					*s_p += filename.substr (i, parent_length - i);

					return s_p;
				}
			else
				{
					(*getErrorStream ())  << "WindowsFileSystem :: makeFilename. Impossible to resolve path " << path << " and filename " << filename << endl;
					return 0;
				}
		}
	else
		{
			switch (path_length)
				{
					case 0:
						{
							return new STRING (filename);
						}
					break;	// case 0

					case 1:
						{
							if (isalpha (path [0]))
								{
									return new STRING (path + ":\\" + filename);
								}
							else
								{
									return new STRING (path + "\\" + filename);
								}
						}
					break;	// case 1

					case 2:
						{
							if ((isalpha (path [0])) && (path [1] == ':'))
								{
									return new STRING (path + "\\" + filename);
								}
							else
								{
									return new STRING (path + "\\" + filename);
								}
						}
					break;	// case 2


					default:
						{
							if (path [path_length - 1] != '\\')
								{
									return new STRING (path + '\\' + filename);
								}
							else
								{
									return new STRING (path + filename);
								}
						}
					break;	// default
				}
		}

	(*getErrorStream ())  << "makeFilename: unable to resolve Path " << path << " " << filename << endl;
	return 0;
}

LOGICAL WindowsFileSystem :: ensurePathValid (const STRING &path)
{
	return FALSE;
}

void WindowsFileSystem :: setCurDir (const STRING & path)
{
	char * dir=_getcwd(NULL,1024);
	afs_old_cur_dir=dir;
	free(dir);
	_chdir(path.c_str());
}


void WindowsFileSystem :: getCurDir (STRING &path)
{
	path = getcwd(NULL,1024);
}

void WindowsFileSystem :: restoreCurDir ()
{
	_chdir(afs_old_cur_dir.c_str());
}

LOGICAL WindowsFileSystem::isAbsolute (const STRING &path)
{
	if (isalpha(path.at(0)) && (path.substr(1,2)==":\\"))
		return TRUE;

	return FALSE;
}


void WindowsFileSystem :: appendSeparator (STRING &str) const
{
	const uint32 length = str.length ();

	switch (length)
		{
			case 1:

			break;

			case 2:
				if (isalpha (str [0]))
					{
						str += '\\';
					}
			break;

			default:
				if (str [length - 1] != '\\')
					{
						str += '\\';
					}
			break;
		}
}
*/
