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
#include "string_utils.h"
#include "streams.h"


#ifdef _DEBUG
	#define TEMP_FILE_DEBUG	(STM_LEVEL_FINE)
#else
	#define TEMP_FILE_DEBUG	(STM_LEVEL_NONE)
#endif



TempFile *TempFile :: GetTempFile (const char *working_dir_s, const uuid_t id, const char * const suffix_s)
{
	TempFile *file_p = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);
	bool success_flag = false;

	if (buffer_p)
		{
			if (working_dir_s)
				{
					if (AppendStringToByteBuffer (buffer_p, working_dir_s))
						{
							size_t l = strlen (working_dir_s);
							char c = GetFileSeparatorChar ();

							if (* (working_dir_s + (l - 1)) != c)
								{
									if (AppendToByteBuffer (buffer_p, &c, 1))
										{
											success_flag = true;
										}
								}
							else
								{
									success_flag = true;
								}
						}		/* if (AppendStringToByteBuffer (buffer_p, ebt_working_directory_s)) */

				}		/* if (ebt_working_directory_s) */
			else
				{
					success_flag = true;
				}

			if (success_flag)
				{
					char *uuid_s = GetUUIDAsString (id);

					success_flag = false;

					if (uuid_s)
						{
							if (AppendStringsToByteBuffer (buffer_p, uuid_s, suffix_s, NULL))
								{
									const char *full_filename_s = DetachByteBufferData(buffer_p);

									buffer_p  = 0;
									file_p = TempFile :: GetTempFile (full_filename_s, false);

									if (file_p)
										{
											file_p -> Close ();
										}
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get temp file \"%s\"", full_filename_s);
										}

								}		/* if (AppendStringsToByteBuffer (buffer_p, uuid_s, suffix_s, NULL)) */

							FreeUUIDString (uuid_s);
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid as string");
						}

				}		/* if (success_flag) */

			if (buffer_p)
				{
					FreeByteBuffer (buffer_p);
				}
		}		/* if (buffer_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get buffer for temp filename");
		}

	return file_p;

}



TempFile *TempFile :: GetTempFile (const char *template_s, const bool temp_flag)
{
	FILE *file_p = NULL;
	TempFile *tf_p = new TempFile;

	if (temp_flag)
		{
			char *copied_template_s = CopyToNewString (template_s, 0, false);

			if (copied_template_s)
				{
					int fd = mkstemp (copied_template_s);

					if (fd >= 1)
						{
							close (fd);

							file_p = fopen (copied_template_s, "w");
							tf_p -> tf_name_mem = MF_SHALLOW_COPY;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Faiiled to create temp file for \"%s\"", copied_template_s);
							FreeCopiedString (copied_template_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Faiiled to copy temp filename for \"%s\"", template_s);
				}
		}
	else
		{
			file_p = fopen (template_s, "w");
			tf_p -> tf_name_s = (char *) template_s;
			tf_p -> tf_name_mem = MF_SHADOW_USE;
		}

	if (file_p)
		{
			tf_p -> tf_handle_f = file_p;
		}
	else
		{
			delete tf_p;
			tf_p = 0;
		}

	return tf_p;
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
			tf_data_s = GetFileContentsAsString (tf_handle_f);
		}

	return tf_data_s;
}


void TempFile :: ClearData ()
{
	if (tf_data_s)
		{
			FreeMemory (tf_data_s);

			#if TEMP_FILE_DEBUG >= STM_LEVEL_FINER
			PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "Clearing data from %s", tf_name_s);
			#endif

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
			switch (tf_name_mem)
				{
					case MF_DEEP_COPY:
					case MF_SHALLOW_COPY:
						FreeCopiedString (tf_name_s);
						break;

					default:
						break;
				}
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

			if (prefix_s)
				{
					memcpy (buffer_p, prefix_s, prefix_length * sizeof (char));
					buffer_p +=  prefix_length * sizeof (char);
				}

			if (suffix_s)
				{
					memcpy (buffer_p, suffix_s, suffix_length * sizeof (char));
					buffer_p += suffix_length * sizeof (char);
				}

			*buffer_p = '\0';
		}

	return buffer_s;
}

