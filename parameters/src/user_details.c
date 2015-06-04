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
