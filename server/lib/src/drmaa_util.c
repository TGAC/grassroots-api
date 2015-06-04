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

	#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, "About to Init Drmaa");
	#endif

	res = drmaa_init (NULL, s_drmaa_diagnosis_s, sizeof (s_drmaa_diagnosis_s) - 1);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			success_flag = true;
		}

	#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, "Init Drmaa %d res %d %s", success_flag, res, s_drmaa_diagnosis_s);
	#endif

	return success_flag;
}


bool ExitDrmaa (void)
{
	bool res_flag = true;
	int res;

	#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, "About to Exit Drmaa");
	#endif

	res = drmaa_exit (s_drmaa_diagnosis_s, sizeof (s_drmaa_diagnosis_s) - 1);

	if (res == DRMAA_ERRNO_SUCCESS)
		{
			res_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "drmaa_exit() failed: %s\n", s_drmaa_diagnosis_s);
		}

	#if APR_JOBS_MANAGER_DEBUG >= STM_LEVEL_FINEST
	PrintLog (STM_LEVEL_FINEST, "Exit Drmaa %d res %d %s", res_flag, res, s_drmaa_diagnosis_s);
	#endif

	return res_flag;
}
