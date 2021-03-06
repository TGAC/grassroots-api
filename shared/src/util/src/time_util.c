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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "time_util.h"
#include "memory_allocations.h"

static bool ConvertNumber (const char * const buffer_s, size_t from, size_t to, int *result_p);

static bool ConvertStringToTime (const char * const time_s, time_t *time_p, bool (*conv_fn) (const char * const time_s, struct tm *time_p, int *offset_p));

/***************************************/


char *GetTimeAsString (const struct tm * const time_p)
{
	const size_t buffer_size = 11;
	char *buffer_s = (char *) AllocMemory (buffer_size * sizeof (char));

	if (buffer_s)
		{
			if (sprintf (buffer_s, "%4d-%02d-%02d", 1900 + (time_p -> tm_year), 1 + (time_p -> tm_mon), time_p -> tm_mday) > 0)
				{
					* (buffer_s + (buffer_size - 1)) = '\0';
					return buffer_s;
				}

			FreeMemory (buffer_s);
		}		/* if (buffer_s) */

	return NULL;
}


/*
 * Tue, 17 Jun 2014 14:26:52 +0000
 * "%a, %d %b %Y %H:%M:%S %z"
 */
bool ConvertDropboxStringToTime (const char * const time_s, struct tm *time_p, int *offset_p)
{
	bool success_flag = false;
	
	if (time_s)
		{
			char month [4];

			int res = sscanf (time_s, "%*s %d %3s %d %2d%*c%2d%*c%2d %d", 
				& (time_p -> tm_mday),
				month,
				& (time_p -> tm_year),				
				& (time_p -> tm_hour),				
				& (time_p -> tm_min),				
				& (time_p -> tm_sec),				
				offset_p);
			
			/* Did we match all seven arguments? */
			if (res == 7)
				{
					const char *months_ss [12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
					int j;
					
					for (j = 11; j >= 0; -- j)
						{
							if (strcmp (month, months_ss [j]) == 0)	
								{
									time_p -> tm_mon = j;
									
									success_flag = true;
									j = -1;
								}
						}
										
				}		/* if (res == 7) */			
			
		}		/* if (time_s) */

	return success_flag;
}


bool ConvertCompactStringToTime (const char * const time_s, struct tm *time_p, int * UNUSED_PARAM (offset_p))
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



bool ConvertCompactStringToEpochTime (const char * const time_s, time_t *time_p)
{
	return ConvertStringToTime (time_s, time_p, ConvertCompactStringToTime);
}


bool ConvertDropboxStringToEpochTime (const char * const time_s, time_t *time_p)
{
	return ConvertStringToTime (time_s, time_p, ConvertDropboxStringToTime);
}


bool GetCurrentTime (struct tm *tm_p)
{
	bool success_flag = false;
	time_t current_time = time (NULL);

	if (current_time != (time_t) -1)
		{
			success_flag =  (localtime_r (&current_time, tm_p) != NULL);
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



static bool ConvertStringToTime (const char * const time_s, time_t *time_p, bool (*conv_fn) (const char * const time_s, struct tm *time_p, int *offset_p))
{
	struct tm t;
	bool success_flag = false;
	int offset = 0;
	
	memset (&t, 0, sizeof (struct tm));

	if (conv_fn (time_s, &t, &offset))
		{			
			*time_p = mktime (&t);
		
			if (offset != 0)
				{
					double d = floor (offset * 0.010);
					int h = (int) d; 
					int m = offset - (d * 100);
					time_t offset_time = (3600 * h) + (60 * m);
					
					if (offset > 0)
						{
							time_p += offset_time; 
						}
					else
						{
							time_p -= offset_time; 							
						}
				}	
		}
		
	return success_flag;
}

