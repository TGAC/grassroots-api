/*
 * temp_file.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include <cstring>
#include <unistd.h>
#include "temp_file.hpp"




TempFile *TempFile :: GetTempFile (char *template_s, const char *mode_s)
{
	TempFile *tf_p = new TempFile;

	int fd = mkstemp (template_s);

	if (fd >= 1)
		{
			tf_p -> tf_name_s = template_s;

			close (fd);

			if ((strcmp (mode_s, "r") == 0) || (tf_p -> Open (mode_s)))
				{
					return tf_p;
				}
		}

	delete tf_p;

	return 0;
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
	if (tf_data_s)
		{
			FreeMemory (tf_data_s);
			tf_data_s = 0;
		}

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


