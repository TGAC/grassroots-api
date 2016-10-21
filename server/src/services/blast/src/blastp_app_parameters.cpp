/*
** Copyright 2014-2016 The Earlham Institute
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
 * blastp_parameters.c
 *
 *  Created on: 18 Oct 2016
 *      Author: billy
 */

#include <string.h>

#include "blastp_app_parameters.hpp"
#include "blast_service.h"
#include "blast_util.h"


/* blastp options */


NamedParameterType BlastPAppParameters :: BPAP_MATRIX = { "matrix", PT_STRING };
NamedParameterType BlastPAppParameters :: BPAP_COMP_BASED_STATS = { "comp_based_stats", PT_UNSIGNED_INT };

const uint32 BlastPAppParameters :: BPAP_NUM_MATRICES = 8;

const char *BlastPAppParameters :: BPAP_MATRICES_SS [BPAP_NUM_MATRICES] =
{
"PAM30",
"PAM70",
"PAM250",
"BLOSUM80",
"BLOSUM62",
"BLOSUM45",
"BLOSUM50",
"BLOSUM90"
};


uint32 BlastPAppParameters :: BPAP_NUM_COMP_BASED_STATS = 4;


/*************************************/
/********* PUBLIC DEFINITIONS ***********/
/*************************************/

BlastPAppParameters :: BlastPAppParameters ()
{

}


BlastPAppParameters :: ~BlastPAppParameters ()
{

}


bool BlastPAppParameters :: ParseParametersToByteBuffer (const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p)
{
	bool success_flag = false;

	/* matrix */
	if (GetAndAddBlastArgsToByteBuffer (params_p, BPAP_MATRIX.npt_name_s, false, buffer_p))
		{
			/* Word Size */
			if (GetAndAddBlastArgsToByteBuffer (params_p, BPAP_COMP_BASED_STATS.npt_name_s, false, buffer_p))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", BPAP_COMP_BASED_STATS.npt_name_s);
				}


		}		/* if (BPAP_COMP_BASED_STATS (params_p, BPAP_MATRIX.npt_name_s, "-reward", buffer_p, false)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", BPAP_MATRIX.npt_name_s);
		}

	return success_flag;
}



/*************************************/
/******* PRIVATE DEFINITIONS *********/
/*************************************/



