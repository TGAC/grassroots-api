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

#include "external_blast_tool.hpp"
#include "string_utils.h"
#include "temp_file.hpp"


const char * const ExternalBlastTool :: EBT_INPUT_SUFFIX_S = ".input";
const char * const ExternalBlastTool :: EBT_LOG_SUFFIX_S = ".log";


char *ExternalBlastTool :: GetJobFilename (const char * const prefix_s, const char * const suffix_s)
{
	char *job_filename_s = NULL;
	char *job_id_s = GetUUIDAsString (bt_job_p -> sj_id);

	if (job_id_s)
		{
			char *file_stem_s = NULL;
			bool alloc_file_stm_flag = false;

			if (ebt_working_directory_s)
				{
					file_stem_s = MakeFilename (ebt_working_directory_s, job_id_s);
					alloc_file_stm_flag = true;
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
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get uuid string for %s", bt_job_p -> sj_name_s);
		}

	return job_filename_s;
}

ExternalBlastTool :: ExternalBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s, const char * const blast_program_name_s)
: BlastTool (job_p, name_s)
{
	ebt_buffer_p = AllocateByteBuffer (1024);

	if (!ebt_buffer_p)
		{
			throw std :: bad_alloc ();
		}
	ebt_output_p = 0;
	ebt_working_directory_s = working_directory_s;
	ebt_blast_s = blast_program_name_s;
}


ExternalBlastTool :: ~ExternalBlastTool ()
{
	if (ebt_output_p)
		{
			delete ebt_output_p;
		}

	FreeByteBuffer (ebt_buffer_p);
}


bool ExternalBlastTool :: AddArgsPair (const char *key_s, const char *value_s)
{
	bool success_flag = false;

	if (AddArg (key_s))
		{
			if (AddArg (value_s))
				{
					success_flag = true;
				}
		}

	return success_flag;
}



bool ExternalBlastTool :: AddArg (const char *arg_s)
{
	bool success_flag = true;

	if (ebt_buffer_p -> bb_current_index > 0)
		{
			success_flag = AppendStringToByteBuffer (ebt_buffer_p, " ");
		}

	if (success_flag)
		{
			success_flag =  AppendStringToByteBuffer (ebt_buffer_p, arg_s);
		}

	return success_flag;
}


const char *ExternalBlastTool :: GetResults ()
{
	const char *results_s = NULL;

	if (ebt_output_p && (ebt_output_p -> Open ("r")))
		{
			results_s = ebt_output_p -> GetData ();
		}

	return results_s;
}


void ExternalBlastTool :: ClearResults ()
{
	if (ebt_output_p)
		{
			ebt_output_p -> ClearData ();
		}
}


/*
#define TAG_BLAST_INPUT_QUERY MAKE_TAG ('B', 'Q', 'U', 'Y')
#define TAG_BLAST_INPUT_FILE MAKE_TAG ('B', 'I', 'N', 'F')
#define TAG_BLAST_OUTPUT_FILE MAKE_TAG ('B', 'O', 'U', 'F')
#define TAG_BLAST_MAX_SEQUENCES MAKE_TAG ('B', 'M', 'S', 'Q')
#define TAG_BLAST_SHORT_QUERIES MAKE_TAG ('B', 'S', 'H', 'Q')
#define TAG_BLAST_EXPECT_THRESHOLD MAKE_TAG ('B', 'E', 'X', 'T')
#define TAG_BLAST_WORD_SIZE MAKE_TAG ('B', 'W', 'D', 'S')
#define TAG_BLAST_MAX_RANGE_MATCHES MAKE_TAG ('B', 'M', 'R', 'G')
#define TAG_BLAST_SUBRANGE_FROM MAKE_TAG ('B', 'Q', 'F', 'R')
#define TAG_BLAST_SUBRANGE_TO MAKE_TAG ('B', 'Q', 'T', 'O')
#define TAG_BLAST_MATCH_SCORE MAKE_TAG ('B', 'M', 'T', 'C')
#define TAG_BLAST_MISMATCH_SCORE MAKE_TAG ('B', 'M', 'S', 'M')
*/

bool ExternalBlastTool :: ParseParameters (ParameterSet *params_p)
{
	bool success_flag = AddArgsPair ("-task", "blastn");
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	if (success_flag)
		{
			success_flag = AddArgsPair ("-num_alignments", "5");
		}
/*
	if (success_flag)
		{
			success_flag = AddArgsPair ("-num_descriptions", "5");
		}
*/
	/* Db */
	if (success_flag)
		{
			success_flag = false;

			if (bt_job_p -> sj_name_s)
				{
					success_flag = AddArgsPair ("-db", bt_job_p -> sj_name_s);
				}
		}

	/* Query Location */
	if (success_flag)
		{
			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_FROM, &value, true))
				{
					if (value.st_string_value_s)
						{
							SharedType to;

							memset (&to, 0, sizeof (SharedType));

							if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_SUBRANGE_TO, &to, true))
								{
									if (to.st_string_value_s)
										{
											ByteBuffer *buffer_p = AllocateByteBuffer (1024);
											success_flag = false;

											if (buffer_p)
												{
													if (AppendStringsToByteBuffer (buffer_p, value.st_string_value_s, "-", to.st_string_value_s, NULL))
														{
															if (AddArg (GetByteBufferData (buffer_p)))
																{
																	success_flag = true;
																}
														}

													FreeByteBuffer (buffer_p);
												}
										}
								}
						}
				}
		}


	/* Reward */
	if (success_flag)
		{
			success_flag = AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_MATCH_SCORE, "-reward", false);
		}


	/* Penalty */
	if (success_flag)
		{
			success_flag = AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_MISMATCH_SCORE, "-penalty", false);
		}

	/* Max target sequences */
	/*
	if (success_flag)
		{
			success_flag = AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_MAX_SEQUENCES, "-max_target_seqs", true);
		} */


	/* Expect threshold */
	if (success_flag)
		{
			success_flag = AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_EXPECT_THRESHOLD, "-evalue", true);
		}

	/* Word Size */
	if (success_flag)
		{
			success_flag = AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_WORD_SIZE, "-word_size", true);
		}

	/* Output Format */
	if (success_flag)
		{
			success_flag = AddArgsPairFromIntegerParameter (params_p, TAG_BLAST_OUTPUT_FORMAT, "-outfmt", true);
		}


	return success_flag;
}


bool ExternalBlastTool :: AddArgsPairFromIntegerParameter (const ParameterSet *params_p, const Tag tag, const char *key_s, const bool unsigned_flag)
{
	bool success_flag = false;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	if (GetParameterValueFromParameterSet (params_p, tag, &value, true))
		{
			int32 param_value = unsigned_flag ? ((int32) value.st_ulong_value) : (value.st_long_value);
			char *value_s = ConvertIntegerToString (param_value);

			if (value_s)
				{
					success_flag = AddArgsPair (key_s, value_s);
					FreeCopiedString (value_s);
				}		/* if (value_s) */
		}

	return success_flag;
}


bool ExternalBlastTool :: SetUpOutputFile ()
{
	bool success_flag = false;

	ebt_output_p = TempFile :: GetTempFile (ebt_working_directory_s, bt_job_p -> sj_id, BS_OUTPUT_SUFFIX_S);

	if (ebt_output_p)
		{
			const char *output_filename_s = ebt_output_p -> GetFilename ();

			if (output_filename_s)
				{
					success_flag = AddArgsPair ("-out", output_filename_s);
				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get output filename for \"%s\"", bt_job_p -> sj_name_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get temp output file for \"%s\"", bt_job_p -> sj_name_s);
		}


	return success_flag;
}


bool ExternalBlastTool :: SetInputFilename (const char * const filename_s)
{
	bool success_flag = false;

	if (filename_s)
		{
			success_flag = AddArgsPair ("-query", filename_s);
		}

	return success_flag;
}
