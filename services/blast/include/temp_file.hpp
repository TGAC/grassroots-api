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
	static TempFile *GetTempFile (char *template_s, const char *mode_s);


	static void DeleteTempFile (TempFile *tf_p);

	~TempFile ();


	const char *GetFilename () const;

	bool Open (const char *mode_s);
	int Close ();

	bool Print (const char *arg_s);

	const char *GetData ();

	bool IsOpen () const;

private:
	char *tf_name_s;
	FILE *tf_handle_f;
	char *tf_data_s;

	TempFile ();

};


#ifdef __cplusplus
extern "C"
{
#endif


BLAST_SERVICE_LOCAL char *GetTempFilenameBuffer (const char * const prefix_s, const char * const working_directory_s);

#ifdef __cplusplus
}
#endif



#endif /* TEMP_FILE_HPP_ */
