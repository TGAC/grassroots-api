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
 * blastn_parameters.c
 *
 *  Created on: 18 Oct 2016
 *      Author: billy
 */


#include "blastn_app_parameters.hpp"
#include "blast_util.h"
#include "streams.h"


static NamedParameterType S_WORD_SIZE = { "word_size", PT_UNSIGNED_INT };
static NamedParameterType S_MATCH_SCORE = { "match_score", PT_SIGNED_INT };
static NamedParameterType S_MISMATCH_SCORE = { "mismatch_score", PT_SIGNED_INT };



BlastNAppParameters :: BlastNAppParameters ()
{

}


BlastNAppParameters :: ~BlastNAppParameters ()
{

}


bool BlastNAppParameters ::AddParameters (BlastServiceData *data_p, ParameterSet *param_set_p)
{
	bool success_flag = false;
	SharedType def;
	Parameter *param_p = NULL;
	ParameterGroup *group_p = NULL;
	ServiceData *service_data_p = & (data_p -> bsd_base_data);

	def.st_ulong_value = 28;

	if ((param_p = EasyCreateAndAddParameterToParameterSet (service_data_p, param_set_p, group_p, S_WORD_SIZE.npt_type, S_WORD_SIZE.npt_name_s, "Word size", "Expected number of chance matches in a random model", def, PL_ALL)) != NULL)
		{
			def.st_long_value = 2;

			if ((param_p = EasyCreateAndAddParameterToParameterSet (service_data_p, param_set_p, group_p, S_MATCH_SCORE.npt_type, S_MATCH_SCORE.npt_name_s, "Reward", "The reward for matching bases", def, PL_ALL)) != NULL)
				{
					def.st_long_value = 3;

					if ((param_p = EasyCreateAndAddParameterToParameterSet (service_data_p, param_set_p, group_p, S_MISMATCH_SCORE.npt_type, S_MISMATCH_SCORE.npt_name_s, "Penalty", "The penalty for mismatching bases", def, PL_ALL)) != NULL)
						{
							success_flag = true;
						}
				}
		}

	return success_flag;
}


bool BlastNAppParameters :: ParseParametersToByteBuffer (const BlastServiceData *data_p, ParameterSet *params_p, ByteBuffer *buffer_p)
{
	bool success_flag = false;

	/* reward */
	if (AddArgsPairFromIntegerParameter (params_p, S_MATCH_SCORE.npt_name_s, "-reward", buffer_p, false, false))
		{
			/* Penalty */
			if (AddArgsPairFromIntegerParameter (params_p, S_MISMATCH_SCORE.npt_name_s, "-penalty", buffer_p, false, false))
				{
					/* Word Size */
					if (AddArgsPairFromIntegerParameter (params_p, S_WORD_SIZE.npt_name_s, "-word_size", buffer_p, true, false))
						{
							success_flag = true;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_WORD_SIZE.npt_name_s);
						}

				}		/* if (AddArgsPairFromIntegerParameter (params_p, S_MISMATCH_SCORE.npt_name_s, "-penalty", buffer_p, false, false)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_MISMATCH_SCORE.npt_name_s);
				}

		}		/* if (AddArgsPairFromIntegerParameter (params_p, S_MATCH_SCORE.npt_name_s, "-reward", buffer_p, false, false)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\"", S_MATCH_SCORE.npt_name_s);
		}

	return success_flag;
}

