#include <stddef.h>
#include <string.h>

#include "filesystem_utils.h"

#include "memory_allocations.h"
#include "string_utils.h"



char *GetFilenameOnly (const char * const full_path_to_file)
{
	const char FS_SEPARATOR = GetFileSeparatorChar ();
	const char *last_sep_p = strrchr (full_path_to_file, FS_SEPARATOR);

	if (last_sep_p)
		{
			++ last_sep_p; /* move past the FS_SEPARATOR */

			if (*last_sep_p != '\0')
				{
					return CopyToNewString (last_sep_p, 0, false);
				}
		}

	return NULL;
}

char *GetPathOnly (const char * const full_path_to_file)
{
	const char FS_SEPARATOR = GetFileSeparatorChar ();
	const char *last_sep_p = strrchr (full_path_to_file, FS_SEPARATOR);

	if (last_sep_p)
		{
			ptrdiff_t l = last_sep_p - full_path_to_file;
			char *path_p = (char *) AllocMemory (l + 1);

			if (path_p)
				{
					strncpy (path_p, full_path_to_file, l);
					* (path_p + l) = '\0';

					return path_p;
				}
		}

	return NULL;
}


bool DeterminePathAndFile (const char * const full_path_s, char ** const path_ss, char ** const filename_ss)
{
	bool success_flag = false;
	const char FS_SEPARATOR = GetFileSeparatorChar ();
	const char *last_sep_p = strrchr (full_path_s, FS_SEPARATOR);

	if (last_sep_p)
		{
			ptrdiff_t path_length = last_sep_p - full_path_s;
			char *path_s = (char *) AllocMemory (path_length + 1);

			if (path_s)
				{
					size_t file_length = strlen (full_path_s) - path_length;
					char *filename_s = (char *) AllocMemory (file_length + 1);

					if (filename_s)
						{
							strncpy (path_s, full_path_s, path_length);
							* (path_s + path_length) = '\0';

							strncpy (filename_s, full_path_s + path_length + 1, file_length);
							* (filename_s + file_length) = '\0';

							*path_ss = path_s;
							*filename_ss = filename_s;

							success_flag = true;
						}		/* if (file_s) */
					else
						{
							FreeCopiedString (path_s);
						}

				}		/* if (path_s) */
		
		}		/* if (last_sep_p) */
	else
		{

		}

	return success_flag;
}


void UsePlatformFileSeparator (char *value_s)
{
	const char FS_SEPARATOR = GetFileSeparatorChar ();

	if (FS_SEPARATOR != '/')
		{
			char *c_p = value_s;
			bool loop_flag = (c_p != NULL);

			while (loop_flag)
				{
					c_p = strchr (c_p, '/');

					if (c_p)
						{
							*c_p = FS_SEPARATOR;
							++ c_p;
						}
					else 
						{
							loop_flag = false;
						}
				}

		}
}


char *SetFileExtension (const char * const filename_s, const char * const extension_s, bool replace_flag)
{
	char *new_filename_s = NULL;
	char *dot_p = replace_flag ? strrchr (filename_s, '.') : NULL;
	size_t l;
	const size_t extension_length = strlen (extension_s);
	
	if (dot_p)
		{
			l = dot_p - filename_s;
		}
	else
		{
			l = strlen (filename_s);
		}
		
		
	new_filename_s = AllocMemory (sizeof (char) * (l + extension_length + 2));
	
	if (new_filename_s)
		{
			strncpy (new_filename_s, filename_s, l);
			dot_p = new_filename_s + l;
			*dot_p = '.';
			strcpy (dot_p + 1, extension_s);
		}
		
	return new_filename_s;
}


char *MakeFilename (const char * const path, const char * const filename)
{
	const size_t path_length = strlen (path);
	const char FS_SEPARATOR = GetFileSeparatorChar ();
	bool needs_separator_flag = false;
	size_t name_length = path_length + strlen (filename);
	char *name_p = NULL;

	if (path_length > 0)
		{
			 needs_separator_flag = ((* (path + path_length - 1) != FS_SEPARATOR) && (* (path + path_length - 1) != '/'));
			 
			 name_length += (needs_separator_flag ? 2 : 1);
		}

	name_p = (char *) AllocMemory (name_length);
	if (name_p)
		{
			char *ptr = name_p;

			strncpy (ptr, path, path_length);
			ptr += path_length;

			if (FS_SEPARATOR != '/')
				{
					char *c_p = name_p;

					while ((c_p = strchr (c_p, '/')) != NULL)
						{
							*c_p = FS_SEPARATOR;
						}
				}

			if (needs_separator_flag)
				{
					*ptr = FS_SEPARATOR;
					++ ptr;
				}

			strcpy (ptr, filename);

			return name_p;
		}

	return NULL;
}


