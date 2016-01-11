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
 * blast_formatter.h
 *
 *  Created on: 8 Jan 2016
 *      Author: billy
 */

#ifndef BLAST_FORMATTER_H_
#define BLAST_FORMATTER_H_

#include "jansson.h"

#include "blast_service_api.h"

class BLAST_SERVICE_LOCAL BlastFormatter
{
public:
	BlastFormatter ();
	virtual ~BlastFormatter ();

	virtual char *GetConvertedOutput (const char * const input_filename_s, const int output_format_code) = 0;
};


class BLAST_SERVICE_LOCAL SystemBlastFormatter : public BlastFormatter
{
public:
	static SystemBlastFormatter *Create (const json_t *config_p);
	virtual ~SystemBlastFormatter ();

	virtual char *GetConvertedOutput (const char * const input_filename_s, const int output_format_code);

protected:
	const char *sbf_blast_formatter_command_s;

private:
	SystemBlastFormatter (const char * const blast_format_command_s);

};

#endif /* BLAST_FORMATTER_H_ */
