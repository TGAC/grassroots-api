/**@file math_utils.h
*/ 

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "wheatis_util_library.h"
#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Compate two doubles taking the epsilon
 * of this computer into account.
 *
 * @param d1 The first double to compare.
 * @param d2 The second double to compare.
 * @return -1 if d1 is less than d2, +1 if d1 is
 * greater than d2 and 0 if they are identical.
 */
WHEATIS_UTIL_API int CompareDoubles (const double d1, const double d2);


/**
 * Compate two floats taking the epsilon
 * of this computer into account.
 *
 * @param f1 The first float to compare.
 * @param f2 The second float to compare.
 * @return -1 if f1 is less than f2, +1 if f1 is
 * greater than f2 and 0 if they are identical.
 */
WHEATIS_UTIL_API int CompareFloats (const float f1, const float f2);


/**
 * Since atof, atoi, etc. return 0 on error we need to do our own
 * "number getting" routine that notifies us of any errors.
 *
 * @param str_pp Pointer to char * where we start getting the number from.
 * @param data_p Pointer where the number will be stored if the converesion was successful.
 * @return TRUE if the conversion was successful, FALSE otherwise.
 */
WHEATIS_UTIL_API bool GetValidRealNumber (const char **str_pp, double64 *data_p);


/**
 * Since atof, atoi, etc. return 0 on error we need to do our own
 * "number getting" routine that notifies us of any errors.
 *
 * @param str_pp Pointer to char * where we start getting the number from.
 * @param data_p Pointer where the number will be stored if the converesion was successful.
 * @return TRUE if the conversion was successful, FALSE otherwise.
 */
WHEATIS_UTIL_API bool GetValidInteger (const char **str_pp, int *answer_p);


WHEATIS_UTIL_API int SortDoubles (const void *v1_p, const void *v2_p);



#ifdef __cplusplus
}
#endif


#endif	/* #ifndef MATH_UTILS_H */
