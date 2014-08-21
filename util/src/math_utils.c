#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

/*
#define MY_DBL_EPSILON 1.0e-08
#define MY_FLT_EPSILON 1.0e-05
*/

#define MY_DBL_EPSILON (DBL_EPSILON)
#define MY_FLT_EPSILON (FLT_EPSILON)

#include "math_utils.h"

static const double64 S_RANDOM_FACTOR = 1.0 / (RAND_MAX + 1.0);


static bool GetNumber (const char **str_pp, double64 *answer_p, bool fractions_flag);


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




bool GetValidRealNumber (const char **str_pp, double *answer_p)
{
	return GetNumber (str_pp, answer_p, true);
}


bool GetValidInteger (const char **str_pp, int *answer_p)
{
	double d;

	if (GetNumber (str_pp, &d, false))
		{
			*answer_p = (int) d;
			return true;
		}
	else
		{
			return false;
		}
}


static bool GetNumber (const char **str_pp, double64 *answer_p, bool fractions_flag)
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
					else if (fractions_flag && (c == '.'))
						{
							decimal_point_index = i;
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



