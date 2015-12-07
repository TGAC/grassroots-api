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
 * drmaa_util.c
 *
 *  Created on: 28 Apr 2015
 *      Author: tyrrells
 */

#include "typedefs.h"
#include "drmaa.h"
#include "streams.h"


#ifdef _DEBUG
	#define DRMAA_UTIL_DEBUG	(STM_LEVEL_FINEST)
#else
	#define DRMAA_UTIL_DEBUG	(STM_LEVEL_NONE)
#endif


static char s_drmaa_diagnosis_s [DRMAA_ERROR_STRING_BUFFER] = { 0 };


bool InitDrmaa (void)
{
	bool success_flag = false;
	int res;

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "About to Init Drmaa");
	#endif

	res = drmaa_init (NULL, s_drmaa_diagnosis_s, sizeof (s_drmaa_diagnosis_s) - 1);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			success_flag = true;
		}

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Init Drmaa %d res %d %s", success_flag, res, s_drmaa_diagnosis_s);
	#endif

	return success_flag;
}


bool ExitDrmaa (void)
{
	bool res_flag = true;
	int res;

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "About to Exit Drmaa");
	#endif

	res = drmaa_exit (s_drmaa_diagnosis_s, sizeof (s_drmaa_diagnosis_s) - 1);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			res_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "drmaa_exit() failed: %s\n", s_drmaa_diagnosis_s);
		}

	#if DRMAA_UTIL_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, __FILE__, __LINE__, "Exit Drmaa %d res %d %s", res_flag, res, s_drmaa_diagnosis_s);
	#endif

	return res_flag;
}
