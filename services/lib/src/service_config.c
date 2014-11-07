#include "service_config.h"

#include "string_utils.h"


static char S_DEFAULT_ROOT_PATH_S [] = "";
static char *s_root_path_s = S_DEFAULT_ROOT_PATH_S;



void FreeServerResources (void)
{
	if (s_root_path_s != S_DEFAULT_ROOT_PATH_S)
		{
			FreeCopiedString (s_root_path_s);
		}
}


bool SetServerRootDirectory (const char * const path_s)
{
	bool success_flag = false;
	
	if (path_s)
		{
			char *copied_path_s = CopyToNewString (path_s, 0, false);
			
			if (copied_path_s)
				{
					if (s_root_path_s != S_DEFAULT_ROOT_PATH_S)
						{
							FreeCopiedString (s_root_path_s);
						}
						
					s_root_path_s = copied_path_s;
					success_flag = true;
				}
		}
	else
		{
			if (s_root_path_s != S_DEFAULT_ROOT_PATH_S)
				{
					FreeCopiedString (s_root_path_s);
				}
				
			s_root_path_s = S_DEFAULT_ROOT_PATH_S;
		}
	
	return success_flag;
}



const char *GetServerRootDirectory (void)
{
	return s_root_path_s;
}
