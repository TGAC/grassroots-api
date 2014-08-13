#include <string.h>

#include <dlfcn.h>

#include "typedefs.h"
#include "memory_allocations.h"
#include "plugin.h"
#include "service.h"
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

	success_flag = (unix_plugin_p -> up_handle_p != NULL);
	
	return success_flag;
}


bool DeallocatePluginService (Plugin * const plugin_p)
{
	bool success_flag = (plugin_p -> pl_service_p == NULL);

	if (!success_flag)
		{
			const char *symbol_name_s = "DeallocateService";
			UnixPlugin *unix_plugin_p = (UnixPlugin *) plugin_p;

			if (unix_plugin_p -> up_handle_p)
				{
					void (*fn_p) (Service * const) = (void (*) (Service * const)) (dlsym (unix_plugin_p -> up_handle_p, symbol_name_s));

					if (fn_p)
						{
							fn_p (plugin_p -> pl_service_p);

							plugin_p -> pl_service_p = NULL;
							success_flag = true;
						}
				}
		}

	return success_flag;
}


//
//	Get Symbol
//
Service *GetServiceFromPlugin (Plugin * const plugin_p)
{
	if (!plugin_p -> pl_service_p)
		{
			const char *symbol_name_s = "AllocateService";
			UnixPlugin *unix_plugin_p = (UnixPlugin *) plugin_p;

			if (unix_plugin_p -> up_handle_p)
				{
					Service *(*fn_p) (void) = (Service *(*) (void)) (dlsym (unix_plugin_p -> up_handle_p, symbol_name_s));

					if (fn_p)
						{
							plugin_p -> pl_service_p = fn_p ();
							
							if (plugin_p -> pl_service_p)
								{
									plugin_p -> pl_service_p -> se_plugin_p = plugin_p;
								}
						}
				}
		}

	return plugin_p -> pl_service_p;
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



