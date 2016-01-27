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
#include "streams.h"
#include "json_tools.h"
#include "string_utils.h"


bool AddPublishDateToJSON (json_t *json_p, const char * const key_s)
{
	bool success_flag = false;
	struct tm current_time;

	if (GetCurrentTime (&current_time))
		{
			#define BUFFER_SIZE (11)
			char buffer_s [BUFFER_SIZE];

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

			if (sprintf (buffer_s, "%4d-%02d-%02d", 1900 + current_time.tm_year, 1 + current_time.tm_mon, current_time.tm_mday) > 0)
				{
					* (buffer_s + (BUFFER_SIZE - 1)) = '\0';

					if (SetDateForSchemaOrg (json_p, key_s, buffer_s))
						{
							success_flag = true;
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create publish date for %s");
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get current time");
		}

	return success_flag;
}


bool SetDateForSchemaOrg (json_t *values_p, const char * const key_s, const char * const iso_date_s)
{
	bool success_flag = false;
	json_t *child_p = json_object ();

	if (child_p)
		{
			if ((json_object_set_new (child_p, "@type", json_string ("Date")) == 0) &&
					(json_object_set_new (child_p, "date", json_string (iso_date_s)) == 0))
				{
					if (json_object_set_new (values_p, key_s, child_p) == 0)
						{
							success_flag = true;
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to set schema org date context for %s = %s", key_s, iso_date_s);
						}

				}
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to add schema org date context for %s = %s", key_s, iso_date_s);
				}

			if (!success_flag)
				{
					WipeJSON (child_p);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to create json child for schema org date context for %s = %s", key_s, iso_date_s);
		}

	return success_flag;
}
