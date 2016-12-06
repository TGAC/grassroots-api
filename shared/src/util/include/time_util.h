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
#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <time.h>

#include "typedefs.h"
#include "grassroots_util_library.h"


#ifdef __cplusplus
extern "C" 
{
#endif


/**
 * Convert a string that is of the form YYYYMMDD or YYYYMMDDhhmmss 
 * into a struct tm.
 * 
 * @param time_s The string to convert.
 * @param time_p The struct tm to store the converted time in.
 * @param offset_p Where the timezone adjustment will be stored. It's in the form (+/-)hhmm.
 * @return <code>true</code> if the value was converted successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool ConvertCompactStringToTime (const char * const time_s, struct tm *time_p, int *offset_p);


/**
 * Convert a string that is of the form YYYYMMDD or YYYYMMDDhhmmss 
 * into a time_t.
 * 
 * @param time_s The string to convert.
 * @param time_p The time_t to store the converted time in.
 * @return <code>true</code> if the value was converted successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool ConvertCompactStringToEpochTime (const char * const time_s, time_t *time_p);



/**
 * Convert a string that is of the form "Sat, 21 Aug 2010 22:31:20 +0000" ("%a, %d %b %Y %H:%M:%S %z")
 * into a struct tm.
 * 
 * @param time_s The string to convert.
 * @param time_p The struct tm to store the converted time in.
 * @param offset_p Where the timezone adjustment will be stored. It's in the form (+/-)hhmm.
 * @return <code>true</code> if the value was converted successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool ConvertDropboxStringToTime (const char * const time_s, struct tm *time_p, int *offset_p);


/**
 * Convert a string that is of the form "Sat, 21 Aug 2010 22:31:20 +0000" ("%a, %d %b %Y %H:%M:%S %z")
 * into a time_t.
 * 
 * @param time_s The string to convert.
 * @param time_p The time_t to store the converted time in.
 * @return <code>true</code> if the value was converted successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool ConvertDropboxStringToEpochTime (const char * const time_s, time_t *time_p);



/**
 * Get the current time
 *
 * @param tm_p Where the current time will be stored.
 * @return <code>true</code> if the time was set successfully, <code>false</code> otherwise.
 */
GRASSROOTS_UTIL_API bool GetCurrentTime (struct tm *tm_p);



/**
 * Get a time as a string in the form YYYY-MM-DD
 *
 * @param tm_p The time to get as a c-style string
 * @return The time as a c-style string or <code>NULL</code> upon error.
 */
GRASSROOTS_UTIL_API char *GetTimeAsString (const struct tm * const time_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef TIME_UTIL_H */
 
