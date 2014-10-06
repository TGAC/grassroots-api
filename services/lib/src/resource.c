#include "resource.h"
#include "string_utils.h"
#include "memory_allocations.h"


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
	resource_p -> re_protocol = FILE_LOCATION_UNKNOWN;
	resource_p -> re_value_s = NULL;
}


void FreeResource (Resource *resource_p)
{
	ClearResource (resource_p);		
	FreeMemory (resource_p);
}


void ClearResource (Resource *resource_p)
{
	if (resource_p -> re_value_s)
		{
			FreeCopiedString (resource_p -> re_value_s);
			resource_p -> re_value_s = NULL;
		}
		
	resource_p -> re_protocol = FILE_LOCATION_UNKNOWN;
}


bool SetResourceValue (Resource *resource_p, const FileLocation fl, const char *value_s)
{
	bool success_flag = false;
	char *new_value_s = CopyToNewString (value_s, 0, false);
	
	if (new_value_s)
		{
			ClearResource (resource_p);
			
			resource_p -> re_protocol = fl;
			resource_p -> re_value_s = new_value_s;
			
			success_flag = true;
		}
	
	return success_flag;
}


bool CopyResource (const Resource * const src_p, Resource * const dest_p)
{
	bool success_flag = false;
	
	if (dest_p -> re_value_s)
		{
			success_flag = SetResourceValue (dest_p, src_p -> re_protocol, src_p -> re_value_s);
		}
	else
		{
			ClearResource (dest_p);
			dest_p -> re_protocol = src_p -> re_protocol;
		}
	
	
	return success_flag;
}

