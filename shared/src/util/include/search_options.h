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
 * @file
 * @brief
 */

/*
 * search_options.h
 *
 *  Created on: 24 Dec 2015
 *      Author: billy
 */

#ifndef SEARCH_OPTIONS_H_
#define SEARCH_OPTIONS_H_



/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_SEARCH_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_SEARCH_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * search_options.c.
 */
#ifdef ALLOCATE_SEARCH_TAGS
	#define SEARCH_PREFIX GRASSROOTS_UTIL_API
	#define VAL(x)	= x
#else
	#define SEARCH_PREFIX extern
	#define VAL(x)
#endif

SEARCH_PREFIX const char *SO_RANGE_S VAL("in");

SEARCH_PREFIX const char *SO_EQUALS_S VAL("=");

SEARCH_PREFIX const char *SO_LESS_THAN_S VAL("<");

SEARCH_PREFIX const char *SO_LESS_THAN_OR_EQUALS_S VAL("<=");

SEARCH_PREFIX const char *SO_GREATER_THAN_S VAL(">");

SEARCH_PREFIX const char *SO_GREATER_THAN_OR_EQUALS_S VAL(">=");

SEARCH_PREFIX const char *SO_NOT_EQUALS_S VAL("!=");

SEARCH_PREFIX const char *SO_LIKE_S VAL("like");

SEARCH_PREFIX const char *SO_CASE_INSENSITIVE_S VAL("insensitive");



#endif /* SEARCH_OPTIONS_H_ */
