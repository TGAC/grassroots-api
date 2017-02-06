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
#include "temp_file.hpp"
#include "string_utils.h"


SystemPolymarkerTool :: SystemPolymarkerTool (PolymarkerServiceData *data_p, ServiceJob *job_p)
	: PolymarkerTool (data_p, job_p)
{

}


SystemPolymarkerTool ::  ~SystemPolymarkerTool ()
{

}


bool SystemPolymarkerTool :: ParseParameters (const ParameterSet * const param_set_p)
{
	bool success_flag = false;
	ByteBuffer *args_p = AllocateByteBuffer (1024);

	if (args_p)
		{
			SharedType value;

			if (GetParameterValueFromParameterSet (param_set_p, PS_CONTIG_FILENAME.npt_name_s, &value, true))
				{
					if (AppendStringsToByteBuffer (args_p, " --contigs ", value.st_string_value_s, NULL))
						{
							if (GetParameterValueFromParameterSet (param_set_p, PS_GENE_ID.npt_name_s, &value, true))
								{
									char *gene_id_s = CopyToNewString (value.st_string_value_s, 0, false);

									if (GetParameterValueFromParameterSet (param_set_p, PS_TARGET_CHROMOSOME.npt_name_s, &value, true))
										{
											char *chromosome_s = CopyToNewString (value.st_string_value_s, 0, false);

											if (GetParameterValueFromParameterSet (param_set_p, PS_SEQUENCE.npt_name_s, &value, true))
												{
													char *sequence_s = CopyToNewString (value.st_string_value_s, 0, false);


												}

										}

								}


							if (AppendStringsToByteBuffer (args_p, " --output ", output_working_dir_s, NULL))
								{

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


bool SystemPolymarkerTool :: WriteInputFile (const char *gene_id_s, const char *target_chromosome_s, const char *sequence_s)
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

			delete input_file_p;
		}


	return success_flag;

}

