/*
** Copyright 2014-2016 The Earlham Institute
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
/**
 * system_polymarker_tool.cpp
 *
 *  Created on: 6 Feb 2017
 *      Author: billy
 *
 * @file
 * @brief
 */


#include "system_polymarker_tool.hpp"
#include "string_utils.h"
#include "process.h"


SystemPolymarkerTool :: SystemPolymarkerTool (PolymarkerServiceData *data_p, ServiceJob *job_p)
	: PolymarkerTool (data_p, job_p)
{
	spt_command_line_args_ss = 0;
}


SystemPolymarkerTool ::  ~SystemPolymarkerTool ()
{
	FreeCommandLineArgs ();
}


void SystemPolymarkerTool :: FreeCommandLineArgs ()
{
	if (spt_command_line_args_ss)
		{
			char **arg_ss = spt_command_line_args_ss;

			while (*arg_ss)
				{
					FreeCopiedString (*arg_ss);
					++ arg_ss;
				}

			FreeMemory (spt_command_line_args_ss);
			spt_command_line_args_ss = 0;
		}
}


bool SystemPolymarkerTool :: ParseParameters (const ParameterSet * const param_set_p)
{
	bool success_flag = false;
	char *contig_s;

	if (GetStringParameter (param_set_p, PS_CONTIG_FILENAME.npt_name_s, &contig_s))
		{
			char *gene_id_s;

			if (GetStringParameter (param_set_p, PS_GENE_ID.npt_name_s, &gene_id_s))
				{
					char *target_chromosome_s;

					if (GetStringParameter (param_set_p, PS_TARGET_CHROMOSOME.npt_name_s, &target_chromosome_s))
						{
							char *sequence_s;

							if (GetStringParameter (param_set_p, PS_SEQUENCE.npt_name_s, &sequence_s))
								{
									TempFile *input_file_p = GetInputFile (gene_id_s, target_chromosome_s, sequence_s);

									if (input_file_p)
										{
											const char *input_s = input_file_p -> GetFilename ();
											char *output_s = GetOutputFolder ();

											if (output_s)
												{
													if (CreateArgs (input_s, output_s, contig_s))
														{
															success_flag = true;
														}
													else
														{
															FreeCopiedString (output_s);
														}

													if (!success_flag)
														{
															FreeCopiedString (output_s);
														}
												}

											delete input_file_p;
										}

									FreeCopiedString (sequence_s);
								}

							FreeCopiedString (target_chromosome_s);
						}

					FreeCopiedString (gene_id_s);
				}

			if (!success_flag)
				{
					FreeCopiedString (contig_s);
				}
		}

	return success_flag;
}


bool SystemPolymarkerTool :: CreateArgs (const char *input_s, char *output_s, char *contigs_s)
{
	char *program_name_s = CopyToNewString (pt_service_data_p -> psd_executable_s, 0, false);

	if (program_name_s)
		{
			char *markers_key_s = CopyToNewString ("--marker_list", 0, false);

			if (markers_key_s)
				{
					char *output_key_s = CopyToNewString ("--output", 0, false);

					if (output_key_s)
						{
							char *contigs_key_s = CopyToNewString ("--contigs", 0, false);

							if (contigs_key_s)
								{
									char *input_copy_s = CopyToNewString (input_s, 0, false);

									if (input_copy_s)
										{
											FreeCommandLineArgs ();

											spt_command_line_args_ss = (char **) AllocMemoryArray (8, sizeof (char *));

											if (spt_command_line_args_ss)
												{
													char **arg_ss = spt_command_line_args_ss;

													*arg_ss = program_name_s;

													* (++ arg_ss) = markers_key_s;
													* (++ arg_ss) = input_copy_s;

													* (++ arg_ss) = output_key_s;
													* (++ arg_ss) = output_s;

													* (++ arg_ss) = contigs_key_s;
													* (++ arg_ss) = contigs_s;

													* (++ arg_ss) = NULL;

													return true;
												}

											FreeCopiedString (input_copy_s);
										}

									FreeCopiedString (contigs_key_s);
								}

							FreeCopiedString (output_key_s);
						}

					FreeCopiedString (markers_key_s);
				}

			FreeCopiedString (program_name_s);
		}

	return false;
}



bool SystemPolymarkerTool :: Run ()
{
	bool success_flag = false;

	int32 process_id = CreateProcess ("/bin/bash", spt_command_line_args_ss, NULL);

	if (process_id >= 0)
		{
			pt_process_id = process_id;
			success_flag = true;
		}


	return success_flag;
}


TempFile *SystemPolymarkerTool :: GetInputFile (const char *gene_id_s, const char *target_chromosome_s, const char *sequence_s)
{
	bool success_flag = false;
	TempFile *input_file_p = TempFile :: GetTempFile (pt_service_data_p -> psd_working_dir_s, pt_service_job_p -> sj_id, ".input");

	if (input_file_p)
		{
			if (input_file_p -> Open ("a"))
				{
					char *line_s = ConcatenateVarargsStrings (gene_id_s, ",", target_chromosome_s, ", ", sequence_s, "\n", NULL);

					if (line_s)
						{
							if (input_file_p -> Print (line_s))
								{
									success_flag = true;

								}

							FreeCopiedString (line_s);
						}
				}

			if (!success_flag)
				{
					delete input_file_p;
					input_file_p = 0;
				}

		}

	return input_file_p;
}


char *SystemPolymarkerTool :: GetOutputFolder ()
{
	char *output_s = 0;
	char uuid_s [UUID_STRING_BUFFER_SIZE];
	char *parent_s;

	ConvertUUIDToString (pt_service_job_p -> sj_id, uuid_s);

	parent_s = MakeFilename (pt_service_data_p -> psd_working_dir_s, uuid_s);

	if (parent_s)
		{
			output_s = MakeFilename (parent_s, "_output");

			FreeCopiedString (parent_s);
		}

	return output_s;
}




bool SystemPolymarkerTool :: GetStringParameter (const ParameterSet * const params_p, const char *param_name_s, char **param_value_ss)
{
	bool success_flag = false;
	SharedType value;

	if (GetParameterValueFromParameterSet (params_p, param_name_s, &value, true))
		{
			*param_value_ss = CopyToNewString (value.st_string_value_s, 0, false);

			if (*param_value_ss)
				{
					success_flag = true;
				}
		}

	return success_flag;
}


