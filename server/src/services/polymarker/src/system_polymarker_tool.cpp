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


SystemPolymarkerTool :: SystemPolymarkerTool (PolymarkerServiceData *data_p, ServiceJob *job_p)
	: PolymarkerTool (data_p, job_p)
{
	spt_contig_s = 0;
	spt_gene_id_s = 0;
	spt_target_chromosome_s = 0;
	spt_sequence_s = 0;
	spt_command_line_s = 0;
}


SystemPolymarkerTool ::  ~SystemPolymarkerTool ()
{
	if (spt_contig_s)
		{
			FreeCopiedString (spt_contig_s);
		}

	if (spt_gene_id_s)
		{
			FreeCopiedString (spt_gene_id_s);
		}

	if (spt_target_chromosome_s)
		{
			FreeCopiedString (spt_target_chromosome_s);
		}

	if (spt_sequence_s)
		{
			FreeCopiedString (spt_sequence_s);
		}

	if (spt_command_line_s)
		{
			FreeCopiedString (spt_command_line_s);
		}
}


bool SystemPolymarkerTool :: ParseParameters (const ParameterSet * const param_set_p)
{
	bool success_flag = false;

	if (GetStringParameter (param_set_p, PS_CONTIG_FILENAME.npt_name_s, &spt_contig_s))
		{
			if (GetStringParameter (param_set_p, PS_GENE_ID.npt_name_s, &spt_gene_id_s))
				{
					if (GetStringParameter (param_set_p, PS_TARGET_CHROMOSOME.npt_name_s, &spt_target_chromosome_s))
						{
							if (GetStringParameter (param_set_p, PS_SEQUENCE.npt_name_s, &spt_sequence_s))
								{
									TempFile *input_file_p = GetInputFile ();

									if (input_file_p)
										{
											const char *input_s = input_file_p -> GetFilename ();
											char *output_s = GetOutputFolder ();

											if (output_s)
												{
													spt_command_line_s = ConcatenateVarargsStrings (pt_service_data_p -> psd_executable_s, " --marker_list ", input_s, " --output ", output_s, " --contigs ", spt_contig_s, NULL);

													if (spt_command_line_s)
														{
															success_flag = true;
														}

													FreeCopiedString (output_s);
												}
										}
								}
						}
				}
		}

	return success_flag;
}




bool SystemPolymarkerTool :: Run ()
{
	bool success_flag = false;

	return success_flag;
}


TempFile *SystemPolymarkerTool :: GetInputFile ()
{
	bool success_flag = false;
	TempFile *input_file_p = TempFile :: GetTempFile (pt_service_data_p -> psd_working_dir_s, pt_service_job_p -> sj_id, ".input");

	if (input_file_p)
		{
			if (input_file_p -> Open ("a"))
				{
					char *line_s = ConcatenateVarargsStrings (spt_gene_id_s, ",", spt_target_chromosome_s, ", ", spt_sequence_s, "\n", NULL);

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


