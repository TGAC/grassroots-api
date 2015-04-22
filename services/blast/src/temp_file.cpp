/*
 * temp_file.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include <cstring>

#include "temp_file.hpp"


TempFile *TempFile :: GetTempFile (const char *mode_s)
{
	TempFile *tf_p = new TempFile;

	if ((tmpnam (tf_p -> tf_name_s)) != NULL)
		{
			if (tf_p -> Open (mode_s))
				{
					return tf_p;
				}
		}

	delete tf_p;

	return 0;
}


const char *TempFile :: GetFilename () const
{
	return ((tf_handle_f) ? tf_name_s : 0);
}


bool TempFile :: Open (const char *mode_s)
{
	int res = 0;

	if (!tf_handle_f)
		{
			tf_handle_f = fopen (tf_name_s, mode_s);
		}

	return (tf_handle_f != 0);
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
	Close ();
	memset (tf_name_s, 0, sizeof (char) * L_tmpnam);
}


TempFile :: ~TempFile ()
{
	if (tf_handle_f)
		{
			fclose (tf_handle_f);
		}
}

