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
#include <string.h>
#include <stdio.h>

#include <dlfcn.h>

#include "typedefs.h"
#include "memory_allocations.h"
#include "plugin.h"
#include "string_utils.h"
#include "streams.h"

#define USING_VALGRIND (1)

#ifdef _DEBUG
	#define UNIX_PLUGIN_DEBUG	(STM_LEVEL_FINER)
#else
	#define UNIX_PLUGIN_DEBUG	(STM_LEVEL_NONE)
#endif


typedef struct UnixPlugin
{
	Plugin up_plugin;
	void *up_handle_p;
} UnixPlugin;



Plugin *AllocatePlugin (const char * const path_s)
{
	UnixPlugin *plugin_p = (UnixPlugin *) AllocMemory (sizeof (UnixPlugin));

	if (plugin_p)
		{
			InitBasePlugin ((Plugin *) plugin_p, path_s);
			plugin_p -> up_handle_p = NULL;
		}

	return ((Plugin *) plugin_p);
}



void ClosePlugin (Plugin * const plugin_p)
{
	UnixPlugin *unix_plugin_p = (UnixPlugin *) plugin_p;

	ClearBasePlugin (plugin_p);

	if (unix_plugin_p -> up_handle_p)
		{
			/*
			 * When using valgrind with dynamically loaded shared objects, if they are closed before
			 * the program exits then the function names get lost from the relevant stack traces and
			 * are printed as "???" instead. So compiling with USING_VALGRIND defined keeps them in memory
			 * so that the stack trace can be generated correctly. Do not use in production servers as
			 * it is a resource leak.
			 */
			#ifndef USING_VALGRIND
			dlclose (unix_plugin_p -> up_handle_p);
			#endif

			unix_plugin_p -> up_handle_p = NULL;
		}
}


void FreePlugin (Plugin * const plugin_p)
{
	#if PLUGIN_DEBUG >= STM_LEVEL_FINER
	PrintErrors (STM_LEVEL_FINER, __FILE__, __LINE__, "FreePlugin %s" , plugin_p -> pl_name_s);
	#endif

	ClosePlugin (plugin_p);
	FreeMemory ((UnixPlugin *) plugin_p);
}


bool OpenPlugin (Plugin * const plugin_p)
{
	bool success_flag = false;
	UnixPlugin *unix_plugin_p = (UnixPlugin *) plugin_p;

	unix_plugin_p -> up_handle_p = dlopen (plugin_p -> pl_path_s, RTLD_LAZY);

	if (unix_plugin_p -> up_handle_p != NULL)
		{
			success_flag = true;
		}
	else
		{
			char *error_s = dlerror ();
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Error opening \"%s\": \"%s\"\n", plugin_p -> pl_path_s, error_s);
		}

	return success_flag;
}


char *MakePluginName (const char * const name)
{
	const size_t l = strlen (name);

	/* We need an extra 7 characters for the "lib" and ".so\0" */
	char *c_p = (char *) AllocMemory ((l + 7) * sizeof (char));

	if (c_p)
		{
			char *ptr = c_p;
			strcpy (ptr, "lib");
			ptr += 3;

			strcpy (ptr, name);
			ptr += l;

			strcpy (ptr, ".so\0");
		}

	return c_p;
}


char *DeterminePluginName (const char * const full_plugin_path_s)
{
	char *name_s = NULL;
	const char * const prefix_s = "lib";
	const char * const suffix_s = ".so";
	
	const char *start_p = strrchr (full_plugin_path_s, '/');

	if (!start_p)
		{
			start_p = full_plugin_path_s;
		}

	start_p = strstr (start_p, prefix_s);
	
	if (start_p)
		{
			const char *end_p = NULL;
			start_p += (strlen (prefix_s));
			
			end_p = strstr (start_p, suffix_s);
			
			if (end_p)
				{
					size_t l = end_p - start_p;
					
					name_s = AllocMemory (l + 1);
					
					if (name_s)
						{
							strncpy (name_s, start_p, l);
							* (name_s + l) = '\0';
						}
				}							
		}

	return name_s;
}




void *GetSymbolFromPlugin (Plugin *plugin_p, const char * const symbol_s)
{
	void *symbol_p = NULL;
	UnixPlugin *unix_plugin_p = (UnixPlugin *) plugin_p;

	if (unix_plugin_p -> up_handle_p)
		{
			symbol_p = dlsym (unix_plugin_p -> up_handle_p, symbol_s);
		}
		
	return symbol_p;
}



