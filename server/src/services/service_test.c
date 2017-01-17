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

#include "services_manager.h"
#include "service.h"

int main (int argc, char *argv [])
{
	int result = 0;
	const char * const path_s = "services";
	const char * const pattern_s = NULL;

	LinkedList *services_p = LoadMatchingServices (path_s, pattern_s);
	
	if (services_p)
		{
			json_t *services_json_p = GetServicesListAsJSON (services_p);
			
			if (services_json_p)
				{
					char *json_s = json_dumps (services_json_p, JSON_INDENT (2));
					
					if (json_s)
						{
							printf ("%s\n", json_s);
							json_free_t (json_s);
						}
					
				}		/* if (services_json_p) */
			 
			FreeLinkedList (services_p);
		}		/* if (services_p) */

	return result;
}

