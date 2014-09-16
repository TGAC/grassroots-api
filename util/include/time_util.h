#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <time.h>

#include "typedefs.h"
#include "wheatis_util_library.h"


#ifdef __cplusplus
extern "C" 
{
#endif


/**
 * Convert a string that is of the form YYYYMMDD or YYYYMMDDhhmmss 
 * into a struct tm.
 * 
 * @time_s The string to convert.
 * @time_p The struct tm to store the converted time in.
 * @return true if the value was converted successfully, false otherwise.
 */
WHEATIS_UTIL_API bool ConvertStringToTime (const char * const time_s, struct tm *time_p);


/**
 * Convert a string that is of the form YYYYMMDD or YYYYMMDDhhmmss 
 * into a time_t.
 * 
 * @time_s The string to convert.
 * @time_p The time_t to store the converted time in.
 * @return true if the value was converted successfully, false otherwise.
 */
WHEATIS_UTIL_API bool ConvertStringToEpochTime (const char * const time_s, time_t *time_p);


#ifdef __cplusplus
}
#endif


#endif		/* #ifndef TIME_UTIL_H */
 
