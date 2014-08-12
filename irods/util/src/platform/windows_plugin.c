
#include <windows.h>

#include "filesystem.h"
#include "hash_table.h"
#include "memory_allocations.h"
#include "plugin.h"
#include "plugin_manager.h"
#include "string_utils.h"


typedef struct WindowsPlugin
{
	Plugin wp_plugin;
	HINSTANCE wp_handle;
} WindowsPlugin;



Plugin *CopyPlugin (const Plugin * const src_p)
{
	Plugin *dest_plugin_p = AllocatePlugin (src_p -> pl_name_s, src_p -> pl_path_s);

	if (dest_plugin_p)
		{
			CopyBasePlugin (src_p, dest_plugin_p);

			((WindowsPlugin *) dest_plugin_p) -> wp_handle = ((WindowsPlugin *) src_p) -> wp_handle;
		}

	return dest_plugin_p;
}



Plugin *AllocatePlugin (const char * const name_s, const char * const path_s)
{
	WindowsPlugin *plugin_p = (WindowsPlugin *) AllocMemory (sizeof (WindowsPlugin));

	if (plugin_p)
		{
			InitBasePlugin ((Plugin *) plugin_p, name_s, path_s);
			plugin_p -> wp_handle = NULL;
		}

	return ((Plugin *) plugin_p);
}


void ClosePlugin (Plugin * const plugin_p)
{
	WindowsPlugin *windows_plugin_p = (WindowsPlugin *) plugin_p;

//	RemovePluginFromPluginManager (plugin_p);
	ClearBasePlugin (plugin_p);

	if (windows_plugin_p -> wp_handle)
		{
			FreeLibrary (windows_plugin_p -> wp_handle);
			windows_plugin_p -> wp_handle = NULL;
		}
}


void FreePlugin (Plugin * const plugin_p)
{
	ClosePlugin (plugin_p);
	FreeMemory ((WindowsPlugin *) plugin_p);
}



BOOLEAN IsPluginOpen (const Plugin * const plugin_p)
{
	if ((plugin_p -> pl_path_s) || (plugin_p -> pl_module_p))
		{
			return TRUE;
		}
	else
		{
			return FALSE;
		}
}

BOOLEAN OpenPlugin (Plugin * const plugin_p, char * const lib_path)
{
	WindowsPlugin *windows_plugin_p = (WindowsPlugin *) plugin_p;
	BOOLEAN success_flag = FALSE;
	
	if (lib_path)
		{
			if (plugin_p -> pl_path_s)
				{
					ClearPluginPath (plugin_p);
				}

			plugin_p -> pl_path_s = CopyToNewString (lib_path, 0, FALSE);

			if (plugin_p -> pl_path_s)
				{
					plugin_p -> pl_path_mem = MF_DEEP_COPY;
					success_flag = TRUE;
				}					
		}
		
	if (success_flag)
		{	
			windows_plugin_p -> wp_handle = LoadLibrary (lib_path);

			success_flag = (windows_plugin_p -> wp_handle != NULL);
			
			if (success_flag)
				{
					AddPluginToPluginManager (plugin_p);
				}
		}

	return success_flag;
}


BOOLEAN DeallocateModule (Plugin * const plugin_p)
{
	BOOLEAN success = TRUE;

	if (plugin_p -> pl_module_p)
		{
			char *symbol_name = NULL;
			success = FALSE;

			switch (plugin_p -> pl_type)
				{
					case PT_LOADER:
						symbol_name = "DeallocateLoader";
						break;

					case PT_ALIGNER:
						symbol_name = "DeallocateAligner";
						break;

					case PT_MODELLER:
						symbol_name = "DeallocateModeller";
						break;

					case PT_SAVER:
						symbol_name = "DeallocateSaver";
						break;

					case PT_PREPROCESSOR:
						symbol_name = "DeallocatePreprocessor";
						break;

					case PT_CLASSIFIER:
						symbol_name = "DeallocateClassifier";
						break;

					case PT_STATS_SAVER:
						symbol_name = "DeallocateStatisticsSaver";
						break;

					case PT_STREAMER:
						symbol_name = "DeallocateStreamer";
						break;

					case PT_SPECTRA_CALCULATOR:
						symbol_name = "DeallocateSpectraCalculator";
						break;

					case PT_DATA_IO:
						symbol_name = "DeallocateDataIO";
						break;

					case PT_UNKNOWN:
					default:

						break;
				}

			if (symbol_name)
				{
					WindowsPlugin *windows_plugin_p = (WindowsPlugin *) plugin_p;

					if (windows_plugin_p -> wp_handle)
						{
							void (*fn_p) (Module * const) = (void (*) (Module * const)) (GetProcAddress (windows_plugin_p -> wp_handle, symbol_name));

							if (fn_p)
								{
									fn_p (plugin_p -> pl_module_p);

									plugin_p -> pl_module_p = NULL;
									success = TRUE;
								}
						}
				}
		}

	return success;
}


//
//	Get Symbol
//
Module *GetModuleFromPlugin (Plugin * const plugin_p, const ConfigurationManager * const config_p)
{
	if (!plugin_p -> pl_module_p)
		{
			char *symbol_name = NULL;

			switch (plugin_p -> pl_type)
				{
					case PT_LOADER:
						symbol_name = "AllocateLoader";
						break;

					case PT_ALIGNER:
						symbol_name = "AllocateAligner";
						break;

					case PT_MODELLER:
						symbol_name = "AllocateModeller";
						break;

					case PT_SAVER:
						symbol_name = "AllocateSaver";
						break;

					case PT_PREPROCESSOR:
						symbol_name = "AllocatePreprocessor";
						break;

					case PT_CLASSIFIER:
						symbol_name = "AllocateClassifier";
						break;

					case PT_STATS_SAVER:
						symbol_name = "AllocateStatisticsSaver";
						break;

					case PT_STREAMER:
						symbol_name = "AllocateStreamer";
						break;

					case PT_SPECTRA_CALCULATOR:
						symbol_name = "AllocateSpectraCalculator";
						break;

					case PT_DATA_IO:
						symbol_name = "AllocateDataIO";
						break;

					case PT_UNKNOWN:
					default:
						break;
				}

			if (symbol_name)
				{
					WindowsPlugin *windows_plugin_p = (WindowsPlugin *) plugin_p;

					if (windows_plugin_p -> wp_handle)
						{
							Module *(*fn_p) (const ConfigurationManager * const) = (Module *(*) (const ConfigurationManager * const)) (GetProcAddress (windows_plugin_p -> wp_handle, symbol_name));

							if (fn_p)
								{
									Module *module_p = fn_p (config_p);

									if (module_p)
										{
											plugin_p -> pl_module_p = module_p;
											module_p -> mo_plugin_p = plugin_p;
										}
								}
						}
				}
		}

	return plugin_p -> pl_module_p;
}


char *MakePluginName (const char * const name)
{
	const size_t l = strlen (name);
	char *c_p = (char *) AllocMemory ((l + 5) * sizeof (char));

	if (c_p)
		{
			strcpy (c_p, name);
			strcpy (c_p + l, ".dll\0");
		}

	return c_p;
}


void PrintPlugin (const Plugin * const plugin_p, OutputStream * const stream_p)
{
	WindowsPlugin *windows_plugin_p = (WindowsPlugin *) plugin_p;

	PrintToOutputStream (stream_p, "windows plugin: handle %x\n", windows_plugin_p -> wp_handle);
	PrintBasePlugin (plugin_p, stream_p);
}

