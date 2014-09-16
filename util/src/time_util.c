#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "time_util.h"


static bool ConvertNumber (const char * const buffer_s, size_t from, size_t to, int *result_p);


bool ConvertStringToTime (const char * const time_s, struct tm *time_p)
{
	bool success_flag = false;
	
	if (time_s)
		{
			const size_t l = strlen (time_s);
			
			if (l >= 8)
				{
					int year;
				
					if (ConvertNumber (time_s, 0, 3, &year))
						{
							int month;

							if (ConvertNumber (time_s, 4, 5, &month))
								{
									int day;

									if (ConvertNumber (time_s, 6, 7, &day))
										{
											int hour = 0;
											int min = 0;
											int sec = 0;
											
											time_p -> tm_year = year - 1900;
											time_p -> tm_mon = month - 1;
											time_p -> tm_mday = day; 

											success_flag = true;
											
											if (l == 14)
												{
													if (ConvertNumber (time_s, 8, 9, &hour))
														{
															if (ConvertNumber (time_s, 10, 11, &min))
																{
																	if (ConvertNumber (time_s, 12, 13, &sec))
																		{
																				
																		}		/* if (ConvertNumber (time_s, 12, 13, &sec)) */
		
																}		/* if (ConvertNumber (time_s, 10, 11, &min)) */
															
														}		/* if (ConvertNumber (time_s, 8, 9, &hour)) */
													
												}		/* if (l == 14) */

											time_p -> tm_hour = hour;
											time_p -> tm_min = min;
											time_p -> tm_sec = sec;

										}		/* if (ConvertNumber (time_s, 0, 3, &year)) */
									
								}		/* if (ConvertNumber (time_s, 0, 3, &year)) */
	
						}		/* if (ConvertNumber (time_s, 0, 3, &year)) */
				
				}		/* if (l >= 8) */

		}		/* if (time_s) */

	return success_flag;
}


bool ConvertStringToEpochTime (const char * const time_s, time_t *time_p)
{
	struct tm t;
	bool success_flag = false;
	
	memset (&t, 0, sizeof (struct tm));

	if (ConvertStringToTime (time_s, &t))
		{
			*time_p = mktime (&t);
		}
		
	return success_flag;
}


/****************************************/

static bool ConvertNumber (const char * const buffer_s, size_t from, size_t to, int *result_p)
{
	bool success_flag = false;
	const char *digit_p = buffer_s + from;
	size_t i;
	int res = 0;
	
	/* check that it's a valid positive integer */
	for (i = from; i <= to; ++ i, ++ digit_p)
		{
			if (isdigit (*digit_p))
				{
					int j = *digit_p - '0';

					res *= 10;
					res += j;
				}
			else
				{
					success_flag = false;
					i = to + 1;		/* force exit from loop */
				}
		}
	
	if (success_flag)
		{
			*result_p = res;
		}
	
	return success_flag;
}
