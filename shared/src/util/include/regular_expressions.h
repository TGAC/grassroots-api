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
 * regular_expressions.h
 *
 *  Created on: 25 Jan 2017
 *      Author: billy
 *
 * @file
 * @brief
 */

#ifndef SHARED_SRC_UTIL_INCLUDE_REGULAR_EXPRESSIONS_H_
#define SHARED_SRC_UTIL_INCLUDE_REGULAR_EXPRESSIONS_H_

#include "pcre.h"


#include "grassroots_util_library.h"
#include "typedefs.h"


const uint32 DEFAULT_NUM_REGEXP_VECTORS = 32;

typedef struct RegExp
{
  pcre *re_compiled_expression_p;
  pcre_extra *re_extra_p;
  int re_num_matches;
  int *re_substring_vectors_p;
  uint32 re_num_vectors;
  int16 re_current_substring_index;
} RegExp;


#ifdef __cplusplus
extern "C"
{
#endif




GRASSROOTS_UTIL_API RegExp *AllocateRegExp (uint32 num_vectors);


GRASSROOTS_UTIL_API void FreeRegExp (RegExp *reg_ex_p);


GRASSROOTS_UTIL_API void ClearRegExp (RegExp *reg_ex_p);


GRASSROOTS_UTIL_API bool SetPattern (RegExp *reg_ex_p, const char *pattern_s, int options);


GRASSROOTS_UTIL_API bool MatchPattern (RegExp *reg_ex_p);



GRASSROOTS_UTIL_API uint32 GetNumberOfMatches (const RegExp *reg_ex_p);


GRASSROOTS_UTIL_API char *GetNextMatch (RegExp *reg_ex_p);


#ifdef __cplusplus
}
#endif


#endif /* SHARED_SRC_UTIL_INCLUDE_REGULAR_EXPRESSIONS_H_ */
