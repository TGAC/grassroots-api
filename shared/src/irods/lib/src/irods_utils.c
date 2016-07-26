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

#include <stdio.h>

#include "irods_utils.h"
#include "grassroots_config.h"

#include "typedefs.h"


bool GetRodsEnv (rodsEnv *rods_env_p)
{
	bool success_flag = true;

	return success_flag;
}


int PrintRodsPath (FILE *out_f, const rodsPath_t * const rods_path_p, const char * const description_s)
{
	return fprintf (out_f, "%s: in \"%s\" out \"%s\"\n", description_s, rods_path_p -> inPath, rods_path_p -> outPath);
}


bool NotifyUser (userInfo_t *user_p, const char * const title_s, msParamArray_t *params_p)
{
	bool success_flag = true;
	/*
	 * depending on the user's preferences, notify them by e.g. email, logfile, etc.
	 */

	 return success_flag;
}
