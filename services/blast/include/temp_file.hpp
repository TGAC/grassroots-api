/*
 * temp_file.hpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#ifndef TEMP_FILE_HPP_
#define TEMP_FILE_HPP_

#include <cstdio>

#include "blast_service.h"

class BLAST_SERVICE_LOCAL TempFile
{
public:
	static TempFile *GetTempFile (const char *mode_s);

	~TempFile ();

	const char *GetFilename () const;

	bool Open (const char *mode_s);
	int Close ();

	FILE *tf_handle_f;

private:
	char tf_name_s [L_tmpnam];

	TempFile ();

};


#endif /* TEMP_FILE_HPP_ */
