#include <string.h>
#include <stdio.h>

#include <dlfcn.h>

#include "typedefs.h"
#include "memory_allocations.h"
#include "plugin.h"
#include "string_utils.h"



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
			dlclose (unix_plugin_p -> up_handle_p);
			unix_plugin_p -> up_handle_p = NULL;
		}
}


void FreePlugin (Plugin * const plugin_p)
{
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
			fprintf (stderr, "Error opening \"%s\": \"%s\"\n", plugin_p -> pl_path_s, error_s);
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


