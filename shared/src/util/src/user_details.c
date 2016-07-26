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
#include "user_details.h"
#include "memory_allocations.h"
#include "string_utils.h"


UserDetails *AllocateUserDetails (const char *username_s)
{
	char *name_s = CopyToNewString (username_s, 0, false);
	
	if (name_s)
		{
			UserDetails *user_p = (UserDetails *) AllocMemory (sizeof (username_s));
			
			if (user_p)
				{
					user_p -> ud_username_s = name_s;
					
					return user_p;
				}

			FreeCopiedString (name_s);
		}
	
	return NULL;
}


void FreeUserDetails (UserDetails *user_details_p)
{
	FreeCopiedString (user_details_p -> ud_username_s);
	FreeMemory (user_details_p);
}
