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


ExternalBlastTool :: ExternalBlastTool (ServiceJob *job_p, const char *name_s, const char *working_directory_s, const char * const blast_program_name_s)
: BlastTool (job_p, name_s)
{
	bool success_flag = false;

	ebt_buffer_p = AllocateByteBuffer (1024);
	ebt_output_p = 0;
	ebt_working_directory_s = working_directory_s;
	ebt_blast_s = blast_program_name_s;

	if (AddArg ("-db"))
		{
			if (AddArg (name_s))
				{
					success_flag = true;
				}
		}

	if (!success_flag)
		{
			throw std :: bad_alloc ();
		}
}


ExternalBlastTool :: ~ExternalBlastTool ()
{
	if (ebt_output_p)
		{
			delete ebt_output_p;
		}

	FreeByteBuffer (ebt_buffer_p);
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

bool ExternalBlastTool :: ParseParameters (ParameterSet *params_p, const char *filename_s)
{
	bool success_flag = false;
	SharedType value;

	memset (&value, 0, sizeof (SharedType));

	if (filename_s)
		{
			if (AddArg ("-query"))
				{
					if (AddArg (filename_s))
						{
							success_flag = true;
						}
				}
		}

	/* Output File */
	if (success_flag)
		{
			char *buffer_p = GetTempFilenameBuffer (NULL, filename_s, NULL);

			success_flag = false;

			if (buffer_p)
				{
					ebt_output_p = TempFile :: GetTempFile (buffer_p, "w");

					if (ebt_output_p)
						{
							success_flag = SetOutputFilename (ebt_output_p -> GetFilename ());

							ebt_output_p -> Close ();
						}
					else
						{
							success_flag = false;
						}
				}
		}


	if (success_flag)
		{
			success_flag =  (AddArg ("-task")) && (AddArg ("blastn"));
		}

	if (success_flag)
		{
			success_flag =  (AddArg ("-num_alignments")) && (AddArg ("5"));
		}

	if (success_flag)
		{
			success_flag =  (AddArg ("-num_descriptions")) && (AddArg ("5"));
		}

	/* Db */
	if (success_flag)
		{
			success_flag = false;

			if (bt_job_p -> sj_name_s)
				{
					if (AddArg ("-db"))
						{
							if (AddArg (bt_job_p -> sj_name_s))
								{
									success_flag = true;
								}
						}
				}
		}



	//return success_flag;

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
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_MATCH_SCORE, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_long_value);

					if (value_s)
						{
							if (AddArg ("-reward"))
								{
									if (AddArg (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	/* Penalty */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_MISMATCH_SCORE, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_long_value);

					if (value_s)
						{
							if (AddArg ("-penalty"))
								{
									if (AddArg (value_s))
										{
											success_flag = true;
										}
								}


							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}

	/* Max target sequences */
	/*
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_MAX_SEQUENCES, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_ulong_value);

					if (value_s)
						{
							if (AddArg ("-max_target_seqs"))
								{
									if (AddArg (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s)
				}
		} */


	/* Expect threshold */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_EXPECT_THRESHOLD, &value, true))
				{
					if (value.st_ulong_value > 0)
						{
							char *value_s = ConvertIntegerToString (value.st_ulong_value);

							if (value_s)
								{
									if (AddArg ("-evalue"))
										{
											if (AddArg (value_s))
												{
													success_flag = true;
												}
										}

									FreeCopiedString (value_s);
								}		/* if (value_s) */
						}
				}
		}


	/* Word Size */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_WORD_SIZE, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_ulong_value);

					if (value_s)
						{
							if (AddArg ("-word_size"))
								{
									if (AddArg (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	/* Output Format */
	if (success_flag)
		{
			success_flag = false;

			if (GetParameterValueFromParameterSet (params_p, TAG_BLAST_OUTPUT_FORMAT, &value, true))
				{
					char *value_s = ConvertIntegerToString (value.st_ulong_value);

					if (value_s)
						{
							if (AddArg ("-outfmt"))
								{
									if (AddArg (value_s))
										{
											success_flag = true;
										}
								}

							FreeCopiedString (value_s);
						}		/* if (value_s) */
				}
		}


	return success_flag;
}


bool ExternalBlastTool :: SetOutputFilename (const char *filename_s)
{
	bool success_flag = false;

	if (AddArg ("-out"))
		{
			if (AddArg (filename_s))
				{
					success_flag = true;
				}
		}

	return success_flag;
}
