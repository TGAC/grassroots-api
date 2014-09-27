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
