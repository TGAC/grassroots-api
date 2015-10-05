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
#include "tag_item.h"


TagItem *FindMatchingTag (TagItem *tags_p, Tag tag)
{
	TagItem *ti_p = tags_p;
	
	if (ti_p)
		{
			bool loop_flag = true;
			
			while (loop_flag)
				{
					if (ti_p -> ti_tag == tag)
						{
							loop_flag = false;
						}
					else if (ti_p -> ti_tag == TAG_DONE)
						{
							loop_flag = false;
							ti_p = NULL;
						}
					else
						{
							++ ti_p;
						}
				}
		}

	return ti_p;
}
