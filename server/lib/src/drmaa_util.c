/*
 * drmaa_util.c
 *
 *  Created on: 28 Apr 2015
 *      Author: tyrrells
 */

#include "typedefs.h"
#include "drmaa.h"
#include "streams.h"

char s_drmaa_diagnosis_s [DRMAA_ERROR_STRING_BUFFER];


bool InitDrmaa (void)
{
	bool success_flag = false;

	if (drmaa_init (NULL, s_drmaa_diagnosis_s, sizeof (s_drmaa_diagnosis_s) - 1) == DRMAA_ERRNO_SUCCESS)
		{
			success_flag = true;
		}

	return success_flag;
}


bool ExitDrmaa (void)
{
	bool res_flag = true;

	if (drmaa_exit (s_drmaa_diagnosis_s, sizeof (s_drmaa_diagnosis_s) -1) == DRMAA_ERRNO_SUCCESS)
		{
			res_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, "drmaa_exit() failed: %s\n", s_drmaa_diagnosis_s);
		}

	return res_flag;
}
