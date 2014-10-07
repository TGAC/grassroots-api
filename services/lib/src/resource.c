#include "resource.h"
#include "string_utils.h"
#include "memory_allocations.h"



static bool ReplaceValue (const char * const src_s, char **dest_ss);



Resource *AllocateResource (void)
{
	Resource *resource_p = (Resource *) AllocMemory (sizeof (Resource));
	
	if (resource_p)
		{
			InitResource (resource_p);
		}
	
	return resource_p;
}


void InitResource (Resource *resource_p)
{
	resource_p -> re_protocol_s = NULL;
	resource_p -> re_value_s = NULL;
}


void FreeResource (Resource *resource_p)
{
	ClearResource (resource_p);
	FreeMemory (resource_p);
}


void ClearResource (Resource *resource_p)
{
	if (resource_p -> re_protocol_s)
		{
			FreeCopiedString (resource_p -> re_protocol_s);
			resource_p -> re_protocol_s = NULL;
		}

	if (resource_p -> re_value_s)
		{
			FreeCopiedString (resource_p -> re_value_s);
			resource_p -> re_value_s = NULL;
		}

}


bool SetResourceValue (Resource *resource_p, const char *protocol_s, const char *value_s)
{
	bool success_flag = false;
	char *new_value_s = NULL;
	char *new_protocol_s = NULL;
	
	if (ReplaceValue (protocol_s, &new_protocol_s))
		{
			if (ReplaceValue (value_s, &new_value_s))
				{
					resource_p -> re_protocol_s = new_protocol_s;
					resource_p -> re_value_s = new_value_s;

					success_flag = true;
				}
		}
			
	
	return success_flag;
}


bool CopyResource (const Resource * const src_p, Resource * const dest_p)
{
	bool success_flag = SetResourceValue (dest_p, src_p -> re_protocol_s, src_p -> re_value_s);
	
	return success_flag;
}



static bool ReplaceValue (const char * const src_s, char **dest_ss)
{
	bool success_flag = false;
		
	if (src_s)
		{
			char *new_value_s = CopyToNewString (src_s, 0, false);
			
			if (new_value_s)
				{
					if (*dest_ss)
						{
							FreeCopiedString (*dest_ss);
						}
						
					*dest_ss = new_value_s;
					success_flag = true;
				}
		}
	else
		{
			if (*dest_ss)
				{
					FreeCopiedString (*dest_ss);
					*dest_ss = NULL;
				}
				
			success_flag = true;
		}
		
	return success_flag;
}
