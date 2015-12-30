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
/*
 * temp_file.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include <cstring>
#include <unistd.h>
#include "temp_file.hpp"

#ifdef _DEBUG
	#define SERVICE_DEBUG	(STM_LEVEL_FINE)
#else
	#define SERVICE_DEBUG	(STM_LEVEL_NONE)
#endif

TempFile *TempFile :: GetTempFile (char *template_s, const bool temp_flag)
{
	FILE *file_p = NULL;
	TempFile *tf_p = new TempFile;

	if (temp_flag)
		{
			int fd = mkstemp (template_s);

			if (fd >= 1)
				{
					close (fd);

					file_p = fopen (template_s, "w");
				}

		}
	else
		{
			file_p = fopen (template_s, "w");
		}

	if (file_p)
		{
			tf_p -> tf_handle_f = file_p;
			tf_p -> tf_name_s = template_s;
		}
	else
		{
			delete tf_p;
			tf_p = 0;
		}

	return tf_p;
}


void TempFile :: DeleteTempFile (TempFile *tf_p)
{
	delete tf_p;
}


const char *TempFile :: GetFilename () const
{
	return tf_name_s;
}


bool TempFile :: IsOpen () const
{
	return (tf_handle_f != 0);
}


bool TempFile :: Open (const char *mode_s)
{
	if (tf_handle_f)
		{
			Close ();
		}

	tf_handle_f = fopen (tf_name_s, mode_s);

	return (tf_handle_f != NULL);
}


bool TempFile :: Print (const char *arg_s)
{
	bool res = false;

	if (tf_handle_f)
		{
			res = (fprintf (tf_handle_f, "%s", arg_s) >= 0);
		}

	return res;
}


const char *TempFile :: GetData ()
{
	ClearData ();

	if (tf_handle_f)
		{
			size_t current_pos = ftell (tf_handle_f);

			// Determine file size
			fseek (tf_handle_f, 0, SEEK_END);
			size_t size = ftell (tf_handle_f);

			tf_data_s = (char *) AllocMemory ((size + 1) * sizeof (char));
			if (tf_data_s)
				{
					rewind (tf_handle_f);

					fread (tf_data_s, sizeof (char), size, tf_handle_f);
					* (tf_data_s + size) = '\0';
					fseek (tf_handle_f, current_pos, SEEK_SET);
				}
		}

	return tf_data_s;
}


void TempFile :: ClearData ()
{
	if (tf_data_s)
		{
			FreeMemory (tf_data_s);
			PrintLog (STM_LEVEL_FINE, __FILE__, __LINE__, "Clearing data from %s", tf_name_s);

			tf_data_s = 0;
		}
}


int TempFile :: Close ()
{
	int res = 0;

	if (tf_handle_f)
		{
			res = fclose (tf_handle_f);
			tf_handle_f = 0;
		}

	return res;
}


TempFile :: TempFile ()
{
	tf_handle_f = 0;
	tf_data_s = 0;
	tf_name_s = 0;
}


TempFile :: ~TempFile ()
{
	Close ();

	if (tf_data_s)
		{
			FreeMemory (tf_data_s);
		}

	if (tf_name_s)
		{
			FreeMemory (tf_name_s);
		}
}


/* need a buffer where the final 6 chars are XXXXXX, see mkstemp */
char *GetTempFilenameBuffer (const char * const working_directory_s, const char * const prefix_s, const char * const temp_suffix_s)
{
	char *buffer_s = 0;
	const char * const suffix_s = temp_suffix_s ? temp_suffix_s : "XXXXXX";
	const size_t working_dir_length = working_directory_s ? strlen (working_directory_s) : 0;
	const size_t suffix_length = suffix_s ? strlen (suffix_s) : 0;
	const size_t prefix_length = strlen (prefix_s);

	size_t size = 1 + working_dir_length + prefix_length + suffix_length;
	bool needs_slash_flag = false;

	if (working_directory_s)
		{
			needs_slash_flag = (* (working_directory_s + (size - 1)) != GetFileSeparatorChar ());
		}

	if (needs_slash_flag)
		{
			++ size;
		}

	buffer_s = (char *) AllocMemory (size * sizeof (char));

	if (buffer_s)
		{
			char *buffer_p = buffer_s;

			if (working_directory_s)
				{
					memcpy (buffer_p, working_directory_s, working_dir_length * sizeof (char));
					buffer_p +=  working_dir_length * sizeof (char);

					if (needs_slash_flag)
						{
							*buffer_p = GetFileSeparatorChar ();
							++ buffer_p;
						}
				}

			memcpy (buffer_p, prefix_s, prefix_length * sizeof (char));
			buffer_p +=  prefix_length * sizeof (char);

			if (suffix_s)
				{
					memcpy (buffer_p, suffix_s, suffix_length * sizeof (char));
					buffer_p += suffix_length * sizeof (char);
				}

			*buffer_p = '\0';
		}

	return buffer_s;
}

