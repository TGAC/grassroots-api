/*
** Copyright 2014-2015 The Genome Analysis Centre
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
/*
 * pathogenomics_utils.c
 *
 *  Created on: 2 Dec 2015
 *      Author: tyrrells
 */

#include "pathogenomics_utils.h"
#include "time_util.h"



bool AddPublishDateToJSON (json_t *json_p, const char * const key_s)
{
	bool success_flag = false;
	struct tm current_time;

	if (GetCurrentTime (&current_time))
		{
			/* Set the "go live" date to be 1 month from now */
			if (current_time.tm_mon == 11)
				{
					++ current_time.tm_year;
					current_time.tm_mon = 0;
				}
			else
				{
					++ current_time.tm_mon;
				}

			#define BUFFER_SIZE (11)
			char buffer_s [BUFFER_SIZE];

			if (sprintf (buffer_s, "%4d-%2d-%2d", 1900 + current_time.tm_year, 1 + current_time.tm_mon, current_time.tm_mday) > 0)
				{
					* (buffer_s + (BUFFER_SIZE - 1)) = '\0';

					if (json_object_set_new (json_p, key_s, json_string (buffer_s)) == 0)
						{
							success_flag = true;
						}
				}
		}

	return success_flag;
}
