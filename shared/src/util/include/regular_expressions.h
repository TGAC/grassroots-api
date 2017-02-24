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

/**
 * The module used for performing regular expressions.
 *
 * @ingroup utility_group
 */
typedef struct RegExp
{
  pcre *re_compiled_expression_p;
  pcre_extra *re_extra_p;
  const char *re_target_s;
  uint32 re_num_matches;
  int *re_substring_vectors_p;
  uint32 re_num_vectors;
  uint32 re_current_substring_index;
} RegExp;


#ifdef __cplusplus
extern "C"
{
#endif



/**
 * Allocate a RegExp.
 *
 * @param num_vectors The maximum number of start and end indices of matching
 * groups that the RegExp will be able to capture.
 * @return The newly-allocated RegExp or <code>NULL</code> upon error.
 * @memberof RegExp
 */
GRASSROOTS_UTIL_API RegExp *AllocateRegExp (uint32 num_vectors);


/**
 * Free a RegExp.
 *
 * @param reg_ex_p The RegExp to free.
 * @memberof RegExp
 */
GRASSROOTS_UTIL_API void FreeRegExp (RegExp *reg_ex_p);


/**
 * Clear a RegExp ready to re-use with a different pattern
 *
 * @param reg_ex_p The RegExp to clear.
 * @memberof RegExp
 */
GRASSROOTS_UTIL_API void ClearRegExp (RegExp *reg_ex_p);


/**
 * Set the pattern that a given RegExp will use for its regular expression.
 *
 * @param reg_ex_p The RegExp to set the pattern for.
 * @param pattern_s The pattern to set.
 * @param options These are the options that can be used by the underlying pcre engine. See
 * http://www.pcre.org/original/doc/html/pcre_compile.html for more information.
 * @return <code>true</code> if the pattern was set successfully, <code>false</code> otherwise.
 * @memberof RegExp
 */
GRASSROOTS_UTIL_API bool SetPattern (RegExp *reg_ex_p, const char *pattern_s, int options);


/**
 * Test whether a regular expression has any matches for a given value.
 *
 * @param reg_ex_p The RegExp to use.
 * @param value_s The value to test for matches.
 * @return <code>true</code> if there were any matches, <code>false</code> otherwise.
 * @memberof RegExp
 */
GRASSROOTS_UTIL_API bool MatchPattern (RegExp *reg_ex_p, const char *value_s);


/**
 * Get the number of matchers after running MatchPattern().

 * @param reg_ex_p The RegExp to check.
 * @return The number of matches.
 * @memberof RegExp
 */
GRASSROOTS_UTIL_API uint32 GetNumberOfMatches (const RegExp *reg_ex_p);


/**
 * Get the next match from a regular expression.
 *
 * @param reg_ex_p The RegExp to get the match from.
 * @return The next match or <code>NULL</code> if there are
 * no more. If a non-<code>NULL</code> value is returned,
 * it will need to be freed with FreeCopiedString() to avoid
 * a memory leak.
 * @memberof RegExp
 */
GRASSROOTS_UTIL_API char *GetNextMatch (RegExp *reg_ex_p);


#ifdef __cplusplus
}
#endif


#endif /* SHARED_SRC_UTIL_INCLUDE_REGULAR_EXPRESSIONS_H_ */
