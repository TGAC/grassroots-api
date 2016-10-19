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

/* blastp options */

/*************************************/
/********* STATIC VARIABLES **********/
/*************************************/


static NamedParameterType S_MATRIX = { "matrix", PT_STRING };
static NamedParameterType S_COMP_BASED_STATS = { "comp_based_stats", PT_UNSIGNED_INT };

static const uint32 S_NUM_MATRICES = 8;

static const char *S_MATRICES_SS [S_NUM_MATRICES] =
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


static uint32 S_NUM_COMP_BASED_STATS = 4;



/*************************************/
/******* STATIC DECLARATIONS *********/
/*************************************/


static bool AddScoringParameters (BlastServiceData *data_p, ParameterSet *param_set_p);

static bool AddMatrixParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);

static bool AddCompositionalAdjustmentsParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p);


/*************************************/
/********* API DEFINITIONS ***********/
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
	if (AddArgsPairFromStringParameter (params_p, S_MATRIX.npt_name_s, "-matrix", buffer_p, false))
		{
			/* Word Size */
			if (AddArgsPairFromIntegerParameter (params_p, S_COMP_BASED_STATS.npt_name_s, "-comp_based_stats", buffer_p, true, false))
				{
					success_flag = true;
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_COMP_BASED_STATS.npt_name_s);
				}


		}		/* if (S_COMP_BASED_STATS (params_p, S_MATRIX.npt_name_s, "-reward", buffer_p, false)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_MATRIX.npt_name_s);
		}

	return success_flag;
}



bool BlastPAppParameters :: AddParameters (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;

	if (AddScoringParameters (data_p, param_set_p))
		{
			success_flag = true;
		}

	return success_flag;
}


/*************************************/
/******* STATIC DEFINITIONS *********/
/*************************************/

static bool AddScoringParameters (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	ParameterGroup *group_p = CreateAndAddParameterGroupToParameterSet ("Scoring Parameters", NULL, & (data_p -> bsd_base_data), param_set_p);

	if (AddMatrixParameter (data_p, param_set_p, group_p))
		{
			if (AddCompositionalAdjustmentsParameter (data_p, param_set_p, group_p))
				{
					success_flag = true;
				}		/* if (AddCompositionalAdjustmentsParameter (data_p, param_set_p, group_p)) */
			else
				{

				}
		}		/* if (AddMatrixParameter (data_p, param_set_p, group_p)) */
	else
		{

		}

	return success_flag;
}



static bool AddMatrixParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	SharedType matrix_values_p [S_NUM_MATRICES];
	uint32 i;
	ParameterMultiOptionArray *options_p = NULL;

	for (i = 0; i < S_NUM_MATRICES; ++ i)
		{
			(matrix_values_p + i) -> st_string_value_s = (char *) * (S_MATRICES_SS + i);
		}

	memset (&def, 0, sizeof (SharedType));

	options_p = AllocateParameterMultiOptionArray (S_NUM_MATRICES, NULL, matrix_values_p, PT_STRING, true);

	if (options_p)
		{
			/* set BLOSUM62 as default */
			def.st_string_value_s = (matrix_values_p + 4) -> st_string_value_s;

			if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, group_p, S_MATRIX.npt_type, false, S_MATRIX.npt_name_s, "Matrix", "The Scoring matrix to use", options_p, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					success_flag = true;
				}
			else
				{
					FreeParameterMultiOptionArray (options_p);
				}
		}

	return success_flag;
}



static bool AddCompositionalAdjustmentsParameter (BlastServiceData *data_p, ParameterSet *param_set_p, ParameterGroup *group_p)
{
	bool success_flag = false;
	Parameter *param_p = NULL;
	SharedType def;
	SharedType comp_values_p [S_NUM_COMP_BASED_STATS];
	uint32 i;
	ParameterMultiOptionArray *options_p = NULL;

	for (i = 0; i < S_NUM_COMP_BASED_STATS; ++ i)
		{
			(comp_values_p + i) -> st_ulong_value = i;
		}

	memset (&def, 0, sizeof (SharedType));

	options_p = AllocateParameterMultiOptionArray (S_NUM_COMP_BASED_STATS, NULL, comp_values_p, PT_UNSIGNED_INT, true);

	if (options_p)
		{
			const char *descriptions_ss [] =
				{
					"No composition-based statistics",
					"Composition-based statistics as in NAR 29:2994-3005, 2001",
					"Composition-based score adjustment as in Bioinformatics 21:902-911, 2005, conditioned on sequence properties",
					"Composition-based score adjustment as in Bioinformatics 21:902-911, 2005, unconditionally"
				};

			def.st_ulong_value = 2;

			if ((param_p = CreateAndAddParameterToParameterSet (& (data_p -> bsd_base_data), param_set_p, group_p, S_COMP_BASED_STATS.npt_type, false, S_COMP_BASED_STATS.npt_name_s, "Compositional adjustments", "Matrix adjustment method to compensate for amino acid composition of sequences.", options_p, def, NULL, NULL, PL_ALL, NULL)) != NULL)
				{
					success_flag = true;
				}
			else
				{
					FreeParameterMultiOptionArray (options_p);
				}
		}

	return success_flag;
}

