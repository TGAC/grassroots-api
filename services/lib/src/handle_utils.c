#include "handle_utils.h"

#include "parameter.h"
#include "file_handle.h"
#include "irods_handle.h"

Handle *GetResourceHandle (const Resource *resource_p, const TagItem *tags_p)
{
	Handle handle_p = NULL;
	
	if (resource_p)
		{
			switch (resource_p -> re_protocol)
				{
					case FILE_LOCATION_LOCAL:
						handle_p = AllocateFileHandle ();
						break;
						
					case FILE_LOCATION_REMOTE:
						handle_p = AllocateFileHandle ();
						break;
						
					case FILE_LOCATION_IRODS:
						{
							TagItem *ti_p = FindMatchingTag (tags_p, TAG_IRODS_CONNECTION);

							if (ti_p)
								{
									rcComm_t *connection_p = (rcComm_t *) (ti_p -> ti_value.st_generic_value_p);
				
									handle_p = AllocateIRodsHandle (connection_p);
								}
							else
								{
									TagItem *ti_p = FindMatchingTag (tags_p, TAG_USER);
									
									if (ti_p)
										{
											const char *username_s = ti_p -> ti_value.st_string_value_s;
											
											ti_p = FindMatchingTag (tags_p, TAG_PASSWORD);
											
											if (ti_p)
												{
													const char *password_s = ti_p -> ti_value.st_string_value_s;
													
													handle_p = GetIRodsHandle (username_s, password_s);
												}
										}
								}							
						}
						break;
						
					case FILE_LOCATION_UNKNOWN:
					default:
						break;
				}
		}
		
	return handle_p;
}
