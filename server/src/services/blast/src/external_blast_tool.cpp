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
/*
 * external_blast_tool.cpp
 *
 *  Created on: 22 Apr 2015
 *      Author: tyrrells
 */

#include <cstring>
#include <stdexcept>

#include "external_blast_tool.hpp"
#include "string_utils.h"
#include "temp_file.hpp"
#include "blast_service_job.h"
#include "blast_service_params.h"
#include "blast_util.h"
#include "math_utils.h"

const char * const ExternalBlastTool :: EBT_INPUT_SUFFIX_S = ".input";
const char * const ExternalBlastTool :: EBT_LOG_SUFFIX_S = ".log";


const char * const ExternalBlastTool :: EBT_COMMAND_LINE_EXECUTABLE_S = "cli";
const char * const ExternalBlastTool :: EBT_WORKING_DIR_S = "working_directory";
const char * const ExternalBlastTool :: EBT_OUTPUT_FORMAT_S = "output_format";
const char * const ExternalBlastTool :: EBT_RESULTS_FILE_S = "output_file";


char *ExternalBlastTool :: GetJobFilename (const char * const prefix_s, const char * const suffix_s)
{
	char *job_filename_s = NULL;
	char *job_id_s = GetUUIDAsString (bt_job_p -> bsj_job.sj_id);

	if (job_id_s)
		{
			char *file_stem_s = NULL;

			if (ebt_working_directory_s)
				{
					file_stem_s = MakeFilename (ebt_working_directory_s, job_id_s);
				}
			else
				{
					file_stem_s = job_id_s;
				}

			if (file_stem_s)
				{
					ByteBuffer *buffer_p = AllocateByteBuffer (1024);

					if (buffer_p)
						{
							bool success_flag = false;

							if (prefix_s)
								{
									success_flag = AppendStringsToByteBuffer (buffer_p, prefix_s, file_stem_s, NULL);
								}
							else
								{
									success_flag = AppendStringToByteBuffer (buffer_p, file_stem_s);
								}

							if (success_flag && suffix_s)
								{
									success_flag = AppendStringToByteBuffer (buffer_p, suffix_s);
								}

							if (success_flag)
								{
									job_filename_s = DetachByteBufferData (buffer_p);
								}
							else
								{
									FreeByteBuffer (buffer_p);
								}

						}		/* if (buffer_p) */

					FreeCopiedString (file_stem_s);
				}		/* if (file_stem_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get file stem for \"%s\"", job_id_s);
				}

			FreeUUIDString (job_id_s);
		}		/* if (job_id_s) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid string for %s", bt_job_p -> bsj_job.sj_name_s);
		}

	return job_filename_s;
}


ExternalBlastTool :: ExternalBlastTool (BlastServiceJob *job_p, const char *name_s, const char *factory_s, const BlastServiceData *data_p, const char * const blast_program_name_s)
: BlastTool (job_p, name_s, factory_s, data_p)
{
	ebt_results_filename_s = 0;
	ebt_working_directory_s = data_p -> bsd_working_dir_s;
	ebt_blast_s = blast_program_name_s;
	ebt_output_format = BS_DEFAULT_OUTPUT_FORMAT;
}



ExternalBlastTool :: ExternalBlastTool (BlastServiceJob *job_p, const BlastServiceData *data_p, const json_t *root_p)
	: BlastTool (job_p, data_p, root_p)
{
	ebt_blast_s = GetJSONString (root_p, EBT_COMMAND_LINE_EXECUTABLE_S);
	if (!ebt_blast_s)
		{
			throw std :: invalid_argument ("blast executable name not set");
		}

	ebt_working_directory_s = GetJSONString (root_p, EBT_WORKING_DIR_S);
	if (!ebt_working_directory_s)
		{
			throw std :: invalid_argument ("working directory not set");
		}

	const char *result_s = GetJSONString (root_p, EBT_RESULTS_FILE_S);
	if (result_s)
		{
			ebt_results_filename_s = CopyToNewString (result_s, 0, false);

			if (!ebt_results_filename_s)
				{
					throw std :: invalid_argument ("failed to copy results filename");
				}
		}
	else
		{
			throw std :: invalid_argument ("results filename not set");
		}

	ebt_output_format = BS_DEFAULT_OUTPUT_FORMAT;
}


ExternalBlastTool :: ~ExternalBlastTool ()
{
	if (ebt_results_filename_s)
		{
			FreeCopiedString (ebt_results_filename_s);
		}
}


char *ExternalBlastTool :: GetResults (BlastFormatter *formatter_p)
{
	char *results_s = NULL;

	if (ebt_results_filename_s)
		{
			if (formatter_p && (ebt_output_format != BS_DEFAULT_OUTPUT_FORMAT))
				{
					results_s = formatter_p -> GetConvertedOutput (ebt_results_filename_s, ebt_output_format);

					if (!results_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to format %s to " UINT32_FMT, ebt_results_filename_s, ebt_output_format);
						}

				}		/* if (formatter_p && (ebt_output_format != BS_DEFAULT_OUTPUT_FORMAT)) */
			else
				{
					if (IsPathValid (ebt_results_filename_s))
						{
							results_s = GetFileContentsAsStringByFilename (ebt_results_filename_s);

							if (!results_s)
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to read data from  %s", ebt_results_filename_s);
								}

						}		/* if (IsPathValid (ebt_results_filename_s)) */
					else
						{
							PrintErrors (STM_LEVEL_INFO, __FILE__, __LINE__, "File %s does not exist", ebt_results_filename_s);

						}
				}
		}		/* if (ebt_results_filename_s) */
	else
		{
			char uuid_s [UUID_STRING_BUFFER_SIZE];

			ConvertUUIDToString (bt_job_p -> bsj_job.sj_id, uuid_s);
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Results output object is NULL for \"%s\"", uuid_s);
		}

	return results_s;
}


bool ExternalBlastTool :: ParseParameters (ParameterSet *params_p, BlastAppParameters *app_params_p)
{
	bool success_flag = false;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	if (GetParameterValueFromParameterSet (params_p, BS_TASK.npt_name_s, &value, true))
		{
			if (AddBlastArgsPair ("task", value.st_string_value_s))
				{
					ArgsProcessor *args_processor_p = GetArgsProcessor ();

					if (GetAndAddBlastArgs (params_p, BS_MAX_SEQUENCES.npt_name_s, false, args_processor_p))
						{
							if (bt_job_p -> bsj_job.sj_name_s)
								{
									if (AddBlastArgsPair ("db", bt_job_p -> bsj_job.sj_name_s))
										{
											if (ParseBlastAppParameters (app_params_p, bt_service_data_p, params_p, args_processor_p))
												{
													/* Expect threshold */
													if (GetAndAddBlastArgs (params_p, BS_EXPECT_THRESHOLD.npt_name_s, false, args_processor_p))
														{
															/* Output Format
															 * If we have a BlastFormatter then the output is always set to 11 which is ASN and
															 * from that we can convert into any other format using a BlastFormatter tool
															 */
															memset (&value, 0, sizeof (SharedType));

															if (GetParameterValueFromParameterSet (params_p, BS_OUTPUT_FORMAT.npt_name_s, &value, true))
																{
																	ebt_output_format = value.st_ulong_value;

																	if (bt_service_data_p -> bsd_formatter_p)
																		{
																			success_flag = AddBlastArgsPair (BS_OUTPUT_FORMAT.npt_name_s, BS_DEFAULT_OUTPUT_FORMAT_S);
																		}
																	else
																		{
																			char *value_s = ConvertIntegerToString (ebt_output_format);

																			if (value_s)
																				{
																					success_flag = AddBlastArgsPair (BS_OUTPUT_FORMAT.npt_name_s, value_s);
																					FreeCopiedString (value_s);
																				}		/* if (value_s) */
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert output format \"" UINT32_FMT "\" to string", ebt_output_format);
																				}
																		}

																}		/* if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_OUTPUT_FORMAT, &value, true)) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get output format");
																}

															if (success_flag)
																{
																	/* Query Location */
																	if (GetParameterValueFromParameterSet (params_p, BS_SUBRANGE_FROM.npt_name_s, &value, true))
																		{
																			uint32 from = value.st_ulong_value;

																			if (GetParameterValueFromParameterSet (params_p, BS_SUBRANGE_TO.npt_name_s, &value, true))
																				{
																					uint32 to = value.st_ulong_value;

																					if ((from != 0) && (to != 0))
																						{
																							ByteBuffer *buffer_p = AllocateByteBuffer (1024);

																							if (buffer_p)
																								{
																									char *from_s = ConvertIntegerToString (from);

																									if (from_s)
																										{
																											char *to_s = ConvertIntegerToString (to);

																											if (to_s)
																												{
																													if (AppendStringsToByteBuffer (buffer_p, from_s, "-", to_s, NULL))
																														{
																															const char *query_loc_s = GetByteBufferData (buffer_p);

																															if (!AddBlastArgsPair ("query_loc", query_loc_s))
																																{
																																	success_flag = false;
																																}
																														}

																													FreeCopiedString (to_s);
																												}		/* if (to_s) */

																											FreeCopiedString (from_s);
																										}		/* if (from_s) */

																									FreeByteBuffer (buffer_p);
																								}		/* if (buffer_p) */

																						}		/* if ((from != 0) && (to != 0)) */

																				}		/* if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_TO, &to, true)) */

																		}		/* if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_FROM, &value, true)) */

																}		/*  if (success_flag) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set output format");
																}


														}		/* if (AddBlastArgsPairFromIntegerParameter (params_p, TAG_BLAST_EXPECT_THRESHOLD, "-evalue", true)) */

												}		/* if (bt_app_params_p -> ParseParametersToByteBuffer (bt_service_data_p, params_p, ebt_buffer_p)) */

										}		/* if (AddBlastArgsPair ("-db", bt_job_p -> sj_name_s))*/
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set database name");
										}
								}		/* if (bt_job_p -> sj_name_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get job name");
								}

						}		/* if (AddABlastrgsPair ("-num_alignments", "5")) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add num_alignments parameter");
						}

				}		/* if (AddBlastArgsPair ("-task", BS_TASK.npt_name_s)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add task parameter");
				}

		}		/* if (GetParameterValueFromParameterSet (params_p, BS_TASK.npt_name_s, &value, true)) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get task parameter value");
		}


	return success_flag;
}


bool ExternalBlastTool :: AddBlastArgsPair (const char *key_s, const char *value_s)
{
	bool success_flag = false;
	ArgsProcessor *ap_p = GetArgsProcessor ();

	if (ap_p)
		{
			success_flag = AddArgsPair (key_s, value_s, ap_p);
		}

	return success_flag;
}


bool ExternalBlastTool :: AddBlastArg (const char *arg_s, const bool hyphen_flag)
{
	bool success_flag = false;
	ArgsProcessor *ap_p = GetArgsProcessor ();

	if (ap_p)
		{
			success_flag = ap_p -> AddArg (arg_s, hyphen_flag);
		}

	return success_flag;
}


bool ExternalBlastTool :: SetUpOutputFile ()
{
	bool success_flag = false;
	char *local_filename_s = 0;
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	ConvertUUIDToString (bt_job_p -> bsj_job.sj_id, uuid_s);

	local_filename_s = ConcatenateStrings (uuid_s, BS_OUTPUT_SUFFIX_S);

	if (local_filename_s)
		{
			ebt_results_filename_s = MakeFilename (ebt_working_directory_s, local_filename_s);

			if (ebt_results_filename_s)
				{
					if (AddBlastArgsPair ("out", ebt_results_filename_s))
						{
							success_flag = true;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to set output filename to \"%s\"", ebt_results_filename_s);
						}
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create filename for %s, %s", ebt_working_directory_s, local_filename_s);
				}

			FreeCopiedString (local_filename_s);
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create local filename for %s, %s", uuid_s, BS_OUTPUT_SUFFIX_S);
		}



	return success_flag;
}


bool ExternalBlastTool :: SetInputFilename (const char * const filename_s)
{
	bool success_flag = false;

	if (filename_s)
		{
			success_flag = AddBlastArgsPair ("query", filename_s);
		}

	return success_flag;
}



bool ExternalBlastTool :: AddToJSON (json_t *root_p)
{
	bool success_flag = BlastTool :: AddToJSON (root_p);

	if (success_flag)
		{
			if (json_object_set_new (root_p, EBT_RESULTS_FILE_S, json_string (ebt_results_filename_s)) == 0)
				{
					if (json_object_set_new (root_p, EBT_COMMAND_LINE_EXECUTABLE_S, json_string (ebt_blast_s)) == 0)
						{
							if (json_object_set_new (root_p, EBT_WORKING_DIR_S, json_string (ebt_working_directory_s)) == 0)
								{
									if (json_object_set_new (root_p, EBT_OUTPUT_FORMAT_S, json_integer (ebt_output_format)) == 0)
										{
											success_flag = true;
										}		/* if (json_object_set_new (root_p, EBT_OUTPUT_FORMAT_S, json_integer (ebt_output_format)) == 0) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s:%d to ExternalBlastTool json", EBT_OUTPUT_FORMAT_S, ebt_output_format);
										}

								}		/* if (json_object_set_new (root_p, BT_FACTORY_NAME_S, json_string (bt_factory_name_s)) == 0) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s:%s to ExternalBlastTool json", EBT_WORKING_DIR_S, ebt_working_directory_s);
								}

						}		/* if (json_object_set_new (root_p, BT_NAME_S, json_string (bt_name_s)) == 0) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s:%s to ExternalBlastTool json", EBT_COMMAND_LINE_EXECUTABLE_S, ebt_blast_s);
						}

				}		/* if (json_object_set_new (root_p, EBT_RESULTS_FILE_S, output_file_s) == 0) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add %s:%s to ExternalBlastTool json", EBT_RESULTS_FILE_S, ebt_results_filename_s);
				}

		}		/* if (success_flag) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "BlastTool :: AddToJSON failed");
		}

	return success_flag;
}


char *ExternalBlastTool :: GetLog ()
{
	char *log_file_s = NULL;
	char sep_s [2];
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	*sep_s = GetFileSeparatorChar ();
	* (sep_s + 1) = '\0';

	ConvertUUIDToString (bt_job_p -> bsj_job.sj_id, uuid_s);

	log_file_s = ConcatenateVarargsStrings (ebt_working_directory_s, sep_s, uuid_s, BS_LOG_SUFFIX_S, NULL);

	if (log_file_s)
		{
			FILE *log_f = fopen (log_file_s, "r");

			if (log_f)
				{
					char *log_data_s = GetFileContentsAsString (log_f);

					if (log_data_s)
						{
							return log_data_s;
						}
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to open log file %s", log_file_s);
						}

				}		/* if (log_f) */
			else
				{
					PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to open log file %s", log_file_s);
				}

			FreeCopiedString (log_file_s);
		}		/* if (log_file_s) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to get log filename for %s", uuid_s);
		}

	return NULL;
}

