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
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
#define MY_DBL_EPSILON 1.0e-08
#define MY_FLT_EPSILON 1.0e-05
*/

#define MY_DBL_EPSILON (DBL_EPSILON)
#define MY_FLT_EPSILON (FLT_EPSILON)

#include "math_utils.h"
#include "memory_allocations.h"
#include "string_utils.h"

static const double64 S_RANDOM_FACTOR = 1.0 / (RAND_MAX + 1.0);


static bool GetNumber (const char **str_pp, double64 *answer_p, bool fractions_flag, const char * const alternative_decimal_points_s);


int CompareDoubles (const double d1, const double d2)
{
	const double diff = d1 - d2;
	const double abs_diff = fabs (diff);

	/* check for when diff is nearly zero */
	if (abs_diff < MY_DBL_EPSILON)
		{
			return 0;
		}
	else
		{
			/* check using a relative difference */
			const double a1 = fabs (d1);
			const double a2 = fabs (d2);
			const double largest = (a1 > a2) ? a1 : a2;

			if (abs_diff <= largest * MY_DBL_EPSILON)
				{
					return 0;
				}
			else if (diff > 0)
				{
					return 1;
				}
			else
				{
					return -1;
				}
		}
}


int CompareFloats (const float f1, const float f2)
{
	const double diff = f1 - f2;
	const double abs_diff = fabs (diff);

	/* check for when diff is nearly zero */
	if (abs_diff < MY_FLT_EPSILON)
		{
			return 0;
		}
	else
		{
			/* check using a relative difference */
			const double a1 = fabs (f1);
			const double a2 = fabs (f2);
			const double largest = (a1 > a2) ? a1 : a2;

			if (abs_diff <= largest * MY_FLT_EPSILON)
				{
					return 0;
				}
			else if (diff > 0)
				{
					return 1;
				}
			else
				{
					return -1;
				}
		}
}


char *ConvertNumberToString (double d, int8 num_dps)
{
	char *value_s = NULL;
	size_t l = 1;	/* init size for terminating null */
	bool minus_flag = false;
	int num_digits = 0;
	
	if (CompareDoubles (d, 0.0) < 0)
		{
			d = -d;
			++ l;
			minus_flag = true;
		}
	
	
	if (CompareDoubles (d, 0.0) != 0)
		{
			num_digits = (int) log10 (d);
			l += (size_t) num_digits;
		}

	++ l;
	
	if (minus_flag)
		{
			d = -d;
		}
	
	
	if (num_dps > 0)
		{
			l += num_dps;
		}
	
	value_s = (char *) AllocMemory (l);
	
	if (value_s)
		{
			char *value_p = value_s;
			char *format_s = NULL;   
			int i = 0;

			num_digits = 7;		/* "%.%xlf\0" */

			if (num_dps > 0)
				{
					i = log10 (num_dps);
					num_digits += (i + 3);
				}
				
			format_s = (char *) AllocMemory (num_digits);
	
			if (format_s)
				{
					char *temp_p = format_s;
					
					*temp_p = '%';
					++ temp_p;
					
					if (i > 0)
						{
							sprintf (temp_p, ".%dlf", i);
						}
					else
						{
							sprintf (temp_p, "d");
						}
					
					if (minus_flag)
						{
							*value_p = '-';
							++ value_p;
						}

					if (i > 0)
						{
							sprintf (value_p, format_s, d);
						}
					else
						{
							sprintf (value_p, format_s, (int) d);
						}

					FreeMemory (format_s);
				}
			else
				{
					FreeCopiedString (value_s);
					value_s = NULL;
				}
		}

	
	return value_s;
}


bool GetValidRealNumber (const char **str_pp, double *answer_p, const char * const alternative_decimal_points_s)
{
	return GetNumber (str_pp, answer_p, true, alternative_decimal_points_s);
}


bool GetValidInteger (const char **str_pp, int *answer_p)
{
	double d;

	if (GetNumber (str_pp, &d, false, NULL))
		{
			*answer_p = (int) d;
			return true;
		}
	else
		{
			return false;
		}
}


bool GetValidLong (const char **str_pp, long *answer_p)
{
	double d;

	if (GetNumber (str_pp, &d, false, NULL))
		{
			*answer_p = (long) d;
			return true;
		}
	else
		{
			return false;
		}
}


static bool GetNumber (const char **str_pp, double64 *answer_p, bool fractions_flag, const char * const alternative_decimal_points_s)
{
	const char *str_p = *str_pp;
	double64 d = 0.0;
	bool is_negative = false;
	int decimal_point_index = -1;
	int i = 0;
	bool loop_flag = true;
	bool got_number_flag = false;

	/* skip past any whitespace */
	while (loop_flag)
		{
			if (*str_p == '\0')
				{
					return false;
				}
			else if (isspace (*str_p))
				{
					++ str_p;
				}
			else
				{
					loop_flag = false;
				}
		}
	

	/* check for a negative number */
	if (*str_p == '-')
		{
			is_negative = true;
			++ str_p;
		}

	if (str_p)
		{
			loop_flag = true;

			while (loop_flag)
				{
					char c = *str_p;

					if (isdigit (c))
						{
							d *= 10.0;
							d += (c - '0');

							if (!got_number_flag)
								{
									got_number_flag = true;
								}

							++ i;
						}
					else if (fractions_flag && (decimal_point_index == -1))
						{
							if (c == '.')
								{
									decimal_point_index = i;
								}
							else if (c != '\0')
								{
									if (alternative_decimal_points_s  && (strchr (alternative_decimal_points_s, c) != NULL))
										{
											decimal_point_index = i;
										}
								}

							if (decimal_point_index == -1)
								{
									loop_flag = false;
								}
						}
					else
						{
							loop_flag = false;
						}

					if (loop_flag)
						{
							++ str_p;
						}
				}
		}

	if (got_number_flag)
		{
			if (is_negative)
				{
					d = -d;
				}

			if (decimal_point_index != -1)
				{
					/* treat .xxxxx the same as 0.xxxxx */
					if (decimal_point_index == 0)
						{
							++ decimal_point_index;
						}

					d *= pow (10.0, (double) (- (i - decimal_point_index)));
				}

			*answer_p = d;
			*str_pp = str_p;
		}

	return got_number_flag;
}


int SortDoubles (const void *v1_p, const void *v2_p)
{
	const double64 *d1_p = (const double64 *) v1_p;
	const double64 *d2_p = (const double64 *) v2_p;

	return (CompareDoubles (*d1_p, *d2_p));
}



