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

#include <dos/dos.h>

#include <libraries/elf.h>

#include <proto/elf.h>
#include <proto/dos.h>

#include "filesystem.h"
#include "hash_table.h"
#include "memory_allocations.h"
#include "plugin.h"
#include "plugin_manager.h"
#include "string_utils.h"


typedef struct AmigaPlugin
{
	Plugin ap_plugin;
	Elf32_Handle ap_handle;
	BPTR ap_seglist_p;
} AmigaPlugin;


static APTR GetSymbol (AmigaPlugin *plugin_p, char *symbol_name_s);


Plugin *CopyPlugin (const Plugin * const src_p)
{
	Plugin *dest_plugin_p = AllocatePlugin (src_p -> pl_name_p);

	if (dest_plugin_p)
		{
			CopyBasePlugin (src_p, dest_plugin_p);

			((AmigaPlugin *) dest_plugin_p) -> ap_handle = ((AmigaPlugin *) src_p) -> ap_handle;
			((AmigaPlugin *) dest_plugin_p) -> ap_seglist_p = ((AmigaPlugin *) src_p) -> ap_seglist_p;
		}

	return dest_plugin_p;
}



Plugin *AllocatePlugin (const char * const name_p)
{
	AmigaPlugin *plugin_p = (AmigaPlugin *) AllocMemory (sizeof (AmigaPlugin));

	if (plugin_p)
		{
			InitBasePlugin ((Plugin *) plugin_p, name_p);
			plugin_p -> ap_handle = NULL;
			plugin_p -> ap_seglist_p = ZERO;
		}

	return ((Plugin *) plugin_p);
}


void ClosePlugin (Plugin * const plugin_p)
{
	AmigaPlugin *amiga_plugin_p = (AmigaPlugin *) plugin_p;

//	RemovePluginFromPluginManager (plugin_p);
	ClearBasePlugin (plugin_p);

	if (amiga_plugin_p -> ap_handle)
		{
			IElf->CloseElfTags (amiga_plugin_p -> ap_handle, CET_CloseInput, TRUE, TAG_DONE);
			amiga_plugin_p -> ap_handle = NULL;
		}

	if (amiga_plugin_p -> ap_seglist_p)
	  {
			IDOS->UnLoadSeg (amiga_plugin_p -> ap_seglist_p);
			amiga_plugin_p -> ap_seglist_p = ZERO;
	  }

}


void FreePlugin (Plugin * const plugin_p)
{
	ClosePlugin (plugin_p);
	FreeMemory ((AmigaPlugin *) plugin_p);
}



BOOLEAN IsPluginOpen (const Plugin * const plugin_p)
{
	if ((plugin_p -> pl_name_p) || (plugin_p -> pl_module_p))
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
	AmigaPlugin *amiga_plugin_p = (AmigaPlugin *) plugin_p;

	amiga_plugin_p -> ap_seglist_p = IDOS->LoadSeg (lib_path);

	if (amiga_plugin_p -> ap_seglist_p)
		{
			if (IDOS->GetSegListInfoTags (amiga_plugin_p -> ap_seglist_p, GSLI_ElfHandle, & (amiga_plugin_p -> ap_handle), TAG_DONE))
				{
					if (IElf->OpenElfTags (OET_ElfHandle, amiga_plugin_p -> ap_handle, TAG_DONE))
						{
							AddPluginToPluginManager (plugin_p);

							return TRUE;
						}
				}

			IDOS->UnLoadSeg (amiga_plugin_p -> ap_seglist_p);
			amiga_plugin_p -> ap_seglist_p = ZERO;

		}		/* if (amiga_plugin_p ->ap_seglist_p) */

	return FALSE;
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

					case PT_double64_IO:
						symbol_name = "DeallocateDataIO";
						break;

					case PT_UNKNOWN:
					default:

						break;
				}

			if (symbol_name)
				{
					APTR symbol_p = GetSymbol ((AmigaPlugin *) plugin_p, symbol_name);

					if (symbol_p)
						{
							void (*fn_p) (Module * const) = (void (*) (Module * const)) symbol_p;

							if (fn_p)
								{
									fn_p (plugin_p -> pl_module_p);

									plugin_p -> pl_module_p = NULL;
									success = TRUE;
								}
						}

				}		/* if (symbol_name) */
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

					case PT_double64_IO:
						symbol_name = "AllocateDataIO";
						break;

					case PT_UNKNOWN:
					default:
						break;
				}

			if (symbol_name)
				{
					APTR symbol_p = GetSymbol ((AmigaPlugin *) plugin_p, symbol_name);

					if (symbol_p)
						{
							Module *(*fn_p) (const ConfigurationManager * const) = (Module *(*) (const ConfigurationManager * const)) symbol_p;

							Module *module_p = fn_p (config_p);

							if (module_p)
								{
									plugin_p -> pl_module_p = module_p;
									module_p -> mo_plugin_p = plugin_p;
								}
						}

				}		/* if (symbol_name) */
		}

	return plugin_p -> pl_module_p;
}


static APTR GetSymbol (AmigaPlugin *plugin_p, char *symbol_name_s)
{
	APTR result_p = NULL;

	if (plugin_p -> ap_handle)
		{
			struct Elf32_SymbolQuery query;

			query.Flags = ELF32_SQ_BYNAME;
			query.Name = symbol_name_s;

			IElf->SymbolQuery (plugin_p -> ap_handle, 1, &query);

			if (query.Found)
				{
					result_p = (APTR) query.Value;
				}
		}

	return result_p;
}



char *MakePluginName (const char * const name)
{
	const size_t l = strlen (name);
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


char *GetPluginConfigName (const Plugin * const plugin_p)
{
	char *config_name_s = NULL;

	if (plugin_p -> pl_name_p)
		{
			char *local_filename_s = GetFilenameOnly (plugin_p -> pl_name_p);

			if (local_filename_s)
				{
					size_t l = strlen (local_filename_s);

					if (l >= 5)
						{
							if ((strncmp (local_filename_s + l - 4, ".so", 3) == 0) ||
								(strncmp (local_filename_s, "lib", 3) == 0))
								{
									l -= 6;		/* for the "lib" & ".so" */
									config_name_s = (char *) AllocMemory ((l + 1) * sizeof (char));

									if (config_name_s)
										{
											strncpy (config_name_s, local_filename_s + 3, l);
											* (config_name_s + l) = '\0';

										}		/* if (config_name_s) */

								}		/* if (strncmp (plugin_p -> pl_name_p + l - 5, ".dll", 4)) */

						}		/* if (l >= 5) */

					FreeCopiedString (local_filename_s);
				}		/* if (local_filename_s) */

		}		/* if (plugin_p -> pl_name_p) */

	return config_name_s;
}



void PrintPlugin (const Plugin * const plugin_p, OutputStream * const stream_p)
{
	AmigaPlugin *amiga_plugin_p = (AmigaPlugin *) plugin_p;

	PrintToOutputStream (stream_p, "amiga plugin: handle %x\n", amiga_plugin_p -> ap_handle);
	PrintBasePlugin (plugin_p, stream_p);
}

