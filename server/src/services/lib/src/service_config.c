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
#include "service_config.h"

#include "string_utils.h"


static char S_DEFAULT_ROOT_PATH_S [] = "";
static char *s_root_path_s = S_DEFAULT_ROOT_PATH_S;

static SchemaVersion s_schema_version;



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




void SetSchemaVersionDetails (const uint32 major, const uint32 minor)
{
	s_schema_version.sv_major = major;
	s_schema_version.sv_minor = minor;
}


const SchemaVersion *GetSchemaVersion (void)
{
	return &s_schema_version;
}
