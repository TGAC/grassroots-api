/*
 * temp_file.hpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#ifndef TEMP_FILE_HPP_
#define TEMP_FILE_HPP_

#include <fstream>
#include <cstdio>

#include "blast_service.h"

using namespace std;

class BLAST_SERVICE_LOCAL TempFile
{
public:
	static TempFile *GetTempFile (const char *mode_s);

	~TempFile ();

	const char *GetFilename () const;

	bool Open (const char *mode_s);
	int Close ();

	bool Print (const char *arg_s);

	const char *GetData ();

	bool IsOpen () const;

private:
	char tf_name_s [L_tmpnam];
	FILE *tf_handle_f;
	char *tf_data_s;

	TempFile ();

};


#endif /* TEMP_FILE_HPP_ */
