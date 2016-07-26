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
#include <stdarg.h>

#include "file_output_stream.h"
#include "string_utils.h"
#include "file_streamer.h"



typedef struct FileOutputStreamerData
{
	ModuleData fos_base_data;

	FileOutputStream *fos_log_stream_p;
	FileOutputStream *fos_errors_stream_p;
} FileOutputStreamerData;





/*********************************************/
/************ STATIC PROTOTYTPES *************/
/*********************************************/


/**
 * Print to the error FILE. If this has not
 * previously been set, it will default to stderr.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error FILE is NULL, then
 * this will return 0.
 */
static int PrintErrorsToFile (const OutputStreamerModule * const module_p, const uint32 level, const char *message, ...);


/**
 * Print to the log FILE.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the progress FILE is NULL, then
 * this will return 0.
 */
static int PrintLogToFile (const OutputStreamerModule * const module_p, const uint32 level, const char *message, ...);


static char *GetStreamName (const char * const output_dir_s, const char *output_filename_s, char *stream_name_s, const char * const suffix_s);

static FileOutputStream *InitialiseIO (const HashTable * const config_p, const char * const key_s, const char * const suffix_s);


static const char *GetFileStreamerName (void);


static const char *GetFileStreamerDescription (void);



static LinkedList *GetFileStreamerPreprocessorParameters (void);


static OutputStream *GetErrorOutputFile (const OutputStreamerModule * const module_p);


static OutputStream *GetLogOutputFile (const OutputStreamerModule * const module_p);


/*********************************************/
/*************** API FUNCTIONS ***************/
/*********************************************/


OutputStreamerModule *AllocateStreamer (const ConfigurationManager * const config_manager_p)
{
	OutputStreamerModule *streamer_p = (OutputStreamerModule *) AllocMemory (sizeof (OutputStreamerModule));

	if (streamer_p)
		{
			FileOutputStreamerData *data_p = (FileOutputStreamerData *) AllocMemory (sizeof (FileOutputStreamerData));

			if (data_p)
				{
					data_p -> fos_log_stream_p = NULL;
					data_p -> fos_errors_stream_p = NULL;
					data_p -> fos_results_stream_p = NULL;

					if (ParseFileStreamerConfiguration (& (data_p -> fos_base_data), config_manager_p))
						{
							InitialiseModule (& (streamer_p -> osm_module), GetFileStreamerName, GetFileStreamerDescription, GetFileStreamerConfigKeyPrefix, GetFileStreamerPreprocessorParameters, ParseFileStreamerConfiguration, GetFileStreamerNumberOfMainTasks, & (data_p -> fos_base_data), NULL, MP_DEFAULT);

							streamer_p -> osm_get_errors_stream_fn = GetErrorOutputFile;
							streamer_p -> osm_get_log_stream_fn = GetLogOutputFile;
							streamer_p -> osm_get_results_stream_fn = GetResultsOutputFile;

							streamer_p -> osm_print_errors_fn = PrintErrorsToFile;
							streamer_p -> osm_print_log_fn = PrintLogToFile;
							streamer_p -> osm_print_results_fn = PrintResultsToFile;

							return streamer_p;
						}		/* if (data_p -> fos_log_stream_p) */

					FreeMemory (data_p);
				}		/* if (data_p) */

			FreeMemory (streamer_p);
		}		/* if (streamer_p) */

	return NULL;
}





void DeallocateStreamer (OutputStreamerModule *streamer_p)
{
	FileOutputStreamerData *data_p = (FileOutputStreamerData *) (streamer_p -> osm_module.mo_data_p);

	ClearModule (& (streamer_p -> osm_module));

	DeallocateFileOutputStream (data_p -> fos_errors_stream_p);
	DeallocateFileOutputStream (data_p -> fos_log_stream_p);
	DeallocateFileOutputStream (data_p -> fos_results_stream_p);

	FreeMemory (data_p);
	FreeMemory (streamer_p);
}


/*********************************************/
/************* STATIC FUNCTIONS **************/
/*********************************************/

static BOOLEAN ParseFileStreamerConfiguration (ModuleData *module_data_p, const ConfigurationManager * const config_manager_p)
{
	FileOutputStreamerData *data_p = (FileOutputStreamerData *) (module_data_p);
	const HashTable * const config_p = config_manager_p -> cm_config_p;

	if (data_p -> fos_log_stream_p)
		{
			DeallocateFileOutputStream (data_p -> fos_log_stream_p);
			data_p -> fos_log_stream_p = NULL;
		}

	if (data_p -> fos_errors_stream_p)
		{
			DeallocateFileOutputStream (data_p -> fos_errors_stream_p);
			data_p -> fos_errors_stream_p = NULL;
		}

	if (data_p -> fos_results_stream_p)
		{
			DeallocateFileOutputStream (data_p -> fos_results_stream_p);
			data_p -> fos_results_stream_p = NULL;
		}


	data_p -> fos_log_stream_p = InitialiseIO (config_p, FILE_OUTPUT_STREAMER_CONFIG_KEY "_LogFilename", ".log");

	if (data_p -> fos_log_stream_p)
		{
			data_p -> fos_errors_stream_p = InitialiseIO (config_p, FILE_OUTPUT_STREAMER_CONFIG_KEY "_ErrorsFilename", ".errors");

			if (data_p -> fos_errors_stream_p)
				{
					data_p -> fos_results_stream_p = InitialiseIO (config_p, FILE_OUTPUT_STREAMER_CONFIG_KEY "_ResultsFilename", ".results");

					if (data_p -> fos_results_stream_p)
						{
							return TRUE;
						}

					DeallocateFileOutputStream ((FileOutputStream *) (data_p -> fos_errors_stream_p));
					data_p -> fos_errors_stream_p = NULL;
				}		/* if (data_p -> fos_log_stream_p) */

			DeallocateFileOutputStream ((FileOutputStream *) (data_p -> fos_log_stream_p));
			data_p -> fos_log_stream_p = NULL;
		}		/* if (data_p -> fos_log_stream_p) */


	return FALSE;
}


static FileOutputStream *InitialiseIO (const HashTable * const config_p, const char * const key_s, const char * const suffix_s)
{
	FileOutputStream *stream_p = NULL;
	const char * const output_dir_s = GetOutputDir ();

	/* Make sure that the output directory exists */
	if (EnsureDirectoryExists (output_dir_s))
		{
			char *output_filename_s = (char *) GetFromHashTable (config_p, "OutputFilename");

			/* Adapt the streams to be relative to our output directory */
			char *name_s = GetStreamName (output_dir_s, output_filename_s, (char *) GetFromHashTable (config_p, key_s), suffix_s);
			stream_p = AllocateFileOutputStream (name_s);

			if (name_s)
				{
					FreeCopiedString (name_s);
				}

		}		/* if (CreateNewDirectory (output_dir_s)) */

	return stream_p;
}


/**
 * Get where result messages will be sent.
 *
 * @return The FILE to send results to.
 */
static OutputStream *GetResultsOutputFile (const OutputStreamerModule * const streamer_p)
{
	return ((OutputStream *) ((((FileOutputStreamerData *) (streamer_p -> osm_module.mo_data_p)) -> fos_results_stream_p)));
}


/**
 * Get where logging messages will be sent.
 *
 * @return The FILE to send results to.
 */
static OutputStream *GetLogOutputFile (const OutputStreamerModule * const streamer_p)
{
	return ((OutputStream *) ((((FileOutputStreamerData *) (streamer_p -> osm_module.mo_data_p)) -> fos_log_stream_p)));
}


/**
 * Get where error messages will be sent.
 *
 * @return The FILE to send results to.
 */
static OutputStream *GetErrorOutputFile (const OutputStreamerModule * const streamer_p)
{
	return ((OutputStream *) ((((FileOutputStreamerData *) (streamer_p -> osm_module.mo_data_p)) -> fos_errors_stream_p)));
}


/**
 * Print to the error FILE. If this has not
 * previously been set, it will default to stderr.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the error FILE is NULL, then
 * this will return 0.
 */
static int PrintErrorsToFile (const OutputStreamerModule * const module_p, const uint32 level, const char *message_s, ...)
{
	int result;
	OutputStream *stream_p = module_p -> osm_get_errors_stream_fn (module_p);
	va_list args;

	va_start (args, message_s);

	result = stream_p -> st_print_fn (stream_p, level, message_s, args);

	va_end (args);

	return result;
}


/**
 * Print to the log FILE.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the log FILE is NULL, then
 * this will return 0.
 */
static int PrintLogToFile (const OutputStreamerModule * const module_p, const uint32 level, const char *message_s, ...)
{
	int result;
	OutputStream *stream_p = module_p -> osm_get_log_stream_fn (module_p);
	va_list args;

	va_start (args, message_s);

	result = stream_p -> st_print_fn (stream_p, level, message_s, args);

	va_end (args);

	return result;
}


/**
 * Print to the results FILE.
 *
 * @param message A char * using the same format as printf, etc.
 * @return On success, the total number of characters written is returned.
 * On failure, a negative number is returned. If the results FILE is NULL, then
 * this will return 0.
 */
static int PrintResultsToFile (const OutputStreamerModule * const module_p, const uint32 level, const char *message_s, ...)
{
	int result;
	OutputStream *stream_p = module_p -> osm_get_results_stream_fn (module_p);
	va_list args;

	va_start (args, message_s);

	result = stream_p -> st_print_fn (stream_p, level, message_s, args);

	va_end (args);

	return result;
}

/**
 *
 * @return A newly-allocated string with the name to use or NULL upon error.
 */
static char *GetStreamName (const char * const output_dir_s, const char *output_filename_s, char *stream_name_s, const char * const suffix_s)
{
	bool allocated_stream_name_flag = false;
	char *filename_s = NULL;


	if (!stream_name_s)
		{
			/*
				if stream_name_s is NULL use the output_filename instead and
				add the relevant suffix.
			*/

			if (output_filename_s)
				{
					stream_name_s = ConcatenateStrings (output_filename_s, suffix_s);
				}
			else
				{
					stream_name_s = ConcatenateStrings ("output", suffix_s);
				}

			if (stream_name_s)
				{
					allocated_stream_name_flag = TRUE;
				}
		}

	if (stream_name_s)
		{
			/* if we've got an output direrctory, put the files there */
			if (output_dir_s)
				{
					filename_s = MakeFilename (output_dir_s, stream_name_s);

					if (allocated_stream_name_flag)
						{
							FreeCopiedString (stream_name_s);
						}
				}
			else
				{
					/* if the stream_name_s was newly allocated, simply use it. */
					if (allocated_stream_name_flag)
						{
							filename_s = stream_name_s;
							allocated_stream_name_flag = false;
						}
					else
						{
							filename_s = CopyToNewString (stream_name_s, 0, TRUE);
						}
				}
		}

	return filename_s;
}


static const char *GetFileStreamerName (void)
{
	return "File Streamer.";
}


static const char *GetFileStreamerDescription (void)
{
	return "Save all error, log and results messages to files.";
}



static LinkedList *GetFileStreamerPreprocessorParameters (void)
{
	return NULL;
}


static int GetFileStreamerNumberOfMainTasks (ModuleData *data_p, const ConfigurationManager *const config_p)
{
	return 0;
}
