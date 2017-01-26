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

/**
 * regular_expressions.c
 *
 *  Created on: 25 Jan 2017
 *      Author: billy
 *
 * @file
 * @brief
 */


#include "regular_expressions.h"
#include "streams.h"
#include "memory_allocations.h"


RegExp *AllocateRegExp (uint32 num_vectors)
{
	int *vectors_p = (int *) AllocMemory (num_vectors * sizeof (int));

	if (vectors_p)
		{
			RegExp *reg_exp_p = (RegExp *) AllocMemory (sizeof (RegExp));

			if (reg_exp_p)
				{
					reg_exp_p -> re_compiled_expression_p = NULL;
					reg_exp_p -> re_extra_p = NULL;
					reg_exp_p -> re_num_matches = -1;
					reg_exp_p -> re_substring_vectors_p = vectors_p;
					reg_exp_p -> re_num_vectors = num_vectors;
					reg_exp_p -> re_current_substring_index = -1;

					return reg_exp_p;
				}

			FreeMemory (vectors_p);
		}

	return NULL;
}


void FreeRegExp (RegExp *reg_ex_p)
{
	ClearRegExp (reg_ex_p);
	FreeMemory (reg_ex_p);
}


void ClearRegExp (RegExp *reg_ex_p)
{
	if (reg_ex_p -> re_extra_p)
		{
			#ifdef PCRE_CONFIG_JIT
			pcre_free_study (reg_ex_p -> re_extra_p);
			#else
			pcre_free (reg_ex_p -> re_extra_p);
			#endif

			reg_ex_p -> re_extra_p = NULL;
		}

	if (reg_ex_p -> re_compiled_expression_p)
		{
			pcre_free (reg_ex_p -> re_compiled_expression_p);
		}

	reg_ex_p -> re_num_matches = -1;

}


bool SetPattern (RegExp *reg_ex_p, const char *pattern_s, int options)
{
	bool success_flag = false;
	const char *error_s = NULL;
	int offset = -1;

	reg_ex_p -> re_compiled_expression_p = pcre_compile (pattern_s, options, &error_s, &offset, NULL);

	if (reg_ex_p -> re_compiled_expression_p)
		{
			reg_ex_p -> re_extra_p = pcre_study (reg_ex_p -> re_compiled_expression_p, 0, &error_s);

			if (! (reg_ex_p -> re_extra_p))
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to optimise regular expression for \"%s\"", pattern_s);
				}

			success_flag = true;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to compile regular expression for \"%s\"", pattern_s);
		}

	return success_flag;
}



bool MatchPattern (RegExp *reg_ex_p, const char *value_s)
{
	bool success_flag = false;

	if (reg_ex_p -> re_compiled_expression_p)
		{
			int res = pcre_exec (reg_ex_p -> re_compiled_expression_p, reg_ex_p -> re_extra_p, value_s, strlen (value_s), 0, 0, reg_ex_p -> re_substring_vectors_p, reg_ex_p -> re_num_vectors);

			if (res > 0)
				{
					reg_ex_p -> re_num_matches = res;
					success_flag = true;
				}
			else if (res == 0)
				{
					/*
					 * There are too many substrings to fit in vectors_p
					 * so use the maximum possible
					 */
					reg_ex_p -> re_num_matches =  (reg_ex_p -> re_num_vectors) / 3;
					success_flag = true;
				}
			else
				{
					switch (res)
						{
							case PCRE_ERROR_NOMATCH:
								PrintLog (STM_LEVEL_FINER, __FILE__, __LINE__, "No match for \"%s\"", value_s);
								break;

							case PCRE_ERROR_NULL:
								PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "An object was NULL");
								break;

							case PCRE_ERROR_NOMEMORY:
								PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Ran out of memory during regular expression matching");
								break;

							case PCRE_ERROR_BADOPTION:
								PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "An incorrect option was passed to regular expression matching");
								break;

							case PCRE_ERROR_BADMAGIC:
								PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "An bad magic error occurred during regular expression matching");
								break;

							case PCRE_ERROR_UNKNOWN_NODE:
								PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "An object was NULL");
								break;

							default:
								PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "An unknown error occurred during regular expression matching");
								break;
						}		/* switch (res) */

				}

		}		/* if (reg_ex_p -> re_compiled_expression_p) */

	return success_flag;
}


uint32 GetNumberOfMatches (const RegExp *reg_ex_p)
{
	return reg_ex_p -> re_num_matches;
}


char *GetNextMatch (RegExp *reg_ex_p)
{

  // char subStrMatchStr[1024];
  // int i, j
  // for(j=0,i=subStrVec[0];i<subStrVec[1];i++,j++)
  //   subStrMatchStr[j] = (*aLineToMatch)[i];
  // subStrMatchStr[subStrVec[1]-subStrVec[0]] = 0;
  //printf("MATCHED SUBSTRING: '%s'\n", subStrMatchStr);

}
